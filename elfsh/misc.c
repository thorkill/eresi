/*
** misc.c for elfsh
** 
** Started on  Fri Nov  2 15:21:56 2001 mayhem
** Updated on  Fri Sep 11 17:26:11 2005 mayhem
*/
#include "elfsh.h"

#if defined(USE_READLN)
 rl_command_func_t *rl_ctrll = NULL;
#endif

/* return the right prompt */
char	*vm_get_prompt()
{
  if (world.state.vm_mode == ELFSH_VMSTATE_IMODE ||
      world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER) 
    {
      snprintf(prompt_token, sizeof(prompt_token), 
	       "%s%s%s%s%s%s%s%s%s ",
	       vm_colorget("%s", "pspecial", "("),
	       (world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER ?
		vm_colorget("%s", "psname" , E2DBG_ARGV0)    :
		vm_colorget("%s", "psname" , ELFSH_SNAME)),
	       vm_colorget("%s", "pspecial", "-"),
	       vm_colorget("%s", "pversion", ELFSH_VERSION),
	       vm_colorget("%s", "pspecial", "-"),
	       vm_colorget("%s", "prelease", ELFSH_RELEASE),
	       vm_colorget("%s", "pspecial", "-"),
	       vm_colorget("%s", "pedition", ELFSH_EDITION),
	       vm_colorget("%s", "pspecial", ")"));
      vm_endline();
      return prompt_token;
    }

  if (world.state.vm_mode == ELFSH_VMSTATE_SCRIPT)
    return "";

  return "UNKNOWN MODE> ";
}

/* return the project name accordingly to mode */
char		*vm_get_mode_name()
{
  char          *mode;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER)
    mode = E2DBG_NAME;
  else
    mode = ELFSH_NAME;
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (mode));
}


/* Our system implementation */
int		vm_system(char *cmd)
{
  char		buf[BUFSIZ];
  int		ret;
  char		**av;
  int		nbr;
  int		argc;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.curjob->io.type == ELFSH_IONET)
    snprintf(buf, BUFSIZ, "%s <&%u >&0 2>&0 ", cmd, world.curjob->io.output_fd); 
  else
    snprintf(buf, BUFSIZ, "%s ", cmd); 
  
  /* If the user shell is unspecified we use system */
  nbr = vm_findblanks(cmd);
  av = vm_doargv(nbr, &argc, cmd);
  av++;
  if (!fork())
    ret = execvp(av[0], av);
  else
    wait(NULL);

  /* Report result */
  if (ret < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Shell not found", 0);	
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}


/* Decide what to do for exiting depending on the current input */
void	vm_exit(int err)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  exit(err);
  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Bad parameter handler */
