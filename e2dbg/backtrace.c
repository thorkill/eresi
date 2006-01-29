/*                                                                                                                                                          
** backtrace.c for e2dbg
**    
** Started on  Tue Aug 16 09:38:03 2005 mayhem                                                                                                                   
*/
#include "elfsh.h"






/* Display backtrace with sym resolution */
int		vm_bt()
{
  elfsh_Addr	frame;
  elfsh_Addr	ret;
  char		*name;
  int		off;
  char		logbuf[BUFSIZ];
  int		i = 0;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  if (!world.curjob)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No current workspace", -1);
  if (!world.curjob->current)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No current file", -1);

  /* Get the current frame by calling the hook */
  frame = (elfsh_Addr) e2dbg_getfp();
  if (!frame)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "No context", (-1));

  /* Backtrace frames */
  while (frame && frame != 0xFFFFFFFF)
    {
      /* Call the getret hook */
      ret = (elfsh_Addr) e2dbg_getret(world.curjob->current, (elfsh_Addr) frame);

      /* Resolve and print current trace frame */
      if (i == 0)
	ret = *(e2dbg_getpc());
      name = vm_resolve(world.curjob->current, (elfsh_Addr) ret, &off);
      if (!name)
	name = "?";

      if (off)
	snprintf(logbuf, BUFSIZ - 1, " [%02d] "XFMT" <%s + %d>\n", i, 
		 (elfsh_Addr) ret, name, off);
      else
	snprintf(logbuf, BUFSIZ - 1, " [%02d] "XFMT" <%s>\n", i, 
		 (elfsh_Addr) ret, name);
      vm_output(logbuf);
      
      /* This seems to be the signal vsyscall return address */
      if (ret == E2DBG_VSYSCALL_RETADDR)
	vm_output(" [**] Detected VSYSCALL retaddr : one function missing\n");
      
      /* Call the nextfp hook */
      frame = e2dbg_nextfp(world.curjob->current, (elfsh_Addr) frame);    
      i++;
    }
  
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
  if (e2dbgworld.context == 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No context available", -1);    
  vm_output(" .:: Backtrace ::. \n");
  ret = vm_bt();
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}

