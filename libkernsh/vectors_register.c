/**
 * @defgroup libkernsh libkernsh
 */
/*
** @file vectors_register.c
** @ingroup libkernsh
**
*/
#include "libkernsh.h"

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


int kernsh_register_kvirtm_write_virtm(u_int ostype, u_int virtmtype, void *fct)
{
  vector_t *sct;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  sct = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KVIRTMWRITEVIRTM);

  dim    = alloca(sizeof(u_int) * 3);
  dim[0] = ostype;
  dim[1] = virtmtype;

#if __DEBUG_KERNSH__
  printf("REGISTER KVIRTM WRITE VIRTM\n");
#endif

  aspect_vectors_insert(sct, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_kvirtm_write_mem(u_int ostype, u_int virtmtype, void *fct)
{
  vector_t *sct;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  sct = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KVIRTMWRITEMEM);

  dim    = alloca(sizeof(u_int) * 3);
  dim[0] = ostype;
  dim[1] = virtmtype;

#if __DEBUG_KERNSH__
  printf("REGISTER KVIRTM WRITE MEM\n");
#endif

  aspect_vectors_insert(sct, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_kvirtm_task_pid(u_int ostype, u_int virtmtype, void *fct)
{
  vector_t *sct;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  sct = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KVIRTMTASKPID);

  dim    = alloca(sizeof(u_int) * 3);
  dim[0] = ostype;
  dim[1] = virtmtype;

#if __DEBUG_KERNSH__
  printf("REGISTER KVIRTM TASK PID\n");
#endif

  aspect_vectors_insert(sct, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_kdump_get_vma(u_int ostype, u_int vmatype, void *fct)
{
  vector_t *vma;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  vma = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KDUMPGETVMA);

  dim    = alloca(sizeof(u_int) * 3);
  dim[0] = ostype;
  dim[1] = vmatype;

#if __DEBUG_KERNSH__
  printf("REGISTER KDUMP GET VMA\n");
#endif

  aspect_vectors_insert(vma, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_kdump_vma(u_int ostype, u_int vmatype, void *fct)
{
  vector_t *vma;
  u_int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  vma = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KDUMPVMA);

  dim    = alloca(sizeof(u_int) * 3);
  dim[0] = ostype;
  dim[1] = vmatype;

#if __DEBUG_KERNSH__
  printf("REGISTER KDUMP VMA\n");
#endif

  aspect_vectors_insert(vma, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
