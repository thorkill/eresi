/*
** screen.c for librevm in elfsh
**
** Started on  Fri Nov  2 15:21:56 2001 mayhem
** Updated on  Thu Jan 04 11:22:11 2007 mayhem
*/
#include "revm.h"


/* Clear the content of the current screen */
int		vm_screen_clear(int i, char c)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  XFREE(__FILE__, __FUNCTION__, __LINE__,world.curjob->ws.screen.buf);
  world.curjob->ws.screen.head = world.curjob->ws.screen.tail = NULL;
  world.curjob->ws.screen.buf = NULL;

#if defined(USE_READLN)
  readln_ctrl_set(i, c);
  world.curjob->ws.io.buf = NULL;
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Update the screen depending of the actual job */
int		vm_screen_update(u_short isnew, u_short prompt_display)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* clear screen */
  vm_output_nolog("\033[2J");
  vm_output_nolog("\033[0;0H");

#if defined(USE_READLN)
  if (isnew)
    world.curjob->ws.io.buf = NULL;
#endif

  vm_flush();

  /* Rebuild the current screen */
  if (world.curjob->ws.screen.buf != NULL)
    {
      vm_output_nolog(world.curjob->ws.screen.head);
      if (world.curjob->ws.screen.head >= world.curjob->ws.screen.tail)
	{
	  vm_output_nolog(world.curjob->ws.screen.head);
	  vm_output_nolog(world.curjob->ws.screen.buf);
	}
    }

#if defined (USE_READLN)
  readln_screen_change(isnew, prompt_display);
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Switch screen with switching workspace */
int		vm_screen_switch()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (cmd_next_workspace() <= 0)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, vm_screen_update(0, 1));
}

