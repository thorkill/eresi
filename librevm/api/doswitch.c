/*
* @file librevm/api/doswitch.c
** @ingroup api
** @brief Do the switch. 
** Started on  Sat Jan 25 11:20:49 2003 jfv
**
** $Id$
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
  if (elfsh_is_runtime_mode() && !actual->linkmap)
    elfsh_set_static_mode();

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
