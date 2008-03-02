/*
** vectors.c for libkernsh
**
** $Id: vectors.c,v 1.9 2007-09-23 17:53:35 pouik Exp $
**
*/
#include "libkernsh.h"
#include "libaspect.h"

libkernshworld_t libkernshworld;


int kernsh_openmem_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("OPENMEM DEFAULT!!!\n");
#endif

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "openmem default !", -1);
}

int kernsh_closemem_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("CLOSEMEM DEFAULT!!!\n");
#endif

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "closemem default !", -1);
}

int kernsh_readmem_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("READMEM DEFAULT!!!\n");
#endif

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "readmem default !", -1);
}

int kernsh_writemem_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("WRITEMEM DEFAULT!!!\n");
#endif

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "writemem default !", -1);
}

int kernsh_sct_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("SCT DEFAULT!!!\n");
#endif

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "sct default !", -1);
}

int kernsh_callsc_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("CALL SC DEFAULT!!!\n");
#endif

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "call sc default !", -1);
}

int kernsh_idt_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("IDT DEFAULT!!!\n");
#endif

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "idt default !", -1);
}

int kernsh_gdt_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("GDT DEFAULT!!!\n");
#endif

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "gdt default !", -1);
}

int kernsh_info_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("INFO DEFAULT!!!\n");
#endif

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "info default !", -1);
}

int kernsh_decompkernel_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("DECOMPKERNEL DEFAULT!!!\n");
#endif

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "decompkernel default !", -1);  
}

int kernsh_loadkernel_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("LOADKERNEL DEFAULT!!!\n");
#endif

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	     "loadkernel default !", -1);
}

int kernsh_autotypes_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("AUTOTYPES DEFAULT!!!\n");
#endif

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "autotypes default !", -1);
}

int kernsh_symbs_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("SYMBS DEFAULT!!!\n");
#endif

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "symbs default !", -1);
}

int kernsh_alloc_contiguous_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("ALLOC CONTIGUOUS DEFAULT!!!\n");
#endif

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "alloc contiguous default !", -1);
}

int kernsh_alloc_noncontiguous_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("ALLOC NON CONTIGUOUS DEFAULT!!!\n");
#endif

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "alloc non contiguous default !", -1);
}

int kernsh_free_contiguous_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("FREE CONTIGUOUS DEFAULT!!!\n");
#endif

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "free contiguous default !", -1);
}

int kernsh_free_noncontiguous_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("FREE NON CONTIGUOUS DEFAULT!!!\n");
#endif
  
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "free non contiguous default !", -1);
}

int kernsh_relink_module_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("RELINK MODULE DEFAULT!!!\n");
#endif
  
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "relink module default !", -1);
}

int kernsh_infect_module_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("INFECT MODULE DEFAULT!!!\n");
#endif
  
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "infect module default !", -1);
}

int kernsh_kload_module_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("KLOAD MODULE DEFAULT!!!\n");
#endif
  
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "kload module default !", -1);
}

int kernsh_kunload_module_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("KUNLOAD MODULE DEFAULT!!!\n");
#endif
  
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "kunload module default !", -1);
}

int kernsh_kvirtm_read_virtm_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("KVIRTM READ VIRTM DEFAULT!!!\n");
#endif
  
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "kvirtm read virtm default !", -1);
}

int kernsh_kvirtm_read_mem_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("KVIRTM READ MEM DEFAULT!!!\n");
#endif
  
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "kvirtm read mem default !", -1);
}

