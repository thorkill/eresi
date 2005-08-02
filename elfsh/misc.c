/*
** misc.c for elfsh
** 
** Started on  Fri Nov  2 15:21:56 2001 mayhem
** Updated on  Fri Mar 11 18:47:41 2005 ym
*/
#include "elfsh.h"



/* return the right prompt */
char	*vm_get_prompt()
{
  if (world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER)
    return E2DBG_PROMPT;
  
  if (world.state.vm_mode == ELFSH_VMSTATE_IMODE)
    return ELFSH_PROMPT;

  if (world.state.vm_mode == ELFSH_VMSTATE_SCRIPT)
    return "";

  return "UNKNOWN MODE> ";
}

/* return the project name accordingly to mode */
char	*vm_get_mode_name()
{
  char          *mode;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER)
    mode = E2DBG_NAME;
  else
    mode = ELFSH_NAME;
  return mode;
}


/* Our system implementation */
int	vm_system(char *cmd)
{
  char	buf[BUFSIZ];
  
  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.curjob->io.type == ELFSH_IONET)
    snprintf(buf, BUFSIZ, "%s <&%u >&0 2>&0 ", cmd, world.curjob->io.output_fd); 
  else
    snprintf(buf, BUFSIZ, "%s ", cmd); 
        
  return (system(buf));
}


/* Decide what to do for exiting depending on the current input */
void	vm_exit(int err)
{
  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  exit(err);
}

/* Bad parameter handler */
void	vm_badparam(char *str)
{
  char	buf[BUFSIZ];
  
  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(buf, BUFSIZ, 
	   "\n [!] Invalid parameters for command %s .::. "
	   "type 'help' for command list \n\n", str);
  vm_output(buf);
}

/* Unknow command handler */
void	vm_unknown(char *str)
{
  char	buf[BUFSIZ];

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(buf, BUFSIZ, "\n [!] Unknown command %s .::. "
	   "type 'help' for command list \n\n", str);
  vm_output(buf);
}

/* Generic error message handler */
void	vm_error(char *label, char *param)
{
  char	buf[BUFSIZ];

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(buf, BUFSIZ, " [!] %s [%s] \n\n", label, param);
  vm_output(buf);
}


/* Open the script file */
int		vm_openscript(char **av)
{
  int		fd;
  int		idx;
  char		actual[16];
  elfshpath_t	*new;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Open script file */
  XOPEN(fd, av[0], O_RDONLY, 0, -1);

  if (dup2(fd, 0) < 0)
    return (-1);

    
  /* Add argument variables */
  for (idx = 1; av[idx]; idx++)
    {
      snprintf(actual, sizeof(actual), "%u", idx);
      new = vm_create_IMMEDSTR(1, strdup(av[idx]));
      hash_add(&vars_hash, strdup(actual), new);
    }

  new = vm_create_IMMED(ELFSH_OBJINT, 1, idx);
  hash_add(&vars_hash, ELFSH_ARGVAR, new);
  
  return (0);
}

/* Say if we are in script mode */
int		vm_testscript(int ac, char **av)
{
  int		fd;
  char		buff[30];

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (ac < 2 || av[1][0] == ELFSH_MINUS)
    return (0);
  XOPEN(fd, av[1], O_RDONLY, 0, 0);
  XREAD(fd, buff, 30, 0);
  buff[29] = 0x00;
  if (strncmp(buff, "#!", 2))
    {
      XCLOSE(fd, 0);
      return (0);
    }
  if (!strstr(buff, av[0]) && !strstr(buff, "elfsh"))
    {
      XCLOSE(fd, 0);
      return (0);
    }
  XCLOSE(fd, 0);
  return (1);
}

/* Print the banner */
void		vm_print_banner()
{
  char		logbuf[BUFSIZ];

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

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
	   "\t .::. Please visit http://www.gnu.org for information about "
	   "Free Software\n\n");
  vm_output(logbuf);
}

/* Print the Unknown buffer */
char		*vm_build_unknown(char *buf, const char *str, u_long type)
{

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(buf, ELFSH_MEANING, "%s %08X", str, (u_int) type);
  return (buf);
}


