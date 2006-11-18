/*
** doswitch.c for elfsh
** 
** Started on  Sat Jan 25 11:20:49 2003 mayhem
*/
#include "elfsh.h"

/* Do the switch */
int		vm_doswitch(int nbr)
{
  elfshobj_t	*actual; 

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  actual = (nbr ? vm_getfile(nbr) : 
	    hash_get(&file_hash, world.curjob->curcmd->param[0]));
					     
  if (actual == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unknown requested object", (-1));
    
  world.curjob->current = actual;
  
  /* Switch to static mode if current file is not mapped */
  if (elfsh_is_debug_mode() && !actual->linkmap)
    elfsh_set_static_mode();

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Change the current object */
int		cmd_doswitch()
{
  char		logbuf[BUFSIZ];
  int		ret;
  int		nbr;
  elfshobj_t	*cur;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.curjob->curcmd->param[0] == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", (-1));
  nbr = atoi(world.curjob->curcmd->param[0]);

  ret = vm_doswitch(nbr);

  if (ret < 0)
    {
      snprintf(logbuf, BUFSIZ - 1,
	       "\n [!] Cant switch on file object %s (use 'list') (switching to a dependence need the id) \n\n", 
	       world.curjob->curcmd->param[0]);
      vm_output(logbuf);
    }
  else
    {
      cur = world.curjob->current;

#if defined(USE_MJOLLNIR)
	mjr_set_current_context(&world.mjr_session, cur->name);
#endif

      snprintf(logbuf, BUFSIZ - 1, "\n [*] Switched on object %u (%s) \n\n",
	       cur->id, cur->name);
      vm_output(logbuf);
      if (elfsh_is_debug_mode() && !cur->linkmap)
	vm_output("\n [!!] Loaded file is not the linkmap, "
		  "switching to STATIC mode\n\n");

    }


  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}
