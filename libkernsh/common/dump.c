/*
** @file dump.c
** @ingroup libkernsh_common
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
  unsigned int         dim[3];
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

/**
 * @brief Get vma of a process id\n
 * Configure :\n
 * LIBKERNSH_VMCONFIG_VMA_PREFIX, LIBKERNSH_VMCONFIG_VMA
 * @param pid The process id
 * @return list_t on success, NULL on error
 */
list_t *kernsh_kdump_get_vma(pid_t pid)
{
  int ret, get;
  char *key;
  unsigned int         dim[3];
  list_t	*l;
  vector_t      *kgv;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(__FILE__, __FUNCTION__, __LINE__, key, BUFSIZ, NULL);
  memset(key, '\0', BUFSIZ);

  snprintf(key, BUFSIZ, "%s_%d", (char *)config_get_data(LIBKERNSH_VMCONFIG_VMA_PREFIX), pid);

  XALLOC(__FILE__, __FUNCTION__, __LINE__, l, sizeof(list_t), NULL);

  if (elist_init(l, key, ASPECT_TYPE_HASH) == 1)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Failed to initialize list", NULL);
    }

  get = (int)config_get_data(LIBKERNSH_VMCONFIG_VMA);

  kgv = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KDUMPGETVMA);
  dim[0] = libkernshworld.os;
  dim[1] = get;

  fct = aspect_vectors_select(kgv, dim);

  //  ret = fct(pid, l);
  ret = kernsh_kdump_get_vma_userland_linux(pid, l);

  if (ret < 0)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Failed to get vma", NULL);
    }

  l = elist_reverse(l);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, l);
}

/**
 * @brief Get vma of a process id from userland\n
 * Configure :\n
 * LIBKERNSH_VMCONFIG_VMA_PREFIX
 * @param pid The process id
 * @param l list_t to store vma
 * @return 0 on success, -1 on error
 */
int kernsh_kdump_get_vma_userland_linux(pid_t pid, list_t *l)
{
  int ret, fd, i, j;
  char buff[BUFSIZ];
  char line[BUFSIZ];
  char *tok, *subtok, *brk, *subbrk, *subsubtok, *subsubbrk, *key;
  libkernshvma_struct_t *vst;
  hash_t *h;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = 0;
  tok = subtok = subsubtok = brk = subbrk = subsubbrk = key = NULL;

  memset(buff, '\0', sizeof(buff));
  snprintf(buff, sizeof(buff), "/proc/%d/maps", pid);

  XOPEN(fd, buff, O_RDONLY, 0,-1);
  
  ret = read(fd, line, sizeof(line));
  
  if (ret <= 0)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Failed to read /proc", -1);
    }

  for (tok = strtok_r(line, "\n", &brk), j = 0; 
       tok; 
       tok = strtok_r(NULL, "\n", &brk), j++)
    {
      XALLOC(__FILE__, __FUNCTION__, __LINE__, vst, sizeof(libkernshvma_struct_t), -1);
      XALLOC(__FILE__, __FUNCTION__, __LINE__, h, sizeof(hash_t), -1);
      XALLOC(__FILE__, __FUNCTION__, __LINE__, key, BUFSIZ, -1);

      for (subtok = strtok_r(tok, " ", &subbrk), i = 0;
	   subtok; 
	   subtok = strtok_r(NULL, " ", &subbrk), i++)
	{
	  switch (i)
	    {
	    case 0 :
	      subsubtok = strtok_r(subtok, "-", &subsubbrk);
	      vst->vm_start = strtoul(subsubtok, NULL, 16);
	      subsubtok = strtok_r(NULL, "-", &subsubbrk);
	      vst->vm_end = strtoul(subsubtok, NULL, 16);
	      break;
	    }
	}

      memset(key, '\0', BUFSIZ);
      snprintf(key, BUFSIZ, "%s_%d_%d", 
	       (char *)config_get_data(LIBKERNSH_VMCONFIG_VMA_PREFIX), pid, j);

      if (hash_init(h, key, 2, ASPECT_TYPE_LONG) == 1)
	{
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Failed to initialize hash", -1);
	}

      hash_add(h, LIBKERNSH_STRING_VM_START, (void *)vst->vm_start);
      hash_add(h, LIBKERNSH_STRING_VM_END,   (void *)vst->vm_end);

      XFREE(__FILE__, __FUNCTION__, __LINE__, vst);

      elist_add(l, key, (void *)h);
    }

  XCLOSE(fd, -1);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Get vma of a process id from kernelland\n
 * Configure :\n
 * LIBKERNSH_VMCONFIG_VMA_PREFIX
 * @param pid The process id
 * @param l list_t to store vma
 * @return 0 on success, -1 on error
 */
