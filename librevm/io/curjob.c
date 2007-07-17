/*
** curjob.c for librevm in ERESI
**
** Implement job API necessary for revm-io
**
** Started September 16 03:11:04 2005 mayhem
**
** $Id: curjob.c,v 1.4 2007-07-17 18:11:25 may Exp $
**
*/
#include "revm.h"


/* Return the current local job */
revmjob_t		*revm_localjob_get()
{
  revmjob_t		*job;
  int                   index;
  char                  **keys;
  int                   keynbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  keys = hash_get_keys(&world.jobs, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      job = hash_get(&world.jobs, keys[index]);
      if (!job->ws.active)
        continue;
      if (job->ws.io.type == REVM_IO_STD)
        PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, job);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,(NULL));
}
