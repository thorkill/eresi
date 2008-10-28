#include "libe2dbg.h"

/* The debugger world */
e2dbgworld_t	e2dbgworld;

/**
 * Realize the output.
 *  @todo: XXX: Need a vector
 * @param str
 * @return
 */
int		e2dbg_output(char *str)
{
  //revm_output(str);
  fprintf(stderr, str);
  return (0);
}


/** 
 * Get the parent object of a breakpoint.
 * Thats needed for the mprotect stuff inside the breakpoint handler 
 * @param addr
 * @return
 */
elfshobj_t      *e2dbg_get_parent_object(eresi_Addr addr)
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
