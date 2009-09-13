/**
* @file libstderesi/cmd/script.c
 * @ingroup cmd 
 * Started on  Fri May 16 15:18:35 2005 jfv
 *
 * 
 *
 * $Id$
 *
 */
#include "libstderesi.h"


/**
 * Script used as command function.
 * Note : this function does not work properly in a script ! 
*/
int			cmd_script()
{
  int                   idx;
  char                  *backup;
  char			*next;
  char			*path;
  int			size;
  revmargv_t		*cmd;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (world.scriptsdir == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No scripts dir specified", -1);
  cmd = world.curjob->curcmd;
  
  /* It can happens that the command bounce to cmd_source has already been set up */
  if (!strcmp(world.curjob->curcmd->name, CMD_SOURCE))
    goto end;

  /* Setup the command bouncing */
  size = strlen(world.scriptsdir) + strlen(world.curjob->curcmd->name) + 6;
  path = alloca(size);
  snprintf(path, size, "%s/%s.esh", world.scriptsdir, world.curjob->curcmd->name);
  backup = world.curjob->curcmd->param[0];
  world.curjob->curcmd->param[0] = strdup(path);
  for (idx = 0; idx < REVM_MAXARGC - 1 && backup; idx++)
    {
      next = world.curjob->curcmd->param[idx + 1];
      world.curjob->curcmd->param[idx + 1] = backup;
      backup = next;
    }  
  world.curjob->curcmd->name  = CMD_SOURCE;

 end:
  if (cmd_source() < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid ERESI source program", (-1));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * @brief Set then scriptsdir directory and add cmd corresponding to scripts 
 * @param dir_name Directory path where scripts are located.
 * @return Success (0) or Error (-1).
 */
int		revm_add_script_cmd(char *dir_name)
{
  DIR		*dir;
  struct dirent *dir_entry;
  char		cmd_name[BUFSIZ];
  int		len;
  char		*argv[2];
  char		str[BUFSIZ];
  revmcmd_t	*curcmd;
  int           index;
  int		cnt = 0;
  char		**keys;
  int		keynbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  argv[0] = "#!elfsh";
  dir = opendir(dir_name);
  if (dir == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Opendir failed", (-1));
  
  /* scripts dir already specified so cleanup command set */
  if (world.scriptsdir != NULL)
    {
      keys = hash_get_keys(&cmd_hash, &keynbr);
      for (index = 0; index < keynbr; index++)
	{
	  curcmd = hash_get(&cmd_hash, keys[index]);
	  if (curcmd->exec == cmd_script)
	    hash_del(&cmd_hash, keys[index]);
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
	      if (!revm_testscript(2, argv))
		continue;
	      
	      snprintf(cmd_name, BUFSIZ - 1, "%s", dir_entry->d_name);
	      cmd_name[len - 1 - 3] = '\0';
	      revm_command_add(strdup(cmd_name), (void *) cmd_script, 
			       (void *) revm_getvarparams, 0, "Synthetic macro command");
	      if (!world.state.revm_quiet)
		{
		  snprintf(cmd_name, BUFSIZ - 1, "\t\t+ %s added\n", 
			   dir_entry->d_name);
		  revm_output(cmd_name);
		}
	      cnt++;
	    }
	}
    }
      
  if (world.scriptsdir != NULL)
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__,world.scriptsdir);
      world.scriptsdir = NULL;
    }
  if (cnt)
    world.scriptsdir = strdup(dir_name);
 
  else 
    revm_output("\t\t [!] No scripts found \n");

  closedir(dir);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * Elfsh/E2dbg command for revm_add_script_cmd() 
 */
int	cmd_scriptsdir()
{
  char	str[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.curjob->curcmd->param[0] == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", (-1));
      
  if (!world.state.revm_quiet)
    {
      snprintf(str, BUFSIZ - 1, "\t [Adding script commands from %s] \n", 
	       world.curjob->curcmd->param[0]);
      revm_output(str);
    }
  
  if (revm_add_script_cmd(world.curjob->curcmd->param[0]) < 0)
    {
      revm_output("\t failed\n\n");
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Macro command adding failed", (-1));
    }
  else
    {
      revm_completion_commands(&cmd_hash);
      if (!world.state.revm_quiet)
	revm_output("\t done\n\n");
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Need doxygen tag
 */
int		cmd_lscripts()
{
  char		str[BUFSIZ];
  listent_t     *actual;
  int           index;
  int		cnt = 1;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* scripts dir already specified so cleanup command set */
  if (world.scriptsdir != NULL)
    {
      revm_output(" .::. Script commands .::.\n");

      for (index = 0; index < cmd_hash.size; index++)
	for (actual = &cmd_hash.ent[index];
	     actual != NULL && actual->key != NULL;
	     actual = actual->next)
	  {
	    if (((revmcmd_t *) actual->data)->exec == cmd_script)
	      {
		snprintf(str, BUFSIZ - 1, " [%.3d] %s\n", cnt, actual->key);
		revm_output(str);
		cnt++;

	      }
	  }
      if (cnt == 1)
	revm_output("[*] No script commands\n");
      revm_output("\n");
    }  
  else
    revm_output(" [!!] No script directory specified, use sdir command.\n\n");

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
