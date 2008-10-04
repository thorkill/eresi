/*
** @file kvirtm.c
** @ingroup libkernsh_user
**
*/

#include "libkernsh.h"

/**
 * @brief Read virtual memory of a pid\n
 * Configure :\n
 * LIBKERNSH_VMCONFIG_VIRTM
 * @param pid The process id
 * @param addr The address to read
 * @param buffer Read virtual memory into the buffer
 * @param len Count bytes to read
 * @return len on success, -1 on error
 */
int kernsh_kvirtm_read_virtm(pid_t pid, unsigned long addr, char *buffer, int len)
{
  int		ret, get, i, j, max_size;
  unsigned int         dim[3];
  vector_t      *krv;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  get = (int)config_get_data(LIBKERNSH_VMCONFIG_VIRTM);
  max_size = LIBKERNSH_PROC_ENTRY_SIZE;

  krv = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KVIRTMREADVIRTM);
  dim[0] = libkernshworld.os;
  dim[1] = get;

  fct = aspect_vectors_select(krv, dim);

  ret = 0;

  if (len > max_size && get == LIBKERNSH_KERNEL_MODE)
    {
      i = len / max_size;
      for (j = 0; j < i; j++)
	{
	  ret += fct(pid, addr+(max_size*j), buffer+(max_size*j), max_size);
	}

      if ((max_size*i) < len)
	{
	  ret += fct(pid, addr+(max_size*i), buffer+(max_size*i), (len - (max_size*i)));
	}
    }
  else
    ret = fct(pid, addr, buffer, len);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Read virtual memory of a pid in /proc
 * @param pid The process id
 * @param addr The address to read
 * @param buffer Read virtual memory into the buffer
 * @param len Count bytes to read
 * @return len on success, -1 on error
 */
int kernsh_kvirtm_read_virtm_proc_linux(pid_t pid, unsigned long addr, char *buffer, int len)
{
  int fd, ret, nlen;
  char *proc_entry_root_tmp;
  char	buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  memset(buff, '\0', sizeof(buff));

  if (len > 1024)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Len is too long > 1024",
		 -1);

  nlen = strlen(PROC_ENTRY_ROOT) + 
    strlen(PROC_ENTRY_KERNSH_VIRTM) + 
    strlen(PROC_ENTRY_KERNSH_VIRTM_MAX) + 
    2;

  XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	 proc_entry_root_tmp, 
	 nlen, 
	 -1);
  
  memset(proc_entry_root_tmp, '\0', nlen);
 
  snprintf(proc_entry_root_tmp, nlen, "%s%s/%s", 
	   PROC_ENTRY_ROOT, 
	   PROC_ENTRY_KERNSH_VIRTM,
	   PROC_ENTRY_KERNSH_VIRTM_VIO_INFO);
 
  XOPEN(fd, proc_entry_root_tmp, O_RDWR, 0777, -1);
  snprintf(buff, sizeof(buff), "%d:0x%lx:%d:%d", 
	   LIBKERNSH_VIRTM_READ_MEM_PID, 
	   addr,
	   len,
	   pid);

  ret = write(fd, buff, strlen(buff));
  if (ret != strlen(buff))
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Impossible to set vio",
		   -1);
    }

  XCLOSE(fd, -1);

  snprintf(proc_entry_root_tmp, nlen, "%s%s/%s", 
	   PROC_ENTRY_ROOT, 
	   PROC_ENTRY_KERNSH_VIRTM,
	   PROC_ENTRY_KERNSH_VIRTM_VIO);
  
  XOPEN(fd, proc_entry_root_tmp, O_RDWR, 0777, -1);
  ret = read(fd, buffer, len);  
  XCLOSE(fd, -1);
  

  XFREE(__FILE__, __FUNCTION__, __LINE__, proc_entry_root_tmp);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Read virtual memory of a pid with syscall
 * @param pid The process id
 * @param addr The address to read
 * @param buffer Read virtual memory into the buffer
 * @param len Count bytes to read
 * @return len on success, -1 on error
 */
