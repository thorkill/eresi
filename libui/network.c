/*
** network.c for elfsh
** 
** Started on  Wed Feb 25 22:22:35 2004 yann_malcom 
*/

#include "elfsh.h"


/* Number of clients connected */
int  	       elfsh_net_client_count = 0;

/* Is net support enable ? */
#if defined(ELFSHNET)
/* Add a client socket to the elfsh_net_client_list. */
elfshjob_t	*vm_socket_add(int socket, struct sockaddr_in *addr)
{
  elfshjob_t	*new;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* If too many clients return (-1) */
  if (elfsh_net_client_count >= MAX_CLIENTS)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "???", (NULL));


  /* If there is already a client by that inet address return (-1) */
  if (hash_get(&world.jobs, inet_ntoa(addr->sin_addr)) != NULL)
    {
#if __DEBUG_NETWORK__
      fprintf(stderr, "[DEBUG NETWORK] Already connected from that addr.\n");
#endif
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "???", (NULL));    
    }
  
#if __DEBUG_NETWORK__
  fprintf(stderr, "[DEBUG NETWORK] adding client on socket %d .\n", socket);
#endif  

  XALLOC(new, sizeof (elfshjob_t), NULL);
  
  new->sock.addr    = *addr;
  new->sock.socket  = socket;
  new->sock.recvd   = NULL;
  new->sock.recvd_f = OLD;
  elfsh_net_client_count++;
  
  new->io.type      = ELFSH_IONET;
  new->io.input_fd  = socket;
  new->io.output_fd = socket;
  new->io.input     = vm_net_input;
  new->io.output    = vm_net_output;
  
  new->createtime   = time(&new->createtime);
  new->active	    = 1;

  hash_add(&world.jobs, inet_ntoa(addr->sin_addr), new);
  
  world.curjob = new;
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}

/* Return the number of buffer in a elfshsock_t recvd */
int		vm_socket_get_nb_recvd(char *inet)
{
  elfshjob_t	*tmp;
  int		i;
  int		index;
  elfshjob_t	*old;
  hashent_t	*actual;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Is there a socket by that name */
  tmp = hash_get(&world.jobs, inet);
  
  if (tmp != NULL)
    {
      if (!tmp->active)
	{
	  for (index = 0; index < world.jobs.size; index++)
	    {
	      for (actual = &world.jobs.ent[index];
		   actual != NULL && actual->key != NULL;
		   actual = actual->next)
		{ 
		  old = world.curjob;
		  world.curjob = tmp;
		  
		  if (vm_own_job((elfshjob_t *) actual->data) && ((elfshjob_t *) actual->data)->active)
		    {
		      world.curjob = old;
		      tmp = (elfshjob_t *) actual->data;
		    }
		  
		  world.curjob = old;
		}
	}	  


	}


      if (tmp->sock.recvd_f == OLD)
        {
	  /* recvd data have already been read so we forget them */
	  tmp->sock.recvd = NULL;
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
        }
      else
        {
	  /* we have to count them */
	  if (tmp->sock.recvd != NULL)
            {
	      /* There might be data */
	      i = 0;
	      while (tmp->sock.recvd[i] != NULL)
                {
		  i++;
                }
	      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, i);
            }
	  else
	    /* recvd is NULL so return 0  but marked as NEW */
            {
#if __DEBUG_NETWORK__ 
	      fprintf(stderr, "[DEBUG NETWORK] Not really a consistant"
		      " state in a elfshsock_t recvd data.\n");
#endif
	      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
            }
        }

    }
  else
    {
#if __DEBUG_NETWORK__ 
      fprintf(stderr, "[DEBUG NETWORK] No socket by that inet addr.\n");
#endif      
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "???", (-1));
    }
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "???", (-1));
}

