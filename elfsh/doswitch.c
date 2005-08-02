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

  actual = (nbr ? vm_getfile(nbr) : 
	    hash_get(&file_hash, world.curjob->curcmd->param[0]));
					     
  if (actual == NULL)
    return (-1);
    
  world.curjob->current = actual;

  if (!actual->linkmap)
    elfsh_set_static_mode();


  return (0);
}


/* Change the current object */
int		cmd_doswitch()
{
  char		logbuf[BUFSIZ];
  int		ret;
  int		nbr;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.curjob->curcmd->param[0] == NULL)
    return (-1);
  nbr = atoi(world.curjob->curcmd->param[0]);

  ret = vm_doswitch(nbr);

  if (ret < 0)
    {
      snprintf(logbuf, BUFSIZ - 1,
	       "\n [!] Cant switch on file object %s (use 'list') \n\n", 
	       world.curjob->curcmd->param[0]);
      vm_output(logbuf);
    }
  else
    {
      snprintf(logbuf, BUFSIZ - 1, "\n [*] Switched on object %u (%s) \n\n",
	       world.curjob->current->id, world.curjob->current->name);
      vm_output(logbuf);
      if (!world.curjob->current->linkmap)
	vm_output("\n [!!] Loaded file is not the linkmap, "
		  "switching to STATIC mode\n\n");

    }


  return (ret);
}
