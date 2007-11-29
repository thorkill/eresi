/*
** @file inform.c
**
** @brief Commands for doing annotations on program objects
**
** Started Jan 21 2007 12:57:03 jfv
** $Id: inform.c,v 1.1 2007-11-29 14:01:56 may Exp $
**
*/
#include "libstderesi.h"




/** Inform a given type */
int		cmd_inform()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  switch (world.curjob->curcmd->argc)
    {
      /* inform type */
    case 1:
      if (revm_informed_print(world.curjob->curcmd->param[0]) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Invalid requested type", -1);
      break;

      /* inform type name */
    case 2:
      if (revm_inform_type(world.curjob->curcmd->param[0],
			   world.curjob->curcmd->param[1], NULL, NULL, 1, 1) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Invalid variable information", -1);
      break;
      
      /* inform type name address */
    case 3:
      if (revm_inform_type(world.curjob->curcmd->param[0],
			 world.curjob->curcmd->param[1],
			 world.curjob->curcmd->param[2], NULL, 1, 1) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Invalid variable information", -1);
      break;
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			"Invalid inform syntax", -1);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/** Uninform a given type */
int		cmd_uninform()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  switch (world.curjob->curcmd->argc)
    {
      /* uninform type */
    case 1:
      if (revm_uninform_type(world.curjob->curcmd->param[0], NULL, 1) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Invalid type information", -1);
      break;

      /* uninform type name */
    case 2:
      if (revm_uninform_type(world.curjob->curcmd->param[0],
			   world.curjob->curcmd->param[1], 1) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Invalid variable information", -1);
      break;
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			"Invalid uninform syntax", -1);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
