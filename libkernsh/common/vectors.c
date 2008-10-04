/**
 * @defgroup libkernsh_common libkernsh_common
 */
/*
** @file vectors.c
** @ingroup libkernsh_common
**
*/
#include "libkernsh.h"

libkernshworld_t libkernshworld;

/**
 * @brief Initialise kernsh's vectors
 * @return 0 on success, -1 on return
 */
int kernsh_init_vectors()
{
  unsigned int *dims;
  char  **strdims;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  aspect_init();
  
  /* Memory ARCH, OS, DEVICE */
#if __DEBUG_KERNSH__
  printf("INIT MEMORY VECTORS\n");
#endif

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 4 * sizeof(unsigned int) , -1);
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

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 3 * sizeof(unsigned int) , -1);
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

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 2 * sizeof(unsigned int) , -1);
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

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 3 * sizeof(unsigned int) , -1);
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

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 3 * sizeof(unsigned int) , -1);
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

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 3 * sizeof(unsigned int) , -1);
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

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 2 * sizeof(unsigned int) , -1);
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

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 3 * sizeof(unsigned int) , -1);
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

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 2 * sizeof(unsigned int) , -1);
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

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 3 * sizeof(unsigned int) , -1);
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

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 2 * sizeof(unsigned int) , -1);
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

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 3 * sizeof(unsigned int) , -1);
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

   /* Register kvirtm write * default vectors */
  aspect_register_vector(LIBKERNSH_VECTOR_NAME_KVIRTMWRITEVIRTM, 
                         kernsh_kvirtm_write_virtm_default,
                         dims, strdims, 2, ASPECT_TYPE_CADDR);

  aspect_register_vector(LIBKERNSH_VECTOR_NAME_KVIRTMWRITEMEM, 
                         kernsh_kvirtm_write_mem_default,
                         dims, strdims, 2, ASPECT_TYPE_CADDR);

  aspect_register_vector(LIBKERNSH_VECTOR_NAME_KVIRTMTASKPID, 
                         kernsh_kvirtm_task_pid_default,
                         dims, strdims, 2, ASPECT_TYPE_CADDR);

#if __DEBUG_KERNSH__
  printf("KDUMP VMA VECTORS\n");