int kernsh_kvirtm_read_virtm_syscall_linux(pid_t pid, unsigned long addr, char *buffer, int len)
{
  int ret;
  unsigned int arg[5];
  int rlen;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__  
  printf("kernsh_kvirtm_read_virtm_syscall_linux\n");
#endif

  ret = 0;

  arg[0] = (unsigned int)pid;
  arg[1] = (unsigned int)(addr - libkernshworld.kernel_start);
  arg[2] = (unsigned int)buffer;
  arg[3] = (unsigned int)len;
  arg[4] = (unsigned int)LIBKERNSH_VIRTM_READ_MEM_PID;
    
  rlen = kernsh_syscall((int)config_get_data(LIBKERNSH_VMCONFIG_VIRTM_NIL_SYSCALL), 5, arg);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, rlen);
}

/**
 * @brief Write virtual memory of a pid\n
 * Configure :\n
 * LIBKERNSH_VMCONFIG_VIRTM
 * @param pid The process id
 * @param addr The address to write
 * @param buffer Write buffer into virtual memory
 * @param len Count bytes to write
 * @return len on success, -1 on error
 */
int kernsh_kvirtm_write_virtm(pid_t pid, unsigned long addr, char *buffer, int len)
{
  int		ret, get, i, j, max_size;;
  unsigned int         dim[3];
  vector_t      *krv;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  get = (int)config_get_data(LIBKERNSH_VMCONFIG_VIRTM);
  max_size = LIBKERNSH_PROC_ENTRY_SIZE;

  krv = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KVIRTMWRITEVIRTM);
  dim[0] = libkernshworld.os;
  dim[1] = get;

  fct = aspect_vectors_select(krv, dim);

  ret = 0;

  if (len > max_size && get == LIBKERNSH_KERNEL_MODE)
    {
      i = len / max_size;
      for (j = 0; j < i; j++)
	{
	  ret += fct(pid, addr+(max_size*j), buffer+(max_size*j), max_size);
	}
      
      if ((max_size*i) < len)
	{
	  ret += fct(pid, addr+(max_size*i), buffer+(max_size*i), (len - (max_size*i)));
	}
    }
  else
    ret = fct(pid, addr, buffer, len);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Write virtual memory of a pid in /proc
 * @param pid The process id
 * @param addr The address to write
 * @param buffer Write buffer into virtual memory
 * @param len Count bytes to write
 * @return len on success, -1 on error
 */
int kernsh_kvirtm_write_virtm_proc_linux(pid_t pid, unsigned long addr, char *buffer, int len)
{
  int fd, ret, nlen;
  char *proc_entry_root_tmp;
  char	buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  memset(buff, '\0', sizeof(buff));

  if (len > 1024)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Len is too long > 1024",
		 -1);

  nlen = strlen(PROC_ENTRY_ROOT) + 
    strlen(PROC_ENTRY_KERNSH_VIRTM) + 
    strlen(PROC_ENTRY_KERNSH_VIRTM_MAX) + 
    2;

  XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	 proc_entry_root_tmp, 
	 nlen, 
	 -1);
  
  memset(proc_entry_root_tmp, '\0', nlen);
 
  snprintf(proc_entry_root_tmp, nlen, "%s%s/%s", 
	   PROC_ENTRY_ROOT, 
	   PROC_ENTRY_KERNSH_VIRTM,
	   PROC_ENTRY_KERNSH_VIRTM_VIO_INFO);
 
  XOPEN(fd, proc_entry_root_tmp, O_RDWR, 0777, -1);
  snprintf(buff, sizeof(buff), "%d:0x%lx:%d:%d", 
	   LIBKERNSH_VIRTM_WRITE_MEM_PID, 
	   addr,
	   len,
	   pid);

  ret = write(fd, buff, strlen(buff));
  if (ret != strlen(buff))
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Impossible to set vio",
		   -1);
    }

  XCLOSE(fd, -1);

  snprintf(proc_entry_root_tmp, nlen, "%s%s/%s", 
	   PROC_ENTRY_ROOT, 
	   PROC_ENTRY_KERNSH_VIRTM,
	   PROC_ENTRY_KERNSH_VIRTM_VIO);

  XOPEN(fd, proc_entry_root_tmp, O_RDWR, 0777, -1);
  ret = write(fd, buffer, len);
  XCLOSE(fd, -1);

  XFREE(__FILE__, __FUNCTION__, __LINE__, proc_entry_root_tmp);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Write virtual memory of a pid with syscall
 * @param pid The process id
 * @param addr The address to write
 * @param buffer Write buffer into virtual memory
 * @param len Count bytes to write
 * @return len on success, -1 on error
 */