/* Only close a socket */
int elfsh_socket_close(int socket)
{

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (shutdown(socket, SHUT_RDWR) < 0)
    {
#if __DEBUG_NETWORK__
      fprintf(stderr, "[DEBUG NETWORK] error in shutdown "
	      "on socket %d\n", socket);
#endif
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "???", (-1));
    }

  if (close(socket) < 0)
    {
#if __DEBUG_NETWORK__
      fprintf(stderr, "[DEBUG NETWORK] error in close "
	      "on socket %d\n", socket);       
#endif
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "???", (-1));
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* 
** Remove a client socket from the list and close it, return (-1) if socket 
** not found. 
*/
int		 vm_socket_del(char *inet_addr)
{
  elfshjob_t	 *tmp;
  int		 nbargc;
  int		 i;
  hashent_t	*actual;
  int		index;
  elfshjob_t	*old;


  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  tmp = hash_get(&world.jobs, inet_addr);
  if (tmp != NULL)
    {   

      for (index = 0; index < world.jobs.size; index++)
	{
	  for (actual = &world.jobs.ent[index];
	       actual != NULL && actual->key != NULL;
	       actual = actual->next)
	    { 
	      old = world.curjob;
	      world.curjob = tmp;

	      if (vm_own_job((elfshjob_t *) actual->data))
		{
		  hash_del(&world.jobs, actual->key);
		}
	      
	      world.curjob = old;

	    }
	}
      
      if (elfsh_socket_close(tmp->sock.socket) < 0)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "???", (-1));
      //      elfsh_net_client_count--;

      if (tmp->sock.recvd_f == NEW)
        {
	  /* We assume that buffer are not referenced, and so won't be used */
	  nbargc = vm_socket_get_nb_recvd(inet_ntoa(tmp->sock.addr.sin_addr));
	  for (i = 0 ; i < nbargc ; i++)
            {
	      XFREE (tmp->sock.recvd[i]);
            }
#if __DEBUG_NETWORK__
	  fprintf(stderr, "[DEBUG NETWORK] We are deleting a socket struct which has"
		 "new received data.\n");
#endif
        }
      XFREE(tmp->sock.recvd);
      hash_del(&world.jobs, inet_addr);
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  else
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "???", (-1));
}

/* Really send the data */
int elfsh_net_send(char *buf, unsigned int len)
{

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (len > strlen(buf))
    {
#if __DEBUG_NETWORK__
      fprintf(stderr, "[DEBUG NETWORK] specified len beyond the end of buf on socket %d\n", 
	      world.curjob->io.output_fd);
#endif
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "???", (-1));
    }
  if (send(world.curjob->io.output_fd, (void *) buf, len, NULL) < 0)
    {
#if __DEBUG_NETWORK__
      fprintf(stderr, "[DEBUG NETWORK] send failed on socket %d\n", 
	      world.curjob->io.output_fd);
#endif
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "???", (-1));
    };
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* send buf on a socket */
int	vm_net_output(char *buf)
{
  int	ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (strlen(buf) > MAX_SEND_SIZE)
    {
#if __DEBUG_NETWORK__
      fprintf(stderr, "[DEBUG NETWORK] Socket [%u] attempt to "
	      "send too much data.\n",
	      world.curjob->io.output_fd);
#endif	  
      if (elfsh_net_send(buf, MAX_SEND_SIZE) >= 0)
	{
	  ret = vm_net_output((char *) (buf + MAX_SEND_SIZE));
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
	}
      else 
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "???", (-1));
    }

  ret = elfsh_net_send(buf, strlen(buf));    
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}

