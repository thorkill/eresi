/**
 * @file api.c
 *
 * This file contain the ELF specific vm-level API
 *
 * Started on  Sat Jan 25 07:48:41 2003 jfv
 *
 * $Id$: api.c,v 1.1 2007-11-29 14:01:56 may Exp $
 *
*/
#include "libstderesi.h"

/**
 * Get the parent object of a breakpoint 
 * Thats needed for the mprotect stuff inside the breakpoint handler 
 * @param addr
 */
elfshobj_t      *revm_get_parent_object(eresi_Addr addr)
{
  elfsh_Phdr    *cur;
  elfshobj_t    *curfile;
  elfshsect_t   *cursect;
  char          **keys;
  int           index;
  int           keynbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  keys = hash_get_keys(&world.curjob->loaded, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      curfile = hash_get(&world.curjob->loaded, keys[index]);
      cursect = elfsh_get_parent_section(curfile, addr, NULL);
      if (cursect)
        {
          cur = elfsh_get_parent_segment(curfile, cursect);
          if (cur)
            PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, curfile);
        }
    }
  
  /* Parent object not found */
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "Unable to get parent object addr", (NULL));
}
