/*
** @file vectors_register.c
** @ingroup libkernsh_common
**
*/
#include "libkernsh.h"

int kernsh_register_openmem(unsigned int archtype, unsigned int ostype, unsigned int devicetype, 
			    void *fct)
{
  vector_t *mem;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  mem = aspect_vector_get(LIBKERNSH_VECTOR_NAME_OPENMEM);

  dim    = alloca(sizeof(unsigned int) * 4);
  dim[0] = archtype;
  dim[1] = ostype;
  dim[2] = devicetype;

#if __DEBUG_KERNSH__
  printf("REGISTER OPENMEM\n");
#endif

  aspect_vectors_insert(mem, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_closemem(unsigned int archtype, unsigned int ostype, unsigned int devicetype, 
			     void *fct)
{
  vector_t *mem;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  mem = aspect_vector_get(LIBKERNSH_VECTOR_NAME_CLOSEMEM);

  dim    = alloca(sizeof(unsigned int) * 4);
  dim[0] = archtype;
  dim[1] = ostype;
  dim[2] = devicetype;

#if __DEBUG_KERNSH__
  printf("REGISTER CLOSEMEM\n");
#endif

  aspect_vectors_insert(mem, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


int kernsh_register_readmem(unsigned int archtype, unsigned int ostype, unsigned int devicetype, 
			    void *fct)
{
  vector_t *mem;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  mem = aspect_vector_get(LIBKERNSH_VECTOR_NAME_READMEM);

  dim    = alloca(sizeof(unsigned int) * 4);
  dim[0] = archtype;
  dim[1] = ostype;
  dim[2] = devicetype;

#if __DEBUG_KERNSH__
  printf("REGISTER READMEM\n");
#endif

  aspect_vectors_insert(mem, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


int kernsh_register_writemem(unsigned int archtype, unsigned int ostype, unsigned int devicetype, 
			    void *fct)
{
  vector_t *mem;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  mem = aspect_vector_get(LIBKERNSH_VECTOR_NAME_WRITEMEM);

  dim    = alloca(sizeof(unsigned int) * 4);
  dim[0] = archtype;
  dim[1] = ostype;
  dim[2] = devicetype;

#if __DEBUG_KERNSH__
  printf("REGISTER WRITEMEM\n");
#endif

  aspect_vectors_insert(mem, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_sct(unsigned int archtype, unsigned int ostype, void *fct)
{
  vector_t *sct;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  sct = aspect_vector_get(LIBKERNSH_VECTOR_NAME_SCT);

  dim    = alloca(sizeof(unsigned int) * 3);
  dim[0] = archtype;
  dim[1] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER SCT\n");
#endif

  aspect_vectors_insert(sct, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_callsc(unsigned int ostype, void *fct)
{

  vector_t *csc;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  csc = aspect_vector_get(LIBKERNSH_VECTOR_NAME_CALLSC);

  dim    = alloca(sizeof(unsigned int) * 1);
  dim[0] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER CALLSC CONTIGUOUS\n");
#endif

  aspect_vectors_insert(csc, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_idt(unsigned int archtype, unsigned int ostype, void *fct)
{
  vector_t *idt;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  idt = aspect_vector_get(LIBKERNSH_VECTOR_NAME_IDT);

  dim    = alloca(sizeof(unsigned int) * 3);
  dim[0] = archtype;
  dim[1] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER IDT\n");
#endif

  aspect_vectors_insert(idt, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_gdt(unsigned int archtype, unsigned int ostype, void *fct)
{
  vector_t *gdt;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  gdt = aspect_vector_get(LIBKERNSH_VECTOR_NAME_GDT);

  dim    = alloca(sizeof(unsigned int) * 3);
  dim[0] = archtype;
  dim[1] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER GDT\n");
#endif

  aspect_vectors_insert(gdt, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_info(unsigned int archtype, unsigned int ostype, void *fct)
{
  vector_t *info;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  info = aspect_vector_get(LIBKERNSH_VECTOR_NAME_INFO);

  dim    = alloca(sizeof(unsigned int) * 3);
  dim[0] = archtype;
  dim[1] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER INFO\n");
#endif

  aspect_vectors_insert(info, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_decompkernel(unsigned int ostype, void *fct)
{

  vector_t *decomp;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  decomp = aspect_vector_get(LIBKERNSH_VECTOR_NAME_DECOMPKERNEL);

  dim    = alloca(sizeof(unsigned int) * 1);
  dim[0] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER DECOMP KERNEL\n");
#endif

  aspect_vectors_insert(decomp, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_symbs_abn(unsigned int archtype, unsigned int ostype, void *fct)
{
  vector_t *abn;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  abn = aspect_vector_get(LIBKERNSH_VECTOR_NAME_ADDRBYNAME);

  dim    = alloca(sizeof(unsigned int) * 2);
  dim[0] = archtype;
  dim[1] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER SYMBS ABN\n");
#endif

  aspect_vectors_insert(abn, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_symbs_nba(unsigned int archtype, unsigned int ostype, void *fct)
{
  vector_t *nba;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  nba = aspect_vector_get(LIBKERNSH_VECTOR_NAME_NAMEBYADDR);

  dim    = alloca(sizeof(unsigned int) * 2);
  dim[0] = archtype;
  dim[1] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER SYMBS NBA\n");
#endif

  aspect_vectors_insert(nba, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_alloc_contiguous(unsigned int ostype, void *fct)
{

  vector_t *alloc;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  alloc = aspect_vector_get(LIBKERNSH_VECTOR_NAME_ALLOCCONTIGUOUS);

  dim    = alloca(sizeof(unsigned int) * 1);
  dim[0] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER ALLOC CONTIGUOUS\n");
#endif

  aspect_vectors_insert(alloc, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_alloc_noncontiguous(unsigned int ostype, void *fct)
{

  vector_t *alloc;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  alloc = aspect_vector_get(LIBKERNSH_VECTOR_NAME_ALLOCNONCONTIGUOUS);

  dim    = alloca(sizeof(unsigned int) * 1);
  dim[0] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER ALLOC NON CONTIGUOUS\n");
#endif

  aspect_vectors_insert(alloc, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_free_contiguous(unsigned int ostype, void *fct)
{

  vector_t *alloc;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  alloc = aspect_vector_get(LIBKERNSH_VECTOR_NAME_FREECONTIGUOUS);

  dim    = alloca(sizeof(unsigned int) * 1);
  dim[0] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER FREE CONTIGUOUS\n");
#endif

  aspect_vectors_insert(alloc, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_free_noncontiguous(unsigned int ostype, void *fct)
{

  vector_t *alloc;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  alloc = aspect_vector_get(LIBKERNSH_VECTOR_NAME_FREENONCONTIGUOUS);

  dim    = alloca(sizeof(unsigned int) * 1);
  dim[0] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER FREE NON CONTIGUOUS\n");
#endif

  aspect_vectors_insert(alloc, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_autotypes(unsigned int archtype, unsigned int ostype, void *fct)
{
  vector_t *autotypes;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  autotypes = aspect_vector_get(LIBKERNSH_VECTOR_NAME_AUTOTYPES);

  dim    = alloca(sizeof(unsigned int) * 3);
  dim[0] = archtype;
  dim[1] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER AUTOTYPES\n");
#endif

  aspect_vectors_insert(autotypes, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_relink(unsigned int ostype, void *fct)
{

  vector_t *rel;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  rel = aspect_vector_get(LIBKERNSH_VECTOR_NAME_RELINKMODULE);

  dim    = alloca(sizeof(unsigned int) * 1);
  dim[0] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER RELINK MODULE\n");
#endif

  aspect_vectors_insert(rel, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_infect(unsigned int ostype, void *fct)
{

  vector_t *inf;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  inf = aspect_vector_get(LIBKERNSH_VECTOR_NAME_INFECTMODULE);

  dim    = alloca(sizeof(unsigned int) * 1);
  dim[0] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER INFECT MODULE\n");
#endif

  aspect_vectors_insert(inf, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_kload(unsigned int ostype, void *fct)
{

  vector_t *kload;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  kload = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KLOADMODULE);

  dim    = alloca(sizeof(unsigned int) * 1);
  dim[0] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER KLOAD MODULE\n");
#endif

  aspect_vectors_insert(kload, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_kunload(unsigned int ostype, void *fct)
{

  vector_t *kunload;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  kunload = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KUNLOADMODULE);

  dim    = alloca(sizeof(unsigned int) * 1);
  dim[0] = ostype;

#if __DEBUG_KERNSH__
  printf("REGISTER KUNLOAD MODULE\n");
#endif

  aspect_vectors_insert(kunload, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_kvirtm_read_virtm(unsigned int ostype, unsigned int virtmtype, void *fct)
{
  vector_t *sct;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  sct = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KVIRTMREADVIRTM);

  dim    = alloca(sizeof(unsigned int) * 3);
  dim[0] = ostype;
  dim[1] = virtmtype;

#if __DEBUG_KERNSH__
  printf("REGISTER KVIRTM READ VIRTM\n");
#endif

  aspect_vectors_insert(sct, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_kvirtm_read_mem(unsigned int ostype, unsigned int virtmtype, void *fct)
{
  vector_t *sct;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  sct = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KVIRTMREADMEM);

  dim    = alloca(sizeof(unsigned int) * 3);
  dim[0] = ostype;
  dim[1] = virtmtype;

#if __DEBUG_KERNSH__
  printf("REGISTER KVIRTM READ MEM\n");
#endif

  aspect_vectors_insert(sct, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


int kernsh_register_kvirtm_write_virtm(unsigned int ostype, unsigned int virtmtype, void *fct)
{
  vector_t *sct;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  sct = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KVIRTMWRITEVIRTM);

  dim    = alloca(sizeof(unsigned int) * 3);
  dim[0] = ostype;
  dim[1] = virtmtype;

#if __DEBUG_KERNSH__
  printf("REGISTER KVIRTM WRITE VIRTM\n");
#endif

  aspect_vectors_insert(sct, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_kvirtm_write_mem(unsigned int ostype, unsigned int virtmtype, void *fct)
{
  vector_t *sct;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  sct = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KVIRTMWRITEMEM);

  dim    = alloca(sizeof(unsigned int) * 3);
  dim[0] = ostype;
  dim[1] = virtmtype;

#if __DEBUG_KERNSH__
  printf("REGISTER KVIRTM WRITE MEM\n");
#endif

  aspect_vectors_insert(sct, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_kvirtm_task_pid(unsigned int ostype, unsigned int virtmtype, void *fct)
{
  vector_t *sct;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  sct = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KVIRTMTASKPID);

  dim    = alloca(sizeof(unsigned int) * 3);
  dim[0] = ostype;
  dim[1] = virtmtype;

#if __DEBUG_KERNSH__
  printf("REGISTER KVIRTM TASK PID\n");
#endif

  aspect_vectors_insert(sct, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_kdump_get_vma(unsigned int ostype, unsigned int vmatype, void *fct)
{
  vector_t *vma;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  vma = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KDUMPGETVMA);

  dim    = alloca(sizeof(unsigned int) * 3);
  dim[0] = ostype;
  dim[1] = vmatype;

#if __DEBUG_KERNSH__
  printf("REGISTER KDUMP GET VMA\n");
#endif

  aspect_vectors_insert(vma, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_register_kdump_vma(unsigned int ostype, unsigned int vmatype, void *fct)
{
  vector_t *vma;
  unsigned int *dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  vma = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KDUMPVMA);

  dim    = alloca(sizeof(unsigned int) * 3);
  dim[0] = ostype;
  dim[1] = vmatype;

#if __DEBUG_KERNSH__
  printf("REGISTER KDUMP VMA\n");
#endif

  aspect_vectors_insert(vma, dim, (int)fct);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