/* Retreive a file object giving its unique ID */
elfshobj_t	*vm_getfile(u_int index)
{
  elfshobj_t	*cur;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  for (cur = world.curjob->list; cur; cur = cur->next)
    if (cur->id == index)
      return (cur);
  for (cur = world.shared; cur; cur = cur->next)
    if (cur->id == index)
      return (cur);
  return (NULL);
}

/* Retreive a module object giving its unique ID */
elfshmod_t	*vm_getmod(u_int index)
{
  elfshmod_t	*cur;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  for (cur = world.modlist; cur; cur = cur->next)
    if (cur->id == index)
      return (cur);
  return (NULL);
}

/* Print error depending on the state of the machine */
int		vm_doerror(void (*fct)(char *str), char *str)
{
  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.state.vm_mode != ELFSH_VMSTATE_CMDLINE || world.state.vm_net)
    fct(str);
  else
    {
      cmd_help();
      exit(-1);
    }
  return (-1);
}

/* Mark the current object SHT to be removed on savnig */
int             cmd_shtrm()
{
  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  world.curjob->current->shtrm = 1;
  if (!world.state.vm_quiet)
    vm_output(" [*] SHT marked as removed, you need to save\n\n");
  return (0);
}

/* Mark the file as stripped */
int		cmd_strip()
{

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  world.curjob->current->strip = 1;
  if (!world.state.vm_quiet)
    vm_output(" [*] Object marked as stripped, you need to save\n\n");
  return (0);
}

/* Mark SHT as removed and file as stripped */
int		cmd_sstrip()
{

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  world.curjob->current->strip = 1;
  world.curjob->current->shtrm = 1;
  if (!world.state.vm_quiet)
    vm_output(" [*] Object marked as sstripped, you need to save\n\n");
  return (0);
}

/* Change the VM state as QUIET */
int		cmd_quiet()
{

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  world.state.vm_quiet = 1;
  return (0);
}

/* Run network initialisation */
int		cmd_network()
{
  elfshjob_t	*job;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.state.vm_net == 1)
    return 0;
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
          vm_addcmd(CMD_LOAD    , (void *) cmd_load     , (void *) vm_getoption, 0);
          vm_addcmd(CMD_UNLOAD  , (void *) cmd_unload   , (void *) vm_getoption, 0);
          vm_addcmd(CMD_SAVE    , (void *) cmd_save     , (void *) vm_getoption, 1);
          vm_addcmd(CMD_SWITCH  , (void *) cmd_doswitch , (void *) vm_getoption, 1);
          vm_addcmd(CMD_METACMD , (void *) cmd_meta     , (void *) NULL, 0);
          vm_addcmd(CMD_QUIT    , (void *) cmd_quit     , (void *) NULL, 0);
          vm_addcmd(CMD_QUIT2   , (void *) cmd_quit     , (void *) NULL, 0);
          vm_addcmd(CMD_LIST    , (void *) cmd_dolist   , (void *) NULL, 0);
          vm_addcmd(CMD_LIST2   , (void *) cmd_dolist   , (void *) NULL, 0);
          vm_addcmd(CMD_STOP    , (void *) cmd_stop     , (void *) NULL, 0);

          // NEED A SWITCH
	}
      vm_output(" [*] Started ELFsh network connection \n\n");
    }

  return (0);
}

/* Change the VM state as VERBOSE (default) */
int		cmd_verb()
{

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  vm_output(" [*] Switched to verbose mode \n");
  world.state.vm_quiet = 0;
  return (0);
}

/* Useful when you have only one terminal */
int		cmd_meta()
{
  int		ret;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!world.state.vm_quiet)
    vm_output(" [*] You are still in elfsh, exit bash "
	      "to get back to the regular prompt \n\n");
#if defined(USE_READLN)
  if (world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER ||
      world.state.vm_mode == ELFSH_VMSTATE_IMODE)
    rl_deprep_terminal();
#endif
  
  ret = vm_system(ELFSH_SHELL" -i");
  
