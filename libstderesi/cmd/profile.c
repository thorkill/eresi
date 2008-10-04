/**
** @file profile.c
** @ingroup libstderesi
** Started on  Fri Nov  2 15:21:56 2001 jfv
** Updated on  Thu Jan 04 11:26:11 2007 jfv
**
** $Id: profile.c,v 1.1 2007-11-29 14:01:56 may Exp $
**
*/
#include "libstderesi.h"


/**
 * Print state of a given flag 
 * @param mask
 * @param feat
 */
void		revm_profile_print(unsigned char mask, char *feat)
{
  char		buf[BUFSIZ];

  snprintf(buf, sizeof(buf), " [-] Profiling level %s <%s> \n",
	   feat, (profiler_is_enabled(mask) ? "enabled" : "disabled"));
  revm_output(buf);
}



/** 
 * Enable or Disable the profiler 
 */
int		cmd_profile()
{
  char		operation;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.curjob->curcmd->argc == 0)
    {
      revm_output(" [*] Current status of the profiler : \n\n");
      revm_profile_print(PROFILE_WARN  , "warnings   ");
      revm_profile_print(PROFILE_FUNCS , "functrace  ");
      revm_profile_print(PROFILE_ALLOC , "allocations");
      revm_profile_print(PROFILE_DEBUG , "debug      ");
      revm_output("\n");
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  if (world.curjob->curcmd->argc != 2)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid profile syntax", -1);

  /* Check subcommand */
  if (!strcmp(world.curjob->curcmd->param[0], "enable"))
    operation = 1;
  else if (!strcmp(world.curjob->curcmd->param[0], "disable"))
    operation = 0;
  else
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid profile subcommand", -1);

  /* Debug profiling */
  if (!strcmp(world.curjob->curcmd->param[1], "debug"))
    {
      if (operation)
	{
	  profiler_enable_debug();
	  revm_output("\t .:: Profiling enabled debug\n\n");
	}
      else
	{
	  profiler_disable_debug();
	  revm_output("\t .:: Profiling disabled debug\n\n");
	}
    }

  /* Error profiling */
  else if (!strcmp(world.curjob->curcmd->param[1], "warn"))
    {
      if (operation)
	{
	  profiler_enable_err();
	  revm_output("\t .:: Profiling enabled warnings\n\n");
	}
      else
	{
	  profiler_disable_err();
	  revm_output("\t .:: Profiling disabled warnings\n\n");
	}
    }

  /* Function tracing profiling */
  else if (!strcmp(world.curjob->curcmd->param[1], "trace"))
    {
      if (operation)
	{
	  profiler_enable_out();
	  revm_output("\t .:: Profiling enabled functrace\n\n");
	}
      else
	{
	  profiler_disable_out();
	  revm_output("\t .:: Profiling disabled functrace\n\n");
	}
    }

  /* Allocations profiling */
  else if (!strcmp(world.curjob->curcmd->param[1], "alloc"))
    {
      if (operation)
	{
	  profiler_enable_alloc();
	  revm_output("\t .:: Profiling enabled alloctrace\n\n");
	}
      else
	{
	  profiler_disable_alloc();
	  revm_output("\t .:: Profiling disabled alloctrace\n\n");
	}
    }

  /* Full profiling */
  else if (!strcmp(world.curjob->curcmd->param[1], "all"))
    {
      if (operation)
	{
	  profiler_enable_all();
	  revm_output("\t .:: Profiling fully enabled \n\n");
	}
      else
	{
	  profiler_disable_all();
	  revm_output("\t .:: Profiling fully disabled \n\n");
	}
    }

  /* Error or success */
  else
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid profile view", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
