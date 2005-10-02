/*
** mode.c for e2dbg
** 
** Started on  Tue Aug 16 09:38:03 2005 mayhem
*/
#include "elfsh.h"






/* Change between static (ondisk) and dynamic (memory) mode */
int		cmd_mode()
{
  char		*param;
  char		buf[256];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  param = world.curjob->curcmd->param[0];
  if (!param)
    {
      if (elfsh_is_static_mode())
	param = "STATIC";
      else if (elfsh_is_debug_mode())
	param = "DYNAMIC";
      else
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unknown e2dbg mode", -1);
      snprintf(buf, sizeof(buf), " [*] e2dbg is in %s MODE \n\n", param);
      vm_output(buf);
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  
  if (*param == 's' || *param == 'S')
    {
      elfsh_set_static_mode();
      vm_output(" [*] e2dbg is now in STATIC mode\n\n"); 
    }
  else if (*param == 'd' || *param == 'D')
    {
      if (world.curjob)
	{
	  if (world.curjob->current)
	    if (!world.curjob->current->linkmap && !world.curjob->current->rhdr.base)
	      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
				"Can't switch to dynamic"
				" mode on not mapped file.", -1);
	}
      elfsh_set_debug_mode();
      vm_output(" [*] e2dbg is now in DYNAMIC mode\n\n"); 
    }
  else
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unknown mode for E2DBG", -1);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

