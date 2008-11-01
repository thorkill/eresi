/**
** @file vectors_call.c
** @ingroup libelfsh
**
** vectors_call.c for libelfsh (The ELF shell library)
** 
** API for calling handlers of vectors in libelfsh
**
** $Id$
*/
#include "libelfsh.h"





/**
 * Call the relocation hook 
 * @param file
 * @param s
 * @param r
 * @param l
 * @param a
 * @param m
 * @return
 */
int		  elfsh_rel(elfshobj_t *file, elfshsect_t *s, elfsh_Rel *r, 
			    eresi_Addr *l, eresi_Addr a, elfshsect_t *m)
{
  u_char        archtype;
  u_char        elftype;
  u_char        ostype;
  int		ret;
  vector_t	*rel;
  u_int		dim[3];
  int		(*fct)(elfshsect_t *n,
		       elfsh_Rel  *n2,
		       eresi_Addr *n3,
		       eresi_Addr  n4,
		       elfshsect_t *n5);

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  rel = aspect_vector_get(ELFSH_HOOK_REL);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  elftype  = elfsh_get_elftype(file);
  ostype   = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      elftype  == ELFSH_FILE_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "RELOCATION handler unexistant for this ARCH/OS", -1);
  dim[0] = archtype;
  dim[1] = elftype;
  dim[2] = ostype;
  fct    = aspect_vectors_select(rel, dim);
  ret = fct(s, r, l, a, m);
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Relocation handler failed", (-1));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Call the relocation hook 
 * @param file
 * @param name
 * @param old
 * @param new
 * @return
 */
int             elfsh_cflow(elfshobj_t *file, char *name, elfsh_Sym *old, 
			    eresi_Addr new)
{
  vector_t	*cflow;
  u_char        archtype;
  u_char        elftype;
  u_char        ostype;
  int           ret;
  int		(*fct)(elfshobj_t *n, char *n2, elfsh_Sym *n3, eresi_Addr n4);
  u_int		dim[3];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  cflow = aspect_vector_get(ELFSH_HOOK_CFLOW);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  elftype = elfsh_get_elftype(file);
  ostype   = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      elftype == ELFSH_FILE_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "CFLOW handler unexistant for this ARCH/OS", -1);

  dim[0] = archtype;
  dim[1] = elftype;
  dim[2] = ostype;
  fct    = aspect_vectors_select(cflow, dim);
  ret = fct(file, name, old, new);
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Control flow redirection handler failed", (-1));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Call the PLT hook 
 * @param file
 * @param s
 * @param new
 * @return
 */
int             elfsh_plt(elfshobj_t *file, elfsh_Sym *s, eresi_Addr new)
{
  vector_t	*plt;
  u_char        archtype;
  u_char        elftype;
  u_char        ostype;
  int           ret;
  int		(*fct)(elfshobj_t *f, elfsh_Sym *s, eresi_Addr a);
  u_int		dim[3];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  plt = aspect_vector_get(ELFSH_HOOK_PLT);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  elftype = elfsh_get_elftype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      elftype  == ELFSH_FILE_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "PLT handler unexistant for this ARCH/OS", -1);

  dim[0] = archtype;
  dim[1] = elftype;
  dim[2] = ostype;
  fct    = aspect_vectors_select(plt, dim);
  ret    = fct(file, s, new);
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "PLT redirection handler failed", (-1));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}



/**
 * Call the ENCODEPLT hook 
 *
 * @param file
 * @param plt
 * @param diff
 * @param off
 * @return
 */
int             elfsh_encodeplt(elfshobj_t *file, elfshsect_t *plt, 
				eresi_Addr diff, u_int off)
{
  vector_t	*encodeplt;
  u_char        archtype;
  u_char        elftype;
  u_char        ostype;
  int           ret;
  int		(*fct)(elfshobj_t *f, elfshsect_t *s, eresi_Addr a, u_int off);
  u_int		dim[3];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  encodeplt = aspect_vector_get(ELFSH_HOOK_ENCODEPLT);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  elftype = elfsh_get_elftype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      elftype  == ELFSH_FILE_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "ENCODEPLT handler unexistant for this ARCH/OS", -1);

  dim[0] = archtype;
  dim[1] = elftype;
  dim[2] = ostype;
  fct    = aspect_vectors_select(encodeplt, dim);

  ret  = fct(file, plt, diff, off);
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "PLT encoding handler failed", (-1));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}