int kernsh_kdump_get_vma_kernelland_linux(pid_t pid, list_t *l)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);
}

/**
 * @brief Dump vma of a process id\n
 * Configure :\n
 * LIBKERNSH_VMCONFIG_VMA_PREFIX, LIBKERNSH_VMCONFIG_VMA, LIBKERNSH_VMCONFIG_STORAGE_PATH
 * @param pid The process id
 * @return 0 on success, -1 on error
 */
int kernsh_kdump_vma(pid_t pid)
{
  int ret, tret, fd, index, get;
  listent_t *actual;
  list_t *l;
  hash_t *h;
  char *sav_val;
  char buff[BUFSIZ];
  char meta[BUFSIZ];
  unsigned int         dim[3];
  vector_t      *kv;
  int          (*fct)();
  unsigned long meta_start, meta_end, meta_size;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = 0;

  sav_val = (char *)config_get_data(LIBKERNSH_VMCONFIG_VMA_PREFIX);

  config_update_key(LIBKERNSH_VMCONFIG_VMA_PREFIX, (void *)"tmp_vma");

  l = kernsh_kdump_get_vma(pid);

  if (l == NULL)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Failed to get vma", -1);
    }

  get = (int)config_get_data(LIBKERNSH_VMCONFIG_VMA);

  kv = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KDUMPVMA);
  dim[0] = libkernshworld.os;
  dim[1] = get;

  fct = aspect_vectors_select(kv, dim);

  memset(buff, '\0', sizeof(buff));
  snprintf(buff, sizeof(buff), "%s%s_%d", 
	   (char *) config_get_data(LIBKERNSH_VMCONFIG_STORAGE_PATH),
	   (char *) config_get_data(LIBKERNSH_VMCONFIG_DUMP_VMA_PREFIX),
	   pid);

  mkdir(buff, 0777);

  memset(buff, '\0', sizeof(buff));
  snprintf(buff, sizeof(buff), "%s%s_%d/metadata_%d", 
	   (char *) config_get_data(LIBKERNSH_VMCONFIG_STORAGE_PATH),
	   (char *) config_get_data(LIBKERNSH_VMCONFIG_DUMP_VMA_PREFIX),
	   pid,
	   get);

  XOPEN(fd, buff, O_RDWR|O_CREAT, 0777, -1);

  for (index = 0, actual = l->head;
       index < l->elmnbr;
       index++, actual = actual->next)
    {
      h = (hash_t *) actual->data;
      printf("%s[%d]", l->name, index);
      if (h)
	{
	  memset(meta, '\0', sizeof(meta));

	  tret = fct(pid, h);
	  ret += tret;

	  meta_start = (int)hash_get(h, LIBKERNSH_STRING_VM_START);
	  meta_end = (int)hash_get(h, LIBKERNSH_STRING_VM_END);
	  meta_size = meta_end - meta_start;

	  snprintf(meta, sizeof(meta), "%s[%d] 0x%lx ... 0x%lx strlen(0x%lx) ==> [0x%lx]\n", 
		   l->name, 
		   index,
		   meta_start,
		   meta_end,
		   meta_size,
		   (unsigned long)tret);
	  XWRITE(fd, meta, strlen(meta), -1);  
	}
      printf("\n");
    }

  XCLOSE(fd, -1);

  hash_del(hash_lists, l->name);
  elist_destroy(l);

  config_update_key(LIBKERNSH_VMCONFIG_VMA_PREFIX, sav_val);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Dump vma of a process id from userland\n
 * Configure :\n
 * LIBKERNSH_VMCONFIG_VMA_PREFIX, LIBKERNSH_VMCONFIG_STORAGE_PATH
 * @param pid The process id
 * @param h the hash table to store vma struct
 * @return 0 on success, -1 on error
 */
