/*
** vectors.c for libkernsh
**
** $Id: vectors.c,v 1.1 2007-07-25 19:53:01 pouik Exp $
**
*/
#include "libkernsh.h"
#include "libaspect.h"

libkernshworld_t libkernshworld;


int kernsh_openmem_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_output("OPENMEM DEFAULT!!!\n");

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_closemem_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_output("CLOSEMEM DEFAULT!!!\n");

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_readmem_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_output("READMEM DEFAULT!!!\n");

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_writemem_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_output("WRITEMEM DEFAULT!!!\n");

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_sct_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_output("SCT DEFAULT!!!\n");

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_idt_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_output("IDT DEFAULT!!!\n");

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_info_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_output("INFO DEFAULT!!!\n");

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_decompkernel_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_output("DECOMPKERNEL DEFAULT!!!\n");
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_loadkernel_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_output("LOADKERNEL DEFAULT!!!\n");
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_autovariables_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_output("AUTOVARIABLES DEFAULT!!!\n");
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_symbs_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  revm_output("SYMBS DEFAULT!!!\n");
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_alloc_contiguous_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  revm_output("ALLOC CONTIGUOUS DEFAULT!!!\n");
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_alloc_noncontiguous_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  revm_output("ALLOC NON CONTIGUOUS DEFAULT!!!\n");
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_free_contiguous_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  revm_output("FREE CONTIGUOUS DEFAULT!!!\n");
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_free_noncontiguous_default()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  revm_output("FREE NON CONTIGUOUS DEFAULT!!!\n");
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Init vectors */
int kernsh_init_vectors()
{
  u_int *dims;
  char  **strdims;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  aspect_init();
  
  /* Memory ARCH, OS, DEVICE */
  revm_output("INIT MEMORY VECTORS\n");

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 4 * sizeof(u_int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,strdims, 4 * sizeof(char *), -1);

  dims[0]    = LIBKERNSH_ARCHNUM;
  dims[1]    = LIBKERNSH_OSNUM;
  dims[2]    = LIBKERNSH_DEVICENUM;

  strdims[0] = "ARCHTYPE";
  strdims[1] = "OSTYPE";
  strdims[2] = "DEVICETYPE";

  /* Register memory default vectors */
  aspect_register_vector("openmem", 
                         kernsh_openmem_default,
                         dims, strdims, 3, ASPECT_TYPE_CADDR);

  aspect_register_vector("closemem", kernsh_closemem_default,
			 dims, strdims, 3, ASPECT_TYPE_CADDR);

  aspect_register_vector("readmem", kernsh_readmem_default,
			 dims, strdims, 3, ASPECT_TYPE_CADDR);

  aspect_register_vector("writemem", kernsh_writemem_default,
			 dims, strdims, 3, ASPECT_TYPE_CADDR);


  /* Syscalls ARCH, OS */
  revm_output("INIT SCT VECTORS\n");
  
  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 3 * sizeof(u_int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,strdims, 3 * sizeof(char *), -1);

  dims[0]    = LIBKERNSH_ARCHNUM;
  dims[1]    = LIBKERNSH_OSNUM;

  strdims[0] = "ARCHTYPE";
  strdims[1] = "OSTYPE";

  /* Register sct default vectors */
  aspect_register_vector("sct", 
                         kernsh_sct_default,
                         dims, strdims, 2, ASPECT_TYPE_CADDR);

  /* IDT ARCH, OS */
  revm_output("INIT IDT VECTORS\n");
  
  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 3 * sizeof(u_int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,strdims, 3 * sizeof(char *), -1);

  dims[0]    = LIBKERNSH_ARCHNUM;
  dims[1]    = LIBKERNSH_OSNUM;

  strdims[0] = "ARCHTYPE";
  strdims[1] = "OSTYPE";

  /* Register sct default vectors */
  aspect_register_vector("idt", 
                         kernsh_idt_default,
                         dims, strdims, 2, ASPECT_TYPE_CADDR);
  
  /* Info ARCH, OS */
  revm_output("INIT INFO VECTORS\n");
  
  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 3 * sizeof(u_int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,strdims, 3 * sizeof(char *), -1);

  dims[0]    = LIBKERNSH_ARCHNUM;
  dims[1]    = LIBKERNSH_OSNUM;

  strdims[0] = "ARCHTYPE";
  strdims[1] = "OSTYPE";

  /* Register info default vectors */
  aspect_register_vector("info",
			 kernsh_info_default,
			 dims, strdims, 2, ASPECT_TYPE_CADDR);

  revm_output("INIT KERNEL DECOMP VECTORS\n");

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 2 * sizeof(u_int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,strdims, 2 * sizeof(char *), -1);

  dims[0]    = LIBKERNSH_OSNUM;

  strdims[0] = "OSTYPE";

  /* Register kernel default vectors */
  aspect_register_vector("decompkernel", 
                         kernsh_decompkernel_default,
                         dims, strdims, 1, ASPECT_TYPE_CADDR);

  revm_output("INIT KERNEL LOAD VECTORS\n");

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 3 * sizeof(u_int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,strdims, 3 * sizeof(char *), -1);

  dims[0]    = LIBKERNSH_ARCHNUM;
  dims[1]    = LIBKERNSH_OSNUM;

  strdims[0] = "ARCHTYPE";
  strdims[1] = "OSTYPE";

  /* Register kernel default vectors */
  aspect_register_vector("loadkernel", 
                         kernsh_loadkernel_default,
                         dims, strdims, 2, ASPECT_TYPE_CADDR);  


  revm_output("INIT SYMBS VECTORS\n");

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 3 * sizeof(u_int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,strdims, 3 * sizeof(char *), -1);

  dims[0]    = LIBKERNSH_ARCHNUM;
  dims[1]    = LIBKERNSH_OSNUM;

  strdims[0] = "ARCHTYPE";
  strdims[1] = "OSTYPE";
  
  /* Register symbs default vectors */
  aspect_register_vector("addr_by_name", 
                         kernsh_symbs_default,
                         dims, strdims, 2, ASPECT_TYPE_CADDR); 

  aspect_register_vector("name_by_addr", 
                         kernsh_symbs_default,
                         dims, strdims, 2, ASPECT_TYPE_CADDR); 

  revm_output("INIT ALLOC CONTIGUOUS VECTORS\n");

  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 2 * sizeof(u_int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,strdims, 2 * sizeof(char *), -1);

  dims[0]    = LIBKERNSH_OSNUM;

  strdims[0] = "OSTYPE";

  /* Register alloc/free default vectors */
  aspect_register_vector("alloc_contiguous", 
                         kernsh_alloc_contiguous_default,
                         dims, strdims, 1, ASPECT_TYPE_CADDR);
  aspect_register_vector("alloc_noncontiguous", 
                         kernsh_alloc_noncontiguous_default,
                         dims, strdims, 1, ASPECT_TYPE_CADDR);
  aspect_register_vector("free_contiguous", 
                         kernsh_free_contiguous_default,
                         dims, strdims, 1, ASPECT_TYPE_CADDR);
  aspect_register_vector("free_noncontiguous", 
			 kernsh_free_noncontiguous_default,
			 dims, strdims, 1, ASPECT_TYPE_CADDR);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


int kernsh_register_vectors()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  kernsh_register_openmem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6, 
			  LIBKERNSH_DEVICE_KMEM,kernsh_openmem_kmem_linux_2_6);
  kernsh_register_openmem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6, 
			  LIBKERNSH_DEVICE_MEM, kernsh_openmem_mem_linux_2_6);
  kernsh_register_openmem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
			  LIBKERNSH_DEVICE_KCORE, 
			  kernsh_openmem_kcore_linux_2_6);
  kernsh_register_openmem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_NETBSD,
			  LIBKERNSH_DEVICE_MEM, kernsh_openmem_netbsd);
  
  kernsh_register_closemem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
			   LIBKERNSH_DEVICE_KMEM, kernsh_closemem_kmem_linux_2_6);
  kernsh_register_closemem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
			   LIBKERNSH_DEVICE_MEM, kernsh_closemem_mem_linux_2_6);
  kernsh_register_closemem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
			   LIBKERNSH_DEVICE_KCORE, 
			   kernsh_closemem_kcore_linux_2_6);
  kernsh_register_closemem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_NETBSD,
			   LIBKERNSH_DEVICE_MEM,
			   kernsh_closemem_netbsd);

  kernsh_register_readmem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
			  LIBKERNSH_DEVICE_KMEM, kernsh_readmem_kmem_linux_2_6);
  kernsh_register_readmem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
			  LIBKERNSH_DEVICE_MEM, kernsh_readmem_mem_linux_2_6);
  kernsh_register_readmem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
			  LIBKERNSH_DEVICE_KCORE, 
			  kernsh_readmem_kcore_linux_2_6);
  kernsh_register_readmem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_NETBSD,
			  LIBKERNSH_DEVICE_MEM, kernsh_readmem_netbsd);

  kernsh_register_writemem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
			   LIBKERNSH_DEVICE_KMEM, kernsh_writemem_kmem_linux_2_6);
  kernsh_register_writemem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
			   LIBKERNSH_DEVICE_MEM, kernsh_writemem_mem_linux_2_6);
  kernsh_register_writemem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
			   LIBKERNSH_DEVICE_KCORE, 
			   kernsh_writemem_kcore_linux_2_6);
  kernsh_register_writemem(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_NETBSD,
			   LIBKERNSH_DEVICE_MEM, kernsh_writemem_netbsd);

  kernsh_register_sct(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
		      kernsh_sct_linux);
  kernsh_register_sct(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_4,
		      kernsh_sct_linux);

  kernsh_register_idt(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
		      kernsh_idt_linux);
  kernsh_register_idt(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_4,
		      kernsh_idt_linux);

  kernsh_register_info(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_6,
		       kernsh_info_linux);
  kernsh_register_info(LIBKERNSH_ARCH_I386, LIBKERNSH_OS_LINUX_2_4,
		       kernsh_info_linux);

  kernsh_register_decompkernel(LIBKERNSH_OS_LINUX_2_6, kernsh_decompkernel_linux);
  kernsh_register_decompkernel(LIBKERNSH_OS_LINUX_2_4, kernsh_decompkernel_linux);

  kernsh_register_loadkernel(LIBKERNSH_ARCH_I386, 
			     LIBKERNSH_OS_LINUX_2_6, kernsh_loadkernel_linux);
  kernsh_register_loadkernel(LIBKERNSH_ARCH_I386,
			     LIBKERNSH_OS_LINUX_2_4, kernsh_loadkernel_linux);

  kernsh_register_symbs_abn(LIBKERNSH_ARCH_I386, 
			    LIBKERNSH_OS_LINUX_2_6, kernsh_get_addr_by_name_linux_2_6);
  kernsh_register_symbs_nba(LIBKERNSH_ARCH_I386,
			    LIBKERNSH_OS_LINUX_2_6, kernsh_get_name_by_addr_linux_2_6);

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

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_openmem(u_int archtype, u_int ostype, u_int devicetype, 
			    void *fct)
{
  vector_t *mem;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  mem = aspect_vector_get("openmem");

  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = ostype;
  dim[2] = devicetype;

  revm_output("REGISTER OPENMEM\n");
  
  aspect_vectors_insert(mem, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_closemem(u_int archtype, u_int ostype, u_int devicetype, 
			     void *fct)
{
  vector_t *mem;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  mem = aspect_vector_get("closemem");

  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = ostype;
  dim[2] = devicetype;

  revm_output("REGISTER CLOSEMEM\n");
  
  aspect_vectors_insert(mem, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


int kernsh_register_readmem(u_int archtype, u_int ostype, u_int devicetype, 
			    void *fct)
{
  vector_t *mem;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  mem = aspect_vector_get("readmem");

  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = ostype;
  dim[2] = devicetype;

  revm_output("REGISTER READMEM\n");
  
  aspect_vectors_insert(mem, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


int kernsh_register_writemem(u_int archtype, u_int ostype, u_int devicetype, 
			    void *fct)
{
  vector_t *mem;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  mem = aspect_vector_get("writemem");

  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = ostype;
  dim[2] = devicetype;

  revm_output("REGISTER WRITEMEM\n");
  
  aspect_vectors_insert(mem, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_sct(u_int archtype, u_int ostype, void *fct)
{
  vector_t *sct;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  sct = aspect_vector_get("sct");

  dim    = alloca(sizeof(u_int) * 3);
  dim[0] = archtype;
  dim[1] = ostype;

  revm_output("REGISTER SCT\n");
  
  aspect_vectors_insert(sct, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_idt(u_int archtype, u_int ostype, void *fct)
{
  vector_t *idt;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  idt = aspect_vector_get("idt");

  dim    = alloca(sizeof(u_int) * 3);
  dim[0] = archtype;
  dim[1] = ostype;

  revm_output("REGISTER IDT\n");
  
  aspect_vectors_insert(idt, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_info(u_int archtype, u_int ostype, void *fct)
{
  vector_t *info;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  info = aspect_vector_get("info");

  dim    = alloca(sizeof(u_int) * 3);
  dim[0] = archtype;
  dim[1] = ostype;

  revm_output("REGISTER INFO\n");
  
  aspect_vectors_insert(info, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_decompkernel(u_int ostype, void *fct)
{

  vector_t *decomp;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  decomp = aspect_vector_get("decompkernel");

  dim    = alloca(sizeof(u_int) * 1);
  dim[0] = ostype;

  revm_output("REGISTER DECOMP KERNEL\n");
  
  aspect_vectors_insert(decomp, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_loadkernel(u_int archtype, u_int ostype, void *fct)
{

  vector_t *load;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  load = aspect_vector_get("loadkernel");

  dim    = alloca(sizeof(u_int) * 2);
  dim[0] = archtype;
  dim[1] = ostype;

  revm_output("REGISTER LOAD KERNEL\n");
  
  aspect_vectors_insert(load, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_symbs_abn(u_int archtype, u_int ostype, void *fct)
{
  vector_t *abn;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  abn = aspect_vector_get("addr_by_name");

  dim    = alloca(sizeof(u_int) * 2);
  dim[0] = archtype;
  dim[1] = ostype;

  revm_output("REGISTER SYMBS ABN\n");
  
  aspect_vectors_insert(abn, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_symbs_nba(u_int archtype, u_int ostype, void *fct)
{
  vector_t *nba;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  nba = aspect_vector_get("name_by_addr");

  dim    = alloca(sizeof(u_int) * 2);
  dim[0] = archtype;
  dim[1] = ostype;

  revm_output("REGISTER SYMBS NBA\n");
  
  aspect_vectors_insert(nba, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_alloc_contiguous(u_int ostype, void *fct)
{

  vector_t *alloc;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  alloc = aspect_vector_get("alloc_contiguous");

  dim    = alloca(sizeof(u_int) * 1);
  dim[0] = ostype;

  revm_output("REGISTER ALLOC CONTIGUOUS\n");
  
  aspect_vectors_insert(alloc, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_alloc_noncontiguous(u_int ostype, void *fct)
{

  vector_t *alloc;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  alloc = aspect_vector_get("alloc_noncontiguous");

  dim    = alloca(sizeof(u_int) * 1);
  dim[0] = ostype;

  revm_output("REGISTER ALLOC NON CONTIGUOUS\n");
  
  aspect_vectors_insert(alloc, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_free_contiguous(u_int ostype, void *fct)
{

  vector_t *alloc;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  alloc = aspect_vector_get("free_contiguous");

  dim    = alloca(sizeof(u_int) * 1);
  dim[0] = ostype;

  revm_output("REGISTER FREE CONTIGUOUS\n");
  
  aspect_vectors_insert(alloc, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_free_noncontiguous(u_int ostype, void *fct)
{

  vector_t *alloc;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  alloc = aspect_vector_get("free_noncontiguous");

  dim    = alloca(sizeof(u_int) * 1);
  dim[0] = ostype;

  revm_output("REGISTER FREE NON CONTIGUOUS\n");
  
  aspect_vectors_insert(alloc, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