/* Create le main socket and bind it to ELFSH_LISTEN_PORT. */
int	elfsh_net_create_serv_socket(int			*serv_sock, 
				     struct sockaddr_in		*addr,
				     u_int			port)
{

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  int	reuseaddr = 1;

  // Let's create the main socket
  if  (((*serv_sock) = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      FATAL("socket");
    }

  if (setsockopt((*serv_sock), SOL_SOCKET, SO_REUSEADDR, &reuseaddr, 
		 sizeof (reuseaddr)) < 0)
    {
      FATAL("setsockopt");
    }

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
    {
      FATAL("listen");
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Update elfshsock_t recvd buffer */
int		vm_update_recvd(elfshsock_t *socket)
{
  char		*buf;
  char		**buf2;
  int		size;
  int		i = 0;
  int		bufnb;
  char		**oldrecvd;
  char		*c;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Allocating space for receiving data ... */
  XALLOC(buf, sizeof(char) * BUFSIZ, NULL);

  size = read(socket->socket, buf, BUFSIZ - 1);
  if (size < 0)
    {

#if __DEBUG_NETWORK__
      fprintf(stderr, "[DBG_NETWORK] read(%d) failed with ret = %d \n", 
	      socket->socket, size);
#endif

      perror("read");
      XFREE(buf);
      vm_exit(-1);
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
	      XFREE(buf);
	      vm_socket_del(inet_ntoa(socket->addr.sin_addr));
	      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
            }
	  else
            {
	      //TODO : XFREE all buffer, if buffers are marked OLD,
	      //vm_net_input has been called so vm_socket_merge_recvd too.
	      XFREE (buf);
	      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
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
        {
	  fprintf(stderr, "[DBG_NETWORK] something got wrong in"
		  " vm_socket_nb_recvd()\n");
	  vm_exit(-1);
        }

      /*
      ** We have the old number of buffer and they have not been read so
      ** we have to append the new one at the end.
      */
      else
        {
	  /* +1 for the new (char *) and +1 for the NULL */
	  XALLOC(buf2, sizeof (char *)*(bufnb + 1 + 1), NULL);
	  /* Now let's copy the old buffer's pointers*/
	  oldrecvd = socket->recvd;
	  for (i = 0; i < bufnb; i++)
            {
	      buf2[i] = oldrecvd[i];
            }
	  /* Now we append the new buffer's pointer and set the NEW flag*/
#if __DEBUG_NETWORK__
	  fprintf(stderr, "buf : %s",buf);
#endif
	  buf2[i++] = buf;
	  buf2[i++] = NULL;
	  socket->recvd = buf2;
	  socket->recvd_f = NEW;
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
        }
    }
  else
    {
      /* buffers have already be read so we can overwrite 
	 recvd and set recvd_flag to NEW */
      XALLOC(buf2, sizeof (char *)*2, NULL);
#if __DEBUG_NETWORK__
      fprintf(stderr, "buf : %s", buf);
#endif
      buf2[0] = buf;
      buf2[1] = NULL;
      socket->recvd = buf2;
      socket->recvd_f = NEW;
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* Never reached, just to shut gcc up */
  return (-ELFSH_MEANING);
}

/* send buf on a dump connection */
int	vm_dump_output(char *buf)
{
  int	ret = 0;
#if defined(ELFSHNET)
  char	*tmp;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(tmp, strlen(buf) + 1 + 1, NULL);
  
  tmp[0] = ELFSH_DUMP_MSG;

  memcpy(tmp + 1, buf, strlen(buf) + 1);

  ret = dump_send(world.curjob->io.pkt->src, tmp, strlen(buf) + 1 + 1);

  //  XFREE (tmp);
#endif
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}

/*Return the first packet data */
char	*vm_dump_input()
{
#if defined(ELFSHNET)
  char	*tmp;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  tmp = elfsh_strdup((char *)(world.curjob->io.pkt->data) + 1);
  
  world.curjob->io.new = 0;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (tmp));
#endif
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "???", (NULL));
}

/* accept a DUMP connection */
int			vm_dump_accept()
{
  struct sockaddr_in    cli_addr;
  int                   cli_len;
  int			new_sd;

  struct sockaddr       loc;
  socklen_t             lloc = sizeof (struct sockaddr);

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

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

      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "???", (-1));
}


/* Accept new connection if there is */
int			vm_net_accept()
{
  socklen_t		temp_addr_len;
  struct sockaddr_in	*temp_addr;
  int			temp_sock;
  int			ret = 0;
  elfshjob_t		*curjob;
  elfshjob_t		*oldjob;
  elfshjob_t		*init;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* There was noise on main socket */
#if __DEBUG_NETWORK__
  fprintf(stderr, "[DEBUG NETWORK] There is somebody"
	  " knocking at the door...\n");
#endif

  init = hash_get(&world.jobs, "net_init");
  if (init == NULL)
    {
      fprintf(stderr, "net_init not found in net_accept_connection \n");
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "???", (-1));
    }

  temp_addr_len = sizeof (struct sockaddr_in);
  XALLOC(temp_addr, temp_addr_len, NULL);
  temp_sock = accept(init->sock.socket, 
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
	  XFREE(temp_addr);
	  ret = -1;
        }
    }  
  else
    {
      // Maybe he was afraid of us
#if __DEBUG_NETWORK__
      fprintf(stderr, "[DEBUG NETWORK] He might have been afraid.\n");
#endif
      XFREE(temp_addr);
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
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}


