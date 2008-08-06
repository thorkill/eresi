/**
 * @file select.c
 ** @ingroup io
 ** @brief The interface for I/O based on select().
 **
** Started on Fri Mar 5 00:55:40 2004 jfv
** Updated on Mon Mar 5 18:47:41 2007 jfv
**
** $Id: select.c,v 1.15 2007-11-29 14:01:56 may Exp $
**/
#include "revm.h"


/**
 * @brief Return the greatest socket from the elfsh_net_client_list and sock. 
 * @ingroup io
 */

/* We should rewrite this. revm_getmaxfd_net() would be ok */

#if defined(ERESI_NET)
int             revm_getmaxfd()
{
  int           index;
  int		keynbr;
  char		**keys;
  u_long	port;
  int           ret;
  revmjob_t    *serv;
  int		fd;

  /* If the network is not UP, the biggest fd is 0 */
  ret  = 0;
  serv = hash_get(&world.jobs, "net_init");
  if (serv == NULL)
    return (ret);
  ret = serv->ws.io.sock.socket;
  ret = (ret > dump_world.sock) ? ret : dump_world.sock;
  keys = hash_get_keys(&dump_world.ports, &keynbr);

  for (index = 0; index < keynbr; index++)
    {
      port = (u_long) hash_get(&dump_world.ports, keys[index]);
      if (port > ret)
	ret = port;
      DEBUG_NET(fprintf(stderr, "[DEBUG NETWORK] Socket (DUMP) ["DFMT"] \n", port));
    }
  
  hash_free_keys(keys);
  keys = hash_get_keys(&world.jobs, &keynbr);

  /* remote client */
  for (index = 0; index < keynbr; index++)
    {
      serv = hash_get(&world.jobs, keys[index]);
      if (!serv->ws.active)
	continue;
      if (serv->ws.io.sock.socket > ret)
	ret = serv->ws.io.sock.socket;
      DEBUG_NET(fprintf(stderr, "[DEBUG NETWORK] Socket [%u] key = %10s \n",
			serv->ws.io.sock.socket, keys[index]);
/* 		__asm__ __volatile__("int3"); */
		);
    }

  hash_free_keys(keys);
  
  switch (world.state.revm_side)
    {
    case REVM_SIDE_CLIENT:
      fd = world.fifo_s2c;
      break;
    case REVM_SIDE_SERVER:
      fd = world.fifo_c2s;
      break;
    }

  if (world.state.revm_mode == REVM_STATE_DEBUGGER && fd > ret)
    return (fd);
  return (ret);
}
#endif






/**
 * @brief Add a main socket and client's sockets to the sockets list used by select
 * and call get_max_fd to get the greatest 
 * @ingroup io
 */
int		revm_prepare_select(fd_set *sel_sockets)
{
  int           index;
  char		**keys;
  int		keynbr;
  revmjob_t	*job;



  if (revm_is_net_supported())
    {
      u_long	port;
      keys = hash_get_keys(&dump_world.ports, &keynbr);
      for (index = 0; index < keynbr; index++)
	{
	  port = (u_long) hash_get(&dump_world.ports, keys[index]);
	  DEBUG_NET(fprintf(stderr, 
			    "[DEBUG NETWORK] prepare_4_select : (DUMP) socket : "DFMT" \n",
			    port));
	  FD_SET(port, sel_sockets);
	}
      hash_free_keys(keys);
    }


  /* Now get the keys for jobs */
  keys = hash_get_keys(&world.jobs, &keynbr);

  /* Remote clients */
  for (index = 0; index < keynbr; index++)
    {
      job = hash_get(&world.jobs, keys[index]);
      if (!job->ws.active)
	continue;
      

      DEBUG_NET(fprintf(stderr, "[DEBUG NETWORK] prepare_4_select : socket : %d \n",
			job->ws.io.sock.socket));

      /* -- We have to find smth better here. */
      if (revm_is_net_supported())
	{
	  if (job->ws.io.type == REVM_IO_DUMP)
	    continue;
	  if (job->ws.io.type == REVM_IO_NET)
	    FD_SET(job->ws.io.sock.socket, sel_sockets);
	}

      
      if (job->ws.io.type == REVM_IO_STD)
	FD_SET(job->ws.io.input_fd, sel_sockets);
    }

  /* Also set the debugger fifo if we are in debugger mode */
  if (world.state.revm_mode == REVM_STATE_DEBUGGER)
    {
      if (world.state.revm_side == REVM_SIDE_CLIENT)
	FD_SET(world.fifo_s2c, sel_sockets);
      else
	FD_SET(world.fifo_c2s, sel_sockets);
    }

  hash_free_keys(keys);
  
  return (revm_getmaxfd());
}



