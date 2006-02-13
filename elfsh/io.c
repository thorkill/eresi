/*
** io.c for elfsh
** 
** Started on  Fri Mar  5 00:55:40 2004 mayhem
** Updated on  Fri Mar 11 18:47:41 2005 ym
*/
#include "elfsh.h"


/* Display str on all term */
int		vm_output_bcast(char *str)
{
  hashent_t	*actual;
  int		index;
  int		ret = 0;
  elfshjob_t	*old;
  
  /* Saving current output parameters */
  old = world.curjob;
  
  /* Net's outputs */
  if (world.state.vm_net)
    {
      for (index = 0; index < world.jobs.size; index++)
        {
	  for (actual = &world.jobs.ent[index];
	       actual != NULL && actual->key != NULL;
	       actual = actual->next)
            {
	      if (!strcmp(actual->key, "local")    ||
		  !strcmp(actual->key, "net_init") ||
		  !strncmp(actual->key, "DUMP", 4) ||
		  !((elfshjob_t *)actual->data)->active);
	      continue;
	      
	      world.curjob = actual->data;
	      ret |= vm_output(str);
            }
        }
    }
  
  /* stdout */
  if (world.state.vm_mode != ELFSH_VMSTATE_CMDLINE)
    {
      world.curjob = hash_get(&world.jobs, "local");
      ret |= vm_output(str);
    }
  
  /* Restore */
  world.curjob = old;
  return ret;
}

/* Display the prompt */
int		vm_display_prompt()
{
  char		*buf;

  buf = ((world.state.vm_mode == ELFSH_VMSTATE_IMODE ||
	  world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER || 
	  (world.state.vm_net && world.curjob->io.type != ELFSH_IOSTD)) ? 
	 vm_get_prompt() : "");
  vm_output(buf);
  return 0;
}


/* OUTPUT handler for stdout */
int		vm_output(char *str)
{
  return (world.curjob->io.output(str));
}


/* I was too lazy to use flex for the moment, pardon me ;) */
char		**vm_input(int *argc)
{
  char		**argv;
  char		*buf;
  int		nbr;

  /* Read, and sanitize a first time to avoid blank lines */
  buf = ((world.state.vm_mode == ELFSH_VMSTATE_IMODE || 
	  world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER) ? 
	 vm_get_prompt() : "");
  buf = vm_getln(buf);
  if (!buf)
    return (NULL);

  /* Blank Line*/
  if (buf == (char *)ELFSH_VOID_INPUT)
    return ((char **)ELFSH_VOID_INPUT);

  
#if defined(USE_READLN)
  //    printf ("[READLN] history : [%s]\n", buf);
  if ( world.state.vm_mode != ELFSH_VMSTATE_SCRIPT && 
       world.curjob->io.type == ELFSH_IOSTD)
    add_history(buf);
#endif
  
  /* Allocate the correct pointer array for argv */
  nbr = vm_findblanks(buf);
  argv = vm_doargv(nbr, argc, buf);
  
  /* Find and replace "\xXX" sequences, then return the array */
  vm_findhex(*argc, argv);
  return (argv);
}



/* OUTPUT handler for stdout */
int		vm_stdoutput(char *str)
{
  printf("%s", str);
  fflush(stdout);
  return (0);
}


/* INPUT handler for stdin */
char		*vm_stdinput()
{
  char		tmpbuf[BUFSIZ + 1];
  int		len;
  
#if defined(USE_READLN)
  if (world.state.vm_mode == ELFSH_VMSTATE_SCRIPT)
    {
#endif
      for (len = 0; len < BUFSIZ; len++)
	switch (read(world.curjob->io.input_fd, tmpbuf + len, 1))
          {
	  case 0:
	    *tmpbuf = 0x00;
	    goto end;
	  case 1:
	    if (tmpbuf[len] == '\n')
	      {
		if (len == 0)
		  {
		    //    if(world.state.vm_mode == ELFSH_VMSTATE_SCRIPT) 
		    //        return (vm_stdinput());
		    //    else
		    return ((char *)ELFSH_VOID_INPUT);
		  }
		tmpbuf[len] = 0x00;
		goto end;
	      }
	    break;
	  default:
	    *tmpbuf = 0x00;
	    goto end;
          }


    end:
      return (*tmpbuf ? strdup(tmpbuf) : NULL);
#if defined(USE_READLN)
    }
  else
    {
      /* input in progress */ 
      if (world.curjob->io.buf == NULL)
	{
          return ((char *)ELFSH_VOID_INPUT);
	}
       
      /* CTRL-D case */
      if (world.curjob->io.buf == (char *)ELFSH_EXIT_INPUT)
	{
          return (NULL);
	}
      
      /* empty string */
      if (strlen(world.curjob->io.buf) == 0)
	{
          return ((char *)ELFSH_VOID_INPUT);
	}
      
      return (strdup(world.curjob->io.buf));
    }
#endif
}



