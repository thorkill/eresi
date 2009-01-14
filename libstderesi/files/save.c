/**
** @file save.c
** @ingroup libstderesi 
**
** Started on  Sat Jan 25 11:22:35 2003 jfv
** $Id$: save.c,v 1.2 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libstderesi.h"


/**
 * @brief Command handler : save object which name is given in parameter to the command.
 * @return Success (0) or Error (-1).
 */
int		cmd_save()
{
  char		logbuf[BUFSIZ];
  char		*str;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  str = world.curjob->curcmd->param[0];

  /* We save more things if we are in the tracer */
  switch (world.state.revm_mode)
    {
    case REVM_STATE_TRACER:
      if (elfsh_save_obj(world.curjob->curfile, str) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to save object", -1);  
      break;
    default:
      if (elfsh_save_obj(world.curjob->curfile, str) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to save object", -1);  
      break;
    }

  snprintf(logbuf, BUFSIZ - 1, "\n [*] Object %s saved successfully \n\n", str);
  revm_output(logbuf);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
