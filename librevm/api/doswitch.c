/*
** @file doswitch.c
** 
** Started on  Sat Jan 25 11:20:49 2003 jfv
**
** $Id: doswitch.c,v 1.6 2008-02-16 12:32:27 thor Exp $
**
*/
#include "revm.h"

/* Do the switch */
int		revm_doswitch(int nbr)
{
  elfshobj_t	*actual; 

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  actual = (nbr ? revm_getfile(nbr) : 
	    hash_get(&file_hash, world.curjob->curcmd->param[0]));
					     
  if (actual == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unknown requested object", (-1));
    
  world.curjob->curfile = actual;
  
  /* Switch to static mode if current file is not mapped */
  if (elfsh_is_debug_mode() && !actual->linkmap)
    elfsh_set_static_mode();

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
