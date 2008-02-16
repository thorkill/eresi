/**
** @file switch.c
** 
** Started on  Sat Jan 25 11:20:49 2003 jfv
**
** $Id: switch.c,v 1.2 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libstderesi.h"


/* Change the current object */
int             cmd_doswitch()
{
  char          logbuf[BUFSIZ];
  int           ret;
  int           nbr;
  elfshobj_t    *cur;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.curjob->curcmd->param[0] == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", (-1));
  nbr = atoi(world.curjob->curcmd->param[0]);

  ret = revm_doswitch(nbr);

  if (ret < 0)
    {
      snprintf(logbuf, BUFSIZ - 1,
               "\n [!] Cant switch on file object %s (use 'list') (switching to a dependence need the id) \n\n", 
               world.curjob->curcmd->param[0]);
      revm_output(logbuf);
    }
  else
    {
      cur = world.curjob->curfile;

#if defined(USE_MJOLLNIR)
      mjr_set_current_context(&world.mjr_session, cur->name);
#endif

      snprintf(logbuf, BUFSIZ - 1, "\n [*] Switched on object %u (%s) \n\n",
               cur->id, cur->name);
      revm_output(logbuf);
      if (elfsh_is_debug_mode() && !cur->linkmap)
        revm_output("\n [!!] Loaded file is not the linkmap, "
                  "switching to STATIC mode\n\n");

    }


  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}
