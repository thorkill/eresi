/*
** @file screen.c
** @ingroup librevm_api
*/
/*
** Started on  Fri Nov  2 15:21:56 2001 jfv
** Updated on  Thu Jan 04 11:22:11 2007 jfv
**
** $Id: screen.c,v 1.7 2008-02-16 12:32:27 thor Exp $
**
*/
#include "revm.h"


/**
 * Clear the content of the current screen 
 * @param i
 * @param c
 * @return
 */
int		revm_screen_clear(int i, char c)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  XFREE(__FILE__, __FUNCTION__, __LINE__,world.curjob->ws.screen.buf);
  world.curjob->ws.screen.head = world.curjob->ws.screen.tail = NULL;
  world.curjob->ws.screen.buf = NULL;
  revm_ctrl_set(i, c);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * Update the screen depending of the actual job 
 *
 * @param isnew
 * @param prompt_display
 * @return
 */
int		revm_screen_update(Bool isnew, u_short prompt_display)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* clear screen */
  revm_output_nolog("\033[2J");
  revm_output_nolog("\033[0;0H");
  revm_buffer_reset(isnew);
  revm_flush();

  /* Rebuild the current screen */
  if (world.curjob->ws.screen.buf != NULL)
    {
      revm_output_nolog(world.curjob->ws.screen.head);
      if (world.curjob->ws.screen.head >= world.curjob->ws.screen.tail)
	{
	  revm_output_nolog(world.curjob->ws.screen.head);
	  revm_output_nolog(world.curjob->ws.screen.buf);
	}
    }

  revm_screen_change(isnew, prompt_display);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * Switch screen with switching workspace 
 *
 */
int		revm_screen_switch()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (revm_workspace_next() <= 0)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, revm_screen_update(0, 1));
}


/**
 * Switch to the next workspace 
 *
 */
int		revm_workspace_next()
{
  unsigned int		index, entrie;
  char	        **keys;
  int		keynbr;
  revmjob_t	*curjob;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  

  keys = hash_get_keys(&world.jobs, &keynbr);
  if (keynbr <= 1)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  /* Search the current index */
  for (index = 0; index < keynbr; index++)
    {
      curjob = hash_get(&world.jobs, keys[index]);
      if (revm_own_job(curjob) && curjob == world.curjob)
	{
	  entrie = index;
	  break;
	}
    }

  /* Search the next entrie */
  for (entrie = (entrie+1) % keynbr; entrie < keynbr; entrie = (entrie+1) % keynbr)
    {
      curjob = hash_get(&world.jobs, keys[entrie]);
      if (revm_own_job(curjob))
	{
	  /* If we found the current job, we made a loop, so we break */
	  if (curjob == world.curjob)
	    break;
	  revm_switch_job(curjob);
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
	}
    }

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to find workspace to switch on", -1);
}
