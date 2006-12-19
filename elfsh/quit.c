/*
** quit.c for elfsh
** 
** Started on  Sat Jan 25 11:19:53 2003 mayhem
*/
#include "elfsh.h"


/* Unload all the loaded object and exit the shell */
int		cmd_quit()
{
  elfshobj_t	*cur;
  int		index;
  char		logbuf[BUFSIZ];
  hashent_t	*actual;
  int		hashidx;
  char		**keys;
  int		keynbr;
  int		z;
  elfshjob_t	*curjob;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Do not unload files if we were sourcing a script */
  if (world.curjob->sourced == 0)
    {
      keys = hash_get_keys(&world.jobs, &keynbr);
      
      /* Unload every workspace */
      for (z = 0; z < keynbr; z++)
	{
	  curjob = hash_get(&world.jobs, keys[z]);
	  if (vm_own_job(curjob))
	    {
	      snprintf(logbuf, BUFSIZ - 1, 
		       "\n [+] Unloading workspace : %u (%s) %c\n",
		       z, curjob->name,
		       (curjob->active ? '*' : ' '));
	      vm_output(logbuf);

	      for (index = 1, hashidx = 0; 
		   hashidx < curjob->loaded.size; hashidx++)
		for (actual = &curjob->loaded.ent[hashidx];
		     actual != NULL && actual->key != NULL;
		     actual = actual->next, index++)
		  {
		    cur = actual->data;
		    if (!world.state.vm_quiet)
		      {
			snprintf(logbuf, BUFSIZ - 1, 
				 " \t[*] Unloading object %u (%s) %c \n", 
				 index, cur->name, 
				 (curjob->current == cur ? '*' : ' '));
			vm_output(logbuf);
		      }
		    elfsh_unload_obj(cur);
		  }
	    }
	}
    }
  
  /* The quit message */
  snprintf(logbuf, BUFSIZ - 1, "\t .:: Bye -:: The %s %s \n",
	   vm_get_mode_name(), 
	   ELFSH_VERSION);
  vm_output(logbuf);
  
  /* Now the ugly code depending on the compilation options */
#if defined(ELFSHNET)
  if (world.curjob->io.type == ELFSH_IONET)
    {
      if (vm_socket_del(inet_ntoa(world.curjob->sock.addr.sin_addr)) < 0)
	fprintf(stderr, "error on quit from client on socket %d\n",
		world.curjob->sock.socket);
    }
  else      
#endif
    { 
      vm_output_bcast("\n");
      if (world.curjob->sourced == 0)
	{
#if defined(USE_READLN)
	  if (world.state.vm_mode == ELFSH_VMSTATE_IMODE ||
	      world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER)
	    {
	      printf(" [*] Writting history (.elfsh_history) \n");
	      write_history(".elfsh_history");
	      rl_callback_handler_remove();
	    }
#endif
	  
	  vm_exit(0);
	}
    }
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