/* One listening step */
int			vm_net_recvd(fd_set *sel_sockets)
{
  elfshsock_t           *temp_socket;
  hashent_t             *actual;
  int                   index;
  elfshjob_t		*init;
  int			sd;
  pkt_t			*data;
  elfshjob_t		*new;
  char			tmp[24];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  init = hash_get(&world.jobs, "net_init");

  // Who has moved ?
  for (index = 0; index < world.jobs.size; index++)
    {

      for (actual = &world.jobs.ent[index];
	   actual != NULL && actual->key != NULL;
	   actual = actual->next)
        {
	  if (!((elfshjob_t *) actual->data)->active)
	    continue;

	  temp_socket = &((elfshjob_t *) actual->data)->sock;

	  if (((elfshjob_t *) actual->data)->io.type != ELFSH_IONET)
	    continue;

	  if (temp_socket->socket == init->sock.socket ||
	      temp_socket->socket == 0)
	    continue;

	  if (FD_ISSET(temp_socket->socket, sel_sockets))
            {
#if __DEBUG_NETWORK__
	      fprintf(stderr, "[DEBUG NETWORK] Client on socket %d "
		      "is trying to say something...\n",
		      temp_socket->socket);
#endif
	      // Let's update the elfshsock_t recvd data */
	      if (vm_update_recvd(temp_socket) < 0)
                {
		  fprintf(stderr, "network : vm_update_recvd() "
			  "error (that should not occur).\n");
		  vm_exit(-1);
                }
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
	  //if (!((elfshjob_t *) actual->data)->active)
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
			 (((pkt_t *) data)->type == htons(RR))?
			 "RR":((((pkt_t *) data)->type ==htons(Rr))?
			       "Rr":((((pkt_t *) data)->type ==htons(DATA))?
				     "DATA":"Unknown")));
#endif

		  /* sanity check */
		  if (ntohl(data->size) != 0)
		    {
		      
		      if (ntohl(data->size) > 0)
			((char *)data->data)[ntohl(data->size) - 1] = '\0';

		      /*
			XXX TODO : 
			change it to a hash table lookup - ym
		      */

		      /* demux DUMP packets */
		      switch (((char *)((pkt_t *)data)->data)[0])
			{
			  /* elfsh cmd, pass it to vm_input() */
			case ELFSH_DUMP_CMD:
			  snprintf(tmp, sizeof (tmp)-1, "DUMP_%s", inet_ntoa(((pkt_t *)data)->src)); 
			  
			  new = hash_get(&world.jobs, tmp);
			  
			  if (new == NULL)
			    {
			      XALLOC(new, sizeof (elfshjob_t), NULL);
			    }
			  else
			    {
			      if (new->io.new == 1)
				{
				  printf("[DUMP] BUG BUG BUG BUG \n");
				}
			      hash_del(&world.jobs, tmp);
			    }
			  
			  new->io.type      = ELFSH_IODUMP;
			  new->io.input_fd  = (-1);
			  new->io.output_fd = (-1);
			  new->io.input     = vm_dump_input;
			  new->io.output    = vm_dump_output;
			  new->io.new       = 1;
			  new->io.pkt       = data;
			  new->createtime = time(&new->createtime);
			  new->active	    = 1;
			  hash_add(&world.jobs, elfsh_strdup(tmp), new);
			  break;
			  /* message, display it */
			case ELFSH_DUMP_MSG:
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



  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);            
}



/* Merge buffers */
char	*vm_socket_merge_recvd(elfshsock_t *socket)
{
  char	*ret;
  int	nb_recvd;
  int	i;
  int	size = 0;
  int	lsize;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  nb_recvd = vm_socket_get_nb_recvd(inet_ntoa(socket->addr.sin_addr));
  if (nb_recvd < 0)  
    {
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "???", (NULL));    
    }
  else
    {
      /* Let's calculate the length of buffer needed.*/
      for (i = 0; i < nb_recvd ; i++)
        {
	  size += strlen(socket->recvd[i]);
        }

      XALLOC(ret , sizeof (char) * (size + 1), NULL);

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
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret)); 
    }
}

