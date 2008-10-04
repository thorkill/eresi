/*
** @file alloc.c
** @ingroup libkernsh_user
** @brief allocation contiguous/non contiguous.
**
*/
#include "libkernsh.h"
#include "libkernsh-symbols.h"
#include "libkernsh-info.h"

/**
 * @brief Allocate contiguous/non contiguous kernel memory\n
 * Configure :\n
 * LIBKERNSH_VMCONFIG_ALLOC
 * @param size Allocate size bytes
 * @param addr Address of the new allocation
 * @return 0 on success, -1 on return
 */
int kernsh_alloc(size_t size, unsigned long *addr)
{
  int mode, ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  mode = (int) config_get_data(LIBKERNSH_VMCONFIG_ALLOC);
  if (mode == 0)
    ret = kernsh_alloc_contiguous(size, addr);
  else
    ret = kernsh_alloc_noncontiguous(size, addr);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Free contiguous/non contiguous kernel memory\n
 * Configure :\n
 * LIBKERNSH_VMCONFIG_ALLOC
 * @param addr Address of the allocation
 * @return 0 on success, -1 on return
 */
int kernsh_free(unsigned long addr)
{
  int mode, ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  mode = (int) config_get_data(LIBKERNSH_VMCONFIG_ALLOC);
  if (mode == 0)
    ret = kernsh_free_contiguous(addr);
  else
    ret = kernsh_free_noncontiguous(addr);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Allocate contiguous kernel memory
 * @param size Allocate size bytes
 * @param addr Address of the new allocation
 * @return 0 on success, -1 on return
 */
int kernsh_alloc_contiguous(size_t size, unsigned long *addr)
{
  int ret;
  unsigned int         dim[2];
  vector_t      *alloc;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!libkernshworld.open)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Memory not open !", -1);
    }

  alloc = aspect_vector_get(LIBKERNSH_VECTOR_NAME_ALLOCCONTIGUOUS);
  dim[0] = libkernshworld.os;

  fct = aspect_vectors_select(alloc, dim);

  ret = fct(size, addr);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Allocate non contiguous kernel memory
 * @param size Allocate size bytes
 * @param addr Address of the new allocation
 * @return 0 on success, -1 on return
 */
int kernsh_alloc_noncontiguous(size_t size, unsigned long *addr)
{
  int ret;
  unsigned int         dim[2];
  vector_t      *alloc;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!libkernshworld.open)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Memory not open !", -1);
    }

  alloc = aspect_vector_get(LIBKERNSH_VECTOR_NAME_ALLOCNONCONTIGUOUS);
  dim[0] = libkernshworld.os;

  fct = aspect_vectors_select(alloc, dim);

  ret = fct(size, addr);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Free contiguous kernel memory
 * @param addr Address of the allocation
 * @return 0 on success, -1 on return
 */
int kernsh_free_contiguous(unsigned long addr)
{
  int ret;
  unsigned int         dim[2];
  vector_t      *alloc;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!libkernshworld.open)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Memory not open !", -1);
    }

  alloc = aspect_vector_get(LIBKERNSH_VECTOR_NAME_FREECONTIGUOUS);
  dim[0] = libkernshworld.os;

  fct = aspect_vectors_select(alloc, dim);

  ret = fct(addr);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Free non contiguous kernel memory
 * @param addr Address of the allocation
 * @return 0 on success, -1 on return
 */
int kernsh_free_noncontiguous(unsigned long addr)
{
  int ret;
  unsigned int         dim[2];
  vector_t      *alloc;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!libkernshworld.open)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Memory not open !", -1);
    }

  alloc = aspect_vector_get(LIBKERNSH_VECTOR_NAME_FREENONCONTIGUOUS);
  dim[0] = libkernshworld.os;

  fct = aspect_vectors_select(alloc, dim);

  ret = fct(addr);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Allocate contiguous kernel memory on Linux\n
 * Configure :\n
 * LIBKERNSH_VMCONFIG_NIL_SYSCALL
 * @param size Allocate size bytes
 * @param addr Address of the new allocation
 * @return 0 on success, -1 on return
 */
int kernsh_alloc_contiguous_linux(size_t size, unsigned long *addr)
{
  int ret;
  char buf[sizeof(KMALLOC)];
  unsigned long kaddr;
  unsigned long nil_syscall;
  libkernshkma_t kmalloc;
  unsigned int arg[1];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Get kmalloc's addr */
#if __LINUX_2_4__
  ret = kernsh_get_addr_by_name("kmalloc", &kaddr, strlen("kmalloc"));
#else
  ret = kernsh_get_addr_by_name("__kmalloc", &kaddr, strlen("__kmalloc"));
#endif

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
  
  /* Save the first byte */
  kernsh_readmem(libkernshworld.kernel_start, &buf, sizeof(buf));

  /* Write kmalloc's opcodes */
  kernsh_writemem(libkernshworld.kernel_start, KMALLOC, sizeof(KMALLOC));

  /* Get the nil syscall */
  kernsh_readmem(libkernshworld.sct + 
		 sizeof(unsigned long) * (int) config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL),
		 &nil_syscall, 
		 sizeof(unsigned long));
 
  /* Write the new addr */
  kernsh_writemem(libkernshworld.sct + 
		  sizeof(unsigned long) * (int) config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL),
		  &libkernshworld.kernel_start,
		  sizeof(unsigned long));

  /* Exec nil syscall which is now kmalloc */
  kernsh_syscall((int)config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL), 1, arg);

  *addr = kmalloc.mem;
  
  /* Restore nil syscall */
  kernsh_writemem(libkernshworld.sct + 
		  sizeof(unsigned long) * (int) config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL),
		  &nil_syscall,
		  sizeof(unsigned long));
  
  /* Restore save bytes */
  kernsh_writemem(libkernshworld.kernel_start, &buf, sizeof(buf));

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Free contiguous kernel memory on Linux\n
 * Configure :\n
 * LIBKERNSH_VMCONFIG_NIL_SYSCALL
 * @param addr Address of the allocation
 * @return 0 on success, -1 on return
 */