void	vm_badparam(char *str)
{
  char	buf[BUFSIZ];
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(buf, BUFSIZ, 
	   "\n [!] Invalid parameters for command %s .::. "
	   "type 'help' for command list \n\n", str);
  vm_output(buf);

  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Unknow command handler */
void	vm_unknown(char *str)
{
  char	buf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(buf, BUFSIZ, "\n [!] Unknown command %s .::. "
	   "type 'help' for command list \n\n", str);
  vm_output(buf);
  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Generic error message handler */

void	vm_error(char *label, char *param)
{
  char	buf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(buf, BUFSIZ, " [!] %s [%s] \n\n", label, param);
  vm_output(buf);
  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/* Open the script file */
int		vm_openscript(char **av)
{
  int		fd;
  int		idx;
  char		actual[16];
  elfshpath_t	*new;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Open script file */
  XOPEN(fd, av[0], O_RDONLY, 0, -1);
  
  world.curjob->io.input_fd = fd;

  /* Add argument variables */
  for (idx = 1; av[idx]; idx++)
    {
      snprintf(actual, sizeof(actual), "%u", idx);
      new = vm_create_IMMEDSTR(1, strdup(av[idx]));
      hash_add(&vars_hash, strdup(actual), new);
    }

  new = vm_create_IMMED(ELFSH_OBJINT, 1, idx);
  hash_add(&vars_hash, ELFSH_ARGVAR, new);
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Say if we are in script mode */
int		vm_testscript(int ac, char **av)
{
  int		fd;
  char		buff[30];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (ac < 2 || av[1][0] == ELFSH_MINUS)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  XOPEN(fd, av[1], O_RDONLY, 0, 0);
  XREAD(fd, buff, 30, 0);
  buff[29] = 0x00;
  if (strncmp(buff, "#!", 2))
    {
      XCLOSE(fd, 0);
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Invalid script interpreter", 0);
    }
  if (!strstr(buff, av[0]) && !strstr(buff, "elfsh"))
    {
      XCLOSE(fd, 0);
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Not an Elfsh script", 0);
    }
  XCLOSE(fd, 0);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}

/* Print the banner */
void		vm_print_banner()
{
  char		logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(logbuf, BUFSIZ - 1,
	   "\n\n\t The %s %s (%s) .::. \n\n %s", 
	   vm_get_mode_name(), 
	   ELFSH_VERSION,
#if defined(ELFSH32)
	   "32 bits built",
#elif defined(ELFSH64)
	   "64 bits built",
#else
	   "Unknown built",
#endif
	   "\t .::. This software is under the General Public License V.2 \n"
	   "\t .::. Please visit http://www.gnu.org \n\n");
  vm_output(logbuf);
  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Print the Unknown buffer */
char		*vm_build_unknown(char *buf, const char *str, u_long type)
{

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(buf, ELFSH_MEANING, "%s %08X", str, (u_int) type);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (buf));
}


/* Retreive a file object giving its unique ID */
elfshobj_t	*vm_getfile(u_int index)
{
  elfshobj_t	*cur;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  for (cur = world.curjob->list; cur; cur = cur->next)
    if (cur->id == index)
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (cur));
  for (cur = world.shared; cur; cur = cur->next)
    if (cur->id == index)
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (cur));
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to find file", (NULL));
}

/* Retreive a module object giving its unique ID */
elfshmod_t	*vm_getmod(u_int index)
{
  elfshmod_t	*cur;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  for (cur = world.modlist; cur; cur = cur->next)
    if (cur->id == index)
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (cur));
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to find module", (NULL));
}

/* Print error depending on the state of the machine */
int		vm_doerror(void (*fct)(char *str), char *str)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.state.vm_mode != ELFSH_VMSTATE_CMDLINE || world.state.vm_net)
    fct(str);
  else
    {
      cmd_help();
      exit(-1);
    }
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Bypassed error printing", (-1));
}