int kernsh_kvirtm_write_virtm_syscall_linux(pid_t pid, unsigned long addr, char *buffer, int len)
{
  int ret;
  unsigned int arg[5];
  int rlen;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__  
  printf("kernsh_kvirtm_write_virtm_syscall_linux\n");
#endif

  ret = 0;

  arg[0] = (unsigned int)pid;
  arg[1] = (unsigned int)(addr - libkernshworld.kernel_start);
  arg[2] = (unsigned int)buffer;
  arg[3] = (unsigned int)len;
  arg[4] = (unsigned int)LIBKERNSH_VIRTM_WRITE_MEM_PID;
    
  rlen = kernsh_syscall((int)config_get_data(LIBKERNSH_VMCONFIG_VIRTM_NIL_SYSCALL), 5, arg);

  if (rlen != len)
    ret = -1;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Open kernsh-virtm module
 * @return 0 on success, -1 on error
 */
int kernsh_kvirtm_openmem()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__  
  printf("OPEN KVIRTM OPEN MEM\n");
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Close kernsh-virtm module
 * @return 0 on success, -1 on error
 */
int kernsh_kvirtm_closemem()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__  
  printf("OPEN KVIRTM CLOSE MEM\n");
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Read kernel memory with kernsh-virtm module
 * @param addr Address to read memory
 * @param buffer Read memory into the buffer
 * @param len Count bytes to read
 * @return len on success, -1 on error
 */
int kernsh_kvirtm_readmem(unsigned long addr, char *buffer, int len)
{
  int		ret, get, i, j, max_size;
  unsigned int         dim[3];
  vector_t      *krv;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = 0;
  max_size = LIBKERNSH_PROC_ENTRY_SIZE;

#if __DEBUG_KERNSH__  
  printf("kernsh_kvirtm_read_mem\n");
#endif

  get = (int)config_get_data(LIBKERNSH_VMCONFIG_VIRTM);

  krv = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KVIRTMREADMEM);
  dim[0] = libkernshworld.os;
  dim[1] = get;

  fct = aspect_vectors_select(krv, dim);

  if (len > max_size && get == LIBKERNSH_KERNEL_MODE)
    {
      i = len / max_size;
      for (j = 0; j < i; j++)
	{
	  ret += fct(addr+(max_size*j), buffer+(max_size*j), max_size);
	}

      if ((max_size*i) < len)
	{
	  ret += fct(addr+(max_size*i), buffer+(max_size*i), (len - (max_size*i)));
	}
    }
  else
    ret = fct(addr, buffer, len);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Read kernel memory with kernsh-virtm module thought syscall
 * @param addr Address to read memory
 * @param buffer Read memory into the buffer
 * @param len Count bytes to read
 * @return len on success, -1 on error
 */
int kernsh_kvirtm_readmem_syscall_linux(unsigned long addr, char *buffer, int len)
{
  int ret;
  unsigned int arg[5];
  int rlen;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__  
  printf("kernsh_kvirtm_read_mem_syscall_linux\n");
#endif

  ret = 0;

  arg[0] = (unsigned int)0;
  arg[1] = (unsigned int)(addr - libkernshworld.kernel_start);
  arg[2] = (unsigned int)buffer;
  arg[3] = (unsigned int)len;
  arg[4] = (unsigned int)LIBKERNSH_VIRTM_READ_MEM;
    
  rlen = kernsh_syscall((int)config_get_data(LIBKERNSH_VMCONFIG_VIRTM_NIL_SYSCALL), 5, arg);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, rlen);
}

