/*
** @file vectors_default.c
** @ingroup common
**
*/

#include "libkernsh.h"


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

int kernsh_kvirtm_write_virtm_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("KVIRTM WRITE VIRTM DEFAULT!!!\n");
#endif
  
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "kvirtm write virtm default !", -1);
}

int kernsh_kvirtm_write_mem_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("KVIRTM WRITE MEM DEFAULT!!!\n");
#endif
  
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "kvirtm write mem default !", -1);
}

int kernsh_kvirtm_task_pid_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("KVIRTM TASK PID DEFAULT!!!\n");
#endif
  
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "kvirtm task pid default !", -1);
}

int kernsh_kdump_get_vma_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("KDUMP GET VMA DEFAULT!!!\n");
#endif
  
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "kdump get vma default !", -1);
}

int kernsh_kdump_vma_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNEL__
  printf("KDUMP VMA DEFAULT!!!\n");
#endif
  
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "kdump vma default !", -1);
}