int kernsh_free_contiguous_linux(unsigned long addr)
{
  int ret;
  char buf[sizeof(KFREE)];
  unsigned long kaddr;
  unsigned long nil_syscall;
  libkernshkfr_t kfree;
  unsigned int arg[1];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Get kfree's addr */
  ret = kernsh_get_addr_by_name("kfree", &kaddr, strlen("kfree"));
  if(ret)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unable to find symbol kfree", -1);
    }

  kfree.kexec = (void *)kaddr;
  kfree.address = (const void *) addr;

  arg[0] = (unsigned int)&kfree;

  /* Save the first byte */
  kernsh_readmem(libkernshworld.kernel_start, &buf, sizeof(buf));

  /* Write kfree's opcodes */
  kernsh_writemem(libkernshworld.kernel_start, KFREE, sizeof(KFREE));

  /* Get the nil syscall */
  kernsh_readmem(libkernshworld.sct + 
		 sizeof(unsigned long) * (int) config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL),
		 &nil_syscall, 
		 sizeof(unsigned long));

  /* Write the new addr */
  kernsh_writemem(libkernshworld.sct + 
		  sizeof(unsigned long) * (int) config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL),
		  &libkernshworld.kernel_start,
		  sizeof(unsigned long));

  /* Exec nil syscall which is now kfree */
  kernsh_syscall((int)config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL), 1, arg);

  /* Restore nil syscall */
  kernsh_writemem(libkernshworld.sct + 
		  sizeof(unsigned long) * (int) config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL),
		  &nil_syscall,
		  sizeof(unsigned long));

  /* Restore save bytes */
  kernsh_writemem(libkernshworld.kernel_start, &buf, sizeof(buf));

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Allocate non contiguous kernel memory on Linux\n
 * Configure :\n
 * LIBKERNSH_VMCONFIG_NIL_SYSCALL
 * @param size Allocate size bytes
 * @param addr Address of the new allocation
 * @return 0 on success, -1 on return
 */
int kernsh_alloc_noncontiguous_linux(size_t size, unsigned long *addr)
{
  int ret;
  char buf[sizeof(VMALLOC)];
  unsigned long kaddr;
  unsigned long nil_syscall;
  libkernshvma_t vmalloc;
  unsigned int arg[1];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Get vmalloc's addr */
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

  /* Save the first byte */
  kernsh_readmem(libkernshworld.kernel_start, &buf, sizeof(buf));

  /* Write vmalloc's opcodes */
  kernsh_writemem(libkernshworld.kernel_start, VMALLOC, sizeof(VMALLOC));
  
  /* Get the nil syscall */
  kernsh_readmem(libkernshworld.sct +
		 sizeof(unsigned long) * (int) config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL),
		 &nil_syscall, 
		 sizeof(unsigned long));

  /* Write the new addr */
  kernsh_writemem(libkernshworld.sct + 
		  sizeof(unsigned long) * (int) config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL),
		  &libkernshworld.kernel_start,
		  sizeof(unsigned long));
  
  /* Exec nil syscall which is now vmlloc */
  kernsh_syscall((int)config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL), 1, arg);

  *addr = vmalloc.mem;

  /* Restore nil syscall */
  kernsh_writemem(libkernshworld.sct + 
		  sizeof(unsigned long) * (int) config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL),
		  &nil_syscall,
		  sizeof(unsigned long));

  /* Restore save bytes */
  kernsh_writemem(libkernshworld.kernel_start, &buf, sizeof(buf));
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Free non contiguous kernel memory on Linux\n
 * Configure :\n
 * LIBKERNSH_VMCONFIG_NIL_SYSCALL
 * @param addr Address of the allocation
 * @return 0 on success, -1 on return
 */
int kernsh_free_noncontiguous_linux(unsigned long addr)
{
  int ret;
  char buf[sizeof(VFREE)];
  unsigned long kaddr;
  unsigned long nil_syscall;
  libkernshvfr_t vfree;
  unsigned int arg[1];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Get vfree's addr */
  ret = kernsh_get_addr_by_name("vfree", &kaddr, strlen("vfree"));
  if(ret)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unable to find symbol vfree", -1);
    }

  vfree.kexec = (void *)kaddr;
  vfree.address = (void *) addr;

  arg[0] = (unsigned int)&vfree;
  
  /* Save the first byte */
  kernsh_readmem(libkernshworld.kernel_start, &buf, sizeof(buf));

  /* Write vfree's opcodes */
  kernsh_writemem(libkernshworld.kernel_start, VFREE, sizeof(VFREE));

  /* Get the nil syscall */
  kernsh_readmem(libkernshworld.sct + 
		 sizeof(unsigned long) * (int) config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL),
		 &nil_syscall, 
		 sizeof(unsigned long));

  /* Write the new addr */
  kernsh_writemem(libkernshworld.sct + 
		  sizeof(unsigned long) * (int) config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL),
		  &libkernshworld.kernel_start,
		  sizeof(unsigned long));

  /* Exec nil syscall which is now vfree */
  kernsh_syscall((int)config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL), 1, arg);

  /* Restore nil syscall */
  kernsh_writemem(libkernshworld.sct + 
		  sizeof(unsigned long) * (int) config_get_data(LIBKERNSH_VMCONFIG_NIL_SYSCALL),
		  &nil_syscall,
		  sizeof(unsigned long));

  /* Restore save bytes */
  kernsh_writemem(libkernshworld.kernel_start, &buf, sizeof(buf));

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  
}