/**
 * @brief Read kernel memory with kernsh-virtm module thought /proc
 * @param addr Address to read memory
 * @param buffer Read memory into the buffer
 * @param len Count bytes to read
 * @return len on success, -1 on error
 */
int kernsh_kvirtm_readmem_proc_linux(unsigned long addr, char *buffer, int len)
{
  int fd, ret, nlen;
  char *proc_entry_root_tmp;
  char	buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__  
  printf("kernsh_kvirtm_read_mem_proc\n");
#endif

  memset(buff, '\0', sizeof(buff));

  if (len > 1024)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Len is too long > 1024",
		 -1);

  nlen = strlen(PROC_ENTRY_ROOT) + 
    strlen(PROC_ENTRY_KERNSH_VIRTM) + 
    strlen(PROC_ENTRY_KERNSH_VIRTM_MAX) + 
    2;

  XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	 proc_entry_root_tmp, 
	 nlen, 
	 -1);
  
  memset(proc_entry_root_tmp, '\0', nlen);
 
  snprintf(proc_entry_root_tmp, nlen, "%s%s/%s", 
	   PROC_ENTRY_ROOT, 
	   PROC_ENTRY_KERNSH_VIRTM,
	   PROC_ENTRY_KERNSH_VIRTM_VIO_INFO);
 
  XOPEN(fd, proc_entry_root_tmp, O_RDWR, 0777, -1);
  snprintf(buff, sizeof(buff), "%d:0x%lx:%d:", 
	   LIBKERNSH_VIRTM_READ_MEM, 
	   (addr - libkernshworld.kernel_start),
	   len);

  ret = write(fd, buff, strlen(buff));
  if (ret != strlen(buff))
    {
      XCLOSE(fd, -1);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Impossible to set vio",
		   -1);
    }
  XCLOSE(fd, -1);

  snprintf(proc_entry_root_tmp, nlen, "%s%s/%s", 
	   PROC_ENTRY_ROOT, 
	   PROC_ENTRY_KERNSH_VIRTM,
	   PROC_ENTRY_KERNSH_VIRTM_VIO);
    
  XOPEN(fd, proc_entry_root_tmp, O_RDWR, 0777, -1);
  ret = read(fd, buffer, len);  
  XCLOSE(fd, -1);
  

  XFREE(__FILE__, __FUNCTION__, __LINE__, proc_entry_root_tmp);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/**
 * @brief Write into kernel memory with kernsh-virtm module
 * @param addr Address to write buffer
 * @param buffer Write buffer into memory
 * @param len Count bytes to write
 * @return len on success, -1 on error
 */
int kernsh_kvirtm_writemem(unsigned long addr, char *buffer, int len)
{
  int		ret, get, i, j, max_size;
  unsigned int         dim[3];
  vector_t      *krv;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = 0;
  max_size = LIBKERNSH_PROC_ENTRY_SIZE;

#if __DEBUG_KERNSH__  
  printf("kernsh_kvirtm_read_mem\n");
#endif

  get = (int)config_get_data(LIBKERNSH_VMCONFIG_VIRTM);

  krv = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KVIRTMWRITEMEM);
  dim[0] = libkernshworld.os;
  dim[1] = get;

  fct = aspect_vectors_select(krv, dim);

  if (len > max_size && get == LIBKERNSH_KERNEL_MODE)
    {
      i = len / max_size;
      for (j = 0; j < i; j++)
	{
	  ret += fct(addr+(max_size*j), buffer+(max_size*j), max_size);
	}

      if ((max_size*i) < len)
	{
	  ret += fct(addr+(max_size*i), buffer+(max_size*i), (len - (max_size*i)));
	}
    }
  else
    ret = fct(addr, buffer, len);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Write into kernel memory with kernsh-virtm module thought syscall
 * @param addr Address to write buffer
 * @param buffer Write buffer into memory
 * @param len Count bytes to write
 * @return len on success, -1 on error
 */
