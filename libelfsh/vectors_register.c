/**
** @file vectors_register.c
** @ingroup libelfsh
**
** vectors_register.c for libelfsh (The ELF shell library)
**
** API for registering handlers in vectors of Libelfsh
**
** $Id$$
*/
#include "libelfsh.h"


int		elfsh_register_readmemf(u_int hostype, u_int exectype, void *fct)
{
  vector_t	*mem;
  u_int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (hostype >= ELFSH_HOST_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Host type", -1);
  if (exectype >= LIBELFSH_MODE_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                 "Invalid execution Mode", -1);
  mem = aspect_vector_get(ELFSH_HOOK_READMEMF);
  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = hostype;
  dim[1] = exectype;
  aspect_vectors_insert(mem, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


int		elfsh_register_writememf(u_int hostype, u_int exectype, void *fct)
{
  vector_t	*mem;
  u_int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (hostype >= ELFSH_HOST_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Host type", -1);
  if (exectype >= LIBELFSH_MODE_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                 "Invalid execution Mode", -1);
  mem = aspect_vector_get(ELFSH_HOOK_WRITEMEMF);
  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = hostype;
  dim[1] = exectype;
  aspect_vectors_insert(mem, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


int		elfsh_register_readmem(u_int ostype, u_int devicetype, void *fct)
{
  vector_t	*mem;
  u_int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (devicetype >= ELFSH_IOTYPE_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                 "Invalid Object type", -1);
  if (ostype >= ELFSH_OS_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                 "Invalid Operating System type", -1);
  mem = aspect_vector_get(ELFSH_HOOK_READMEM);
  dim    = alloca(sizeof(u_int) * 3);
  dim[0] = ostype;
  dim[1] = devicetype;
  aspect_vectors_insert(mem, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


int		elfsh_register_readmema(u_int ostype, u_int devicetype, void *fct)
{
  vector_t	*mem;
  u_int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (devicetype >= ELFSH_IOTYPE_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                 "Invalid Object type", -1);
  if (ostype >= ELFSH_OS_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                 "Invalid Operating System type", -1);
  mem = aspect_vector_get(ELFSH_HOOK_READMEMA);
  dim    = alloca(sizeof(u_int) * 3);
  dim[0] = ostype;
  dim[1] = devicetype;
  aspect_vectors_insert(mem, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


int		elfsh_register_writemem(u_int ostype, u_int devicetype, void *fct)

{
  vector_t	*mem;
  u_int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (devicetype >= ELFSH_IOTYPE_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                 "Invalid Object type", -1);
  if (ostype >= ELFSH_OS_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                 "Invalid Operating System type", -1);
  mem = aspect_vector_get(ELFSH_HOOK_WRITEMEM);
  dim    = alloca(sizeof(u_int) * 3);
  dim[0] = ostype;
  dim[1] = devicetype;
  aspect_vectors_insert(mem, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Registration handlers
 *
 * @param archtype
 * @param objtype
 * @param ostype
 * @param fct
 * @return
 */
int	elfsh_register_altplthook(u_char archtype,
                                  u_char objtype,
                                  u_char ostype,
                                  void   *fct)
{
  vector_t	*altplt;
  u_int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  altplt = aspect_vector_get(ELFSH_HOOK_ALTPLT);

  if (archtype >= ELFSH_ARCH_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_FILE_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Object type", -1);
  if (ostype >= ELFSH_OS_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Operating System type", -1);

  dim = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = objtype;
  dim[2] = ostype;
  aspect_vectors_insert(altplt, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Register an EXTPLT handler
 *
 * @param archtype
 * @param objtype
 * @param ostype
 * @param fct
 * @return
 */
int		elfsh_register_extplthook(u_char archtype,
                                          u_char objtype,
                                          u_char ostype,
                                          void   *fct)
{
  vector_t	*extplt;
  u_int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  extplt = aspect_vector_get(ELFSH_HOOK_EXTPLT);
  if (archtype >= ELFSH_ARCH_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_FILE_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Object type", -1);
  if (ostype >= ELFSH_OS_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Operating System type", -1);

  dim = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = objtype;
  dim[2] = ostype;
  aspect_vectors_insert(extplt, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Register an PLT handler
 *
 * @param archtype
 * @param objtype
 * @param ostype
 * @param fct
 * @return
 */
int		elfsh_register_plthook(u_char archtype, u_char objtype,
                                       u_char ostype, void *fct)
{
  vector_t	*plt;
  u_int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  plt = aspect_vector_get(ELFSH_HOOK_PLT);
  if (archtype >= ELFSH_ARCH_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_FILE_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Object type", -1);
  if (ostype >= ELFSH_OS_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Operating System type", -1);

  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = objtype;
  dim[2] = ostype;
  aspect_vectors_insert(plt, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Register an ENCODEPLT handler
 * @param archtype
 * @param objtype
 * @param ostype
 * @param fct
 * @return
 */
int	elfsh_register_encodeplthook(u_char archtype, u_char objtype,
                                     u_char ostype, void *fct)
{
  vector_t	*encodeplt;
  u_int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  encodeplt = aspect_vector_get(ELFSH_HOOK_ENCODEPLT);

  if (archtype >= ELFSH_ARCH_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_FILE_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Object type", -1);
  if (ostype >= ELFSH_OS_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Operating System type", -1);

  dim = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = objtype;
  dim[2] = ostype;
  aspect_vectors_insert(encodeplt, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Register an ENCODEPLT1 handler
 * @param archtype
 * @param objtype
 * @param ostype
 * @param fct
 * @return
 */
int	elfsh_register_encodeplt1hook(u_char archtype, u_char objtype,
                                      u_char ostype, void *fct)
{
  vector_t	*encodeplt1;
  u_int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  encodeplt1 = aspect_vector_get(ELFSH_HOOK_ENCODEPLT1);

  if (archtype >= ELFSH_ARCH_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_FILE_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Object type", -1);
  if (ostype >= ELFSH_OS_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Operating System type", -1);
  dim = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = objtype;
  dim[2] = ostype;
  aspect_vectors_insert(encodeplt1, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/**
 * Register an ET_REL injection handler
 * @param archtype
 * @param objtype
 * @param ostype
 * @param fct
 * @return
 */
int	elfsh_register_relhook(u_char archtype, u_char objtype, u_char ostype,
                               void *fct)
{
  vector_t	*rel;
  u_int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  rel = aspect_vector_get(ELFSH_HOOK_REL);

  if (archtype >= ELFSH_ARCH_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_FILE_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Object type", -1);
  if (ostype >= ELFSH_OS_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Operating System type", -1);

  dim = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = objtype;
  dim[2] = ostype;
  aspect_vectors_insert(rel, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Register a control flow redirection handler
 * @param archtype
 * @param objtype
 * @param ostype
 * @param fct
 * @return
 */
int	elfsh_register_cflowhook(u_char archtype, u_char objtype,
                                 u_char ostype, void *fct)
{
  vector_t	*cflow;
  u_int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  cflow = aspect_vector_get(ELFSH_HOOK_CFLOW);

  if (archtype >= ELFSH_ARCH_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_FILE_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Object type", -1);
  if (ostype >= ELFSH_OS_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Operating System type", -1);

  dim = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = objtype;
  dim[2] = ostype;
  aspect_vectors_insert(cflow, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Register a args counting redirection handler
 * @param archtype
 * @param objtype
 * @param ostype
 * @param fct
 * @return
 */
int	elfsh_register_argchook(u_char archtype, u_char objtype,
                                u_char ostype, void *fct)
{
  vector_t	*argcp;
  u_int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  argcp = aspect_vector_get(ELFSH_HOOK_ARGC);

  if (archtype >= ELFSH_ARCH_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_FILE_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Object type", -1);
  if (ostype >= ELFSH_OS_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Operating System type", -1);

  dim = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = objtype;
  dim[2] = ostype;
  aspect_vectors_insert(argcp, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/**
 * @brief Register a runtime map vector handler
 * @param hostype The host type to register the handler for.
 * @param fct The function pointer to register.
 * @return Success (0) or Error (-1)
 */
int		elfsh_register_allochook(u_char hostype, void *fct)
{
  vector_t	*map;
  u_int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  map = aspect_vector_get(ELFSH_HOOK_ALLOC);
  if (hostype >= ELFSH_HOST_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Host type", -1);
  dim = alloca(sizeof(u_int));
  dim[0] = hostype;
  aspect_vectors_insert(map, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/**
 * @brief Register a mprotect vector handler
 * @param hostype The host type to register the handler for.
 * @param fct The function pointer to register.
 * @return Success (0) or Error (-1)
 */
int		elfsh_register_mprotecthook(u_char hostype, void *fct)
{
  vector_t	*mprot;
  u_int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  mprot = aspect_vector_get(ELFSH_HOOK_MPROTECT);
  if (hostype >= ELFSH_HOST_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Host type", -1);
  dim = alloca(sizeof(u_int));
  dim[0] = hostype;
  aspect_vectors_insert(mprot, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * @brief Register a munprotect vector handler
 * @param hostype The host type to register the handler for.
 * @param fct The function pointer to register.
 * @return Success (0) or Error (-1)
 */
int		elfsh_register_munprotecthook(u_char hostype, void *fct)
{
  vector_t	*munprot;
  u_int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  munprot = aspect_vector_get(ELFSH_HOOK_MUNPROTECT);
  if (hostype >= ELFSH_HOST_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Host type", -1);
  dim = alloca(sizeof(u_int));
  dim[0] = hostype;
  aspect_vectors_insert(munprot, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
