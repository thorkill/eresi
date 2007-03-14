/*
** network.c for librevm in ERESI
**
** The high-level network interface
**
** Started Wed Feb 25 22:22:35 2004 yann_malcom
** Updated Mon Mar 05 04:37:10 2007 mayhem 
**
** $Id: network.c,v 1.3 2007-03-14 12:51:45 may Exp $
**
*/
#include "revm.h"


/* Number of clients connected */
int  	       elfsh_net_client_count = 0;



/* Is net support enable ? */
#if defined(ELFSHNET)
/* Add a client socket to the elfsh_net_client_list. */
revmjob_t	*vm_socket_add(int socket, struct sockaddr_in *addr)
{
  revmjob_t	*new;
  char		logbuf[30];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* If too many clients return (-1) */
  if (elfsh_net_client_count >= REVM_MAX_CLIENTS)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Too many clients", (NULL));

  /* If there is already a client by that inet address return (-1) */
  if (hash_get(&world.jobs, inet_ntoa(addr->sin_addr)) != NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Already connected", NULL);

#if __DEBUG_NETWORK__
  fprintf(stderr, "[DEBUG NETWORK] adding client on socket %d .\n", socket);
#endif

  XALLOC(__FILE__, __FUNCTION__, __LINE__,new, sizeof (revmjob_t), NULL);

  new->ws.sock.addr    = *addr;
  new->ws.sock.socket  = socket;
  new->ws.sock.recvd   = NULL;
  new->ws.sock.recvd_f = OLD;
  elfsh_net_client_count++;

  new->ws.io.type      = ELFSH_IONET;
  new->ws.io.input_fd  = socket;
  new->ws.io.output_fd = socket;
  new->ws.io.input     = vm_net_input;
  new->ws.io.output    = vm_net_output;
  new->ws.createtime   = time(&new->ws.createtime);
  new->ws.active       = 1;

  snprintf(logbuf, sizeof(logbuf), "client-%u_loaded", elfsh_net_client_count);
  hash_init(&new->loaded, logbuf, 51, ASPECT_TYPE_UNKNOW);
  snprintf(logbuf, sizeof(logbuf), "client-%u_dbgloaded", elfsh_net_client_count);
  hash_init(&new->dbgloaded, logbuf, 11, ASPECT_TYPE_UNKNOW);
  hash_add(&world.jobs, inet_ntoa(addr->sin_addr), new);
  world.curjob = new;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}



/* Return the number of buffer in a revmsock_t recvd */
int		vm_socket_get_nb_recvd(char *inet)
{
  revmjob_t	*tmp;
  int		i;
  int		index;
  revmjob_t	*old;
  hashent_t	*actual;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Is there a socket by that name */
  tmp = hash_get(&world.jobs, inet);

  if (tmp != NULL)
    {
      if (!tmp->ws.active)
	{
	  for (index = 0; index < world.jobs.size; index++)
	    {
	      for (actual = &world.jobs.ent[index];
		   actual != NULL && actual->key != NULL;
		   actual = actual->next)
		{
		  old = world.curjob;
		  world.curjob = tmp;

		  if (vm_own_job((revmjob_t *) actual->data) && 
		      ((revmjob_t *) actual->data)->ws.active)
		    {
		      world.curjob = old;
		      tmp = (revmjob_t *) actual->data;
		    }

		  world.curjob = old;
		}
	    }
	}

      if (tmp->ws.sock.recvd_f == OLD)
        {
	  /* recvd data have already been read so we forget them */
	  tmp->ws.sock.recvd = NULL;
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
        }
      else
        {
	  /* we have to count them */
	  if (tmp->ws.sock.recvd != NULL)
            {
	      /* There might be data */
	      i = 0;
	      while (tmp->ws.sock.recvd[i] != NULL)
                {
		  i++;
                }
	      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, i);
            }
	  else
	    /* recvd is NULL so return 0  but marked as NEW */
            {
#if __DEBUG_NETWORK__ 
	      fprintf(stderr, "[DEBUG NETWORK] Not really a consistant"
		      " state in a revmsock_t recvd data.\n");
#endif
	      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
            }
        }

    }
  else
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "No socket by that inet addr", (-1));

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "Failed to receive", (-1));
}