int kernsh_kvirtm_writemem_syscall_linux(unsigned long addr, char *buffer, int len)
{
  int ret;
  unsigned int arg[5];
  int rlen;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__  
  printf("kernsh_kvirtm_read_mem_syscall_linux\n");
#endif

  ret = 0;

  arg[0] = (unsigned int)0;
  arg[1] = (unsigned int)(addr - libkernshworld.kernel_start);
  arg[2] = (unsigned int)buffer;
  arg[3] = (unsigned int)len;
  arg[4] = (unsigned int)LIBKERNSH_VIRTM_WRITE_MEM;
    
  rlen = kernsh_syscall((int)config_get_data(LIBKERNSH_VMCONFIG_VIRTM_NIL_SYSCALL), 5, arg);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, rlen);
}

/**
 * @brief Write kernel memory with kernsh-virtm module thought /proc
 * @param addr Address to write buffer
 * @param buffer Write buffer into memory
 * @param len Count bytes to write
 * @return len on success, -1 on error
 */
int kernsh_kvirtm_writemem_proc_linux(unsigned long addr, char *buffer, int len)
{
  int fd, ret, nlen;
  char *proc_entry_root_tmp;
  char	buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__  
  printf("kernsh_kvirtm_read_mem_proc\n");
#endif

  memset(buff, '\0', sizeof(buff));

  if (len > 1024)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Len is too long > 1024",
		 -1);

  nlen = strlen(PROC_ENTRY_ROOT) + 
    strlen(PROC_ENTRY_KERNSH_VIRTM) + 
    strlen(PROC_ENTRY_KERNSH_VIRTM_MAX) + 
    2;

  XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	 proc_entry_root_tmp, 
	 nlen, 
	 -1);
  
  memset(proc_entry_root_tmp, '\0', nlen);
 
  snprintf(proc_entry_root_tmp, nlen, "%s%s/%s", 
	   PROC_ENTRY_ROOT, 
	   PROC_ENTRY_KERNSH_VIRTM,
	   PROC_ENTRY_KERNSH_VIRTM_VIO_INFO);
 
  XOPEN(fd, proc_entry_root_tmp, O_RDWR, 0777, -1);
  snprintf(buff, sizeof(buff), "%d:0x%lx:%d:", 
	   LIBKERNSH_VIRTM_WRITE_MEM, 
	   (addr - libkernshworld.kernel_start),
	   len);

  ret = write(fd, buff, strlen(buff));
  if (ret != strlen(buff))
    {
      XCLOSE(fd, -1);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Impossible to set vio",
		   -1);
    }
  XCLOSE(fd, -1);

  snprintf(proc_entry_root_tmp, nlen, "%s%s/%s", 
	   PROC_ENTRY_ROOT, 
	   PROC_ENTRY_KERNSH_VIRTM,
	   PROC_ENTRY_KERNSH_VIRTM_VIO);

  XOPEN(fd, proc_entry_root_tmp, O_RDWR, 0777, -1);
  ret = write(fd, buffer, len);
  XCLOSE(fd, -1);

  XFREE(__FILE__, __FUNCTION__, __LINE__, proc_entry_root_tmp);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Task pid\n
 * Configure :\n
 * LIBKERNSH_VMCONFIG_VIRTM
 * @param pid The process id
 * @param h The list
 * @return 0 on success, -1 on error
 */
