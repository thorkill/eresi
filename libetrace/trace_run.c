/*
** @file trace_run.c
**
** Libetrace implementation in the ERESI project.
**
** Started on  Sun Jun 22 12:10:12 2007 jfv
** $Id: libetrace.h,v 1.2 2007-11-29 10:25:02 rival Exp $
**
*/
#include "libetrace.h"


char	**traces_lastarg = NULL;

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


/**
 * Run a trace session (used in etrace)
 * @param file target
 * @param argv argument list
 * @param argc argument counter
 */
int		traces_run(elfshobj_t *file, char **argv, int argc)
{
  char		*home;
  char		*filename;
  size_t	len;
  unsigned int		index;
  char		*path;
  char		*av[argc+2];
  int		ret;
  struct timeval now, after;
  double	difftime;
  char		buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  argc = 0;
  if (traces_lastarg)
    {
      while (traces_lastarg[argc] != NULL)
	argc++;
      argv = traces_lastarg;
    }
  else
    {
      XALLOC(__FILE__, __FUNCTION__, __LINE__, argv, sizeof(char*), -1);
      argv[0] = NULL;
    }

  if (argc < 0 || !argv)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid parameters", -1); 

  if (elfsh_get_objtype(elfsh_get_hdr(file)) != ET_EXEC &&
      elfsh_get_objtype(elfsh_get_hdr(file)) != ET_DYN)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "We need a ET_EXEC binary", -1);

  home = getenv("HOME");

  if (!home)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Cannot find home directory", -1);
  
  snprintf(buf, BUFSIZ - 1, "%s/%s", home, REVM_TRACE_REP);

  /* Create temp directory if needed */
  mkdir(buf, 0700);

  /* We search the filename (we have a path) */
  len = strlen(file->name);
  filename = file->name;

  for (index = len; index > 0; index--)
    if (file->name[index] == '/')
      break;
  
  if (INTERVAL(0, index+1, len))
    filename = file->name + index + 1;

  /* Full path */
  snprintf(buf, BUFSIZ - 1, "%s/%s/%s", home, REVM_TRACE_REP, filename);
  path = strdup(buf);

  if (etrace_save_obj(file, path) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Failed to save object", -1);

  av[0] = path;
  for (index = 0; index < argc; index++)
    av[index+1] = argv[index];
  av[index+1] = NULL;

  gettimeofday(&now, NULL);

  /* Print start */
  aspectworld.profile("Start execution of ");
  for (index = 0; index < argc+1; index++)
    {
      snprintf(buf, BUFSIZ - 1, "%s ", av[index]);
      aspectworld.profile(buf);
    }
  aspectworld.profile("\n");

  /* Start with arguments */
  if (!fork())
    ret = execv(av[0], av);
  else
    wait(NULL);

  gettimeofday(&after, NULL);

  difftime = (after.tv_sec - now.tv_sec) + ((double) (after.tv_usec - now.tv_usec)) * 0.000001;

  snprintf(buf, BUFSIZ - 1, "End execution (%.6f sec)\n\n", difftime);
  aspectworld.profile(buf);

  XFREE(__FILE__, __FUNCTION__, __LINE__, path);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
