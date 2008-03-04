/*
 * ** kvirtm.c for libkernsh
 * ** 
 * ** $Id: kvirtm.c,v 1.0 2008-02-25 20:05:00 pouik Exp $
 * **
 * */

#include "libkernsh.h"
#include "libkernsh-virtm.h"

int kernsh_kvirtm_read_virtm(pid_t pid, unsigned long addr, char *buffer, int len)
{
  int		ret, get;
  u_int         dim[3];
  vector_t      *krv;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  get = (int)config_get_data(LIBKERNSH_VMCONFIG_VIRTM);

  krv = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KVIRTMREADVIRTM);
  dim[0] = libkernshworld.os;
  dim[1] = get;

  fct = aspect_vectors_select(krv, dim);

  ret = fct(pid, addr, buffer, len);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

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
  read(fd, buffer, len);  
  printf("LEN %d\n", len);
  XCLOSE(fd, -1);

  XFREE(__FILE__, __FUNCTION__, __LINE__, proc_entry_root_tmp);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_kvirtm_openmem()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__  
  printf("OPEN KVIRTM OPEN MEM\n");
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_kvirtm_closemem()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__  
  printf("OPEN KVIRTM CLOSE MEM\n");
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_kvirtm_readmem(unsigned long addr, char *buffer, int len)
{
  int		ret, get, i, j, max_size;
  u_int         dim[3];
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

  if (len > max_size && get == LIBKERNSH_PROC_MODE)
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
    
  rlen = kernsh_syscall((int)config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL), 5, arg);

  if (rlen != len)
    ret = -1;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

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
  read(fd, buffer, len);  
  XCLOSE(fd, -1);

  XFREE(__FILE__, __FUNCTION__, __LINE__, proc_entry_root_tmp);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