/*Return the first buffers */
char			*vm_net_input()
{
  elfshsock_t		*temp_socket;
  hashent_t		*actual;
  int			index;
  char			*ret;
#if __DEBUG_NETWORK__
  int			i;
#endif

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_NETWORK__
  fprintf(stderr, "[DEBUG NETWORK] vm_net_input\n");
#endif

  for (index = 0; index < world.jobs.size; index++)
    {

      for (actual = &world.jobs.ent[index];
	   actual != NULL && actual->key != NULL;
	   actual = actual->next)
        {
	  if (!((elfshjob_t *) actual->data)->active)
	    continue;

	  temp_socket = & ((elfshjob_t *) actual->data)->sock;


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
	      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
            }
        }
    }

  /* No new buffer */
#if __DEBUG_NETWORK__
  fprintf(stderr, "[DEBUG NETWORK] vm_network_input no new buffer\n");
#endif 

  return ((char *) ELFSH_VOID_INPUT);
}

/* Listening initialisation */
int			vm_net_init()
{
  elfshjob_t		*init;
  struct sockaddr_in	dump_sockaddr_main;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  init = hash_get(&world.jobs, "net_init");
  if (init == NULL)
    {
      fprintf(stderr, "Cannot find netinit job ! \n");
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "???", (-1));
    }

  /* remote client main socket */
  if (elfsh_net_create_serv_socket(&(init->sock.socket), &(init->sock.addr), ELFSH_PORT) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "???", (-1));

  /* remote elfsh main socket (DUMP) */
  if (elfsh_net_create_serv_socket(&(dump_world.sock), &(dump_sockaddr_main), ELFSH_DUMP_PORT) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "???", (-1));

  /* initialisation of DUMP */
  if (dump_init() < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "???", (-1));

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Stopping network | not workspace compliant ... */
int		      vm_net_stop()
{
  hashent_t           *actual;
  int                 index;
  elfshsock_t         *temp_socket;
  elfshjob_t	      *serv;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Closing all client's sockets*/
  for (index = 0; index < world.jobs.size; index++)
    {

      for (actual = &world.jobs.ent[index];
	   actual != NULL && actual->key != NULL;
	   actual = actual->next)
        {  
	  temp_socket = &((elfshjob_t *) actual->data)->sock;

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
	  serv->sock.socket);
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

  if (serv == NULL || 
      elfsh_socket_close(serv->sock.socket) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "???", (-1));
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


#else


elfshjob_t	*vm_socket_add(int socket, struct sockaddr_in *addr)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "???", (NULL));
}

int		vm_socket_get_nb_recvd(char *inet)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		elfsh_socket_close(int socket)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		 vm_socket_del(char *inet_addr)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		elfsh_net_send(char *buf, unsigned int len)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		vm_net_output(char *buf)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0); 
}

int		elfsh_net_create_serv_socket(int *serv_sock, 
                                     struct sockaddr_in	*addr)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		vm_update_recvd(elfshsock_t *socket)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		vm_net_accept_connection()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		vm_dump_accept_connection()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		vm_net_recvd(fd_set *sel_sockets)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);            
}

char		*vm_socket_merge_recvd(elfshsock_t *socket)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "???", (NULL)); 
}

char		*vm_net_input()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "???", (NULL));
}

int		vm_net_init()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		vm_net_stop()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

#endif


