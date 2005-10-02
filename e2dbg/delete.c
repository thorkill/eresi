/*                                                                                                                                                          
** delete.c for e2dbg
**    
** Delete breakpoints
**
** Started on  Tue Aug 16 09:38:03 2005 mayhem                                                                                                                   
*/
#include "elfsh.h"






/* Delete a breakpoint */
int		cmd_delete()
{
  elfsh_Addr	addr;
  elfshbp_t	*bp;
  char		*name;
  int		prot;
  int		off;
  char		logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Lookup breakpoint */
  
  bp = vm_lookup_bp(vm_lookup_var(world.curjob->curcmd->param[0]));
  if (!bp)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unknown breakpoint", -1);
  addr = bp->addr;
  
  /* Delete the breakpoint */
  prot = elfsh_munprotect(bp->obj, addr,  1);
  *(u_char *) addr = bp->savedinstr[0];
  elfsh_mprotect(addr, 1, prot);

  name = vm_resolve(bp->obj, addr, &off);
  snprintf(logbuf, BUFSIZ, XFMT, addr);
  hash_del(&e2dbgworld.bp, logbuf);

  if (off)
    snprintf(logbuf, BUFSIZ, " [*] Breakpoint at %08X <%s + %u> removed\n\n", 
	 addr, name, off);
  else
    snprintf(logbuf, BUFSIZ, " [*] Breakpoint at %08X <%s> removed\n\n", 
	 addr, name);
  vm_output(logbuf);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}