/* Mark the current object SHT to be removed on savnig */
int             cmd_shtrm()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  world.curjob->current->shtrm = 1;
  if (!world.state.vm_quiet)
    vm_output(" [*] SHT marked as removed, you need to save\n\n");
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Mark the file as stripped */
int		cmd_strip()
{

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  world.curjob->current->strip = 1;
  if (!world.state.vm_quiet)
    vm_output(" [*] Object marked as stripped, you need to save\n\n");
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Mark SHT as removed and file as stripped */
int		cmd_sstrip()
{

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  world.curjob->current->strip = 1;
  world.curjob->current->shtrm = 1;
  if (!world.state.vm_quiet)
    vm_output(" [*] Object marked as sstripped, you need to save\n\n");
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Change the VM state as QUIET */
int		cmd_quiet()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  world.state.vm_quiet = 1;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Run network initialisation */
int		cmd_network()
{
  elfshjob_t	*job;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.state.vm_net == 1)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  XALLOC(job, sizeof (elfshjob_t), -1);
  
  /* network job setup */
  job->io.type   = ELFSH_IONET;
  job->io.input  = vm_net_input;
  job->io.output = vm_net_output;
  job->createtime = time(&job->createtime);

  hash_add(&world.jobs, "net_init", job);
  //world.curjob = job;
  world.state.vm_net = 1;

  if (vm_net_init() < 0)
    fprintf (stderr,"vm_initio : Initialisation of network : Failed \n");
  else
    {
      if (world.state.vm_mode == ELFSH_VMSTATE_CMDLINE)
	{
          // We had interactive mode commands which aren't load in CMDLINE mode
          vm_addcmd(CMD_LOAD    , (void *) cmd_load     , (void *) vm_getoption, 0, HLP_LOAD);
          vm_addcmd(CMD_UNLOAD  , (void *) cmd_unload   , (void *) vm_getoption, 0, HLP_UNLOAD);
          vm_addcmd(CMD_SAVE    , (void *) cmd_save     , (void *) vm_getoption, 1, HLP_SAVE);
          vm_addcmd(CMD_SWITCH  , (void *) cmd_doswitch , (void *) vm_getoption, 1, HLP_SWITCH);
          vm_addcmd(CMD_METACMD , (void *) cmd_meta     , (void *) NULL, 0, HLP_METACMD);
          vm_addcmd(CMD_QUIT    , (void *) cmd_quit     , (void *) NULL, 0, HLP_QUIT);
          vm_addcmd(CMD_QUIT2   , (void *) cmd_quit     , (void *) NULL, 0, HLP_QUIT);
          vm_addcmd(CMD_LIST    , (void *) cmd_dolist   , (void *) NULL, 0, HLP_LIST);
          vm_addcmd(CMD_LIST2   , (void *) cmd_dolist   , (void *) NULL, 0, HLP_LIST);
          vm_addcmd(CMD_STOP    , (void *) cmd_stop     , (void *) NULL, 0, HLP_STOP);

          // NEED A SWITCH
	}
      vm_output(" [*] Started ELFsh network connection \n\n");
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Change the VM state as VERBOSE (default) */
int		cmd_verb()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  vm_output(" [*] Switched to verbose mode \n");
  world.state.vm_quiet = 0;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Useful when you have only one terminal */
int		cmd_meta()
{
  int		ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!world.state.vm_quiet)
    vm_output(" [*] You are still in elfsh, exit bash "
	      "to get back to the regular prompt \n\n");
#if defined(USE_READLN)
  if (world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER ||
      world.state.vm_mode == ELFSH_VMSTATE_IMODE)
    rl_deprep_terminal();
#endif
  
  ret = vm_system(vm_lookup_var("$SHELL"));
  
#if defined(USE_READLN)
  if (world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER ||
      world.state.vm_mode == ELFSH_VMSTATE_IMODE)
    rl_prep_terminal(1);
#endif 

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}



/* Edit a file */
int             cmd_edit()
{
  int           ret;
  char		buff[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(USE_READLN)
  if (world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER ||
      world.state.vm_mode == ELFSH_VMSTATE_IMODE)
    rl_deprep_terminal();
#endif

  snprintf(buff, BUFSIZ, "%s %s", 
	   vm_lookup_var("$EDITOR"),
	   world.curjob->curcmd->param[0]);

  ret = vm_system(buff);

#if defined(USE_READLN)
  if (world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER ||
      world.state.vm_mode == ELFSH_VMSTATE_IMODE)
    rl_prep_terminal(1);
#endif
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}






/* Useful for debugging */
int	cmd_stop()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (raise(SIGSTOP)));
}

/* Useful to differentiate 0 and a string */
int	vm_isnbr(char *string)
{
  size_t len = strlen(string);
  size_t ii;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  for (ii=0; ii < len; ii++)
    if (!isdigit((int) string[ii]))
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}



/* Precise a general weak bounded regex for all options */
int		cmd_glregx()
{
  char		*str;
  char		logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  str = strdup(world.curjob->curcmd->param[0]);
  if (regcomp(&world.state.vm_regx, str, REG_EXTENDED) < 0)	
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Failed to compute regex", -1);
  world.state.vm_use_regx = 1;
  if (world.state.vm_sregx)
    XFREE(world.state.vm_sregx);
  world.state.vm_sregx = str;
  if (!world.state.vm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1, " [*] Changed global regex to %s \n\n", str);
      vm_output(logbuf);
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}				


/* Read the sorting parameter */
int		cmd_sort()
{
  char		logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.curjob->curcmd->param[0][0] != ELFSH_SORT_BY_ADDR && 
      world.curjob->curcmd->param[0][0] != ELFSH_SORT_BY_SIZE)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid sorting type", -1);
  if (!world.state.vm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1, " [*] Switched to %s sorting\n\n", 
	       (world.curjob->curcmd->param[0][0] == ELFSH_SORT_BY_ADDR ?
		"ADDR" : "SIZE"));
      vm_output(logbuf);
    }
  world.state.sort = world.curjob->curcmd->param[0];
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Precise a general weak bounded regex for all options */
int		cmd_alert()
{
  char		*str;
  char		logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  str = strdup(world.curjob->curcmd->param[0]);
  if (regcomp(&world.state.vm_alert, str, REG_EXTENDED) < 0)	
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Failed to compute regex", -1);
  world.state.vm_use_alert = 1;
  if (world.state.vm_salert)
    XFREE(world.state.vm_salert);
  world.state.vm_salert = str;
  if (!world.state.vm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1, " [*] Changed alert regex to %s \n\n", str);
      vm_output(logbuf);
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}				

