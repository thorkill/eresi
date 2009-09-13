/**
* @file libkernsh/user/alloc.c
** @ingroup usermode
** @brief allocation contiguous/non contiguous.
**
*/
#include "libkernsh.h"
#include "libkernsh-symbols.h"
#include "libkernsh-info.h"

/**
 * @brief Allocate contiguous/non contiguous kernel memory\n
 * Configure :\n
 * LIBKERNSH_CONFIG_ALLOC
 * @param size Allocate size bytes
 * @param addr Address of the new allocation
 * @return 0 on success, -1 on return
 */
eresi_Addr	kernsh_alloc(elfshobj_t *file, size_t size, int prot)
{
  int		mode;
  int		ret;
  eresi_Addr	raddr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  mode = (int) config_get_data(LIBKERNSH_CONFIG_ALLOC);
  if (mode == 0)
    ret = kernsh_alloc_contiguous(size, &raddr);
  else
    ret = kernsh_alloc_noncontiguous(size, &raddr);
  
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Failed to allocate kernel memory", ELFSH_INVALID_ADDR);  

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, raddr);
}

/**
 * @brief Free contiguous/non contiguous kernel memory\n
 * Configure :\n
 * LIBKERNSH_CONFIG_ALLOC
 * @param addr Address of the allocation
 * @return 0 on success, -1 on return
 */
