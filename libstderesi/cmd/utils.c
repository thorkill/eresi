/**
 * @file utils.c
 * @ingroup libstderesi
 * Started on  Fri Nov  2 15:21:56 2001 jfv
 * Updated on  Thu Jan 04 11:26:11 2007 jfv
 *
 * $Id$: utils.c,v 1.1 2007-11-29 14:01:56 may Exp $
 *
 */
#include "libstderesi.h"


/** 
 * Useful when you have only one terminal 
 */
int		cmd_meta()
{
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!world.state.revm_quiet)
    revm_output(" [*] You are still in elfsh, exit bash "
	      "to get back to the regular prompt \n\n");

  revm_terminal_unprepare(world.state.revm_mode);
  ret = revm_system(revm_lookup_string("$SHELL"));
  revm_terminal_prepare(world.state.revm_mode);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}



/** 
 * Edit a file 
 */
int             cmd_edit()
{
  int           ret;
  char		buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_terminal_unprepare(world.state.revm_mode);

  snprintf(buff, BUFSIZ, "%s %s",
	   revm_lookup_string("$EDITOR"),
	   world.curjob->curcmd->param[0]);

  ret = revm_system(buff);

  revm_terminal_prepare(world.state.revm_mode);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}


/** 
 * Stop the scripting and pass in interactive mode 
 */
int	cmd_stop()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, REVM_SCRIPT_STOP);
}


/** 
 * Precise a general weak bounded regex for all options 
 */
int		cmd_alert()
{
  char		*str;
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  str = strdup(world.curjob->curcmd->param[0]);
 
  if (regcomp(&world.state.revm_alert, str, REG_EXTENDED) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Failed to compute regex", -1);
  world.state.revm_use_alert = 1;
  if (world.state.revm_salert)
    XFREE(__FILE__, __FUNCTION__, __LINE__,world.state.revm_salert);
  world.state.revm_salert = str;
  if (!world.state.revm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1, " [*] Changed alert regex to %s \n\n", str);
      revm_output(logbuf);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * Get ASCII string for hash table 
 */
char	*revm_ascii_type(hash_t *cur)
{
  return (aspect_typename_get(cur->type));
}

/**
 * Get ASCII string for hash table 
 * @param cur
 * @return
*/
char	*revm_ascii_ltype(list_t *cur)
{
  return (aspect_typename_get(cur->type));
}


/**
 * Get ASCII string for hash table 
 * @param cur
 * @return
*/
char	*revm_ascii_vtype(vector_t *cur)
{
  return (aspect_typename_get(cur->type));
}