/* Set the share flag */
int		cmd_shared()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  world.state.vm_shared = 1;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* print the list of net-client */
int		cmd_netlist()
{
  int		i = 0;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(ELFSHNET)
  hashent_t	*actual;
  int		index;
  char		buf[BUFSIZ];
  char		*addr;

  vm_output(" .::. Net client list .::. \n");
  for (index = 0; index < world.jobs.size; index++)
    {
      for (actual = &world.jobs.ent[index];
	   actual != NULL && actual->key != NULL;
	   actual = actual->next)
        {
	  if (!((elfshjob_t *) actual->data)->active)
	    continue;

	  if (((elfshjob_t *) actual->data)->io.type == ELFSH_IONET)
            {
	      addr = inet_ntoa(((elfshjob_t *) actual->data)->sock.addr.sin_addr);
	      snprintf(buf, BUFSIZ - 1, " [%.3d] IP %s\n", i, addr);
	      vm_output(buf);
	      i++;
            }
        }
    }
#endif

  if (i == 0)
    vm_output(" [*] No client\n");
    
  vm_output("\n");
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* kill a client by its ip address */
int		cmd_netkill()
{
  char		*tokill;
  char          buf[BUFSIZ];
    
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  tokill = world.curjob->curcmd->param[0];
  
  tokill = vm_lookup_var(tokill);

  if (world.curjob->io.type == ELFSH_IONET)
    {
      vm_output(" [*] You can't kill connection.\n");
      vm_output("\n");
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  
  if (vm_socket_del(tokill) < 0)
    {
#if __DEBUG_NETWORK__
      fprintf(stderr, "[DEBUG NETWORK] error on killing a client : %s\n",tokill);
#endif
      snprintf(buf, BUFSIZ - 1, " [*] %s not connected.\n", tokill);
      vm_output(buf);
      vm_output("\n");
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  snprintf(buf, BUFSIZ - 1, " [*] %s's connection closed.\n", tokill);
  vm_output(buf);
  vm_output("\n");
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0); 
}


/* print the list of dump neighbors */
int		cmd_peerslist()
{
  int           i = 0;

#if defined(ELFSHNET)
  hashent_t     *actual;
  int           index;
  char          buf[BUFSIZ];
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  vm_output(" .::. DUMP neighbors list .::. \n");
  for (index = 0; index < dump_world.ports.size; index++)
    {
      for (actual = &dump_world.ports.ent[index];
	   actual != NULL && actual->key != NULL;
	   actual = actual->next)
	{
	  snprintf(buf, BUFSIZ - 1, " [%.3d] IP %s\n", i, actual->key);
	  vm_output(buf);
	  i++;
	}
    }
#else 
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
#endif
    
  if (i == 0)
    vm_output(" [*] No connection\n");
  
  vm_output("\n");
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* connect to a dump node */
int		cmd_connect()
{
#if defined(ELFSHNET) 

  char          *toconnect;
  char          buf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
 
  toconnect = world.curjob->curcmd->param[0];

  toconnect = vm_lookup_var(toconnect);

  if (world.state.vm_net != 1)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  
  if (world.curjob->io.type == ELFSH_IONET)
    {
      vm_output(" [*] You can't add DUMP connection.\n");
      vm_output("\n");
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  
  if (dump_connect_to(toconnect, ELFSH_DUMP_PORT) < 0)
    {
      snprintf(buf, BUFSIZ - 1, " [*] connection to %s failed.\n", toconnect);
      vm_output(buf);
      vm_output("\n");
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  
  snprintf(buf, BUFSIZ - 1, " [*] connection to %s established.\n", toconnect);
  vm_output(buf);
  vm_output("\n");
#else
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
#endif

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* disconnect from a dump node */
int		cmd_discon()
{
#if defined(ELFSHNET) 

  char          *todisconnect;
  char          buf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  todisconnect = world.curjob->curcmd->param[0];

  todisconnect = vm_lookup_var(todisconnect);

  if (world.state.vm_net != 1)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  
  if (world.curjob->io.type == ELFSH_IONET)
    {
      vm_output(" [*] You can't remove DUMP connection.\n");
      vm_output("\n");
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  
  if (dump_disconnect_host(todisconnect) < 0)
    {
      snprintf(buf, BUFSIZ - 1, " [*] Not connected to %s.\n", todisconnect);
      vm_output(buf);
      vm_output("\n");
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  
  snprintf(buf, BUFSIZ - 1, " [*] connection to %s closed.\n", todisconnect);
  vm_output(buf);
  vm_output("\n");
#else 
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
#endif
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* send a command to a remote elfsh node */
int			cmd_rcmd()
{
#if defined(ELFSHNET)

  int			idx;
  char			*to;
  char			buf[BUFSIZ];
  struct hostent        *h;
  struct sockaddr_in    serv_addr;
  char			data[BUFSIZ];
  u_int			sz = BUFSIZ - 1;
  int			ret;
   
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.state.vm_net != 1)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Not in 'net' mode", (-1));


  /* 1 char for the ELFSH_DUMP_CMD */
  data[0] = ' ';
  data[1] = '\0';

  sz -= 2;
  
  for (idx = 1; world.curjob->curcmd->param[idx] != NULL; idx++)
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

  if (idx < 2)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Invalid number of parameters", (-1));

  to = world.curjob->curcmd->param[0];

  to = vm_lookup_var(to);

  h = gethostbyname(to);

  if (h == NULL)
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Unknown host", (-1));

  memcpy((char *) &serv_addr.sin_addr.s_addr,
	 h->h_addr_list[0],
	 h->h_length);

  snprintf(buf, BUFSIZ - 1, " [*] send command to %s : %s.\n", to, data);

  vm_output(buf);
  vm_output("\n");

  data[0] = ELFSH_DUMP_CMD;

  ret = dump_send(serv_addr.sin_addr, strdup(data), 1 + strlen(data + 1) + 1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
#else 
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
#endif

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Enable or Disable the profiler */
int		cmd_profile()
{
  switch (elfsh_is_prof_enable())
    {
    case ELFSH_NOPROF:
      elfsh_prof_enable_err();
      vm_output("\t .:: Profiling enable (err)\n\n");
      break;
    case ELFSH_ERRPROF:
      elfsh_prof_enable_out();
      vm_output("\t .:: Profiling enable (out)\n\n");
      break;
    case ELFSH_OUTPROF:
      elfsh_prof_disable();
      vm_output("\t .:: Profiling disable\n\n");
      break;
    }

  return 0;
}


/* Change the shell variable */
int		vm_setshell(char *str)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (vm_setvar_str(ELFSH_SHELLVAR, str) < 0)
   ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Cannot modify shell var", -1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* The internal basename function */
char		*vm_basename(char *str)
{
  char		*cur;
  char		*ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = NULL;
  cur = str;
  while ((cur = strchr(cur, '/')))
    if (!*(cur + 1))
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "No basename", (NULL));
    else
      ret = ++cur;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}



/* Simple internal cat builtin */
int		cmd_cat()
{
  char		*tocat;
  char          buf[BUFSIZ];
  int		fd;
  int		len = 0;
  int		tmplen = 0;
    
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  tocat = vm_lookup_var(world.curjob->curcmd->param[0]);
  if ((fd = open(tocat, O_RDONLY, 0)) < 0)          
    {
      vm_output("Can't open file\n");
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Open failed", (-1));
    }

  while (1)
    {
      tmplen = read(fd, buf + len, 1);  
      if (tmplen < 0)
	{
	  XCLOSE(fd,0);
	  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Read failed", (-1));
	}
      if (tmplen == 0)
	break;
      len += tmplen;
      if (len > BUFSIZ - 1)
	{
	  XCLOSE(fd, 0);
	  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			    "File too long", (-1));
	}
    }

  buf[len] = '\0';
  vm_output(buf);
  vm_output("\n");

  XCLOSE(fd, 0);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



int		vm_clearscreen(int i, char c)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  XFREE(world.curjob->screen.buf);
  world.curjob->screen.head = world.curjob->screen.tail = world.curjob->screen.buf = NULL;
  
#if defined(USE_READLN)
  if (rl_ctrll)
    rl_ctrll(i, c);
  world.curjob->io.buf = NULL;
#endif

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


int		vm_install_clearscreen()
{
#if defined(USE_READLN)
  Keymap	    map;
#endif
  char		    keyseq[2];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(USE_READLN)
  map = rl_get_keymap();
  keyseq[0] = CTRL('l');
  keyseq[1] = '\0';
  rl_ctrll = rl_function_of_keyseq(keyseq, map, NULL);
  rl_bind_key(CTRL('l'), (rl_command_func_t *) vm_clearscreen); 
#endif

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



#ifdef __DEBUG_TEST__
int		cmd_test()
{
  char		    buf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!cmd_next_workspace())
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

#if defined(USE_READLN)
  world.curjob->io.buf = NULL;
#endif

  vm_flush();

  /* clear screen */
  vm_output_nolog("\033[2J");
  vm_output_nolog("\033[0;0H");

  /*
  if (rl_ctrll)
    rl_ctrll(1, (int) CTRL('l'));
  */
  //world.curjob->io.output(world.curjob->screen.buf);
  
  //printf("head : %x tail %x\n", world.curjob->screen.head, world.curjob->screen.tail);

#if defined(USE_READLN)
  if (world.curjob->screen.buf == NULL)
    {
      vm_output_nolog(vm_get_prompt());
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  if (world.curjob->screen.head < world.curjob->screen.tail)
    {
      //  printf("head < tail\n"); 
      snprintf(buf, world.curjob->screen.tail - world.curjob->screen.head, 
	       "%s", world.curjob->screen.head);
    }
  else 
    {
      //printf("head => tail\n");
      snprintf(buf, world.curjob->screen.x * world.curjob->screen.y 
	       - (world.curjob->screen.head - world.curjob->screen.buf), 
	       "%s", world.curjob->screen.head);
      strcat(buf, world.curjob->screen.buf);
    }
#endif  

  vm_output_nolog(buf);
  vm_output_nolog(" ");


  /*
  {
    char buf[256];
    snprintf(buf, 256, "\033[00;01;35m%#x\033[00m", world.curjob);
    vm_output_nolog(buf);
  }
  */

  //  vm_output_nolog(" ");

  //rl_forced_update_display(); 

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
#endif
