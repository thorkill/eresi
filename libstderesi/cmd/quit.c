/**
** @file quit.c
** @ingroup libstderesi 
** Started on  Sat Jan 25 11:19:53 2003 jfv
**
** $Id: quit.c,v 1.3 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libstderesi.h"

unsigned char quit_msg_setup = 0;
char	quit_msg[512];

/**
 * Need doxygen tag.
 * @param msg
 */
void    revm_quitmsg_set(char *msg)
{
  strncpy(quit_msg, msg, sizeof(quit_msg) - 1);
  quit_msg_setup = 1;
}

/**
 * Unload all the loaded object and exit the shell 
 */
int		cmd_quit()
{
  elfshobj_t	*cur;
  int		index;
  char		logbuf[BUFSIZ];
  int		objnbr;
  char		**keys;
  char		**objkeys;
  int		keynbr;
  int		z;
  revmjob_t	*curjob;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Unload files anyway */
  keys = hash_get_keys(&world.jobs, &keynbr);
  
  /* Unload every workspace */
  for (z = 0; z < keynbr; z++)
    {
      curjob = hash_get(&world.jobs, keys[z]);
      if (!revm_own_job(curjob))
	continue;
      
      snprintf(logbuf, BUFSIZ - 1, 
	       "\n [+] Unloading workspace : %u (%s) %c\n",
	       z, curjob->ws.name, (curjob->ws.active ? '*' : ' '));
      revm_output(logbuf);
      
      /* Unload all objects from this job */
      objkeys = hash_get_keys(&curjob->loaded, &objnbr);
      for (index = 0; index < objnbr; index++)
	{
	  cur = hash_get(&curjob->loaded, objkeys[index]);
	  if (!world.state.revm_quiet)
	    {
	      snprintf(logbuf, BUFSIZ - 1, 
		       " \t[*] Unloading object %u (%s) %c \n", 
		       index + 1, cur->name, 
		       (curjob->curfile == cur ? '*' : ' '));
	      revm_output(logbuf);
	    }
	  elfsh_unload_obj(cur);
	}
    }

  /* The quit message */
  if (quit_msg_setup == 0)
    {
      snprintf(logbuf, BUFSIZ - 1, "\t .:: Bye -:: The %s %s \n",
	       revm_modename_get(), REVM_VERSION);
      revm_quitmsg_set(logbuf);
    }
  if (world.curjob->sourced == 0)
    revm_output(quit_msg);
  
  /* Now the ugly code depending on the compilation options */
#if defined(ERESI_NET)
  if (world.curjob->ws.io.type == REVM_IO_NET &&
      revm_socket_del(inet_ntoa(world.curjob->ws.io.sock.addr.sin_addr)) < 0)
    fprintf(stderr, "error on quit from client on socket %d\n",
	    world.curjob->ws.io.sock.socket);
  else      
#endif
    { 
      revm_output_bcast("\n");
      revm_quit(world.state.revm_mode);
      
      /* Remove the 2 FIFO on quit : will let the client aware 
	 about our end of execution */
      fprintf(stderr, " [*] Removing 2 FIFO from server side \n");
      unlink(REVM_FIFO_S2C);
      unlink(REVM_FIFO_C2S);
      revm_exit(0);
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, REVM_SCRIPT_QUIT);
}
