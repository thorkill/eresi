/**
** @file save.c
** 
** Started on  Sat Jan 25 11:22:35 2003 jfv
** Last update Wed Mar 10 12:32:35 2004 jfv
**
** $Id: save.c,v 1.2 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libstderesi.h"


/**
 * Save objects which name is given as parameter 
 */
int		cmd_save()
{
  char		logbuf[BUFSIZ];
  char		*str;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  str = world.curjob->curcmd->param[0];
  if (elfsh_save_obj(world.curjob->curfile, str) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to save object", -1);  
  snprintf(logbuf, BUFSIZ - 1, "\n [*] Object %s saved successfully \n\n", str);
  revm_output(logbuf);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
