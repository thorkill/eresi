/**
 * @file delete.c
 *    
 * Delete breakpoints
 *
 * Started on  Tue Aug 16 09:38:03 2005 jfv
 *
 * $Id: delete.c,v 1.1 2008-02-16 12:32:27 thor Exp $
 *
 */
#include "libe2dbg.h"


/**
 * Delete a breakpoint for real.
 * @param Breakpoint structure to delete
 * @return Nothing.
 */
int		e2dbg_breakpoint_delete(elfshbp_t *bp)
{
  eresi_Addr	addr;
  char		*name;
  int		prot;
  elfsh_SAddr	off;
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  addr = bp->addr;
  
  /* Delete the breakpoint */
  prot = elfsh_munprotect(bp->obj, addr,  1);
  e2dbg_deletebreak(bp);
  elfsh_mprotect(addr, 1, prot);
  name = revm_resolve(bp->obj, addr, &off);
  snprintf(logbuf, BUFSIZ, XFMT, addr);
  hash_del(&e2dbgworld.bp, logbuf);

  if (off)
    snprintf(logbuf, BUFSIZ, " [*] Breakpoint at " AFMT 
	     " <%s + " UFMT "> removed\n", addr, name, off);
  else
    snprintf(logbuf, BUFSIZ, " [*] Breakpoint at " AFMT " <%s> removed\n", 
	 addr, name);
  e2dbg_output(logbuf);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** Delete breakpoints
 * @return Success (0) or error (-1).
 */
int		cmd_delete()
{
  elfshbp_t	*bp;
  char		**keys;
  int		keynbr;
  int		index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Lookup breakpoint */
  for (index = 0; world.curjob->curcmd->param[index]; index++)
    {
      bp = e2dbg_breakpoint_lookup(revm_lookup_string(world.curjob->curcmd->param[index]));
      if (!bp)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unknown breakpoint", -1);
      e2dbg_breakpoint_delete(bp);
    }

  /* Delete all breakpoints if no argument was given */
  if (!index)
    {
      keys = hash_get_keys(&e2dbgworld.bp, &keynbr);
      for (index = 0; index < keynbr; index++)
	{
	  bp = hash_get(&e2dbgworld.bp, keys[index]);
	  e2dbg_breakpoint_delete(bp);
	}
      hash_free_keys(keys);
    }

  e2dbg_output("\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
