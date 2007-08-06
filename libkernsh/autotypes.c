/*
** autotypes.c for libkernsh
**
** $Id: autotypes.c,v 1.1 2007-08-06 15:40:39 pouik Exp $
**
*/
#include "libkernsh.h"
#include "libaspect.h"

int kernsh_autotypes()
{
  int ret;
  u_int         dim[3];
  vector_t      *autotypes;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!libkernshworld.open)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Memory not open !", -1);
    }

  autotypes = aspect_vector_get("autotypes");
  dim[0] = libkernshworld.arch;
  dim[1] = libkernshworld.os;
  
  fct = aspect_vectors_select(autotypes, dim);
  
  ret = fct();

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int kernsh_autotypes_linux_2_6()
{
  int ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  ret = kernsh_autotask_linux_2_6();
  if (ret)
    {

    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_autotask_linux_2_6()
{
  unsigned long init_task;
  char buffer[1024];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  kernsh_get_addr_by_name("init_task", &init_task, strlen("init_task"));
  
  kernsh_readmem(init_task, buffer, sizeof(buffer));


  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_autotypes_linux_2_4()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_autotask_linux_2_4()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_autotypes_netbsd()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_autotask_netbsd()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_autotypes_freebsd()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_autotask_freebsd()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