int		kernsh_free(eresi_Addr addr)
{
  int		mode, ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  mode = (int) config_get_data(LIBKERNSH_CONFIG_ALLOC);
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
int		kernsh_alloc_contiguous(size_t size, eresi_Addr *addr)
{
  int		ret;
  u_int         dim[2];
  vector_t      *alloc;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!libkernshworld.open)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Memory not open !", -1);

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
int		kernsh_alloc_noncontiguous(size_t size, eresi_Addr *addr)
{
  int		ret;
  u_int         dim[2];
  vector_t      *alloc;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!libkernshworld.open)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Memory not open !", -1);
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
int		kernsh_free_contiguous(eresi_Addr addr)
{
  int		ret;
  u_int         dim[2];
  vector_t      *alloc;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!libkernshworld.open)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Memory not open !", -1);
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
int		kernsh_free_noncontiguous(eresi_Addr addr)
{
  int		ret;
  u_int         dim[2];
  vector_t      *alloc;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!libkernshworld.open)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Memory not open !", -1);
  alloc = aspect_vector_get(LIBKERNSH_VECTOR_NAME_FREENONCONTIGUOUS);
  dim[0] = libkernshworld.os;
  fct = aspect_vectors_select(alloc, dim);
  ret = fct(addr);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Allocate contiguous kernel memory on Linux\n
 * Configure :\n
 * LIBKERNSH_CONFIG_NIL_SYSCALL
 * @param size Allocate size bytes
 * @param addr Address of the new allocation
 * @return 0 on success, -1 on return
 */
int		kernsh_alloc_contiguous_linux(size_t size, eresi_Addr *addr)
{
  int		ret;
  char		buf[sizeof(KMALLOC)];
  eresi_Addr	kaddr;
  eresi_Addr	nil_syscall;
  libkernshkma_t kmalloc;
  unsigned int	arg[1];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Get kmalloc's addr */
#if __LINUX_2_4__
  ret = kernsh_get_addr_by_name("kmalloc", &kaddr, strlen("kmalloc"));
#else
  ret = kernsh_get_addr_by_name("__kmalloc", &kaddr, strlen("__kmalloc"));
#endif

  if(ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to find symbol kmalloc", -1);

  kmalloc.kexec = (void *) kaddr;
  kmalloc.size  = size;
  kmalloc.flags = GFP_KERNEL;
  kmalloc.mem   = 0;

  /* XXX: NOT 64 BITS COMPATIBLE */
  arg[0] = (unsigned int) &kmalloc;
  
  /* Save the first byte */
  elfsh_readmema(libkernshworld.root, libkernshworld.kernel_start, &buf, sizeof(buf));

  /* Write kmalloc's opcodes */
  elfsh_writemem(libkernshworld.root, libkernshworld.kernel_start, KMALLOC, sizeof(KMALLOC));

  /* Get the nil syscall */
  elfsh_readmema(libkernshworld.root, libkernshworld.sct + 
		 sizeof(eresi_Addr) * (int) config_get_data(LIBKERNSH_CONFIG_NIL_SYSCALL),
		 &nil_syscall, sizeof(eresi_Addr));
 
  /* Write the new addr */
  elfsh_writemem(libkernshworld.root, libkernshworld.sct + 
		 sizeof(eresi_Addr) * (int) config_get_data(LIBKERNSH_CONFIG_NIL_SYSCALL),
		 &libkernshworld.kernel_start, sizeof(eresi_Addr));
  
  /* Exec nil syscall which is now kmalloc */
  kernsh_syscall((int)config_get_data(LIBKERNSH_CONFIG_NIL_SYSCALL), 1, arg);
  *addr = kmalloc.mem;
  
  /* Restore nil syscall */
  elfsh_writemem(libkernshworld.root, libkernshworld.sct + 
		 sizeof(eresi_Addr) * (int) config_get_data(LIBKERNSH_CONFIG_NIL_SYSCALL),
		 &nil_syscall, sizeof(eresi_Addr));
  
  /* Restore save bytes */
  elfsh_writemem(libkernshworld.root, libkernshworld.kernel_start, &buf, sizeof(buf));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Free contiguous kernel memory on Linux\n
 * Configure :\n
 * LIBKERNSH_CONFIG_NIL_SYSCALL
 * @param addr Address of the allocation
 * @return 0 on success, -1 on return
 */
int		kernsh_free_contiguous_linux(eresi_Addr addr)
{
  int		ret;
  char		buf[sizeof(KFREE)];
  eresi_Addr	kaddr;
  eresi_Addr	nil_syscall;
  libkernshkfr_t kfree;
  unsigned int	arg[1];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Get kfree's addr */
  ret = kernsh_get_addr_by_name("kfree", &kaddr, strlen("kfree"));
  if (ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to find symbol kfree", -1);

  /* FIXME: not 64 bits compatible */
  kfree.kexec   = (void *) kaddr;
  kfree.address = (const void *) addr;
  arg[0]        = (unsigned int) &kfree;

  /* Save the first byte */
  elfsh_readmema(libkernshworld.root, libkernshworld.kernel_start, &buf, sizeof(buf));

  /* Write kfree's opcodes */
  elfsh_writemem(libkernshworld.root, libkernshworld.kernel_start, KFREE, sizeof(KFREE));

  /* Get the nil syscall */
  elfsh_readmema(libkernshworld.root, libkernshworld.sct + 
		 sizeof(eresi_Addr) * (int) config_get_data(LIBKERNSH_CONFIG_NIL_SYSCALL),
		 &nil_syscall, sizeof(eresi_Addr));

  /* Write the new addr */
  elfsh_writemem(libkernshworld.root, libkernshworld.sct + 
		 sizeof(eresi_Addr) * (int) config_get_data(LIBKERNSH_CONFIG_NIL_SYSCALL),
		 &libkernshworld.kernel_start, sizeof(eresi_Addr));
  
  /* Exec nil syscall which is now kfree */
  kernsh_syscall((int) config_get_data(LIBKERNSH_CONFIG_NIL_SYSCALL), 1, arg);

  /* Restore nil syscall */
  elfsh_writemem(libkernshworld.root, libkernshworld.sct + 
		 sizeof(eresi_Addr) * (int) config_get_data(LIBKERNSH_CONFIG_NIL_SYSCALL),
		 &nil_syscall, sizeof(eresi_Addr));

  /* Restore save bytes */
  elfsh_writemem(libkernshworld.root, libkernshworld.kernel_start, &buf, sizeof(buf));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Allocate non contiguous kernel memory on Linux\n
 * Configure :\n
 * LIBKERNSH_CONFIG_NIL_SYSCALL
 * @param size Allocate size bytes
 * @param addr Address of the new allocation
 * @return 0 on success, -1 on return
 */
int		kernsh_alloc_noncontiguous_linux(size_t size, eresi_Addr *addr)
{
  int		ret;
  char		buf[sizeof(VMALLOC)];
  eresi_Addr	kaddr;
  eresi_Addr	nil_syscall;
  libkernshvma_t vmalloc;
  unsigned int	arg[1];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Get vmalloc's addr */
  ret = kernsh_get_addr_by_name("vmalloc", &kaddr, strlen("vmalloc"));
  if (ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to find symbol vmalloc", -1);

  /* XXX: Not 64 bits compatible ? */
  vmalloc.kexec = (void *) kaddr;
  vmalloc.size  = size;
  vmalloc.mem   = 0;
  arg[0]        = (unsigned int) &vmalloc;

  /* Save the first byte */
  elfsh_readmema(libkernshworld.root, libkernshworld.kernel_start, &buf, sizeof(buf));

  /* Write vmalloc's opcodes */
  elfsh_writemem(libkernshworld.root, libkernshworld.kernel_start, VMALLOC, sizeof(VMALLOC));
  
  /* Get the nil syscall */
  elfsh_readmema(libkernshworld.root, libkernshworld.sct +
		 sizeof(eresi_Addr) * (int) config_get_data(LIBKERNSH_CONFIG_NIL_SYSCALL),
		 &nil_syscall, sizeof(eresi_Addr));

  /* Write the new addr */
  elfsh_writemem(libkernshworld.root, libkernshworld.sct + 
		 sizeof(eresi_Addr) * (int) config_get_data(LIBKERNSH_CONFIG_NIL_SYSCALL),
		 &libkernshworld.kernel_start, sizeof(eresi_Addr));
  
  /* Exec nil syscall which is now vmlloc */
  kernsh_syscall((int)config_get_data(LIBKERNSH_CONFIG_NIL_SYSCALL), 1, arg);
  *addr = vmalloc.mem;

  /* Restore nil syscall */
  elfsh_writemem(libkernshworld.root, libkernshworld.sct + 
		 sizeof(eresi_Addr) * (int) config_get_data(LIBKERNSH_CONFIG_NIL_SYSCALL),
		 &nil_syscall, sizeof(eresi_Addr));

  /* Restore save bytes */
  elfsh_writemem(libkernshworld.root, libkernshworld.kernel_start, &buf, sizeof(buf));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Free non contiguous kernel memory on Linux\n
 * Configure :\n
 * LIBKERNSH_CONFIG_NIL_SYSCALL
 * @param addr Address of the allocation
 * @return 0 on success, -1 on return
 */
int			kernsh_free_noncontiguous_linux(eresi_Addr addr)
{
  int			ret;
  char			buf[sizeof(VFREE)];
  eresi_Addr		kaddr;
  eresi_Addr		nil_syscall;
  libkernshvfr_t	vfree;
  unsigned int		arg[1];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Get vfree's addr */
  ret = kernsh_get_addr_by_name("vfree", &kaddr, strlen("vfree"));
  if (ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to find symbol vfree", -1);

  /* XXX: not 64 bits compatible ? */
  vfree.kexec   = (void *) kaddr;
  vfree.address = (void *) addr;
  arg[0]        = (unsigned int) &vfree;
  
  /* Save the first byte */
  elfsh_readmema(libkernshworld.root, libkernshworld.kernel_start, &buf, sizeof(buf));

  /* Write vfree's opcodes */
  elfsh_writemem(libkernshworld.root, libkernshworld.kernel_start, VFREE, sizeof(VFREE));

  /* Get the nil syscall */
  elfsh_readmema(libkernshworld.root, libkernshworld.sct + 
		 sizeof(eresi_Addr) * (int) config_get_data(LIBKERNSH_CONFIG_NIL_SYSCALL),
		 &nil_syscall, sizeof(eresi_Addr));

  /* Write the new addr */
  elfsh_writemem(libkernshworld.root, libkernshworld.sct + 
		 sizeof(eresi_Addr) * (int) config_get_data(LIBKERNSH_CONFIG_NIL_SYSCALL),
		 &libkernshworld.kernel_start,
		 sizeof(eresi_Addr));
  
  /* Exec nil syscall which is now vfree */
  kernsh_syscall((int)config_get_data(LIBKERNSH_CONFIG_NIL_SYSCALL), 1, arg);

  /* Restore nil syscall */
  elfsh_writemem(libkernshworld.root, libkernshworld.sct + 
		 sizeof(eresi_Addr) * (int) config_get_data(LIBKERNSH_CONFIG_NIL_SYSCALL),
		 &nil_syscall, sizeof(eresi_Addr));

  /* Restore save bytes */
  elfsh_writemem(libkernshworld.root, libkernshworld.kernel_start, &buf, sizeof(buf));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  
}
