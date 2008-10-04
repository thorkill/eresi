/**
 * @file check_func.c
** @ingroup libetrace
** 
** @brief All functions that help check, remote untracable content.
** 
** Started Jul 2 2005 00:03:44 mxatone
** 
**
** $Id: check_untracable.c,v 1.2 2007-11-29 10:25:02 rival Exp $
**
*/
#include "libetrace.h"

/* Hash table of untracable functions */
hash_t		traces_untracable;
unsigned char		untracable_ostype;


/**
 * @brief Fix this doxygen tag.
 * @param name
 * @return
 */
static int		etrace_add_untracable(char *name)
{
  NOPROFILER_IN();

  hash_add(&traces_untracable, strdup(name), (void *)1);

  NOPROFILER_ROUT(0);
}

/**
 * @brief Fix this doxygen tag.
 * @param file
 * @param name
 * @return
 */
int		etrace_untracable(elfshobj_t *file, char *name)
{
  unsigned char		ostype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ostype = elfsh_get_ostype(file);

  /* Init the table if needed */
  if (traces_untracable.ent == NULL || ostype != untracable_ostype)
    {
      if (traces_untracable.ent == NULL)
	hash_empty(TRACE_UNTRACABLE_NAME);

      hash_init(&traces_untracable, TRACE_UNTRACABLE_NAME, 11, ASPECT_TYPE_UNKNOW);

      /* Not support printf like functions */
      etrace_add_untracable("printf");
      etrace_add_untracable("fprintf");
      etrace_add_untracable("sprintf");
      etrace_add_untracable("snprintf");
      etrace_add_untracable("vprintf");
      etrace_add_untracable("vfprintf");
      etrace_add_untracable("vsprintf");
      etrace_add_untracable("vsnprintf");

      /* sscanf use a printf like prototype */
      etrace_add_untracable("scanf");
      etrace_add_untracable("fscanf");
      etrace_add_untracable("sscanf");
      etrace_add_untracable("vscanf");
      etrace_add_untracable("vsscanf");
      etrace_add_untracable("vfscanf");

      /* ioctl use a printf like prototype */
      etrace_add_untracable("ioctl");
      
      if (ostype == ELFSH_OS_LINUX)
	{
	  etrace_add_untracable("__libc_start_main");
	  etrace_add_untracable("_start");
	  etrace_add_untracable("_init");
	  etrace_add_untracable("_fini");
	}

      if (ostype == ELFSH_OS_FREEBSD)
	{
	  // x86 failed on this function
	  // TODO: correct them
	  etrace_add_untracable("getcwd");
	}

      untracable_ostype = ostype;
    }

  if (hash_get(&traces_untracable, name) != NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
}
