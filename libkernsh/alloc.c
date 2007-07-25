/*
** alloc.c for libkernsh : allocation contiguous/non contiguous
**
** $Id: alloc.c,v 1.1 2007-07-25 19:53:01 pouik Exp $
**
*/
#include "libkernsh.h"
#include "libkernsh-symbols.h"

int kernsh_alloc_contiguous(size_t size, unsigned long *addr)
{
  int ret;
  u_int         dim[2];
  vector_t      *alloc;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  alloc = aspect_vector_get("alloc_contiguous");
  dim[0] = libkernshworld.os;

  fct = aspect_vectors_select(alloc, dim);

  ret = fct(size, addr);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int kernsh_alloc_noncontiguous(size_t size, unsigned long *addr)
{
  int ret;
  u_int         dim[2];
  vector_t      *alloc;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  alloc = aspect_vector_get("alloc_noncontiguous");
  dim[0] = libkernshworld.os;

  fct = aspect_vectors_select(alloc, dim);

  ret = fct(size, addr);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int kernsh_free_contiguous(unsigned long addr)
{
  int ret;
  u_int         dim[2];
  vector_t      *alloc;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  alloc = aspect_vector_get("free_contiguous");
  dim[0] = libkernshworld.os;

  fct = aspect_vectors_select(alloc, dim);

  ret = fct(addr);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int kernsh_free_noncontiguous(unsigned long addr)
{
  int ret;
  u_int         dim[2];
  vector_t      *alloc;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  alloc = aspect_vector_get("free_noncontiguous");
  dim[0] = libkernshworld.os;

  fct = aspect_vectors_select(alloc, dim);

  ret = fct(addr);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int kernsh_alloc_contiguous_linux(size_t size, unsigned long *addr)
{
  int ret;
  char buf[sizeof(KMALLOC)];
  unsigned long kaddr;
  unsigned long nil_syscall;
  libkernshkma_t kmalloc;
  unsigned int arg[1];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = kernsh_get_addr_by_name("__kmalloc", &kaddr, strlen("__kmalloc"));
  if(ret)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unable to find symbol kmalloc", -1);
    }

  kmalloc.kexec = (void *)kaddr;
  kmalloc.size = size;
  kmalloc.flags = GFP_KERNEL;
  kmalloc.mem = 0;

  arg[0] = (unsigned int)&kmalloc;
  
  kernsh_readmem(libkernshworld.kernel_start, &buf, sizeof(buf));
  kernsh_writemem(libkernshworld.kernel_start, KMALLOC, sizeof(KMALLOC));

  kernsh_readmem(libkernshworld.sct + 
		 sizeof(unsigned long) * (int) config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL),
		 &nil_syscall, 
		 sizeof(unsigned long));
 
  kernsh_writemem(libkernshworld.sct + 
		  sizeof(unsigned long) * (int) config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL),
		  &libkernshworld.kernel_start,
		  sizeof(unsigned long));
  kernsh_syscall((int)config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL), 1, arg);

  *addr = kmalloc.mem;
  
  kernsh_writemem(libkernshworld.sct + 
		  sizeof(unsigned long) * (int) config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL),
		  &nil_syscall,
		  sizeof(unsigned long));
  
  kernsh_writemem(libkernshworld.kernel_start, &buf, sizeof(buf));

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_free_contiguous_linux(unsigned long addr)
{
  int ret;
  char buf[sizeof(KFREE)];
  unsigned long kaddr;
  unsigned long nil_syscall;
  libkernshkfr_t kfree;
  unsigned int arg[1];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = kernsh_get_addr_by_name("kfree", &kaddr, strlen("kfree"));
  if(ret)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unable to find symbol kfree", -1);
    }

  kfree.kexec = (void *)kaddr;
  kfree.address = (const void *) addr;

  arg[0] = (unsigned int)&kfree;
  
  kernsh_readmem(libkernshworld.kernel_start, &buf, sizeof(buf));
  kernsh_writemem(libkernshworld.kernel_start, KFREE, sizeof(KFREE));

  kernsh_readmem(libkernshworld.sct + 
		 sizeof(unsigned long) * (int) config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL),
		 &nil_syscall, 
		 sizeof(unsigned long));
  kernsh_writemem(libkernshworld.sct + 
		  sizeof(unsigned long) * (int) config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL),
		  &libkernshworld.kernel_start,
		  sizeof(unsigned long));
  kernsh_syscall((int)config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL), 1, arg);

  kernsh_writemem(libkernshworld.sct + 
		  sizeof(unsigned long) * (int) config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL),
		  &nil_syscall,
		  sizeof(unsigned long));
  
  kernsh_writemem(libkernshworld.kernel_start, &buf, sizeof(buf));

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_alloc_noncontiguous_linux(size_t size, unsigned long *addr)
{
  int ret;
  char buf[sizeof(VMALLOC)];
  unsigned long kaddr;
  unsigned long nil_syscall;
  libkernshvma_t vmalloc;
  unsigned int arg[1];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = kernsh_get_addr_by_name("vmalloc", &kaddr, strlen("vmalloc"));
  if(ret)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unable to find symbol vmalloc", -1);
    }
    
  vmalloc.kexec = (void *)kaddr;
  vmalloc.size = size;
  vmalloc.mem = 0;
  
  arg[0] = (unsigned int)&vmalloc;
  
  kernsh_readmem(libkernshworld.kernel_start, &buf, sizeof(buf));
  kernsh_writemem(libkernshworld.kernel_start, VMALLOC, sizeof(VMALLOC));
  
  kernsh_readmem(libkernshworld.sct +
		 sizeof(unsigned long) * (int) config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL),
		 &nil_syscall, 
		 sizeof(unsigned long));
  kernsh_writemem(libkernshworld.sct + 
		  sizeof(unsigned long) * (int) config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL),
		  &libkernshworld.kernel_start,
		  sizeof(unsigned long));
  kernsh_syscall((int)config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL), 1, arg);

  *addr = vmalloc.mem;

  kernsh_writemem(libkernshworld.sct + 
		  sizeof(unsigned long) * (int) config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL),
		  &nil_syscall,
		  sizeof(unsigned long));
  
  kernsh_writemem(libkernshworld.kernel_start, &buf, sizeof(buf));
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_free_noncontiguous_linux(unsigned long addr)
{
  int ret;
  char buf[sizeof(VFREE)];
  unsigned long kaddr;
  unsigned long nil_syscall;
  libkernshvfr_t vfree;
  unsigned int arg[1];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = kernsh_get_addr_by_name("vfree", &kaddr, strlen("vfree"));
  if(ret)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unable to find symbol vfree", -1);
    }

  vfree.kexec = (void *)kaddr;
  vfree.address = (void *) addr;

  arg[0] = (unsigned int)&vfree;
  
  kernsh_readmem(libkernshworld.kernel_start, &buf, sizeof(buf));
  kernsh_writemem(libkernshworld.kernel_start, VFREE, sizeof(VFREE));

  kernsh_readmem(libkernshworld.sct + 
		 sizeof(unsigned long) * (int) config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL),
		 &nil_syscall, 
		 sizeof(unsigned long));
  printf("NIL SYSCALL 0x%lx\n", nil_syscall);
  kernsh_writemem(libkernshworld.sct + 
		  sizeof(unsigned long) * (int) config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL),
		  &libkernshworld.kernel_start,
		  sizeof(unsigned long));
  kernsh_syscall((int)config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL), 1, arg);

  kernsh_writemem(libkernshworld.sct + 
		  sizeof(unsigned long) * (int) config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL),
		  &nil_syscall,
		  sizeof(unsigned long));
  
  kernsh_writemem(libkernshworld.kernel_start, &buf, sizeof(buf));

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  
}