/* Init vectors */
int kernsh_init_vectors()
{
  u_int *dims;
  char  **strdims;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  aspect_init();
  
  /* Memory ARCH, OS, DEVICE */
#if __DEBUG_KERNSH__
  printf("INIT MEMORY VECTORS\n");
#endif

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 4 * sizeof(u_int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,strdims, 4 * sizeof(char *), -1);

  dims[0]    = LIBKERNSH_ARCHNUM;
  dims[1]    = LIBKERNSH_OSNUM;
  dims[2]    = LIBKERNSH_DEVICENUM;

  strdims[0] = "ARCHTYPE";
  strdims[1] = "OSTYPE";
  strdims[2] = "DEVICETYPE";

  /* Register memory default vectors */
  aspect_register_vector(LIBKERNSH_VECTOR_NAME_OPENMEM, 
                         kernsh_openmem_default,
                         dims, strdims, 3, ASPECT_TYPE_CADDR);

  aspect_register_vector(LIBKERNSH_VECTOR_NAME_CLOSEMEM, 
			 kernsh_closemem_default,
			 dims, strdims, 3, ASPECT_TYPE_CADDR);

  aspect_register_vector(LIBKERNSH_VECTOR_NAME_READMEM, 
			 kernsh_readmem_default,
			 dims, strdims, 3, ASPECT_TYPE_CADDR);

  aspect_register_vector(LIBKERNSH_VECTOR_NAME_WRITEMEM, 
			 kernsh_writemem_default,
			 dims, strdims, 3, ASPECT_TYPE_CADDR);


  /* Syscalls ARCH, OS */
#if __DEBUG_KERNSH__
  printf("INIT SCT VECTORS\n");
#endif

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 3 * sizeof(u_int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,strdims, 3 * sizeof(char *), -1);

  dims[0]    = LIBKERNSH_ARCHNUM;
  dims[1]    = LIBKERNSH_OSNUM;

  strdims[0] = "ARCHTYPE";
  strdims[1] = "OSTYPE";

  /* Register sct default vectors */
  aspect_register_vector(LIBKERNSH_VECTOR_NAME_SCT, 
                         kernsh_sct_default,
                         dims, strdims, 2, ASPECT_TYPE_CADDR);

  /* Call syscall OS */
#if __DEBUG_KERNSH__
  printf("INIT CALL SYSCALL VECTORS\n");
#endif

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 2 * sizeof(u_int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,strdims, 2 * sizeof(char *), -1);

  dims[0]    = LIBKERNSH_OSNUM;

  strdims[0] = "OSTYPE";

  /* Register call syscall default vectors */
  aspect_register_vector(LIBKERNSH_VECTOR_NAME_CALLSC,
                         kernsh_callsc_default,
                         dims, strdims, 1, ASPECT_TYPE_CADDR);


  /* IDT ARCH, OS */
#if __DEBUG_KERNSH__
  printf("INIT IDT VECTORS\n");
#endif

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 3 * sizeof(u_int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,strdims, 3 * sizeof(char *), -1);

  dims[0]    = LIBKERNSH_ARCHNUM;
  dims[1]    = LIBKERNSH_OSNUM;

  strdims[0] = "ARCHTYPE";
  strdims[1] = "OSTYPE";

  /* Register idt default vectors */
  aspect_register_vector(LIBKERNSH_VECTOR_NAME_IDT, 
                         kernsh_idt_default,
                         dims, strdims, 2, ASPECT_TYPE_CADDR);
  
  /* GDT ARCH, OS */
#if __DEBUG_KERNSH__
  printf("INIT GDT VECTORS\n");
#endif

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 3 * sizeof(u_int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,strdims, 3 * sizeof(char *), -1);

  dims[0]    = LIBKERNSH_ARCHNUM;
  dims[1]    = LIBKERNSH_OSNUM;

  strdims[0] = "ARCHTYPE";
  strdims[1] = "OSTYPE";

  /* Register gdt default vectors */
  aspect_register_vector(LIBKERNSH_VECTOR_NAME_GDT, 
                         kernsh_gdt_default,
                         dims, strdims, 2, ASPECT_TYPE_CADDR);

  /* Info ARCH, OS */
#if __DEBUG_KERNSH__
  printf("INIT INFO VECTORS\n");
#endif

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 3 * sizeof(u_int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,strdims, 3 * sizeof(char *), -1);

  dims[0]    = LIBKERNSH_ARCHNUM;
  dims[1]    = LIBKERNSH_OSNUM;

  strdims[0] = "ARCHTYPE";
  strdims[1] = "OSTYPE";

  /* Register info default vectors */
  aspect_register_vector(LIBKERNSH_VECTOR_NAME_INFO,
			 kernsh_info_default,
			 dims, strdims, 2, ASPECT_TYPE_CADDR);
#if __DEBUG_KERNSH__
  printf("INIT KERNEL DECOMP VECTORS\n");
#endif

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 2 * sizeof(u_int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,strdims, 2 * sizeof(char *), -1);

  dims[0]    = LIBKERNSH_OSNUM;

  strdims[0] = "OSTYPE";

  /* Register kernel default vectors */
  aspect_register_vector(LIBKERNSH_VECTOR_NAME_DECOMPKERNEL, 
                         kernsh_decompkernel_default,
                         dims, strdims, 1, ASPECT_TYPE_CADDR);

#if __DEBUG_KERNSH__
  printf("INIT SYMBS VECTORS\n");
#endif

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 3 * sizeof(u_int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,strdims, 3 * sizeof(char *), -1);

  dims[0]    = LIBKERNSH_ARCHNUM;
  dims[1]    = LIBKERNSH_OSNUM;

  strdims[0] = "ARCHTYPE";
  strdims[1] = "OSTYPE";
  
  /* Register symbs default vectors */
  aspect_register_vector(LIBKERNSH_VECTOR_NAME_ADDRBYNAME, 
                         kernsh_symbs_default,
                         dims, strdims, 2, ASPECT_TYPE_CADDR); 

  aspect_register_vector(LIBKERNSH_VECTOR_NAME_NAMEBYADDR, 
                         kernsh_symbs_default,
                         dims, strdims, 2, ASPECT_TYPE_CADDR); 

#if __DEBUG_KERNSH__
  printf("INIT ALLOC CONTIGUOUS VECTORS\n");
#endif

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 2 * sizeof(u_int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,strdims, 2 * sizeof(char *), -1);

  dims[0]    = LIBKERNSH_OSNUM;

  strdims[0] = "OSTYPE";

  /* Register alloc/free default vectors */
  aspect_register_vector(LIBKERNSH_VECTOR_NAME_ALLOCCONTIGUOUS,
                         kernsh_alloc_contiguous_default,
                         dims, strdims, 1, ASPECT_TYPE_CADDR);

  aspect_register_vector(LIBKERNSH_VECTOR_NAME_ALLOCNONCONTIGUOUS,
                         kernsh_alloc_noncontiguous_default,
                         dims, strdims, 1, ASPECT_TYPE_CADDR);

  aspect_register_vector(LIBKERNSH_VECTOR_NAME_FREECONTIGUOUS, 
                         kernsh_free_contiguous_default,
                         dims, strdims, 1, ASPECT_TYPE_CADDR);

  aspect_register_vector(LIBKERNSH_VECTOR_NAME_FREENONCONTIGUOUS, 
			 kernsh_free_noncontiguous_default,
			 dims, strdims, 1, ASPECT_TYPE_CADDR);

  /* autotypes ARCH, OS, DEVICE */
#if __DEBUG_KERNSH__
  printf("INIT AUTOTYPES VECTORS\n");
#endif

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 3 * sizeof(u_int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,strdims, 3 * sizeof(char *), -1);

  dims[0]    = LIBKERNSH_ARCHNUM;
  dims[1]    = LIBKERNSH_OSNUM;

  strdims[0] = "ARCHTYPE";
  strdims[1] = "OSTYPE";

  /* Register memory default vectors */
  aspect_register_vector(LIBKERNSH_VECTOR_NAME_AUTOTYPES, 
                         kernsh_autotypes_default,
                         dims, strdims, 2, ASPECT_TYPE_CADDR);

#if __DEBUG_KERNSH__
  printf("INIT RELINK MODULE VECTORS\n");
#endif

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 2 * sizeof(u_int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,strdims, 2 * sizeof(char *), -1);

  dims[0]    = LIBKERNSH_OSNUM;

  strdims[0] = "OSTYPE";

  /* Register relink module vectors */
  aspect_register_vector(LIBKERNSH_VECTOR_NAME_RELINKMODULE, 
                         kernsh_relink_module_default,
                         dims, strdims, 1, ASPECT_TYPE_CADDR);

#if __DEBUG_KERNSH__
  printf("INIT INFECT MODULE VECTORS\n");
#endif
  
  /* Register infect module vectors */
  aspect_register_vector(LIBKERNSH_VECTOR_NAME_INFECTMODULE, 
                         kernsh_infect_module_default,
                         dims, strdims, 1, ASPECT_TYPE_CADDR);
  
  aspect_register_vector(LIBKERNSH_VECTOR_NAME_KLOADMODULE, 
                         kernsh_kload_module_default,
                         dims, strdims, 1, ASPECT_TYPE_CADDR);

  aspect_register_vector(LIBKERNSH_VECTOR_NAME_KUNLOADMODULE, 
                         kernsh_kunload_module_default,
                         dims, strdims, 1, ASPECT_TYPE_CADDR);

#if __DEBUG_KERNSH__
  printf("KVIRTM READ * VECTORS\n");
#endif

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 3 * sizeof(u_int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,strdims, 3 * sizeof(char *), -1);

  dims[0]    = LIBKERNSH_OSNUM;
  dims[1]    = LIBKERNSH_VIRTMNUM;

  strdims[0] = "OSTYPE";
  strdims[1] = "VIRTMTYPE";

  /* Register kvirtm read * default vectors */
  aspect_register_vector(LIBKERNSH_VECTOR_NAME_KVIRTMREADVIRTM, 
                         kernsh_kvirtm_read_virtm_default,
                         dims, strdims, 2, ASPECT_TYPE_CADDR);

  aspect_register_vector(LIBKERNSH_VECTOR_NAME_KVIRTMREADMEM, 
                         kernsh_kvirtm_read_mem_default,
                         dims, strdims, 2, ASPECT_TYPE_CADDR);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


int kernsh_register_vectors()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  kernsh_register_openmem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6, 
			  LIBKERNSH_DEVICE_KMEM,
			  kernsh_openmem_kmem_linux_2_6);
  kernsh_register_openmem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_4, 
			  LIBKERNSH_DEVICE_KMEM,
			  kernsh_openmem_kmem_linux_2_4);
  kernsh_register_openmem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6, 
			  LIBKERNSH_DEVICE_MEM, 
			  kernsh_openmem_mem_linux);
  kernsh_register_openmem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_4, 
			  LIBKERNSH_DEVICE_MEM, 
			  kernsh_openmem_mem_linux);
  kernsh_register_openmem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
			  LIBKERNSH_DEVICE_KCORE, 
			  kernsh_openmem_kcore_linux_2_6);
  kernsh_register_openmem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_NETBSD,
			  LIBKERNSH_DEVICE_MEM, 
			  kernsh_openmem_netbsd);
  
  kernsh_register_closemem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
			   LIBKERNSH_DEVICE_KMEM, 
			   kernsh_closemem_kmem_linux_2_6);
  kernsh_register_closemem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_4,
			   LIBKERNSH_DEVICE_KMEM, 
			   kernsh_closemem_kmem_linux_2_4);
  kernsh_register_closemem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
			   LIBKERNSH_DEVICE_MEM, 
			   kernsh_closemem_mem_linux);
  kernsh_register_closemem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_4,
			   LIBKERNSH_DEVICE_MEM, 
			   kernsh_closemem_mem_linux);
  kernsh_register_closemem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
			   LIBKERNSH_DEVICE_KCORE, 
			   kernsh_closemem_kcore_linux_2_6);
  kernsh_register_closemem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_NETBSD,
			   LIBKERNSH_DEVICE_MEM,
			   kernsh_closemem_netbsd);

  kernsh_register_readmem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
			  LIBKERNSH_DEVICE_KMEM, 
			  kernsh_readmem_kmem_linux_2_6);
  kernsh_register_readmem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_4,
			  LIBKERNSH_DEVICE_KMEM, 
			  kernsh_readmem_kmem_linux_2_4);
  kernsh_register_readmem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
			  LIBKERNSH_DEVICE_MEM, 
			  kernsh_readmem_mem_linux);
  kernsh_register_readmem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_4,
			  LIBKERNSH_DEVICE_MEM, 
			  kernsh_readmem_mem_linux);
  kernsh_register_readmem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
			  LIBKERNSH_DEVICE_KCORE, 
			  kernsh_readmem_kcore_linux_2_6);
  kernsh_register_readmem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_NETBSD,
			  LIBKERNSH_DEVICE_MEM, 
			  kernsh_readmem_netbsd);

  kernsh_register_writemem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
			   LIBKERNSH_DEVICE_KMEM, 
			   kernsh_writemem_kmem_linux_2_6);
  kernsh_register_writemem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_4,
			   LIBKERNSH_DEVICE_KMEM, 
			   kernsh_writemem_kmem_linux_2_4);
  kernsh_register_writemem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
			   LIBKERNSH_DEVICE_MEM, 
			   kernsh_writemem_mem_linux);
  kernsh_register_writemem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_4,
			   LIBKERNSH_DEVICE_MEM, 
			   kernsh_writemem_mem_linux);
  kernsh_register_writemem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
			   LIBKERNSH_DEVICE_KCORE, 
			   kernsh_writemem_kcore_linux_2_6);
  kernsh_register_writemem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_NETBSD,
			   LIBKERNSH_DEVICE_MEM, kernsh_writemem_netbsd);

  kernsh_register_sct(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
		      kernsh_sct_linux);
  kernsh_register_sct(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_4,
		      kernsh_sct_linux);

  kernsh_register_callsc(LIBKERNSH_OS_LINUX_2_6, 
			 kernsh_syscall_linux);
  kernsh_register_callsc(LIBKERNSH_OS_LINUX_2_4, 
			 kernsh_syscall_linux);

  kernsh_register_idt(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
		      kernsh_idt_linux);
  kernsh_register_idt(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_4,
		      kernsh_idt_linux);

  kernsh_register_gdt(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
		      kernsh_gdt_linux);
  kernsh_register_gdt(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_4,
		      kernsh_gdt_linux);

  kernsh_register_info(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
		       kernsh_info_linux);
  kernsh_register_info(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_4,
		       kernsh_info_linux);

  kernsh_register_decompkernel(LIBKERNSH_OS_LINUX_2_6, 
			       kernsh_decompkernel_linux);
  kernsh_register_decompkernel(LIBKERNSH_OS_LINUX_2_4, 
			       kernsh_decompkernel_linux);

  kernsh_register_symbs_abn(LIBKERNSH_ARCH_I386, 
			    LIBKERNSH_OS_LINUX_2_6, 
			    kernsh_get_addr_by_name_linux_2_6);
  kernsh_register_symbs_abn(LIBKERNSH_ARCH_I386, 
			    LIBKERNSH_OS_LINUX_2_4, 
			    kernsh_get_addr_by_name_linux_2_4);
  kernsh_register_symbs_nba(LIBKERNSH_ARCH_I386,
			    LIBKERNSH_OS_LINUX_2_6, 
			    kernsh_get_name_by_addr_linux_2_6);
  kernsh_register_symbs_nba(LIBKERNSH_ARCH_I386,
			    LIBKERNSH_OS_LINUX_2_4, 
			    kernsh_get_name_by_addr_linux_2_4);

  kernsh_register_alloc_contiguous(LIBKERNSH_OS_LINUX_2_6, 
				   kernsh_alloc_contiguous_linux);
  kernsh_register_alloc_contiguous(LIBKERNSH_OS_LINUX_2_4, 
				   kernsh_alloc_contiguous_linux);

  kernsh_register_alloc_noncontiguous(LIBKERNSH_OS_LINUX_2_6, 
				      kernsh_alloc_noncontiguous_linux);
  kernsh_register_alloc_noncontiguous(LIBKERNSH_OS_LINUX_2_4, 
				      kernsh_alloc_noncontiguous_linux);

  kernsh_register_free_contiguous(LIBKERNSH_OS_LINUX_2_6, 
				  kernsh_free_contiguous_linux);
  kernsh_register_free_contiguous(LIBKERNSH_OS_LINUX_2_4, 
				  kernsh_free_contiguous_linux);

  kernsh_register_free_noncontiguous(LIBKERNSH_OS_LINUX_2_6, 
				     kernsh_free_noncontiguous_linux);
  kernsh_register_free_noncontiguous(LIBKERNSH_OS_LINUX_2_4, 
				     kernsh_free_noncontiguous_linux);

  kernsh_register_autotypes(LIBKERNSH_ARCH_I386, 
			    LIBKERNSH_OS_LINUX_2_6,
	      		    kernsh_autotypes_linux_2_6);


  kernsh_register_relink(LIBKERNSH_OS_LINUX_2_4, 
			 kernsh_relink_module_linux);
  kernsh_register_relink(LIBKERNSH_OS_LINUX_2_6, 
			 kernsh_relink_module_linux);

  kernsh_register_infect(LIBKERNSH_OS_LINUX_2_6, 
			 kernsh_infect_module_linux_2_6);
  kernsh_register_infect(LIBKERNSH_OS_LINUX_2_4, 
			 kernsh_infect_module_linux_2_4);

  kernsh_register_kload(LIBKERNSH_OS_LINUX_2_6, 
			 kernsh_kload_module_linux);
  kernsh_register_kload(LIBKERNSH_OS_LINUX_2_4, 
			  kernsh_kload_module_linux);

  kernsh_register_kunload(LIBKERNSH_OS_LINUX_2_6, 
			  kernsh_kunload_module_linux);
  kernsh_register_kunload(LIBKERNSH_OS_LINUX_2_4, 
			  kernsh_kunload_module_linux);

  kernsh_register_kvirtm_read_virtm(LIBKERNSH_OS_LINUX_2_6,
				    LIBKERNSH_PROC_MODE,
				    kernsh_kvirtm_read_virtm_proc_linux);

  kernsh_register_kvirtm_read_virtm(LIBKERNSH_OS_LINUX_2_4,
				    LIBKERNSH_PROC_MODE,
				    kernsh_kvirtm_read_virtm_proc_linux);

  kernsh_register_kvirtm_read_mem(LIBKERNSH_OS_LINUX_2_6,
				  LIBKERNSH_PROC_MODE,
				  kernsh_kvirtm_read_mem_proc_linux);

  kernsh_register_kvirtm_read_mem(LIBKERNSH_OS_LINUX_2_4,
				  LIBKERNSH_PROC_MODE,
				  kernsh_kvirtm_read_mem_proc_linux);

  kernsh_register_kvirtm_read_mem(LIBKERNSH_OS_LINUX_2_6,
				  LIBKERNSH_SYSCALL_MODE,
				  kernsh_kvirtm_read_mem_syscall_linux);

  kernsh_register_kvirtm_read_mem(LIBKERNSH_OS_LINUX_2_4,
				  LIBKERNSH_SYSCALL_MODE,
				  kernsh_kvirtm_read_mem_syscall_linux);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_openmem(u_int archtype, u_int ostype, u_int devicetype, 
			    void *fct)
{
  vector_t *mem;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  mem = aspect_vector_get(LIBKERNSH_VECTOR_NAME_OPENMEM);

  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = ostype;
  dim[2] = devicetype;

#if __DEBUG_KERNSH__
  printf("REGISTER OPENMEM\n");
#endif

  aspect_vectors_insert(mem, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_closemem(u_int archtype, u_int ostype, u_int devicetype, 
			     void *fct)
{
  vector_t *mem;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  mem = aspect_vector_get(LIBKERNSH_VECTOR_NAME_CLOSEMEM);

  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = ostype;
  dim[2] = devicetype;

#if __DEBUG_KERNSH__
  printf("REGISTER CLOSEMEM\n");
#endif

  aspect_vectors_insert(mem, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


int kernsh_register_readmem(u_int archtype, u_int ostype, u_int devicetype, 
			    void *fct)
{
  vector_t *mem;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  mem = aspect_vector_get(LIBKERNSH_VECTOR_NAME_READMEM);

  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = ostype;
  dim[2] = devicetype;

#if __DEBUG_KERNSH__
  printf("REGISTER READMEM\n");
#endif

  aspect_vectors_insert(mem, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


int kernsh_register_writemem(u_int archtype, u_int ostype, u_int devicetype, 
			    void *fct)
{
  vector_t *mem;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  mem = aspect_vector_get(LIBKERNSH_VECTOR_NAME_WRITEMEM);

  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = ostype;
  dim[2] = devicetype;

#if __DEBUG_KERNSH__
  printf("REGISTER WRITEMEM\n");
#endif

  aspect_vectors_insert(mem, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_sct(u_int archtype, u_int ostype, void *fct)
{
  vector_t *sct;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  sct = aspect_vector_get(LIBKERNSH_VECTOR_NAME_SCT);

  dim    = alloca(sizeof(u_int) * 3);
  dim[0] = archtype;
  dim[1] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER SCT\n");
#endif

  aspect_vectors_insert(sct, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_callsc(u_int ostype, void *fct)
{

  vector_t *csc;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  csc = aspect_vector_get(LIBKERNSH_VECTOR_NAME_CALLSC);

  dim    = alloca(sizeof(u_int) * 1);
  dim[0] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER CALLSC CONTIGUOUS\n");
#endif

  aspect_vectors_insert(csc, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_idt(u_int archtype, u_int ostype, void *fct)
{
  vector_t *idt;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  idt = aspect_vector_get(LIBKERNSH_VECTOR_NAME_IDT);

  dim    = alloca(sizeof(u_int) * 3);
  dim[0] = archtype;
  dim[1] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER IDT\n");
#endif

  aspect_vectors_insert(idt, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_gdt(u_int archtype, u_int ostype, void *fct)
{
  vector_t *gdt;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  gdt = aspect_vector_get(LIBKERNSH_VECTOR_NAME_GDT);

  dim    = alloca(sizeof(u_int) * 3);
  dim[0] = archtype;
  dim[1] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER GDT\n");
#endif

  aspect_vectors_insert(gdt, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_info(u_int archtype, u_int ostype, void *fct)
{
  vector_t *info;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  info = aspect_vector_get(LIBKERNSH_VECTOR_NAME_INFO);

  dim    = alloca(sizeof(u_int) * 3);
  dim[0] = archtype;
  dim[1] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER INFO\n");
#endif

  aspect_vectors_insert(info, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_decompkernel(u_int ostype, void *fct)
{

  vector_t *decomp;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  decomp = aspect_vector_get(LIBKERNSH_VECTOR_NAME_DECOMPKERNEL);

  dim    = alloca(sizeof(u_int) * 1);
  dim[0] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER DECOMP KERNEL\n");
#endif

  aspect_vectors_insert(decomp, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_symbs_abn(u_int archtype, u_int ostype, void *fct)
{
  vector_t *abn;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  abn = aspect_vector_get(LIBKERNSH_VECTOR_NAME_ADDRBYNAME);

  dim    = alloca(sizeof(u_int) * 2);
  dim[0] = archtype;
  dim[1] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER SYMBS ABN\n");
#endif

  aspect_vectors_insert(abn, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_symbs_nba(u_int archtype, u_int ostype, void *fct)
{
  vector_t *nba;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  nba = aspect_vector_get(LIBKERNSH_VECTOR_NAME_NAMEBYADDR);

  dim    = alloca(sizeof(u_int) * 2);
  dim[0] = archtype;
  dim[1] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER SYMBS NBA\n");
#endif

  aspect_vectors_insert(nba, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_alloc_contiguous(u_int ostype, void *fct)
{

  vector_t *alloc;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  alloc = aspect_vector_get(LIBKERNSH_VECTOR_NAME_ALLOCCONTIGUOUS);

  dim    = alloca(sizeof(u_int) * 1);
  dim[0] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER ALLOC CONTIGUOUS\n");
#endif

  aspect_vectors_insert(alloc, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_alloc_noncontiguous(u_int ostype, void *fct)
{

  vector_t *alloc;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  alloc = aspect_vector_get(LIBKERNSH_VECTOR_NAME_ALLOCNONCONTIGUOUS);

  dim    = alloca(sizeof(u_int) * 1);
  dim[0] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER ALLOC NON CONTIGUOUS\n");
#endif

  aspect_vectors_insert(alloc, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_free_contiguous(u_int ostype, void *fct)
{

  vector_t *alloc;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  alloc = aspect_vector_get(LIBKERNSH_VECTOR_NAME_FREECONTIGUOUS);

  dim    = alloca(sizeof(u_int) * 1);
  dim[0] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER FREE CONTIGUOUS\n");
#endif

  aspect_vectors_insert(alloc, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_free_noncontiguous(u_int ostype, void *fct)
{

  vector_t *alloc;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  alloc = aspect_vector_get(LIBKERNSH_VECTOR_NAME_FREENONCONTIGUOUS);

  dim    = alloca(sizeof(u_int) * 1);
  dim[0] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER FREE NON CONTIGUOUS\n");
#endif

  aspect_vectors_insert(alloc, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_autotypes(u_int archtype, u_int ostype, void *fct)
{
  vector_t *autotypes;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  autotypes = aspect_vector_get(LIBKERNSH_VECTOR_NAME_AUTOTYPES);

  dim    = alloca(sizeof(u_int) * 3);
  dim[0] = archtype;
  dim[1] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER AUTOTYPES\n");
#endif

  aspect_vectors_insert(autotypes, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_relink(u_int ostype, void *fct)
{

  vector_t *rel;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  rel = aspect_vector_get(LIBKERNSH_VECTOR_NAME_RELINKMODULE);

  dim    = alloca(sizeof(u_int) * 1);
  dim[0] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER RELINK MODULE\n");
#endif

  aspect_vectors_insert(rel, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_infect(u_int ostype, void *fct)
{

  vector_t *inf;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  inf = aspect_vector_get(LIBKERNSH_VECTOR_NAME_INFECTMODULE);

  dim    = alloca(sizeof(u_int) * 1);
  dim[0] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER INFECT MODULE\n");
#endif

  aspect_vectors_insert(inf, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_kload(u_int ostype, void *fct)
{

  vector_t *kload;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  kload = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KLOADMODULE);

  dim    = alloca(sizeof(u_int) * 1);
  dim[0] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER KLOAD MODULE\n");
#endif

  aspect_vectors_insert(kload, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_kunload(u_int ostype, void *fct)
{

  vector_t *kunload;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  kunload = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KUNLOADMODULE);

  dim    = alloca(sizeof(u_int) * 1);
  dim[0] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER KUNLOAD MODULE\n");
#endif

  aspect_vectors_insert(kunload, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_kvirtm_read_virtm(u_int ostype, u_int virtmtype, void *fct)
{
  vector_t *sct;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  sct = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KVIRTMREADVIRTM);

  dim    = alloca(sizeof(u_int) * 3);
  dim[0] = ostype;
  dim[1] = virtmtype;

#if __DEBUG_KERNSH__
  printf("REGISTER KVIRTM READ VIRTM\n");
#endif

  aspect_vectors_insert(sct, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_kvirtm_read_mem(u_int ostype, u_int virtmtype, void *fct)
{
  vector_t *sct;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  sct = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KVIRTMREADMEM);

  dim    = alloca(sizeof(u_int) * 3);
  dim[0] = ostype;
  dim[1] = virtmtype;

#if __DEBUG_KERNSH__
  printf("REGISTER KVIRTM READ MEM\n");
#endif

  aspect_vectors_insert(sct, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