/* Initialize Input/Output hooks */
int		vm_initio()
{
  elfshjob_t	*initial;
  
  XALLOC(initial, sizeof(elfshjob_t), -1);
  initial->io.type      = ELFSH_IOSTD;
  initial->io.input_fd  = 0;
  initial->io.input     = vm_stdinput;
  initial->io.output_fd = 1;
  initial->io.output    = vm_stdoutput;
  initial->active	= 1;
  initial->createtime = time(&initial->createtime);
  world.initial = world.curjob = initial;

  hash_init(&world.jobs, 11);

  hash_add(&world.jobs, "local", initial);

  return (0);
}

/* Return the greatest socket from the elfsh_net_client_list and sock. */
int             elfsh_getmaxfd()
{
  int           ret;
  elfshjob_t    *serv;

#if  defined(ELFSHNET)
  int           index;
  hashent_t     *actual;
#endif

  /* If the network is not UP, the biggest fd is 0 */
  serv = hash_get(&world.jobs, "net_init");
  if (serv == NULL)
    return (0);

  ret = serv->sock.socket;

#if  defined(ELFSHNET)
  /* remote elfsh (DUMP) */
  ret = (ret > dump_world.sock) ? ret : dump_world.sock;
  
  for (index = 0; index < dump_world.ports.size; index++)
    {
      for (actual = &dump_world.ports.ent[index];
           actual != NULL && actual->key != NULL;
           actual = actual->next)
        {
	  if (!((elfshjob_t *) actual->data)->active)
	    continue;
#if __DEBUG_NETWORK__
          fprintf(stderr,
		  "[DEBUG NETWORK] Socket (DUMP) [%u] \n",
		  (int) actual->data);
           
#endif
	  if ((long) actual->data > ret)
	    ret = (long) actual->data;
	}
    }

  /* remote client */
  for (index = 0; index < world.jobs.size; index++)
    {
      for (actual = &world.jobs.ent[index];
           actual != NULL && actual->key != NULL;
           actual = actual->next)
        {
	  
	  if (!((elfshjob_t *) actual->data)->active)
	    continue;
#if __DEBUG_NETWORK__
          fprintf(stderr,
		  "[DEBUG NETWORK] Socket [%u] key = %10s ; next = %p\n",
		  ((elfshjob_t *) actual->data)->sock.socket,
		  actual->key, actual->next);
#endif

          if (((elfshjob_t *) actual->data)->sock.socket > ret)
            ret = ((elfshjob_t *) actual->data)->sock.socket;
        }
    }
#endif

  return ret;
}