int kernsh_kdump_vma_userland_linux(pid_t pid, hash_t *h)
{
  int ret, fd, i;
  unsigned long vm_start, vm_end, size;
  char buff[BUFSIZ];
  char *new_vma;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = 0;

#if defined(__linux__)
  if (ptrace(PTRACE_ATTACH, pid, 0, 0) < 0)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Failed to attach pid", -1);
    }
  waitpid(pid, NULL, WUNTRACED);
#endif

  vm_start = (int)hash_get(h, LIBKERNSH_STRING_VM_START);
  vm_end = (int)hash_get(h, LIBKERNSH_STRING_VM_END);

  size = vm_end - vm_start;

  printf("VM_* 0x%lx -> 0x%lx ==> strlen(0x%lx)\n", vm_start, vm_end, size);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, new_vma, size + 1, -1);
  
  memset(new_vma, '\0', size);

#if defined(__linux__)
  for (i=0; i < size; i++)
    {
      new_vma[i]=(char)ptrace(PTRACE_PEEKTEXT, pid, vm_start+i, 0);
      if (errno == 0)
	ret++;
    }
#endif

#if defined(__linux__)
  ptrace(PTRACE_DETACH, pid, 0, 0);
#endif

  memset(buff, '\0', sizeof(buff));
  snprintf(buff, sizeof(buff), "%s%s_%d/dump_userland_0x%lx", 
	   (char *) config_get_data(LIBKERNSH_VMCONFIG_STORAGE_PATH),
	   (char *) config_get_data(LIBKERNSH_VMCONFIG_DUMP_VMA_PREFIX),
	   pid, 
	   vm_start);

  printf("DUMP USERLAND 0x%lx into %s\n", vm_start, buff);

  XOPEN(fd, buff, O_RDWR|O_CREAT, 0777, -1);
  XWRITE(fd, new_vma, size, -1);
  XCLOSE(fd, -1);
  
  XFREE(__FILE__, __FUNCTION__, __LINE__, new_vma);
    
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Dump vma of a process id from kernelland\n
 * Configure :\n
 * LIBKERNSH_VMCONFIG_VMA_PREFIX, LIBKERNSH_VMCONFIG_STORAGE_PATH
 * @param pid The process id
 * @param h the hash table to store vma struct
 * @return 0 on success, -1 on error
 */
int kernsh_kdump_vma_kernelland_linux(pid_t pid, hash_t *h)
{
  int ret, fd;
  unsigned long vm_start, vm_end, size;
  char buff[BUFSIZ];
  char *new_vma;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = 0;

  vm_start = (int)hash_get(h, LIBKERNSH_STRING_VM_START);
  vm_end = (int)hash_get(h, LIBKERNSH_STRING_VM_END);

  size = vm_end - vm_start;

  printf("VM_* 0x%lx -> 0x%lx ==> strlen(0x%lx)\n", vm_start, vm_end, size);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, new_vma, size + 1, -1);
  
  memset(new_vma, '\0', size);

  ret = kernsh_kvirtm_read_virtm(pid, vm_start, new_vma, (int)size);
  if (ret <= 0)
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__, new_vma);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Failed to dump vma", -1);
    }
  
  memset(buff, '\0', sizeof(buff));
  snprintf(buff, sizeof(buff), "%s%s_%d/dump_kernelland_0x%lx", 
	   (char *) config_get_data(LIBKERNSH_VMCONFIG_STORAGE_PATH),
	   (char *) config_get_data(LIBKERNSH_VMCONFIG_DUMP_VMA_PREFIX),
	   pid, 
	   vm_start);

  printf("DUMP KERNELLAND 0x%lx into %s\n", vm_start, buff);

  XOPEN(fd, buff, O_RDWR|O_CREAT, 0777, -1);
  XWRITE(fd, new_vma, size, -1);
  XCLOSE(fd, -1);

  XFREE(__FILE__, __FUNCTION__, __LINE__, new_vma);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