#endif

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 3 * sizeof(unsigned int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,strdims, 3 * sizeof(char *), -1);

  dims[0]    = LIBKERNSH_OSNUM;
  dims[1]    = LIBKERNSH_VMANUM;

  strdims[0] = "OSTYPE";
  strdims[1] = "VMATYPE";

  aspect_register_vector(LIBKERNSH_VECTOR_NAME_KDUMPGETVMA, 
			 kernsh_kdump_get_vma_default,
			 dims, strdims, 2, ASPECT_TYPE_CADDR);

  aspect_register_vector(LIBKERNSH_VECTOR_NAME_KDUMPVMA, 
			 kernsh_kdump_vma_default,
			 dims, strdims, 2, ASPECT_TYPE_CADDR);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Register kernsh's vectors
 * @return 0 on success, -1 on return
 */
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
  kernsh_register_openmem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6, 
			  LIBKERNSH_DEVICE_KVIRTM,
			  kernsh_kvirtm_openmem);
  kernsh_register_openmem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_4, 
			  LIBKERNSH_DEVICE_KVIRTM,
			  kernsh_kvirtm_openmem);

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
  kernsh_register_closemem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
			   LIBKERNSH_DEVICE_KVIRTM, 
			   kernsh_kvirtm_closemem);
  kernsh_register_closemem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_4,
			   LIBKERNSH_DEVICE_KVIRTM, 
			   kernsh_kvirtm_closemem);

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
  kernsh_register_readmem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
			  LIBKERNSH_DEVICE_KVIRTM, 
			  kernsh_kvirtm_readmem);
  kernsh_register_readmem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_4,
			  LIBKERNSH_DEVICE_KVIRTM, 
			  kernsh_kvirtm_readmem);

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

  kernsh_register_writemem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
			   LIBKERNSH_DEVICE_KVIRTM, 
			   kernsh_kvirtm_writemem);
  kernsh_register_writemem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_4,
			   LIBKERNSH_DEVICE_KVIRTM, 
			   kernsh_kvirtm_writemem);

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

  /* kvirtm read/write virtm */
  kernsh_register_kvirtm_read_virtm(LIBKERNSH_OS_LINUX_2_6,
				    LIBKERNSH_KERNEL_MODE,
				    kernsh_kvirtm_read_virtm_proc_linux);
  
  kernsh_register_kvirtm_read_virtm(LIBKERNSH_OS_LINUX_2_4,
				    LIBKERNSH_KERNEL_MODE,
				    kernsh_kvirtm_read_virtm_proc_linux);
  
  kernsh_register_kvirtm_read_virtm(LIBKERNSH_OS_LINUX_2_6,
				    LIBKERNSH_USER_MODE,
				    kernsh_kvirtm_read_virtm_syscall_linux);
  
   kernsh_register_kvirtm_read_virtm(LIBKERNSH_OS_LINUX_2_4,
				    LIBKERNSH_USER_MODE,
				    kernsh_kvirtm_read_virtm_syscall_linux);

  kernsh_register_kvirtm_read_virtm(LIBKERNSH_OS_LINUX_2_4,
				    LIBKERNSH_USER_MODE,
				    kernsh_kvirtm_read_virtm_syscall_linux);

  kernsh_register_kvirtm_write_virtm(LIBKERNSH_OS_LINUX_2_6,
				     LIBKERNSH_KERNEL_MODE,
				     kernsh_kvirtm_write_virtm_proc_linux);

  kernsh_register_kvirtm_write_virtm(LIBKERNSH_OS_LINUX_2_4,
				     LIBKERNSH_KERNEL_MODE,
				     kernsh_kvirtm_write_virtm_proc_linux);

  kernsh_register_kvirtm_write_virtm(LIBKERNSH_OS_LINUX_2_6,
				     LIBKERNSH_USER_MODE,
				     kernsh_kvirtm_write_virtm_syscall_linux);

  kernsh_register_kvirtm_write_virtm(LIBKERNSH_OS_LINUX_2_4,
				     LIBKERNSH_USER_MODE,
				     kernsh_kvirtm_write_virtm_syscall_linux);

  /* kvirtm read/write mem */
  kernsh_register_kvirtm_read_mem(LIBKERNSH_OS_LINUX_2_6,
				  LIBKERNSH_KERNEL_MODE,
				  kernsh_kvirtm_readmem_proc_linux);

  kernsh_register_kvirtm_read_mem(LIBKERNSH_OS_LINUX_2_4,
				  LIBKERNSH_KERNEL_MODE,
				  kernsh_kvirtm_readmem_proc_linux);

  kernsh_register_kvirtm_read_mem(LIBKERNSH_OS_LINUX_2_6,
				  LIBKERNSH_USER_MODE,
				  kernsh_kvirtm_readmem_syscall_linux);

  kernsh_register_kvirtm_read_mem(LIBKERNSH_OS_LINUX_2_4,
				  LIBKERNSH_USER_MODE,
				  kernsh_kvirtm_readmem_syscall_linux);

  kernsh_register_kvirtm_write_mem(LIBKERNSH_OS_LINUX_2_6,
				   LIBKERNSH_KERNEL_MODE,
				   kernsh_kvirtm_writemem_proc_linux);

  kernsh_register_kvirtm_write_mem(LIBKERNSH_OS_LINUX_2_4,
				   LIBKERNSH_KERNEL_MODE,
				   kernsh_kvirtm_writemem_proc_linux);

  kernsh_register_kvirtm_write_mem(LIBKERNSH_OS_LINUX_2_6,
				   LIBKERNSH_USER_MODE,
				   kernsh_kvirtm_writemem_syscall_linux);
  
  kernsh_register_kvirtm_write_mem(LIBKERNSH_OS_LINUX_2_4,
				   LIBKERNSH_USER_MODE,
				   kernsh_kvirtm_writemem_syscall_linux);
  
  kernsh_register_kvirtm_task_pid(LIBKERNSH_OS_LINUX_2_6,
				  LIBKERNSH_KERNEL_MODE,
				  kernsh_kvirtm_task_pid_proc_linux);
  
  kernsh_register_kvirtm_task_pid(LIBKERNSH_OS_LINUX_2_4,
				  LIBKERNSH_KERNEL_MODE,
				  kernsh_kvirtm_task_pid_proc_linux);

  kernsh_register_kvirtm_task_pid(LIBKERNSH_OS_LINUX_2_6,
				  LIBKERNSH_USER_MODE,
				  kernsh_kvirtm_task_pid_syscall_linux);
  
  kernsh_register_kvirtm_task_pid(LIBKERNSH_OS_LINUX_2_4,
				  LIBKERNSH_USER_MODE,
				  kernsh_kvirtm_task_pid_syscall_linux);

  kernsh_register_kdump_get_vma(LIBKERNSH_OS_LINUX_2_6,
				LIBKERNSH_VMA_USERLAND,
				kernsh_kdump_get_vma_userland_linux);

  kernsh_register_kdump_get_vma(LIBKERNSH_OS_LINUX_2_4,
				LIBKERNSH_VMA_USERLAND,
				kernsh_kdump_get_vma_userland_linux);

  kernsh_register_kdump_vma(LIBKERNSH_OS_LINUX_2_6,
			    LIBKERNSH_VMA_USERLAND,
			    kernsh_kdump_vma_userland_linux);
  
  kernsh_register_kdump_vma(LIBKERNSH_OS_LINUX_2_4,
			    LIBKERNSH_VMA_USERLAND,
			    kernsh_kdump_vma_userland_linux);

  kernsh_register_kdump_vma(LIBKERNSH_OS_LINUX_2_6,
			    LIBKERNSH_VMA_KERNELLAND,
			    kernsh_kdump_vma_kernelland_linux);
  
  kernsh_register_kdump_vma(LIBKERNSH_OS_LINUX_2_4,
			    LIBKERNSH_VMA_KERNELLAND,
			    kernsh_kdump_vma_kernelland_linux);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
