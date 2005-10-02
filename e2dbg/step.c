/*                                                                                                                                                          
** delete.c for e2dbg
**    
** Delete breakpoints
**
** Started on  Tue Aug 16 09:38:03 2005 mayhem                                                                                                                   
*/
#include "elfsh.h"


/* Step command */
int		cmd_step()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (!e2dbgworld.context)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "You must be in a SIGTRAP handler", -1);

  if (e2dbgworld.step)
    {
      if (e2dbg_resetstep() < 0)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Failed to disable stepping", -1);
      vm_output("\n [*] Disabled stepping\n\n");
      e2dbgworld.step = 0;
    }
  else
    {
      if (e2dbg_setstep() < 0)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Failed to enable stepping", -1);
      vm_output("\n [*] Enabled stepping (now 'cont' to step) \n\n");
      e2dbgworld.step = 1;
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
