/**
** @file net.c
** @ingroup libstderesi
** Started on  Fri Nov  2 15:21:56 2001 jfv
** Updated on  Thu Jan 04 11:26:11 2007 jfv
**
** $Id: net.c,v 1.1 2007-11-29 14:01:56 may Exp $
**
*/
#include "libstderesi.h"

/** 
 * Run network initialisation 
 */
int		cmd_network()
{
  revmjob_t	*job;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.state.revm_net == 1)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,job, sizeof (revmjob_t), -1);

  /* network job setup */
  job->ws.io.type   = REVM_IO_NET;
  job->ws.io.input  = revm_net_input;
  job->ws.io.output = revm_net_output;
  job->ws.createtime = time(&job->ws.createtime);

  hash_add(&world.jobs, "net_init", job);
/*   world.curjob = job;				// to comment again ? */
  world.state.revm_net = 1;

  if (revm_net_init() < 0)
    fprintf (stderr,"revm_initio : Initialisation of network : Failed \n");
  else
    {
      if (world.state.revm_mode == REVM_STATE_CMDLINE)
	{
          // We had interactive mode commands which aren't load in CMDLINE mode
          revm_command_add(CMD_LOAD    , (void *) cmd_load     , (void *) revm_getoption, 0, HLP_LOAD);
          revm_command_add(CMD_UNLOAD  , (void *) cmd_unload   , (void *) revm_getoption, 0, HLP_UNLOAD);
          revm_command_add(CMD_SAVE    , (void *) cmd_save     , (void *) revm_getoption, 1, HLP_SAVE);
          revm_command_add(CMD_SWITCH  , (void *) cmd_doswitch , (void *) revm_getoption, 1, HLP_SWITCH);
          revm_command_add(CMD_METACMD , (void *) cmd_meta     , (void *) NULL,           0, HLP_METACMD);
          revm_command_add(CMD_QUIT    , (void *) cmd_quit     , (void *) NULL,           0, HLP_QUIT);
          revm_command_add(CMD_QUIT2   , (void *) cmd_quit     , (void *) NULL,           0, HLP_QUIT);
          revm_command_add(CMD_LIST    , (void *) cmd_dolist   , (void *) NULL,           0, HLP_LIST);
          revm_command_add(CMD_LIST2   , (void *) cmd_dolist   , (void *) NULL,           0, HLP_LIST);
          revm_command_add(CMD_STOP    , (void *) cmd_stop     , (void *) NULL,           0, HLP_STOP);
	}
      revm_output(" [*] Started ERESI network stack\n\n");
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** 
 * print the list of net-client 
 */
int		cmd_netlist()
{
  int		i = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(ERESI_NET)
  int		index;
  char		buf[BUFSIZ];
  char		*addr;
  revmjob_t	*curjob;
  char		**keys;
  int		keynbr;

  revm_output(" .::. Net client list .::. \n");
  keys = hash_get_keys(&world.jobs, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      curjob = (revmjob_t *) hash_get(&world.jobs, keys[index]);
      if (!curjob->ws.active)
	continue;
      if (curjob->ws.io.type == REVM_IO_NET)
	{
	  addr = inet_ntoa(curjob->ws.io.sock.addr.sin_addr);
	  snprintf(buf, BUFSIZ - 1, " [%.3d] IP %s\n", i, addr);
	  revm_output(buf);
	  i++;
	}
    }
  hash_free_keys(keys);
#endif

  if (i == 0)
    revm_output(" [*] No client\n");
  revm_output("\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** 
 * kill a client by its ip address 
 */
int		cmd_netkill()
{
  char		*tokill;
  char          buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  tokill = revm_cur_job_param(0);

  tokill = revm_lookup_string(tokill);

  if (world.curjob->ws.io.type == REVM_IO_NET)
    {
      revm_output(" [*] You can't kill connection.\n");
      revm_output("\n");
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  if (revm_socket_del(tokill) < 0)
    {
#if __DEBUG_NETWORK__
      fprintf(stderr, "[DEBUG NETWORK] error on killing a client : %s\n",tokill);
#endif
      snprintf(buf, BUFSIZ - 1, " [*] %s not connected.\n", tokill);
      revm_output(buf);
      revm_output("\n");
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  snprintf(buf, BUFSIZ - 1, " [*] %s's connection closed.\n", tokill);
  revm_output(buf);
  revm_output("\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * print the list of dump neighbors 
 */
int		cmd_peerslist()
{
  int           i = 0;

#if defined(ERESI_NET)
  listent_t     *actual;
  int           index;
  char          buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_output(" .::. DUMP neighbors list .::. \n");
  for (index = 0; index < dump_world.ports.size; index++)
    {
      for (actual = &dump_world.ports.ent[index];
	   actual != NULL && actual->key != NULL;
	   actual = actual->next)
	{
	  snprintf(buf, BUFSIZ - 1, " [%.3d] IP %s\n", i, actual->key);
	  revm_output(buf);
	  i++;
	}
    }
#else
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#endif

  if (i == 0)
    revm_output(" [*] No connection\n");

  revm_output("\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * connect to a dump node 
 */
int		cmd_connect()
{
#if defined(ERESI_NET)
  char          *toconnect;
  char          buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  toconnect = revm_cur_job_param(0);
  toconnect = revm_lookup_string(toconnect);

  if (world.state.revm_net != 1)
    {
      revm_output(" [*] First enable network support.\n");
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  if (world.curjob->ws.io.type == REVM_IO_NET)
    {
      revm_output(" [*] You can't add DUMP connection.\n");
      revm_output("\n");
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  if (dump_connect_to(toconnect, REVM_DUMP_PORT) < 0)
    {
      snprintf(buf, BUFSIZ - 1, " [*] connection to %s failed.\n", toconnect);
      revm_output(buf);
      revm_output("\n");
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  snprintf(buf, BUFSIZ - 1, " [*] connection to %s established.\n", toconnect);
  revm_output(buf);
  revm_output("\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
#else
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  revm_output(" [*] This version of ERESI was compiled without network support.\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
#endif
}


/** 
 * disconnect from a dump node 
 */
int		cmd_discon()
{
#if defined(ERESI_NET)

  char          *todisconnect;
  char          buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  todisconnect = revm_cur_job_param(0);

  todisconnect = revm_lookup_string(todisconnect);

  if (world.state.revm_net != 1)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  if (world.curjob->ws.io.type == REVM_IO_NET)
    {
      revm_output(" [*] You can't remove DUMP connection.\n");
      revm_output("\n");
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  if (dump_disconnect_host(todisconnect) < 0)
    {
      snprintf(buf, BUFSIZ - 1, " [*] Not connected to %s.\n", todisconnect);
      revm_output(buf);
      revm_output("\n");
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  snprintf(buf, BUFSIZ - 1, " [*] connection to %s closed.\n", todisconnect);
  revm_output(buf);
  revm_output("\n");
#else
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#endif
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** 
 * send a command to a remote elfsh node 
 */
int			cmd_rcmd()
{
#if defined(ERESI_NET)

  int			idx;
  char			*to;
  char			buf[BUFSIZ];
  struct hostent        *h;
  struct sockaddr_in    serv_addr;
  char			data[BUFSIZ];
  u_int			sz = BUFSIZ - 1;
  int			ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* NOT GOOD, to remove, should be able to connnect + rcmd without net */
   if (!revm_is_net_enabled())
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Not in 'net' mode", (-1));

   
   data[0] = REVM_DUMP_CMD;
   data[1] = '\0';

  sz -= 2;

  fprintf(stderr, " Sizeof param: %#x ", strlen(revm_cur_job_param(0)));
  for (idx = 0; revm_cur_job_param(idx) != NULL; idx++)
    {
      strncat(data, " ", sz - 1);
      sz -= 1;
      if (sz < 2)
	break;
      strncat(data, revm_cur_job_param(idx), sz - 1);
      sz -= strlen(revm_cur_job_param(idx));
      if (sz < 2)
	break;
    }

  if (idx < 2)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Invalid number of parameters", (-1));

  fprintf(stderr, " Hey man: %s ", revm_cur_job_param(idx - 1));
  to = revm_cur_job_param(idx - 1);

  to = revm_lookup_string(to);

  h = gethostbyname(to);

  if (h == NULL)
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Unknown host", (-1));

  memcpy((char *) &serv_addr.sin_addr.s_addr,
	 h->h_addr_list[0],
	 h->h_length);

  snprintf(buf, BUFSIZ - 1, " [*] send command to %s : %s.\n", to, data);

  revm_output(buf);
  revm_output("\n");

 
  ret = dump_send(serv_addr.sin_addr, strdup(data), 1 + strlen(data + 1) + 1);
 
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
#else
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