/*
** Add a main socket and client's sockets to the sockets list used by select
** and call get_max_fd to get the greatest.
*/
int		elfsh_prepare4select(fd_set *sel_sockets)
{
  hashent_t     *actual;
  int           index;

#if defined(ELFSHNET)
  /* remote elfsh (DUMP) */
  for (index = 0; index < dump_world.ports.size; index++)
    {
      for (actual = &dump_world.ports.ent[index];
           actual != NULL && actual->key != NULL;
           actual = actual->next)
        {
	  if (!((elfshjob_t *) actual->data)->active)
	    continue;

#if __DEBUG_NETWORK__
          fprintf(stderr, "[DEBUG NETWORK] prepare_4_select : (DUMP) socket : %d \n",
		  (int) actual->data);
#endif
          FD_SET((long) actual->data, sel_sockets);
        }
    }
#endif
  
  /* remote client */
  for (index = 0; index < world.jobs.size; index++)
    {
      for (actual = &world.jobs.ent[index];
           actual != NULL && actual->key != NULL;
           actual = actual->next)
        {
	  if (!((elfshjob_t *) actual->data)->active)
	    continue;

#if _DEBUG_NETWORK__
	  fprintf(stderr, "[DEBUG NETWORK] prepare_4_select : socket : %d \n",
		  ((elfshjob_t *)actual->data)->sock.socket);
#endif
#if defined(ELFSHNET)
	  if (((elfshjob_t *) actual->data)->io.type == ELFSH_IODUMP)
	    continue;

	  if (((elfshjob_t *) actual->data)->io.type == ELFSH_IONET)	  
	    FD_SET(((elfshjob_t *) actual->data)->sock.socket, sel_sockets);
#endif
	  
	  if (((elfshjob_t *) actual->data)->io.type == ELFSH_IOSTD)
	    FD_SET(((elfshjob_t *) actual->data)->io.input_fd, sel_sockets);
	}
      
    }
  return elfsh_getmaxfd();
}

/* Set IO to the choosen socket */
int			vm_socket_getnew()
{
  hashent_t		*actual;
  int			index;

  for (index = 0; index < world.jobs.size; index++)
    {
      for (actual = &world.jobs.ent[index];
           actual != NULL && actual->key != NULL;
           actual = actual->next)
        { 
	  if (!((elfshjob_t *) actual->data)->active)
	    continue;

	  if (((elfshjob_t *) actual->data)->io.type == ELFSH_IODUMP && 
	      ((elfshjob_t *) actual->data)->io.new != 0)
	    {
	      world.curjob = (elfshjob_t *) actual->data;
	      return 1;
	    }
	  
	  if (((elfshjob_t *) actual->data)->io.type == ELFSH_IONET)
	    {
	      
	      if (((elfshjob_t *) actual->data)->sock.recvd_f == NEW &&
		  ((elfshjob_t *) actual->data)->sock.ready_f == YES)
		{
		  world.curjob = (elfshjob_t *) actual->data;
		  return 1;
		}
	    }
	}
    }
  return 0;
}

/* Clean jobs */
int                   vm_clean_jobs()
{
  hashent_t           *actual;
  int                 index;

  for (index = 0; index < world.jobs.size; index++)
    {
      for (actual = &world.jobs.ent[index];
           actual != NULL && actual->key != NULL;
           actual = actual->next)
        {

	  switch (((elfshjob_t *) actual->data)->io.type)
	    {
	    case ELFSH_IODUMP:
#if defined(ELFSHNET)
	      if (((elfshjob_t *) actual->data)->io.new == 0)
		{
#if __DEBUG_NETWORK__
		  printf("CLEAN : %s\n", actual->key);
#endif
		  /* if we delete jobs here, we do not have 
		     session persistence and every load must be
		     done through shared load */

		  /* XXX can we free io.pkt ?? */

		  //		  dump_free (((elfshjob_t *) actual->data)->io.pkt);
		  //		  free (((elfshjob_t *) actual->data));
		  //		  hash_del (&world.jobs, actual->key);
		}
#endif
	      break;
	    default:
	      break;
	    }
        }
    }
  return 0;
}

/* Return the current local job */
elfshjob_t		*vm_get_curlocaljob()
{
  hashent_t		*actual;
  int			index;

  for (index = 0; index < world.jobs.size; index++)
    {
      for (actual = &world.jobs.ent[index];
           actual != NULL && actual->key != NULL;
           actual = actual->next)
        { 
	  if (!((elfshjob_t *) actual->data)->active)
	    continue;

	  if (((elfshjob_t *) actual->data)->io.type == ELFSH_IOSTD)
	    return actual->data;

	}
    }
  
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "No active local job", NULL);
}

