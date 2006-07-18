/*                                                                                                                                                          
** backtrace.c for e2dbg
**    
** Started on  Tue Aug 16 09:38:03 2005 mayhem                                                                                                                   
*/
#include "elfsh.h"






/* Display backtrace with sym resolution */
int		vm_bt()
{
  e2dbgthread_t	*t;
  elfsh_Addr	frame;
  elfsh_Addr	ret;
  char		*name, *name2;
  int		off, off2;
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
      /* Check if the next frame is pointing on our stack */
      if (e2dbgworld.curthread->stackaddr > frame ||
	  e2dbgworld.curthread->stackaddr + e2dbgworld.curthread->stacksize < frame)
	{
	  snprintf(logbuf, BUFSIZ, 
		   " [*] Invalid next frame address %08X (stackaddr = %08X, size = %u)\n\n", 
		   frame, e2dbgworld.curthread->stackaddr, e2dbgworld.curthread->stacksize);
	  vm_output(logbuf);
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	}

      /* Call the getret hook */
      ret = (elfsh_Addr) e2dbg_getret(world.curjob->current, (elfsh_Addr) frame);

      /* Resolve and print current trace frame */
      if (i == 0)
	ret = *(e2dbg_getpc());
      name = vm_resolve(world.curjob->current, (elfsh_Addr) ret, &off);
      if (!name)
	name = "?";

      /* Just insert the real entry point where we reach the thread entry point of e2dbg */
      if (strstr(name, "e2dbg_thread_start"))
	{
	  snprintf(logbuf, BUFSIZ - 1, "%u", e2dbgworld.stoppedpid);
	  t = hash_get(&e2dbgworld.threads, logbuf);
	  name2 = vm_resolve(world.curjob->current, (elfsh_Addr) t->entry, &off2);
	  if (name2)
	    {
	      if (off2)
		snprintf(logbuf, BUFSIZ - 1, " [%02d] "XFMT" in "XFMT" <%s + %u> -ENTRY-\n", i, 
			 ret, (elfsh_Addr) t->entry, name2, off2);
	      else
		snprintf(logbuf, BUFSIZ - 1, " [%02d] "XFMT" in "XFMT " <%s> -ENTRY-\n", i, 
			 ret, (elfsh_Addr) t->entry, name2);
	      vm_output(logbuf);
	      i++;
	    }
	}

      /* Filter the name in case we have a known-pattern address */
      if (ret == E2DBG_VSYSCALL_RETADDR)
	name = "KERNEL VSYSCALL PAGE : one function missing";
      else if (ret == E2DBG_SIGTRAMP_RETADDR)
	name = "KERNEL SIGNAL TRAMPOLINE";
      
      /* Print the current level frame */
      if (off)
	snprintf(logbuf, BUFSIZ - 1, " [%02d] "XFMT" in "XFMT" <%s + %d>\n", i, 
		 (elfsh_Addr) ret, ret - off, name, off);
      else
	snprintf(logbuf, BUFSIZ - 1, " [%02d] "XFMT" in "XFMT" <%s>\n", i, 
		 (elfsh_Addr) ret, ret, name);
      vm_output(logbuf);

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
  if (e2dbgworld.curthread == NULL || e2dbgworld.curthread->context == 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No current thread context available", -1);    
  vm_output(" .:: Backtrace ::. \n");
  ret = vm_bt();
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}

