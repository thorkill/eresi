/*
** source.c for elfsh
** 
** Started on  Fri May 16 15:18:35 2005 mayhem
**
*/
#include "elfsh.h"


/* Allow for the sourcing of elfsh scripts */
int		cmd_source()
{
  elfshpath_t	*tmp;
  char		*str;
  int		fd;
  int		saved;
  char		savedmode;
  elfshargv_t	*savedcmd;
  char          *(*savedinput)();  
  char		**argv;
  char		**av;
  int		ac;
  char		buf[BUFSIZ];

  int           idx;
  char          actual[16];
  elfshpath_t   *new;
  int		argc = 0 ;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* build argc */
  for (idx = 0; world.curjob->curcmd->param[idx] != NULL; idx++)
    {
      argc++;
    }

  /* Resolve the parameter */
  str = world.curjob->curcmd->param[0];
  if (*str == ELFSH_VARPREF)
    {
      str = vm_lookup_var(++str);
      if (!str)
	return (-1);
      tmp = hash_get(&vars_hash, str);
      if (!tmp || tmp->type != ELFSH_OBJSTR)
	return (-1);
      str = tmp->immed_val.str;
    }

  /* Open the script file and pass its parameters */
  argv = alloca((2 + argc) * sizeof(char *));

  argv[0] = "#!elfsh";
  argv[1] = str;
  ac = 1;
  for (ac = 1; world.curjob->curcmd->param[ac]; ac++)
    {
      argv[ac + 1] = world.curjob->curcmd->param[ac]; 
    }
  argv[ac + 1] = NULL;

  if (!vm_testscript(2, argv))
    ELFSH_SETERROR("[vm:cmd_source] Invalid script file", -1);

  /* Add argument variables */
  for (idx = 1; argv[idx + 1]; idx++)
    {
      snprintf(actual, sizeof(actual), "%u", idx);
      new = vm_create_IMMEDSTR(1, strdup(argv[idx + 1]));
      hash_del(&vars_hash, actual);
      hash_add(&vars_hash, strdup(actual), new);
    }

  new = vm_create_IMMED(ELFSH_OBJINT, 1, idx);
  hash_del(&vars_hash, ELFSH_ARGVAR);
  hash_add(&vars_hash, ELFSH_ARGVAR, new);

  XOPEN(fd, str, O_RDONLY, 0, -1);

  world.curjob->sourced++;
  hash_init(&labels_hash[world.curjob->sourced], 251);

  saved     = world.curjob->io.input_fd;
  savedinput= world.curjob->io.input;
  savedmode = world.state.vm_mode;
  savedcmd  = world.curjob->curcmd;

  /* Temporary takes input from the script file */
  vm_setinput(world.curjob, fd);
  world.curjob->io.input_fd = fd;
  world.state.vm_mode = ELFSH_VMSTATE_SCRIPT;
  world.curjob->io.input = vm_stdinput;

  /* A simple dedicated interpretation loop on the script file */
  do {

    av = vm_input(&ac);
    if (av == ((char **) ELFSH_VOID_INPUT))
      continue;
    else if (!av)
      break;

    if (vm_parseopt(ac, av) < 0)
      {
	free(av);
	return (-1);
      }

    if (!world.curjob->curcmd                           || 
	(world.curjob->curcmd->name                     &&
	 (!strcmp(world.curjob->curcmd->name, CMD_QUIT) ||
	  !strcmp(world.curjob->curcmd->name, CMD_QUIT2))))
      break;

  } while (1);
  
  /* We execute the parsed script and restore the original context */
  world.curjob->curcmd      = world.curjob->script[world.curjob->sourced];
  vm_execscript();
  world.curjob->io.input_fd = saved;
  world.state.vm_mode       = savedmode;
  world.curjob->curcmd      = savedcmd;
  world.curjob->io.input    = savedinput;
  snprintf(buf, BUFSIZ, "\n [*] %s sourcing -OK- \n\n", str);
  vm_output(buf);
  
  world.curjob->script[world.curjob->sourced] = NULL;
  hash_destroy(&labels_hash[world.curjob->sourced]);
  world.curjob->sourced--;

  /* Del argument variables */
  for (idx = 1; argv[idx + 1]; idx++)
    {
      snprintf(actual, sizeof(actual), "%u", idx);
      hash_del(&vars_hash, actual);
    }

  hash_del(&vars_hash, ELFSH_ARGVAR);

  return (0);
}

/* Execute a command given by a string in the current job */
int	vm_exec_str(char *str)
{
  int	nbr;
  char  **av;
  int	ac;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  nbr = vm_findblanks(str);
  av = vm_doargv(nbr, &ac, str);

  if (vm_parseopt(ac, av) < 0)
    {
      free(av);
      return (-1);
    }
  
  return (0);
}



