/*
** misc.c for elfsh
**
** Started on  Fri Nov  2 15:21:56 2001 mayhem
** Updated on  Wed Jan 03 17:31:11 2007 mayhem
**
** $Id: misc.c,v 1.14 2007-03-07 16:45:34 thor Exp $
**
*/
#include "elfsh.h"


/* Mark the current object SHT to be removed on savnig */
int             cmd_shtrm()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  world.curjob->current->shtrm = 1;
  if (!world.state.vm_quiet)
    vm_output(" [*] SHT marked as removed, you need to save\n\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Mark the file as stripped */
int		cmd_strip()
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  world.curjob->current->strip = 1;
  if (!world.state.vm_quiet)
    vm_output(" [*] Object marked as stripped, you need to save\n\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Mark SHT as removed and file as stripped */
int		cmd_sstrip()
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  world.curjob->current->strip = 1;
  world.curjob->current->shtrm = 1;
  if (!world.state.vm_quiet)
    vm_output(" [*] Object marked as sstripped, you need to save\n\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