#if defined(USE_READLN)
  if (world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER ||
      world.state.vm_mode == ELFSH_VMSTATE_IMODE)
    rl_prep_terminal(1);
#endif 

  return ret;
}

/* Useful for debugging */
int	cmd_stop()
{

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  return (raise(SIGSTOP));
}

/* Useful to differentiate 0 and a string */
int	vm_isnbr(char *string)
{
  size_t len = strlen(string);
  size_t ii;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  for (ii=0; ii < len; ii++)
    if (!isdigit((int) string[ii]))
      return (0);
  return (1);
}



/* Precise a general weak bounded regex for all options */
int		cmd_glregx()
{
  char		*str;
  char		logbuf[BUFSIZ];

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  str = strdup(world.curjob->curcmd->param[0]);
  if (regcomp(&world.state.vm_regx, str, REG_EXTENDED) < 0)	
    ELFSH_SETERROR("[elfsh:glregx] Failed to compute regex.\n", -1);							
  world.state.vm_use_regx = 1;
  if (world.state.vm_sregx)
    free(world.state.vm_sregx);
  world.state.vm_sregx = str;
  if (!world.state.vm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1, " [*] Changed global regex to %s \n\n", str);
      vm_output(logbuf);
    }
  return (0);
}				


/* Read the sorting parameter */
int		cmd_sort()
{
  char		logbuf[BUFSIZ];

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.curjob->curcmd->param[0][0] != ELFSH_SORT_BY_ADDR && 
      world.curjob->curcmd->param[0][0] != ELFSH_SORT_BY_SIZE)
    ELFSH_SETERROR("[elfsh:sort] Invalid sorting type.\n", -1);
  if (!world.state.vm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1, " [*] Switched to %s sorting\n\n", 
	       (world.curjob->curcmd->param[0][0] == ELFSH_SORT_BY_ADDR ?
		"ADDR" : "SIZE"));
      vm_output(logbuf);
    }
  world.state.sort = world.curjob->curcmd->param[0];
  return (0);
}

/* Set the share flag */
int		cmd_shared()
{

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  world.state.vm_shared = 1;
  return (0);
}

/* print the list of net-client */
int		cmd_netlist()
{
  int		i = 0;

#if defined(ELFSHNET)
  hashent_t	*actual;
  int		index;
  char		buf[BUFSIZ];
  char		*addr;


  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

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
  return 0;
}

/* kill a client by its ip address */
int		cmd_netkill()
{
  char		*tokill;
  char          buf[BUFSIZ];
    
  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  tokill = world.curjob->curcmd->param[0];
  
  if (world.curjob->io.type == ELFSH_IONET)
    {
      vm_output(" [*] You can't kill connection.\n");
      vm_output("\n");
      return 0;
    }
  
  if (vm_socket_del(tokill) < 0)
    {
#if __DEBUG_NETWORK__
      fprintf(stderr, "[DEBUG NETWORK] error on killing a client : %s\n",tokill);
#endif
      snprintf(buf, BUFSIZ - 1, " [*] %s not connected.\n", tokill);
      vm_output(buf);
      vm_output("\n");
      return (0);
    }
  snprintf(buf, BUFSIZ - 1, " [*] %s's connection closed.\n", tokill);
  vm_output(buf);
  vm_output("\n");
  return (0); 
}


/* print the list of dump neighbors */
int		cmd_peerslist()
{
  int           i = 0;

#if defined(ELFSHNET)
  hashent_t     *actual;
  int           index;
  char          buf[BUFSIZ];
  
  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

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
#endif
    
  if (i == 0)
    vm_output(" [*] No connection\n");
  
  vm_output("\n");
  return 0;
}


/* connect to a dump node */
int		cmd_connect()
{
#if defined(ELFSHNET) 

  char          *toconnect;
  char          buf[BUFSIZ];

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
 
  toconnect = world.curjob->curcmd->param[0];

  if (world.state.vm_net != 1)
    return 0;

  
  if (world.curjob->io.type == ELFSH_IONET)
    {
      vm_output(" [*] You can't add DUMP connection.\n");
      vm_output("\n");
      return 0;
    }
  
  if (dump_connect_to(toconnect, ELFSH_DUMP_PORT) < 0)
    {
      snprintf(buf, BUFSIZ - 1, " [*] connection to %s failed.\n", toconnect);
      vm_output(buf);
      vm_output("\n");
      return (0);
    }
  
  snprintf(buf, BUFSIZ - 1, " [*] connection to %s established.\n", toconnect);
  vm_output(buf);
  vm_output("\n");
#endif
  return 0;
}