/** 
 * Call the ENCODEPLT1 hook 
 *
 * @param file
 * @param plt
 * @param extplt 
 * @param diff
 * @return
 */
int             elfsh_encodeplt1(elfshobj_t *file, elfshsect_t *plt, 
				 elfshsect_t *extplt, eresi_Addr diff)
{
  vector_t	*encodeplt1;
  u_char        archtype;
  u_char        elftype;
  u_char        ostype;
  int           ret;
  int		(*fct)(elfshobj_t *f, elfshsect_t *s, elfshsect_t *s2,
		       eresi_Addr a);
  u_int		dim[3];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  encodeplt1 = aspect_vector_get(ELFSH_HOOK_ENCODEPLT1);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  elftype = elfsh_get_elftype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      elftype  == ELFSH_FILE_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "ENCODEPLT1 handler unexistant for this ARCH/OS", -1);

  dim[0] = archtype;
  dim[1] = elftype;
  dim[2] = ostype;
  fct    = aspect_vectors_select(encodeplt1, dim);
  ret  = fct(file, plt, extplt, diff);
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "PLT1 encoding handler failed", (-1));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}



/**
 * Call the ALTPLT hook 
 *
 * @param file
 * @param s
 * @param new
 * @return
 */
int             elfsh_altplt(elfshobj_t *file, elfsh_Sym *s, eresi_Addr new)
{
  vector_t	*altplt;
  u_char        archtype;
  u_char        elftype;
  u_char        ostype;
  int           ret;
  int		(*fct)(elfshobj_t *file, elfsh_Sym *s, eresi_Addr a);
  u_int		dim[3];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  altplt = aspect_vector_get(ELFSH_HOOK_ALTPLT);
 
  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  elftype = elfsh_get_elftype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      elftype  == ELFSH_FILE_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "ALTPLT handler unexistant for this ARCH/OS", -1);

  dim[0] = archtype;
  dim[1] = elftype;
  dim[2] = ostype;
  fct    = aspect_vectors_select(altplt, dim);
  ret  = fct(file, s, new);
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "ALTPLT redirection handler failed", (-1));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/**
 * Call the EXTPLT hook 
 *
 * @param extplt
 * @param altgot
 * @param dynsym
 * @param relplt
 * @return
 */
int             elfsh_extplt(elfshsect_t *extplt, elfshsect_t *altgot, 
			     elfshsect_t *dynsym, elfshsect_t *relplt)
{
  vector_t	*vextplt;
  u_char        archtype;
  u_char        elftype;
  u_char        ostype;
  int           ret;
  int		(*fct)(elfshsect_t *extplt, elfshsect_t *altgot,
		       elfshsect_t *dynsym, elfshsect_t *relplt);
  u_int		dim[3];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  vextplt = aspect_vector_get(ELFSH_HOOK_EXTPLT);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(extplt->parent);
  elftype = elfsh_get_elftype(extplt->parent);
  ostype = elfsh_get_ostype(extplt->parent);
  if (archtype == ELFSH_ARCH_ERROR ||
      elftype  == ELFSH_FILE_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "EXTPLT handler unexistant for this ARCH/OS", -1);
  dim[0] = archtype;
  dim[1] = elftype;
  dim[2] = ostype;
  fct    = aspect_vectors_select(vextplt, dim);
  ret  = fct(extplt, altgot, dynsym, relplt);
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "EXTPLT redirection handler failed", (-1));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/**
 * Call the arg count hook 
 *
 * @param file
 * @param off
 * @param vaddr
 * @return
 */
int		  *elfsh_args_count(elfshobj_t *file, u_int off, eresi_Addr vaddr)
{
  vector_t	*argch;
  u_char        archtype;
  u_char        elftype;
  u_char        ostype;
  int		*(*fct)(elfshobj_t *file, u_int off, eresi_Addr vaddr);
  u_int		dim[3];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  argch = aspect_vector_get(ELFSH_HOOK_ARGC);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  elftype = elfsh_get_elftype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      elftype  == ELFSH_FILE_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "ARGC handler unexistant for this ARCH/OS", NULL);
  
  dim[0] = archtype;
  dim[1] = elftype;
  dim[2] = ostype;
  fct    = aspect_vectors_select(argch, dim);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, fct(file, off, vaddr));
}



/**
 * @brief Read the analysed object memory
 * @param sect Section to be analysed
 * @return size on success, -1 on error
 */