/**
 * @brief Check if we had any network event
 * @ingroup io
 */
void			revm_check_net_select(fd_set *sel_sockets, 
					    int cursock)
{
  // Read net command if any.
  if (revm_net_recvd(sel_sockets) < 0)
    fprintf(stderr, "vmnet_select : revm_net_recvd() failed\n");
  
  /* Check remote clients */
  if (FD_ISSET(cursock, sel_sockets))
    {
      if (revm_net_accept() < 0)
	fprintf(stderr, "Connection rejected\n");
    }
  
  /* Check the DUMP connection */
  if (FD_ISSET(dump_world.sock, sel_sockets))
    {
      if (revm_dump_accept() < 0)
	fprintf(stderr, "Connection rejected\n");
    }
}



/** 
 * @brief Set IO to the choosen socket 
 * @ingroup io 
*/
int			revm_socket_getnew()
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
	  cur->ws.io.sock.recvd_f == NEW && 
	  cur->ws.io.sock.ready_f == YES)
	{
	  world.curjob = cur;
	  return (1);
	}
    }
  return (0);
}

/***
 * @brief Print prompt depending on the current REVM mode
 * @ingroup io
 */
int			revm_preselect_prompt()
{
  if (world.state.revm_mode != REVM_STATE_CMDLINE || world.state.revm_net)
    {
      if (world.curjob->ws.io.type != REVM_IO_DUMP)
	{
	  if (revm_is_stdinput())
	    {
	      /* Do not display the prompt for the client side 
		 This part can't use any READLN stuff because
		 we always be on server */
	      if (!(world.state.revm_mode == REVM_STATE_DEBUGGER &&
		    world.state.revm_side == REVM_SIDE_CLIENT))
		revm_display_prompt();
	    }
	}
    }
  return (0);
}


/**
 * @brief Wait for all input 
 * @ingroup io
 */
int                     revm_select()
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
  revm_flush();

  /* In case of not already handle data */
  if (revm_socket_getnew())
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));

  /* clean jobs hash table */
  revm_clean_jobs();

  do
    {
      FD_ZERO(&sel_sockets);

      if (revm_is_net_supported())
	{
	  if (world.state.revm_net && init)
	    {
	      FD_SET(init->ws.io.sock.socket, &sel_sockets);
	      FD_SET(dump_world.sock, &sel_sockets);
	    }
	}


      // Prepare for the select() call
      max_fd = revm_prepare_select(&sel_sockets);

      /* Display the prompt */
      revm_preselect_prompt();
      
      do
	{
	  err = select(max_fd + 1, &sel_sockets, NULL, NULL, NULL);
	  /* Retry in case of error */
	} while(err < 1 && errno == EINTR);
	
      /* Select which command will be proceded */
      if (revm_is_net_supported())
	{
	  if (world.state.revm_net)
	    {
	      if (init)
		revm_check_net_select(&sel_sockets, init->ws.io.sock.socket);
	  
	      if (revm_socket_getnew())
		PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,(0));
	  
	      DEBUG_NET(fprintf(stderr, 
				"[DEBUG NETWORK] Select broken by a new connexion.\n"));
	      continue;
	    }
	}
      
      /* Come back now if we are in command line */
      if (world.state.revm_mode == REVM_STATE_CMDLINE)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,(0));

      /* If we are in interactive mode .. */
      world.curjob = revm_localjob_get();
      if (!world.curjob)
	{
	  //fprintf(stderr, "Entering NOCURJOB condition .. \n");
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (-1));
	}
	  
      /* If the event appeared on the debugger FIFO, 
	 change the input file descriptor for it */
      if (world.state.revm_mode == REVM_STATE_DEBUGGER)
	{
	  fifofd = (world.state.revm_side == REVM_SIDE_CLIENT ?
		    world.fifo_s2c : world.fifo_c2s);
	  
	  if (FD_ISSET(fifofd, &sel_sockets))
	    {
	      world.curjob->ws.io.old_input = world.curjob->ws.io.input;
	      world.curjob->ws.io.input = revm_fifoinput;
	      
	      DEBUG_NET(
			if (world.state.revm_mode == REVM_STATE_DEBUGGER && 
			    world.state.revm_side == REVM_SIDE_CLIENT)
			  fprintf(stderr, "(client) Event appeared on fifo \n");
			else if (world.state.revm_mode == REVM_STATE_DEBUGGER && 
				 world.state.revm_side == REVM_SIDE_SERVER)
			  fprintf(stderr, "(server) Event appeared on fifo \n"));
	    }
	}
      revm_prompt_postselect_restore(&sel_sockets);
    } while (0);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,(0));
}