/* disconnect from a dump node */
int		cmd_discon()
{
#if defined(ELFSHNET) 

  char          *todisconnect;
  char          buf[BUFSIZ];

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  todisconnect = world.curjob->curcmd->param[0];

  if (world.state.vm_net != 1)
    return 0;
  
  if (world.curjob->io.type == ELFSH_IONET)
    {
      vm_output(" [*] You can't remove DUMP connection.\n");
      vm_output("\n");
      return 0;
    }
  
  if (dump_disconnect_host(todisconnect) < 0)
    {
      snprintf(buf, BUFSIZ - 1, " [*] Not connected to %s.\n", todisconnect);
      vm_output(buf);
      vm_output("\n");
      return (0);
    }
  
  snprintf(buf, BUFSIZ - 1, " [*] connection to %s closed.\n", todisconnect);
  vm_output(buf);
  vm_output("\n");
#endif
  return 0;
}

/* send a command to a remote elfsh node */
int			cmd_ip()
{
#if defined(ELFSHNET)

  int			idx;
  char			*to;
  char			buf[BUFSIZ];
  struct hostent        *h;
  struct sockaddr_in    serv_addr;
  char			data[BUFSIZ];
  u_int			sz = BUFSIZ - 1;
   
  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.state.vm_net != 1)
    return 0;

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

  to = world.curjob->curcmd->param[0];

  if (world.state.vm_net != 1)
    return 0;


  h = gethostbyname(to);

  if (h == NULL)
    {
      printf("[EE] Unknown host '%s'\n", to);
      return 0;
    }

  memcpy((char *) &serv_addr.sin_addr.s_addr,
	 h->h_addr_list[0],
	 h->h_length);

  snprintf(buf, BUFSIZ - 1, " [*] send command to %s : %s.\n", to, data);

  vm_output(buf);
  vm_output("\n");

  data[0] = ELFSH_DUMP_CMD;

  return dump_send(serv_addr.sin_addr, strdup(data), 1 + strlen(data + 1) + 1);
#endif
  return 0;
}


int		cmd_profile()
{
  
  if (elfsh_is_prof_enable())
    {
      elfsh_prof_disable();
      vm_output("\t .:: Profiling disable\n\n");
    }
  else
    {
      elfsh_prof_enable();
      vm_output("\t .:: Profiling enable\n\n");
    }
  return (0);
}


/* The internal basename function */
char		*vm_basename(char *str)
{
  char		*cur;
  char		*ret;

  ret = NULL;
  cur = str;
  while ((cur = strchr(cur, '/')))
    if (!*(cur + 1))
      return (NULL);
    else
      ret = ++cur;
  return (ret);
}




int		cmd_cat()
{
  char		*tocat;
  char          buf[BUFSIZ];
  int		fd;
  int		len = 0;
  int		tmplen = 0;
    
  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  tocat = world.curjob->curcmd->param[0];

  //XOPEN(fd, tocat, O_RDONLY, 0, 0);
  if ((fd = open(tocat, O_RDONLY, 0)) < 0)          
    {
      vm_output("Can't open file\n");
      return (-1);
    }

  while (1)
    {
      tmplen = read(fd, buf + len, 1);  
      if (tmplen < 0)
	{
	  XCLOSE(fd,0);
	  return (-1);
	}
      if (tmplen == 0)
	break;
      len += tmplen;
      if (len > BUFSIZ - 1)
	{
	  vm_output("[vm:cat] File too long\n");
	  XCLOSE(fd, 0);
	  return (-1);
	}
    }

  buf[len] = '\0';
  vm_output(buf);
  vm_output("\n");

  XCLOSE(fd, 0);
  return (0);
}