void		*elfsh_readmem(elfshsect_t *sect)
{
  void		*ret;
  u_int         dim[2];
  vector_t      *mem;
  void          *(*fct)();
  u_char        archtype;
  u_char        iotype;
  u_char        ostype;
  elfshobj_t	*file;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!sect || !sect->parent)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", NULL);
  file = sect->parent;
  
  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  iotype = elfsh_get_iotype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      ostype   == ELFSH_OS_ERROR   ||
      iotype   == ELFSH_IOTYPE_ERROR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "READMEM handler inexistant for this ARCH/OS", NULL);
  mem = aspect_vector_get(ELFSH_HOOK_READMEM);
  dim[0] = ostype;
  dim[1] = iotype;
  fct = aspect_vectors_select(mem, dim);
/*   printf("dim[0]: %d, dim[1]: %d, fct: %p\n", dim[0], dim[1], fct); */
/*   fflush(stdout); */
  ret = fct(sect);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/**
 * @brief Read the analysed object memory
 * @param file File to read data from.
 * @param addr Virtual Address in file to read data.
 * @return A pointer on the read data
 */
void		*elfsh_readmema(elfshobj_t *file, eresi_Addr addr, void *buf, u_int size)
{
  void		*ret;
  u_int         dim[4];
  vector_t      *mem;
  void          *(*fct)();
  u_char        archtype;
  u_char        iotype;
  u_char        ostype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!file || !addr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", NULL);
  
  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  iotype = elfsh_get_iotype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      ostype   == ELFSH_OS_ERROR   ||
      iotype   == ELFSH_IOTYPE_ERROR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "READMEM handler inexistant for this ARCH/OS", NULL);
  mem = aspect_vector_get(ELFSH_HOOK_READMEMA);
  dim[0] = archtype;
  dim[1] = ostype;
  dim[2] = iotype;
  fct = aspect_vectors_select(mem, dim);
  ret = fct(file, addr, buf, size);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/**
 * @brief Write into analyzed object memory
 * @param offset Offset to write memory
 * @param buf Write buf into memory
 * @param size Count bytes to write
 * @return Written size on success, -1 on error.
 */
int		elfsh_writemem(elfshobj_t *file, eresi_Addr addr, void *buf, u_int size)
{
  int		ret;
  u_int         dim[4];
  vector_t      *mem;
  int          (*fct)();
  u_char        archtype;
  u_char        iotype;
  u_char        ostype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!file || !addr || !buf || size <= 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid input parameters", -1);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  iotype = elfsh_get_iotype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      ostype   == ELFSH_OS_ERROR   ||
      iotype   == ELFSH_IOTYPE_ERROR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "READMEM handler inexistant for this ARCH/OS", NULL);
  mem = aspect_vector_get(ELFSH_HOOK_WRITEMEM);
  dim[0] = archtype;
  dim[1] = ostype;
  dim[2] = iotype;
  fct = aspect_vectors_select(mem, dim);
  ret = fct(addr, buf, size);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/**
 * @brief Write in raw memory at indicated file offset
 * @param file The elf object
 * @param foffset Offset to write memory
 * @param src_buff Write buf into memoryr
 * @param len Count bytes to write
 * @return len on success, -1 on error
 */
int		elfsh_writememf(elfshobj_t *file, u_int foffset, void *src_buff, int len)
{
  int		ret;
  u_int         dim[2];
  vector_t      *mem;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  mem = aspect_vector_get(ELFSH_HOOK_WRITEMEMF);
  dim[0] = elfsh_get_hosttype(file);
  dim[1] = elfsh_get_mode();
  fct = aspect_vectors_select(mem, dim);
  ret = fct(file, foffset, src_buff, len);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/**
 * @brief Read raw memory at indicated file offset
 * @param file The elf object
 * @param foffset Offset to read memory
 * @param dest_buff Read memory into the buf
 * @param len Count bytes to read
 * @return len on success, -1 on error
 */
int		elfsh_readmemf(elfshobj_t *file, u_int foffset, void *dest_buff, int len)
{
  int		ret;
  u_int         dim[2];
  vector_t      *mem;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  mem = aspect_vector_get(ELFSH_HOOK_READMEMF);
  dim[0] = elfsh_get_hosttype(file);
  dim[1] = elfsh_get_mode();
  fct = aspect_vectors_select(mem, dim);
  ret = fct(file, foffset, dest_buff, len);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}
