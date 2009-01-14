/**
** @file interp.c
** 
** Started on  Fri Nov  2 15:20:41 2001 jfv
** Last update Wed Mar 10 12:21:34 2004 jfv
**
** $Id$: interp.c,v 1.2 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libstderesi.h"




/** 
 * @brief Display the interpretor (.interp) section 
 */
int		cmd_interp()
{
  char		*name;
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if ((name = elfsh_get_interp(world.curjob->curfile)) == NULL)
    RET(-1);
  snprintf(logbuf, BUFSIZ - 1, "\n [SHT_INTERP] : %s \n\n", name);
  revm_output(logbuf);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