/* Only close a socket */
int vm_socket_close(int socket)
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (shutdown(socket, SHUT_RDWR) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Failed to shutdown socket", (-1));
  if (close(socket) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Failed to close socket", (-1));

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/*
** Remove a client socket from the list and close it, return (-1) if socket 
** not found
*/
int		 vm_socket_del(char *inet_addr)
{
  revmjob_t	 *tmp;
  int		 nbargc;
  int		 i;
  hashent_t	*actual;
  int		index;
  revmjob_t	*old;


  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  tmp = hash_get(&world.jobs, inet_addr);
  if (!tmp)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Failed to find INET address for socket", (-1));

  for (index = 0; index < world.jobs.size; index++)
    {
      for (actual = &world.jobs.ent[index];
	   actual != NULL && actual->key != NULL;
	   actual = actual->next)
	{
	  old = world.curjob;
	  world.curjob = tmp;
	  
	  if (vm_own_job((revmjob_t *) actual->data))
	    {
	      hash_del(&world.jobs, actual->key);
	    }
	  
	  world.curjob = old;
	  
	}
    }
  
  if (vm_socket_close(tmp->ws.sock.socket) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Failed to close socket", (-1));

  //elfsh_net_client_count--;
  if (tmp->ws.sock.recvd_f == NEW)
    {
      /* We assume that buffer are not referenced, and so won't be used */
      nbargc = vm_socket_get_nb_recvd(inet_ntoa(tmp->ws.sock.addr.sin_addr));
      for (i = 0 ; i < nbargc ; i++)
	XFREE (__FILE__, __FUNCTION__, __LINE__, tmp->ws.sock.recvd[i]);

#if __DEBUG_NETWORK__
      fprintf(stderr, "[DEBUG NETWORK] We are deleting a socket struct which has"
	      "new received data.\n");
#endif
    }

  XFREE(__FILE__, __FUNCTION__, __LINE__,tmp->ws.sock.recvd);
  hash_del(&world.jobs, inet_addr);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Really send the data */
int vm_netsend(char *buf, unsigned int len)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
 
 if (len > strlen(buf))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid input length parameter", (-1));
 if (send(world.curjob->ws.io.output_fd, (void *) buf, len, 0) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Failed to send data", (-1));

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* send buf on a socket */
int	vm_net_output(char *buf)
{
  int	ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (strlen(buf) > REVM_MAX_SEND_SIZE)
    {
#if __DEBUG_NETWORK__
      fprintf(stderr, "[DEBUG NETWORK] Socket [%u] attempt to "
	      "send too much data.\n",
	      world.curjob->ws.io.output_fd);
#endif
      if (vm_netsend(buf, REVM_MAX_SEND_SIZE) >= 0)
	{
	  ret = vm_net_output((char *) (buf + REVM_MAX_SEND_SIZE));
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
	}
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Failed to send on net interface", (-1));
    }

  ret = vm_netsend(buf, strlen(buf));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}



/* Create le main socket and bind it to ELFSH_LISTEN_PORT. */
int	vm_create_server(int			*serv_sock, 
			 struct sockaddr_in	*addr,
			 u_int			port)
{
  int	reuseaddr = 1;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  // Let's create the main socket
  if  (((*serv_sock) = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    FATAL("socket");

  if (setsockopt((*serv_sock), SOL_SOCKET, 
		 SO_REUSEADDR, &reuseaddr, 
		 sizeof (reuseaddr)) < 0)
    FATAL("setsockopt");

  //memset(addr, 0, sizeof (struct sockaddr_in));
  addr->sin_family = AF_INET;
  addr->sin_port   = htons(port);
  inet_aton("0.0.0.0", &addr->sin_addr);

  if (bind((*serv_sock), (struct sockaddr *) addr,
	   sizeof (struct sockaddr_in)) < 0)
    {
      close((*serv_sock));
      FATAL("bind");
    }

  if (listen((*serv_sock), 5) < 0) 
    FATAL("listen");

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}





/* Update revmsock_t recvd buffer */
int		vm_update_recvd(revmsock_t *socket)
{
  char		*buf;
  char		**buf2;
  int		size;
  int		i = 0;
  int		bufnb;
  char		**oldrecvd;
  char		*c;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Allocating space for receiving data ... */
  XALLOC(__FILE__, __FUNCTION__, __LINE__,
	 buf, sizeof(char) * BUFSIZ, -1);

  size = read(socket->socket, buf, BUFSIZ - 1);
  if (size < 0)
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__, buf);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Cannot receive on socket", -1);
    }

  /* That's ok, let's continue */
  else
    {

#if __DEBUG_NETWORK__
      fprintf(stderr, "[DBG_NETWORK] read(%d) [OK] with ret = %d \n",
	      socket->socket, size);
#endif

      if (size == 0)
        {

#if __DEBUG_NETWORK__
	  fprintf(stderr,
		  "[DBG_NETWORK] connection to %s closed by remote host.\n",
		  inet_ntoa(socket->addr.sin_addr));
#endif

	  if (socket->recvd_f == OLD)
            {
	      XFREE(__FILE__, __FUNCTION__, __LINE__,buf);
	      vm_socket_del(inet_ntoa(socket->addr.sin_addr));
	      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
            }
	  else
            {
	      //TODO : XFREE all buffer, if buffers are marked OLD,
	      //vm_net_input has been called so vm_socket_merge_recvd too.
	      XFREE (__FILE__, __FUNCTION__, __LINE__, buf);
	      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
            }
        }
      else
        {
	  buf[size] = '\0';
	  /* Remove the last char in case of telnet client */
	  if (size > 0)
            {
	      /* We want a trailing \n to handle the line */
	      socket->ready_f = NO;
	      if(buf[size-1] == 0x0a)
                {
		  buf[size-1] = '\0';
		  /* we got one \n */
		  socket->ready_f = YES;
                }
            }
	  if (size > 1)
            {
	      if(buf[size-2] == 0x0d)
                {
		  buf[size-2] = '\0';
		  /* we got a \n */
		  socket->ready_f = YES;
                }
            }
	  /* In case of multiple command pasted at once, we want to
	     handle them at once */
	  while ((c = strchr(buf, '\n')))
	    (*c) = ' ';

        }
    }

  if (socket->recvd_f == NEW)
    {

#if __DEBUG_NETWORK__
      fprintf(stderr, "[DEBUG NETWORK] Select said there is something \n"
	     "new to read on the socket BUT there is already \n"
	     "data in the buffer which has not been read.\n");
#endif

      /* Something got wrong in vm_socket_get_nb_recvd() */	
      bufnb = vm_socket_get_nb_recvd(inet_ntoa(socket->addr.sin_addr));
      if (bufnb < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Cannot receive on socket", -1);

      /*
      ** We have the old number of buffer and they have not been read so
      ** we have to append the new one at the end.
      */
      else
        {
	  /* +1 for the new (char *) and +1 for the NULL */
	  XALLOC(__FILE__, __FUNCTION__, __LINE__,
		 buf2, sizeof (char *)*(bufnb + 1 + 1), -1);

	  /* Now let's copy the old buffer's pointers*/
	  oldrecvd = socket->recvd;
	  for (i = 0; i < bufnb; i++)
	    buf2[i] = oldrecvd[i];

	  /* Now we append the new buffer's pointer and set the NEW flag*/
#if __DEBUG_NETWORK__
	  fprintf(stderr, "buf : %s",buf);
#endif
	  buf2[i++] = buf;
	  buf2[i++] = NULL;
	  socket->recvd = buf2;
	  socket->recvd_f = NEW;
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
        }
    }
  else
    {
      /* buffers have already be read so we can overwrite 
	 recvd and set recvd_flag to NEW */
      XALLOC(__FILE__, __FUNCTION__, __LINE__,buf2, sizeof (char *)*2, -1);
#if __DEBUG_NETWORK__
      fprintf(stderr, "buf : %s", buf);
#endif
      buf2[0] = buf;
      buf2[1] = NULL;
      socket->recvd = buf2;
      socket->recvd_f = NEW;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* Never reached, just to shut gcc up */
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
	       "Should never be reached", -1);
}






/* send buf on a dump connection */
int	vm_dump_output(char *buf)
{
  int	ret = 0;
  char	*tmp;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, tmp, strlen(buf) + 1 + 1, -1);
  tmp[0] = REVM_DUMP_MSG;
  memcpy(tmp + 1, buf, strlen(buf) + 1);
  ret = dump_send(world.curjob->ws.io.pkt->src, tmp, strlen(buf) + 1 + 1);
  //XFREE (tmp);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}





/* Return the first packet data */
char	*vm_dump_input()
{
  char	*tmp;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  tmp = strdup((char *)(world.curjob->ws.io.pkt->data) + 1);
  world.curjob->ws.io.new = 0;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (tmp));
}




/* accept a DUMP connection */
int			vm_dump_accept()
{
  struct sockaddr_in    cli_addr;
  u_int                 cli_len;
  int			new_sd;

  struct sockaddr       loc;
  socklen_t             lloc = sizeof (struct sockaddr);

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  cli_len = sizeof (cli_addr);
  new_sd = accept(dump_world.sock,
		  (struct sockaddr *) &cli_addr,
		  &cli_len);
  if (new_sd > 0)
    {
#if __DEBUG_NETWORK__
      fprintf(stderr, "[+] Connection from : %s\n",
	     inet_ntoa(cli_addr.sin_addr));
#endif

      /* getsockname ()*/
      getsockname(new_sd, &loc, &lloc);

#if __DEBUG_NETWORK__
      fprintf(stderr, "[+] add new id \n");
#endif

      dump_add_myid(((struct sockaddr_in *) &loc)->sin_addr, new_sd);

#if __DEBUG_NETWORK__
      fprintf(stderr, "[+] added\n");
      fprintf(stderr, "[+] my new id : %s \n",
	     inet_ntoa(dump_get_myid(new_sd)));
      fprintf(stderr, "[+] Adding a DUMP neighbor : [%d,%s]\n",
	     new_sd, inet_ntoa(cli_addr.sin_addr));
#endif

      dump_add_neighbor(new_sd, cli_addr.sin_addr);

      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "Failed to accept client", (-1));
}




/* Accept new connection if there is */
int			vm_net_accept()
{
  socklen_t		temp_addr_len;
  struct sockaddr_in	*temp_addr;
  int			temp_sock;
  int			ret = 0;
  revmjob_t		*curjob;
  revmjob_t		*oldjob;
  revmjob_t		*init;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* There was noise on main socket */
#if __DEBUG_NETWORK__
  fprintf(stderr, "[DEBUG NETWORK] There is somebody"
	  " knocking at the door...\n");
#endif

  init = hash_get(&world.jobs, "net_init");
  if (init == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Failed to find initial net job", (-1));

  temp_addr_len = sizeof (struct sockaddr_in);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, temp_addr, temp_addr_len, -1);
  temp_sock = accept(init->ws.sock.socket, 
		     (struct sockaddr *) temp_addr, &temp_addr_len);

  if (temp_sock >= 0)
    {
      // Somebody want to join us. We are so "attractive" ...
#if __DEBUG_NETWORK__
      fprintf(stderr, "[DEBUG NETWORK] Client "
	      "accepted on socket %d.\n",temp_sock);
#endif

      curjob = vm_socket_add(temp_sock, temp_addr);
      if (curjob == NULL)
        {
#if __DEBUG_NETWORK__
	  fprintf(stderr, "[DEBUG NETWORK] We have "
		  "decided to abort the connection\n");
#endif
	  close(temp_sock);
	  XFREE(__FILE__, __FUNCTION__, __LINE__,temp_addr);
	  ret = -1;
        }
    }
  else
    {
      // Maybe he was afraid of us
#if __DEBUG_NETWORK__
      fprintf(stderr, "[DEBUG NETWORK] He might have been afraid.\n");
#endif
      XFREE(__FILE__, __FUNCTION__, __LINE__,temp_addr);
      ret = -1;
    }

  /* Display a prompt on this socket */
  if (ret >= 0)
    {
      oldjob = world.curjob;
      world.curjob = curjob;
      vm_output("\n [*] Connection granted to ELFsh daemon\n\n");
      vm_display_prompt();
      world.curjob = oldjob;
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}




/* One listening step */
int			vm_net_recvd(fd_set *sel_sockets)
{
  revmsock_t           *temp_socket;
  hashent_t             *actual;
  int                   index;
  revmjob_t		*init;
  int			sd;
  pkt_t			*data;
  revmjob_t		*new;
  char			tmp[24];
  char			logbuf[30];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  init = hash_get(&world.jobs, "net_init");

  // Who has moved ?
  for (index = 0; index < world.jobs.size; index++)
    {

      for (actual = &world.jobs.ent[index];
	   actual != NULL && actual->key != NULL;
	   actual = actual->next)
        {
	  if (!((revmjob_t *) actual->data)->ws.active)
	    continue;

	  temp_socket = &((revmjob_t *) actual->data)->ws.sock;

	  if (((revmjob_t *) actual->data)->ws.io.type != ELFSH_IONET)
	    continue;

	  if (temp_socket->socket == init->ws.sock.socket ||
	      temp_socket->socket == 0)
	    continue;

	  if (FD_ISSET(temp_socket->socket, sel_sockets))
            {
#if __DEBUG_NETWORK__
	      fprintf(stderr, "[DEBUG NETWORK] Client on socket %d "
		      "is trying to say something...\n",
		      temp_socket->socket);
#endif
	      // Let's update the revmsock_t recvd data */
	      if (vm_update_recvd(temp_socket) < 0)
		PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			     "Cannot receive on socket", -1);
            }
        }
    }


  // scan dump
  for (index = 0; index < dump_world.ports.size; index++)
    {
      for (actual = &dump_world.ports.ent[index];
           actual != NULL && actual->key != NULL;
           actual = actual->next)
        {
	  //if (!((revmjob_t *) actual->data)->ws.active)
	  //  continue;

	  sd = (long) actual->data;
	  if (FD_ISSET(sd, sel_sockets))
            {
#if __DEBUG_NETWORK__
              fprintf(stderr, "[+] Calling DUMP recv callback [%d]\n", sd);
#endif
              data = dump_receive_cb(sd);

              /* connection closed by libdump */
              if (data == (void *) (-1))
                {
#if __DEBUG_NETWORK__
                  fprintf(stderr, " socket %d considered as close \n", sd);
#endif
		  /* XXX delete pending jobs */

                  continue;
                }
#if __DEBUG_NETWORK__
	      dump_print_pkt((pkt_t *) data);
#endif

	      /* we have something to do */
	      if (data != NULL)
                {

#if __DEBUG_NETWORK__
                  fprintf(stderr, "[+] packet type : %s\n",
			  (((pkt_t *) data)->type   == htons(RR))   ? "RR"   :
			  ((((pkt_t *) data)->type  == htons(Rr))   ? "Rr"   :
			   ((((pkt_t *) data)->type == htons(DATA)) ? "DATA" :
			    "Unknown")));
#endif

		  /* sanity check */
		  if (ntohl(data->size) != 0)
		    {

		      if (ntohl(data->size) > 0)
			((char *)data->data)[ntohl(data->size) - 1] = '\0';

		      /* XXX TODO : change it to a hash table lookup - ym */
		      /* demux DUMP packets */

		      switch (((char *)((pkt_t *)data)->data)[0])
			{

			  /* elfsh cmd, pass it to vm_input() */
			case REVM_DUMP_CMD:
			  snprintf(tmp, sizeof (tmp)-1, "DUMP_%s", 
				   inet_ntoa(((pkt_t *)data)->src)); 

			  new = hash_get(&world.jobs, tmp);
			  if (new == NULL)
			    {
			      XALLOC(__FILE__, __FUNCTION__, __LINE__,
				     new, sizeof (revmjob_t), -1);
			      snprintf(logbuf, sizeof(logbuf), "%s_loaded", tmp);
			      hash_init(&new->loaded, logbuf, 
					51, ASPECT_TYPE_UNKNOW);
			      snprintf(logbuf, sizeof(logbuf), "%s_dbgloaded",tmp);
			      hash_init(&new->dbgloaded, logbuf,
					11, ASPECT_TYPE_UNKNOW);
			    }
			  else
			    {
			      if (new->ws.io.new == 1)
				PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
					     "Fatal internal DUMP error", -1);
			      hash_del(&world.jobs, tmp);
			    }

			  new->ws.io.type      = ELFSH_IODUMP;
			  new->ws.io.input_fd  = (-1);
			  new->ws.io.output_fd = (-1);
			  new->ws.io.input     = vm_dump_input;
			  new->ws.io.output    = vm_dump_output;
			  new->ws.io.new       = 1;
			  new->ws.io.pkt       = data;
			  new->ws.createtime   = time(&new->ws.createtime);
			  new->ws.active       = 1;
			  hash_add(&world.jobs, strdup(tmp), new);
 
			  break;
			  /* message, display it */
			case REVM_DUMP_MSG:
			  vm_output((char *)((pkt_t *) data)->data + 1);
			  dump_free(data);
			  break;
			  /* unknown */
			default:
			  vm_output("unknown ELFSH_DUMP packet type\n");
			  dump_free(data);
			  break;
			}
		    }
                }
#if __DEBUG_NETWORK__
              else
                fprintf(stderr, "[+] null (non-data packet)\n");
#endif
            }
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Merge buffers */
char	*vm_socket_merge_recvd(revmsock_t *socket)
{
  char	*ret;
  int	nb_recvd;
  int	i;
  int	size = 0;
  int	lsize;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  nb_recvd = vm_socket_get_nb_recvd(inet_ntoa(socket->addr.sin_addr));
  if (nb_recvd < 0)  
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Failed to receive on socket", NULL);    

  /* Let's calculate the length of buffer needed.*/
  for (i = 0; i < nb_recvd ; i++)
    size += strlen(socket->recvd[i]);
  
  XALLOC(__FILE__, __FUNCTION__, __LINE__,
	 ret , sizeof (char) * (size + 1), NULL);
  
  /* And now we have to fill it */
  /* First we reset size*/
  size  = 0;
  lsize = 0;
  for (i = 0; i < nb_recvd ; i++)
    {
      lsize = strlen(socket->recvd[i]);
      /* Smaller by one in order to discard the '\0'*/
      strncpy((ret+size), socket->recvd[i], lsize);
      size += lsize;
    }
  /* Important because we do not copy the last '\0'*/
  ret[size] = '\0';
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}



/* Return the first buffers */
char			*vm_net_input()
{
  revmsock_t		*temp_socket;
  hashent_t		*actual;
  int			index;
  char			*ret;
#if __DEBUG_NETWORK__
  int			i;
#endif

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_NETWORK__
  fprintf(stderr, "[DEBUG NETWORK] vm_net_input\n");
#endif

  for (index = 0; index < world.jobs.size; index++)
    {

      for (actual = &world.jobs.ent[index];
	   actual != NULL && actual->key != NULL;
	   actual = actual->next)
        {
	  if (!((revmjob_t *) actual->data)->ws.active)
	    continue;

	  temp_socket = & ((revmjob_t *) actual->data)->ws.sock;


	  if (temp_socket->recvd_f == NEW &&
	      temp_socket->ready_f == YES)
            {
#if __DEBUG_NETWORK__
	      fprintf(stderr, "[DEBUG NETWORK] vm_network_input "
		      "on %d -> NEW buffer\n",
		      temp_socket->socket);
#endif

	      ret = vm_socket_merge_recvd(temp_socket);

#if __DEBUG_NETWORK__
	      fprintf(stderr, "ret : %s\n", ret);
	      for (i = 0 ; i < strlen(ret) ; i++)
		fprintf(stderr, "%#x\n", ret[i]);
#endif

	      temp_socket->recvd_f = OLD;
	      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
            }
        }
    }

  /* No new buffer */
#if __DEBUG_NETWORK__
  fprintf(stderr, "[DEBUG NETWORK] vm_network_input no new buffer\n");
#endif 

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		(char *) REVM_INPUT_VOID);
}





/* Listening initialisation */
int			vm_net_init()
{
  revmjob_t		*init;
  struct sockaddr_in	dump_sockaddr_main;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  init = hash_get(&world.jobs, "net_init");
  if (init == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot find initial net job", (-1));

  /* remote client main socket */
  if (vm_create_server(&(init->ws.sock.socket), 
		       &(init->ws.sock.addr), REVM_PORT) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Failed to create net server", (-1));

  /* remote elfsh main socket (DUMP) */
  if (vm_create_server(&(dump_world.sock), 
		       &(dump_sockaddr_main), REVM_DUMP_PORT) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Fail to create dump server", (-1));

  /* initialisation of DUMP */
  if (dump_init() < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Failed to initialize DUMP", (-1));

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Stopping network | not workspace compliant ... */
int		      vm_net_stop()
{
  hashent_t           *actual;
  int                 index;
  revmsock_t         *temp_socket;
  revmjob_t	      *serv;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Closing all client's sockets*/
  for (index = 0; index < world.jobs.size; index++)
    {

      for (actual = &world.jobs.ent[index];
	   actual != NULL && actual->key != NULL;
	   actual = actual->next)
        {  
	  temp_socket = &((revmjob_t *) actual->data)->ws.sock;

#if __DEBUG_NETWORK__
	  fprintf(stderr, "[DEBUG NETWORK] Closing "
		  "socket : %d \n", temp_socket->socket);
#endif

	  if (vm_socket_del(inet_ntoa(temp_socket->addr.sin_addr)) < 0)
            {
#if __DEBUG_NETWORK__
	      fprintf(stderr, "[DEBUG NETWORK] error on deleting a socket\n");    
#endif
            }
        }
    }

  /* Closing serv socket */
  serv = hash_get(&world.jobs, "net_init");

#if __DEBUG_NETWORK__
  fprintf(stderr, "[DEBUG NETWORK] Closing socket : %d \n", 
	  serv->ws.sock.socket);
#endif

  /* Closing DUMP connections */
  for (index = 0; index < dump_world.ports.size; index++)
    {
      for (actual = &dump_world.ports.ent[index];
           actual != NULL && actual->key != NULL;
           actual = actual->next)
        {
	  dump_disconnect((long) actual->data);
	}
    }

  /* Closing DUMP main socket */
  close(dump_world.sock);
  if (serv == NULL || vm_socket_close(serv->ws.sock.socket) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Failed to close socket", (-1));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Clean jobs */
int			vm_clean_jobs()
{
  revmjob_t		*job;
  int			index;
  char			**keys;
  int			keynbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  keys = hash_get_keys(&world.jobs, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      job = hash_get(&world.jobs, keys[index]);
      switch (job->ws.io.type)
	{
	  
	case ELFSH_IODUMP:
	  if (job->ws.io.new == 0)
	    {
#if __DEBUG_NETWORK__
	      printf("CLEAN : %s\n", keys[index]);
#endif
	      /* If we delete jobs here, we do not have
		 session persistence and every load must be
		 done through shared load */
	      /* XXX can we free ws.io.pkt ?? */
	    }
	  break;
        }
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Case where the network is not enabled */
#else


int		vm_clean_jobs()
{
  return (0);
}

int		vm_getmaxfd() 
{ 
  return (world.fifofd); 
}

revmjob_t	*vm_socket_add(int socket, struct sockaddr_in *addr)
{
  return (NULL);
}

int		vm_socket_get_nb_recvd(char *inet)
{
  return (0);
}

int		vm_socket_close(int socket)
{
  return (0);
}

int		 vm_socket_del(char *inet_addr)
{
  return (0);
}

int		vm_netsend(char *buf, unsigned int len)
{
  return (0);
}

int		vm_net_output(char *buf)
{
  return (0);
}

int		vm_create_server(int *serv_sock, 
				 struct sockaddr_in *addr)
{
  return (0);
}

int		vm_update_recvd(revmsock_t *socket)
{
  return (0);
}

int		vm_net_accept_connection()
{
  return (0);
}

int		vm_dump_accept_connection()
{
  return (0);
}

int		vm_net_recvd(fd_set *sel_sockets)
{
  return (0);
}

char		*vm_socket_merge_recvd(revmsock_t *socket)
{
  return (NULL);
}

char		*vm_net_input()
{
  return (NULL);
}

int		vm_net_init()
{
  return (0);
}

int		vm_net_stop()
{
  return (0);
}

#endif


