/*
** mode.c for e2dbg
** 
** Started on  Tue Aug 16 09:38:03 2005 jfv
**
** $Id: mode.c,v 1.1 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libe2dbg.h"


/* Change between static (ondisk) and dynamic (memory) mode */
int		cmd_mode()
{
  char		*param;
  char		buf[256];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  param = world.curjob->curcmd->param[0];
  if (!param)
    {
      if (elfsh_is_static_mode())
	param = "STATIC";
      else if (elfsh_is_runtime_mode())
	param = "DYNAMIC";
      else
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unknown e2dbg mode", -1);
      snprintf(buf, sizeof(buf), " [*] e2dbg is in %s MODE \n\n", param);
      e2dbg_output(buf);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  
  if (*param == 's' || *param == 'S')
    {
      elfsh_set_static_mode();
      e2dbg_output(" [*] e2dbg is now in STATIC mode\n\n"); 
    }
  else if (*param == 'd' || *param == 'D')
    {
      if (world.curjob)
	{
	  if (world.curjob->curfile)
	    if (!world.curjob->curfile->linkmap && 
		!world.curjob->curfile->rhdr.base &&
		world.curjob->curfile->hostype != ELFSH_HOST_GDB)
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
				"Can't switch to dynamic"
				" mode on not mapped file.", -1);
	}
      elfsh_set_runtime_mode();
      e2dbg_output(" [*] e2dbg is now in DYNAMIC mode\n\n"); 
    }
  else
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unknown mode for E2DBG", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

