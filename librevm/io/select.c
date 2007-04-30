/*
** select.c for librevm in ERESI
**
** The interface for I/O based on select()
**
** Started on Fri Mar 5 00:55:40 2004 mayhem
** Updated on Mon Mar 5 18:47:41 2007 mayhem
**
** $Id: select.c,v 1.9 2007-04-30 19:54:12 mxatone Exp $
**
*/
#include "revm.h"



/* Return the greatest socket from the elfsh_net_client_list and sock. */
#if defined(ELFSHNET)
int             vm_getmaxfd()
{
  int           index;
  int		keynbr;
  char		**keys;
  u_long	port;
  int           ret;
  revmjob_t    *serv;

  /* If the network is not UP, the biggest fd is 0 */
  ret  = 0;
  serv = hash_get(&world.jobs, "net_init");
  if (serv == NULL)
    return (ret);
  ret = serv->ws.sock.socket;
  ret = (ret > dump_world.sock) ? ret : dump_world.sock;
  keys = hash_get_keys(&dump_world.ports, &keynbr);

  for (index = 0; index < keynbr; index++)
    {
      port = (u_long) hash_get(&dump_world.ports, keys[index]);
      if (port > ret)
	ret = port;
#if __DEBUG_NETWORK__
      fprintf(stderr, "[DEBUG NETWORK] Socket (DUMP) [%u] \n", port);
#endif
    }
  
  hash_free_keys(keys);
  keys = hash_get_keys(&world.jobs, &keynbr);

  /* remote client */
  for (index = 0; index < keynbr; index++)
    {
      serv = hash_get(&world.jobs, keys[index]);
      if (!serv->ws.active)
	continue;
      if (serv->ws.sock.socket > ret)
	ret = serv->ws.sock.socket;
#if __DEBUG_NETWORK__
      fprintf(stderr, "[DEBUG NETWORK] Socket [%u] key = %10s \n",
	      serv->sock.socket, keys[index]);
#endif
    }

  hash_free_keys(keys);
  if (world.state.vm_mode == REVM_STATE_DEBUGGER && world.fifofd > ret)
    ret = world.fifofd;
  return (ret);
}
#endif






/** Add a main socket and client's sockets to the sockets list used by select
** and call get_max_fd to get the greatest */
int		vm_prepare_select(fd_set *sel_sockets)
{
  int           index;
  char		**keys;
  int		keynbr;
  revmjob_t	*job;
#if defined(ELFSHNET)
  u_long	port;

  keys = hash_get_keys(&dump_world.ports, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      port = (u_long) hash_get(&dump_world.ports, keys[index]);
#if __DEBUG_NETWORK__
      fprintf(stderr, 
	      "[DEBUG NETWORK] prepare_4_select : (DUMP) socket : %d \n",
	      port);
#endif
      FD_SET(port, sel_sockets);
    }
  hash_free_keys(keys);
#endif

  /* Now get the keys for jobs */
  keys = hash_get_keys(&world.jobs, &keynbr);

  /* Remote clients */
  for (index = 0; index < keynbr; index++)
    {
      job = hash_get(&world.jobs, keys[index]);
      if (!job->ws.active)
	continue;
      
#if _DEBUG_NETWORK__
      fprintf(stderr, "[DEBUG NETWORK] prepare_4_select : socket : %d \n",
	      job->ws.sock.socket);
#endif

#if defined(ELFSHNET)
      if (job->ws.io.type == REVM_IO_DUMP)
	continue;
      if (job->ws.io.type == REVM_IO_NET)
	FD_SET(job->ws.sock.socket, sel_sockets);
#endif
      
      if (job->ws.io.type == REVM_IO_STD)
	FD_SET(job->ws.io.input_fd, sel_sockets);
    }

  /* Also set the debugger fifo if we are in debugger mode */
  if (world.state.vm_mode == REVM_STATE_DEBUGGER)
    {
      if (world.state.vm_side == REVM_SIDE_CLIENT)
	FD_SET(world.fifo_s2c, sel_sockets);
      else
	FD_SET(world.fifo_c2s, sel_sockets);
    }

  hash_free_keys(keys);
  return (vm_getmaxfd());
}



/* Check if we had any network event */
void			vm_check_net_select(fd_set *sel_sockets, 
					    int cursock)
{
#if defined(ELFSHNET)
  // Read net command if any.
  if (vm_net_recvd(sel_sockets) < 0)
    fprintf(stderr, "vmnet_select : vm_net_recvd() failed\n");
  
  /* Check remote clients */
  if (FD_ISSET(cursock, sel_sockets))
    {
      if (vm_net_accept() < 0)
	fprintf(stderr, "Connection rejected\n");
    }
  
  /* Check the DUMP connection */
  if (FD_ISSET(dump_world.sock, sel_sockets))
    {
      if (vm_dump_accept() < 0)
	fprintf(stderr, "Connection rejected\n");
    }
#endif
}