int kernsh_kvirtm_task_pid(pid_t pid, list_t *h)
{
  int		ret, get;
  unsigned int         dim[3];
  vector_t      *krv;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = 0;

#if __DEBUG_KERNSH__  
  printf("kernsh_kvirtm_task_pid\n");
#endif

  get = (int)config_get_data(LIBKERNSH_VMCONFIG_VIRTM);

  krv = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KVIRTMTASKPID);
  dim[0] = libkernshworld.os;
  dim[1] = get;

  fct = aspect_vectors_select(krv, dim);

  ret = fct(pid, h);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Task pid thought /proc\n
 * Configure :\n
 * LIBKERNSH_VMCONFIG_VIRTM
 * @param pid The process id
 * @param h The list
 * @return 0 on success, -1 on error
 */
int kernsh_kvirtm_task_pid_proc_linux(pid_t pid, list_t *h)
{
  int fd, ret, nlen, blen;
  kvirtm_virtual_task_struct_t *kvtst;
  char *key, *proc_entry_root_tmp, *buffer, *p;
  char	buff[BUFSIZ];
  unsigned long *pkvtst;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = 0;

  XALLOC(__FILE__, 
	 __FUNCTION__, 
	 __LINE__, 
	 kvtst,
	 sizeof(kvirtm_virtual_task_struct_t), 
	 -1);

  XALLOC(__FILE__, 
	 __FUNCTION__, 
	 __LINE__, 
	 key,
	 BUFSIZ, 
	 -1);

  memset(kvtst, '\0', sizeof(kvirtm_virtual_task_struct_t)); 
  memset(key, '\0', BUFSIZ);
  snprintf(key,	BUFSIZ, "%d", pid);
  
  memset(buff, '\0', sizeof(buff));
  
  nlen = strlen(PROC_ENTRY_ROOT) + 
    strlen(PROC_ENTRY_KERNSH_VIRTM) + 
    strlen(PROC_ENTRY_KERNSH_VIRTM_MAX) + 
    2;
  
  XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	 proc_entry_root_tmp, 
	 nlen, 
	 -1);
  
  memset(proc_entry_root_tmp, '\0', nlen);
 
  snprintf(proc_entry_root_tmp, nlen, "%s%s/%s", 
	   PROC_ENTRY_ROOT, 
	   PROC_ENTRY_KERNSH_VIRTM,
	   PROC_ENTRY_KERNSH_VIRTM_VIO_INFO);

   
  blen = sizeof(kvirtm_virtual_task_struct_t) * 2 + 
    (sizeof(kvirtm_virtual_task_struct_t) / sizeof(unsigned long)) * 3;

  XOPEN(fd, proc_entry_root_tmp, O_RDWR, 0777, -1);
  snprintf(buff, sizeof(buff), "%d:0x0:%d:%d:", 
	   LIBKERNSH_VIRTM_TASK_PID, 
	   blen,
	   pid);

  ret = write(fd, buff, strlen(buff));
  if (ret != strlen(buff))
    {
      XCLOSE(fd, -1);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Impossible to set vio",
		   -1);
    }
  XCLOSE(fd, -1);
  

  snprintf(proc_entry_root_tmp, nlen, "%s%s/%s", 
	   PROC_ENTRY_ROOT, 
	   PROC_ENTRY_KERNSH_VIRTM,
	   PROC_ENTRY_KERNSH_VIRTM_VIO);

  XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	 buffer, 
	 blen + 1, 
	 -1);

  XOPEN(fd, proc_entry_root_tmp, O_RDWR, 0777, -1);
  ret = read(fd, buffer, blen);  
  XCLOSE(fd, -1);

  for (p = (char *)strtok(buffer, ":"), pkvtst = (unsigned long *)kvtst; 
       p;  
       p = (char *)strtok(NULL, ":"), pkvtst++)
    {
      *pkvtst = strtoul(p, NULL, 16);
    }

  elist_add(h, key, (void *) kvtst);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Task pid thought syscall\n
 * Configure :\n
 * LIBKERNSH_VMCONFIG_VIRTM
 * @param pid The process id
 * @param h The list
 * @return 0 on success, -1 on error
 */
int kernsh_kvirtm_task_pid_syscall_linux(pid_t pid, list_t *h)
{
  int		ret;
  //  kvirtm_virtual_task_struct_t *kvtst;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = 0;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}
