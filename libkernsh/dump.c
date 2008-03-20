/*
** @file dump.c
** @ingroup libkernsh
**
*/

#include "libkernsh.h"

/**
 * @brief Dump elf\n
 * Configure :\n
 * LIBKERNSH_VMCONFIG_VIRTM
 * @param pid The process id
 * @param filename The output filename
 * @return 0 on success, -1 on error
 */
int kernsh_dump_kvirtm_elf(pid_t pid, char *filename)
{
  int		ret, get;
  u_int         dim[3];
  vector_t      *krv;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = 0;

#if __DEBUG_KERNSH__  
  printf("kernsh_kvirtm_dump_elf\n");
#endif

  get = (int)config_get_data(LIBKERNSH_VMCONFIG_VIRTM);

  krv = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KVIRTMDUMPELF);
  dim[0] = libkernshworld.os;
  dim[1] = get;

  fct = aspect_vectors_select(krv, dim);

  ret = fct(pid, filename);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int kernsh_get_vma_userland(pid_t pid, list_t *l)
{
  int ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = -1;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int kernsh_dump_vma_userland(pid_t pid)
{
  int ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = -1;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}
