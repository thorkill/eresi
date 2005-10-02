/*                                                                                                                                                          
** backtrace.c for e2dbg
**    
** Started on  Tue Aug 16 09:38:03 2005 mayhem                                                                                                                   
*/
#include "elfsh.h"






/* Display backtrace with sym resolution */
int		vm_bt()
{
  void		*frame;
  void		*ret;
  char		*name;
  int		off;
  char		logbuf[BUFSIZ];
  int		i = 0;
  elfshpath_t	*ssp;
  elfsh_Addr	savedsp;
  char		printed;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  if (!world.curjob)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No current workspace", -1);
  if (!world.curjob->current)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No current file", -1);
  ssp = hash_get(&vars_hash, ELFSH_SSPVAR);
  if (!ssp)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No saved SP", -1);
  savedsp = ssp->immed_val.ent;
  printed = 0;

  /* Get the current frame */
  frame = elfsh_bt_get_frame();
  
  /* Backtrace frames */
  do {

    /* XXX probably arch dependant */
    /* Assume that the ret address is one word behind frame pointer */
    if (!printed && savedsp <= ((elfsh_Addr) frame + 4))
      {
	vm_output(" [**] <----- Here start e2dbg internal bt ----->\n");
	if (e2dbgworld.curbp)
	  {
	    snprintf(logbuf, BUFSIZ, " [**] " XFMT " %s\n",
		     e2dbgworld.curbp->addr, e2dbgworld.curbp->symname);
	    vm_output(logbuf);
	  }
	printed = 1;
      }

    ret = (void *) (*((long *) frame + 1));
    /* XXX end */

    /* Resolve and print current trace frame */
    name = vm_resolve(world.curjob->current, (elfsh_Addr) ret, &off);
    if (!name)
      name = "?";
    if (off)
      snprintf(logbuf, BUFSIZ - 1, " [%02d] "XFMT" <%s + %d>\n", i, 
	       (elfsh_Addr) ret, 
	       name, 
	       off);
    else
      snprintf(logbuf, BUFSIZ - 1, " [%02d] "XFMT" <%s>\n", i, 
	       (elfsh_Addr) ret, 
	       name);
    vm_output(logbuf);

    /* This seems to be the signal vsyscall return address */
    if (ret == E2DBG_VSYSCALL_RETADDR)
      vm_output(" [**] Dectected VSYSCALL return address : one function missing\n");

    frame = elfsh_bt(frame);    
    i++;

  } while (frame); 

  vm_output("\n");
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Backtrace command */
int		cmd_bt()
{
  int		ret;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!elfsh_is_debug_mode())
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Not in dynamic or debugger mode", -1);
  vm_output(" .:: Backtrace ::. \n");
  ret = vm_bt();
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}

