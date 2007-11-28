/**
 * @file check_func.c
** check_func.c for libetrace
** 
** @brief All functions that help check, remote untracable content 
** 
** Started Jul 2 2005 00:03:44 mxatone
** 
**
** $Id: check_untracable.c,v 1.1 2007-11-28 09:32:06 rival Exp $
**
*/

#include "libelfsh.h"
#include "libasm.h"
#include "libetrace.h"

static int		elfsh_traces_add_untracable(char *name)
{
  NOPROFILER_IN();

  hash_add(&traces_untracable, strdup(name), (void *)1);

  NOPROFILER_ROUT(0);
}

int		elfsh_traces_untracable(elfshobj_t *file, char *name)
{
  u_char		ostype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ostype = elfsh_get_ostype(file);

  /* Init the table if needed */
  if (traces_untracable.ent == NULL || ostype != untracable_ostype)
    {
      if (traces_untracable.ent == NULL)
	hash_empty(TRACE_UNTRACABLE_NAME);

      hash_init(&traces_untracable, TRACE_UNTRACABLE_NAME, 11, ASPECT_TYPE_UNKNOW);

      /* Not support printf like functions */
      elfsh_traces_add_untracable("printf");
      elfsh_traces_add_untracable("fprintf");
      elfsh_traces_add_untracable("sprintf");
      elfsh_traces_add_untracable("snprintf");
      elfsh_traces_add_untracable("vprintf");
      elfsh_traces_add_untracable("vfprintf");
      elfsh_traces_add_untracable("vsprintf");
      elfsh_traces_add_untracable("vsnprintf");

      /* sscanf use a printf like prototype */
      elfsh_traces_add_untracable("scanf");
      elfsh_traces_add_untracable("fscanf");
      elfsh_traces_add_untracable("sscanf");
      elfsh_traces_add_untracable("vscanf");
      elfsh_traces_add_untracable("vsscanf");
      elfsh_traces_add_untracable("vfscanf");

      /* ioctl use a printf like prototype */
      elfsh_traces_add_untracable("ioctl");
      
      if (ostype == ELFSH_OS_LINUX)
	{
	  elfsh_traces_add_untracable("__libc_start_main");
	  elfsh_traces_add_untracable("_start");
	  elfsh_traces_add_untracable("_init");
	  elfsh_traces_add_untracable("_fini");
	}

      if (ostype == ELFSH_OS_FREEBSD)
	{
	  // x86 failed on this function
	  // TODO: correct them
	  elfsh_traces_add_untracable("getcwd");
	}

      untracable_ostype = ostype;
    }

  if (hash_get(&traces_untracable, name) != NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
}