/* Script used as command function */
int	cmd_script()
{
  int                   idx;
  char                  data[BUFSIZ];
  u_int                 sz = BUFSIZ - 1;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.scriptsdir == NULL)
    ELFSH_SETERROR("[vm:cmd_script] No scripts dir specified", -1);

  snprintf(data, BUFSIZ - 1, "source %s/%s.esh",  world.scriptsdir, world.curjob->curcmd->name);

  for (idx = 0; world.curjob->curcmd->param[idx] != NULL; idx++)
    {
      strncat(data, " ", sz - 1);
      sz -= 1;
      if (sz < 2)
        break;
      strncat(data, world.curjob->curcmd->param[idx], sz - 1);
      sz -= strlen(world.curjob->curcmd->param[idx]);
      if (sz < 2)
        break;
    }
  
  vm_exec_str(data);

  return (0);
}


/* Set then scriptsdir directory and add cmd corresponding to scripts */
int	vm_add_script_cmd(char *dir_name)
{
  DIR	*dir;
  struct dirent *dir_entry;
  char	cmd_name[BUFSIZ];
  int	len;
  char  *argv[2];
  char	str[BUFSIZ];
  hashent_t     *actual;
  int           index;
  int	cnt = 0;


  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  argv[0] = "#!elfsh";

  dir = opendir(dir_name);
  
  if (dir == NULL)
    {
      perror("opendir");
      return (-1);
    }

  /* scripts dir already specified so cleanup command set */
  if (world.scriptsdir != NULL)
    {
      for (index = 0; index < cmd_hash.size; index++)
	for (actual = &cmd_hash.ent[index];
	     actual != NULL && actual->key != NULL;
	     actual = actual->next)
	  {
	    if (((elfshcmd_t *) actual->data)->exec == cmd_script)
	      {
		free(actual->data);
		hash_del(&cmd_hash, actual->key);
	      }
	  }
    }

  
  while( (dir_entry = readdir(dir)))
    {
      if ((len = strlen(dir_entry->d_name)) > 4)
	{
	  if (!strncmp(".esh", &(dir_entry->d_name[len - 1 - 3]), 4))
	    {
	      snprintf(str, BUFSIZ - 1, "%s/%s", dir_name, dir_entry->d_name);
	      argv[1] = str;
	      if (!vm_testscript(2, argv))
		continue;
	      
	      snprintf(cmd_name, BUFSIZ - 1, "%s", dir_entry->d_name);
	      cmd_name[len - 1 - 3] = '\0';
	      vm_addcmd(strdup(cmd_name), (void *) cmd_script, (void *) vm_getvarparams, 0);
	      snprintf(cmd_name, BUFSIZ - 1, "\t\t+ %s added\n", dir_entry->d_name);
	      vm_output(cmd_name);
	      cnt++;
	    }
	}
    }
      

  if (world.scriptsdir != NULL)
    {
      free(world.scriptsdir);
      world.scriptsdir = NULL;
    }
  if (cnt)
    world.scriptsdir = strdup(dir_name);
  else 
    vm_output("\t\t!! No scripts found !!\n");

  closedir(dir);
  return (0);
}


/* Elfsh/E2dbg command for vm_add_script_cmd() */
int	cmd_scriptsdir()
{
  char	str[BUFSIZ];
#if defined(USE_READLN)
  int   cmdnum;
#endif


  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.curjob->curcmd->param[0] == NULL)
      return (-1);

  snprintf(str, BUFSIZ - 1, "\t [Adding script commands from %s] \n", world.curjob->curcmd->param[0]);
  vm_output(str);
  
  if (vm_add_script_cmd(world.curjob->curcmd->param[0]) < 0)
    {
      vm_output("\t failed\n\n");
      return (-1);
    }
  else
    {
#if defined(USE_READLN)
      world.cmds = hash_get_keys(&cmd_hash, &cmdnum);
#endif
      vm_output("\t done\n\n");
    }

  return (0);
}

int	cmd_lscripts()
{
  char	str[BUFSIZ];
  hashent_t     *actual;
  int           index;
  int		cnt = 1;


  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* scripts dir already specified so cleanup command set */
  if (world.scriptsdir != NULL)
    {
      vm_output(" .::. Script commands .::.\n");

      for (index = 0; index < cmd_hash.size; index++)
	for (actual = &cmd_hash.ent[index];
	     actual != NULL && actual->key != NULL;
	     actual = actual->next)
	  {
	    if (((elfshcmd_t *) actual->data)->exec == cmd_script)
	      {
		snprintf(str, BUFSIZ - 1, " [%.3d] %s\n", cnt, actual->key);
		vm_output(str);
		cnt++;

	      }
	  }
      if (cnt == 1)
	vm_output("[*] No script commands\n");
      vm_output("\n");
    }  
  else
    vm_output(" [!!] No script directory specified, use sdir command.\n\n");

  return (0);
}