/* Wait for all input */
int                     vm_select()
{
  fd_set		sel_sockets;
  int                   max_fd;
  int                   cont;
  elfshjob_t            *init;



  // By default do only one select.
  init = hash_get(&world.jobs, "net_init");
  cont = 0;

  /* In case of not already handle data */
  if (vm_socket_getnew())
    return 0;

  /* clean jobs hash table */
  vm_clean_jobs();

  do
    {
      FD_ZERO(&sel_sockets);

#if defined(ELFSHNET)
      if (world.state.vm_net && init)
        {
          // We add the serv_sock to the sockets list for select
          FD_SET(init->sock.socket, &sel_sockets);
	  /* add DUMP main socket */
	  FD_SET(dump_world.sock, &sel_sockets);
        }
#endif

      // Prepare for the select() call
      max_fd = elfsh_prepare4select(&sel_sockets);

      // In the case of normal loop print prompt
      if (cont == 0 && 
	  (world.state.vm_mode != ELFSH_VMSTATE_CMDLINE || 
	   world.state.vm_net))
        {   
	  if (world.curjob->io.type != ELFSH_IODUMP)
	    {
	      
	      // Display prompt
#if defined(USE_READLN)
	      if (world.curjob->io.type == ELFSH_IOSTD)
		{
		  if (world.curjob->io.buf != NULL)
		    {
		      rl_forced_update_display(); 
		    }
		}
	      else
#endif
		{
		  vm_display_prompt();
		}
	    }
        }

      // Reset cont if a loop has already be done
      cont = 0;

      if (select(max_fd + 1, &sel_sockets, NULL, NULL, NULL) > 0)
        {
#if defined(ELFSHNET)
          if (world.state.vm_net && init)
            {
              // Read net command if any.
              if (vm_net_recvd(&sel_sockets) < 0)
                fprintf(stderr, "vmnet_select : vm_net_recvd()"
			" failed\n");

              // Is there anybody out there (remote client) ?
              if (FD_ISSET(init->sock.socket, &sel_sockets))
                {
                  if (vm_net_accept() < 0)
                    fprintf(stderr, "Connection rejected\n");
                }

              // Is there anybody out there (DUMP) ?
              if (FD_ISSET(dump_world.sock, &sel_sockets))
                {
                  if (vm_dump_accept() < 0)
                    fprintf(stderr, "Connection rejected\n");
                }
            }
#endif
          if (world.state.vm_mode != ELFSH_VMSTATE_CMDLINE)
            {
              if (FD_ISSET(0, &sel_sockets))
                {
		  world.curjob = vm_get_curlocaljob();
		  if (!world.curjob)
		    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
				      "Local job not found",
				      -1);		    

#if defined (USE_READLN)
		  world.curjob->io.buf = NULL;

		  rl_callback_read_char();

		  if (world.curjob->io.buf != NULL)
                    {
		      /* restore prompt that will be display
		       * on next  rl_forced_update_display()
		       * or rl_callback_read_char() */
		      rl_restore_prompt();
		      return 0;
                    }
#endif
		  return 0;
                }
            }

          // Select which command will be proceded
          if (world.state.vm_net)
            {
              if (vm_socket_getnew())
                {
                  return 0;
                }
              else
                {
#if __DEBUG_NETWORK__
                  fprintf(stderr, "[DEBUG NETWORK] Select broken by"
			  " a new connexion.\n");
#endif
                  // Let's re-select
                  cont = 1;
                }
            }
	} 
    } while (cont);
  
  return 0;
}

/* readline line handler */
void    vm_ln_handler(char *c)
{

#if defined(USE_READLN)
  world.curjob->io.buf = c;
  if (c != NULL)
    {
      /* save (remove) prompt if a complete line was typed 
       * so that the line is not displayed on return */
      rl_save_prompt();
    }
  if (c == NULL)
    {
      /* special to enable exit on CTRL-D */
      world.curjob->io.buf = (char *)ELFSH_EXIT_INPUT;
      rl_save_prompt();
    }
#endif
  return;
}


/* Change the Input file */
void	vm_setinput(elfshjob_t *j, int fd)
{
  j->io.input_fd = fd;
}

/* Change the Input file */
void	vm_setoutput(elfshjob_t *j, int fd)
{
  j->io.output_fd = fd;
}