/* Set IO to the choosen socket */
int			vm_socket_getnew()
{
  revmjob_t		*cur;
  int			index;
  char			**keys;
  int			keynbr;

  keys = hash_get_keys(&world.jobs, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      cur = hash_get(&world.jobs, keys[index]);
      if (!cur || !cur->ws.active)
	continue;
      if (cur->ws.io.type == REVM_IO_DUMP && cur->ws.io.new != 0)
	{
	  world.curjob = cur;
	  return (1);
	}
      if (cur->ws.io.type == REVM_IO_NET && 
	  cur->ws.sock.recvd_f == NEW && cur->ws.sock.ready_f == YES)
	{
	  world.curjob = cur;
	  return (1);
	}
    }
  return (0);
}

int			vm_preselect_prompt()
{
  // In the case of normal loop print prompt
  if (world.state.vm_mode != REVM_STATE_CMDLINE ||
      world.state.vm_net)
    {
      if (world.curjob->ws.io.type != REVM_IO_DUMP)
	{

	  // Display prompt
#if defined(USE_READLN)
	  if (world.curjob->ws.io.type == REVM_IO_STD)
	    {
	      if (world.curjob->ws.io.buf != NULL) 
		{
		  
		  /* On the client side, we consider that the prompt is already
		     returned by the server */
		  if (!(world.state.vm_mode == REVM_STATE_DEBUGGER &&
			world.state.vm_side == REVM_SIDE_CLIENT))
		    rl_forced_update_display();
		  else
		    rl_on_new_line_with_prompt();
		  vm_log(vm_get_prompt());
		}
	    }
	  else 
#endif
	    {
	      /* Do not display the prompt for the client side */
	      if (!(world.state.vm_mode == REVM_STATE_DEBUGGER &&
		    world.state.vm_side == REVM_SIDE_CLIENT))
		vm_display_prompt();
	    }
	}
    }

  return (0);
}


/* Wait for all input */
int                     vm_select()
{
  fd_set		sel_sockets;
  int                   max_fd;
  revmjob_t            *init;
  int			err;
  int			fifofd;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* By default do only one select. */
  init = hash_get(&world.jobs, "net_init");

  /* Flush pending outputs */
  vm_flush();

  /* In case of not already handle data */
  if (vm_socket_getnew())
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));

  /* clean jobs hash table */
  vm_clean_jobs();

  do
    {
      FD_ZERO(&sel_sockets);

#if defined(ELFSHNET)
      if (world.state.vm_net && init)
        {
          FD_SET(init->ws.sock.socket, &sel_sockets);
	  FD_SET(dump_world.sock, &sel_sockets);
        }
#endif

      // Prepare for the select() call
      max_fd = vm_prepare_select(&sel_sockets);

      /* Display the prompt */
      vm_preselect_prompt();
      
    retry:
      err = select(max_fd + 1, &sel_sockets, NULL, NULL, NULL);
      
      /* Retry in case of error */
      if (err < 1 && errno == EINTR)
	goto retry;
      

      /* Select which command will be proceded */
#if defined(ELFSHNET)
      if (world.state.vm_net)
	{
	  if (init)
	    vm_check_net_select(&sel_sockets, init->ws.sock.socket);
	  
	  if (vm_socket_getnew())
	    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,(0));
	  
#if __DEBUG_NETWORK__
	  fprintf(stderr, 
		  "[DEBUG NETWORK] Select broken by a new connexion.\n");
#endif
	  continue;
	}
#endif
      
      /* Come back now if we are in command line */
      if (world.state.vm_mode == REVM_STATE_CMDLINE)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,(0));

      /* If we are in interactive mode .. */
      world.curjob = vm_localjob_get();
      if (!world.curjob)
	{
	  //fprintf(stderr, "Entering NOCURJOB condition .. \n");
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (-1));
	}
	  
      /* If the event appeared on the debugger FIFO, 
	 change the input file descriptor for it */
      if (world.state.vm_mode == REVM_STATE_DEBUGGER)
	{
	  fifofd = (world.state.vm_side == REVM_SIDE_CLIENT ?
		    world.fifo_s2c : world.fifo_c2s);
	  
	  if (FD_ISSET(fifofd, &sel_sockets))
	    {
	      world.curjob->ws.io.old_input = world.curjob->ws.io.input;
	      world.curjob->ws.io.input = vm_fifoinput;
	      
	      /* Debug only */
	      /*
	      if (world.state.vm_mode == REVM_STATE_DEBUGGER && 
		  world.state.vm_side == REVM_SIDE_CLIENT)
		fprintf(stderr, "(client) Event appeared on fifo \n");
	      else if (world.state.vm_mode == REVM_STATE_DEBUGGER && 
		       world.state.vm_side == REVM_SIDE_SERVER)
		fprintf(stderr, "(server) Event appeared on fifo \n");
	      */

	    }
	}
	  
#if defined (USE_READLN)
      if (world.state.vm_side == REVM_SIDE_CLIENT && FD_ISSET(0, &sel_sockets))
	readln_prompt_restore();
#endif

    } while (0);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,(0));
}
