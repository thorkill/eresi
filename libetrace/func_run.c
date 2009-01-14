/*
** @file func_run.c
**
** Libetrace implementation in the ERESI project.
**
** Started on  Sun Jun 22 12:10:12 2007 jfv
** $Id$: libetrace.h,v 1.2 2007-11-29 10:25:02 rival Exp $
**
*/

char **traces_lastarg = NULL;

/**
 * Add arguments for a futur traces_run
 * @param argc arguments number 
 * @param argv arguments
 * @see traces_run
 */
int			trace_param_add(int argc, char **argv)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (argc < 1 || argv == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1); 

  /* Copy arguments */
  XALLOC(__FILE__, __FUNCTION__, __LINE__, traces_lastarg, sizeof(char *)*(argc+1), -1);
  memcpy(traces_lastarg, argv, sizeof(char*)*argc);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0); 
}

