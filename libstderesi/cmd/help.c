/*
** @file help.c for libstderesi in ERESI
** @ingroup libstderesi
** Started on  Sat Jan 25 11:19:18 2003 jfv
** $Id: help.c,v 1.1 2007-11-29 14:01:56 may Exp $
**
*/
#include "libstderesi.h"


/** 
 * The ELFsh modules Help command 
 */
int		cmd_modhelp()
{
  revmmod_t	*mod;
  unsigned int		id;
  char		buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  mod = 0;
  id = atoi(world.curjob->curcmd->param[0]);
  if (!id)
    {
      if (access(world.curjob->curcmd->param[0], R_OK) != 0)
	{
	  snprintf(buf, sizeof(buf), "%s%s", ERESI_MODPATH, world.curjob->curcmd->param[0]);
	  if (access(buf, R_OK) != 0)
	    {
	      snprintf(buf, sizeof(buf), "%s%s.so",
		       ERESI_MODPATH, world.curjob->curcmd->param[0]);
	      if (access(buf, R_OK) != 0)
		PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
				  "Cannot find module", -1);
	    }
	  mod = hash_get(&mod_hash, buf);
	}
    }
  else
    mod = revm_getmod(id);
  
  if (mod == NULL || mod->help == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Module unavailable", -1);
  mod->help();
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * The Help command 
 */
int		cmd_help()
{
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  ret = revm_help(world.curjob->curcmd->param[0]);
  if (!ret)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "Unable to help on such command", -1);
}
