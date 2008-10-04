/**
 * @file hooks.c
 * @ingroup libelfsh
** hooks.c for libelfsh (The ELF shell library)
** 
** All the function pointers for all provided techniques.
** 
** This is using the libaspect vectors
** 
** Started Jan 11 2004 02:57:03 jfv
** 
**
** $Id: hooks.c,v 1.26 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libelfsh.h"

hash_t	interp_hash;


unsigned char	elfsh_ostype[5] = {
  ELFOSABI_LINUX,		
  ELFOSABI_FREEBSD,	
  ELFOSABI_NETBSD,		
  ELFOSABI_OPENBSD,	
  ELFOSABI_SOLARIS,	
};

								
/**
 * Default hooks handlers
 *
 * @param null
 * @param null2
 * @param null3
 * @return
 */
int	elfsh_default_plthandler(elfshobj_t *null, 
				 elfsh_Sym  *null2, 
				 eresi_Addr null3)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}

/**
 * Used for encoding a random PLT entry
 * Do nothing by default, this is not fatal 
 *
 * @param file
 * @param sect
 * @param diff
 * @return
 */
int	elfsh_default_encodeplthandler(elfshobj_t *file, 
				       elfshsect_t *sect, 
				       eresi_Addr diff)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}

/**
 * Used for encoding of the first PLT entry 
 *
 * @param file
 * @param sect
 * @param sect2
 * @return
 */
int	elfsh_default_encodeplt1handler(elfshobj_t *file, 
					elfshsect_t *sect,
					elfshsect_t *sect2,
					eresi_Addr diff)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}

/**
 *
 * @param null
 * @param null2
 * @param null3
 * @param null4
 * @param null5
 */
int	elfsh_default_relhandler(elfshsect_t *null, 
				 elfsh_Rel  * null2, 
				 eresi_Addr * null3, 
				 eresi_Addr   null4,
				 elfshsect_t *null5)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}

/**
 *
 * @param null
 * @param nulls
 * @param null2
 * @param null3
 * @return
 */
int	elfsh_default_cflowhandler(elfshobj_t   *null,
				   char		*nulls,
				   elfsh_Sym    *null2,
				   eresi_Addr	null3)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}


/**
 *
 * @param o
 * @param d
 * @param t
 * @param f
 * @return
 */
int	elfsh_default_extplthandler(elfshsect_t *o, elfshsect_t *d, 
				    elfshsect_t *t, elfshsect_t *f)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}

/**
 * Used on architectures where altplt hijack is not required, thus induce no fatal error
 * DO NOT use this as a default handler, unless you know exactly what you are doing
 *
 * @param null
 * @param null2
 * @param null3
 * @return
 */
int	elfsh_void_altplthandler(elfshobj_t *null, 
				 elfsh_Sym  *null2, 
				 eresi_Addr null3)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 *
 * @param addr
 * @return
 */
int	elfsh_default_argchandler(eresi_Addr addr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
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
int	elfsh_register_altplthook(unsigned char archtype, 
				  unsigned char objtype, 
				  unsigned char ostype, 
				  void	 *fct)
{
  vector_t	*altplt;
  unsigned int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  altplt = aspect_vector_get(ELFSH_HOOK_ALTPLT);

  if (archtype >= ELFSH_ARCHNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_TYPENUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);

  dim = alloca(sizeof(unsigned int) * 4);
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
int		elfsh_register_extplthook(unsigned char archtype, 
					  unsigned char objtype, 
					  unsigned char ostype, 
					  void	 *fct)
{
  vector_t	*extplt;
  unsigned int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  extplt = aspect_vector_get(ELFSH_HOOK_EXTPLT);
  if (archtype >= ELFSH_ARCHNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_TYPENUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);

  dim = alloca(sizeof(unsigned int) * 4);
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
int		elfsh_register_plthook(unsigned char archtype, unsigned char objtype, 
				       unsigned char ostype, void *fct)
{
  vector_t	*plt;
  unsigned int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  plt = aspect_vector_get(ELFSH_HOOK_PLT);
  if (archtype >= ELFSH_ARCHNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_TYPENUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);

  dim    = alloca(sizeof(unsigned int) * 4);
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
int	elfsh_register_encodeplthook(unsigned char archtype, unsigned char objtype, 
				     unsigned char ostype, void *fct)
{
  vector_t	*encodeplt;
  unsigned int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  encodeplt = aspect_vector_get(ELFSH_HOOK_ENCODEPLT);

  if (archtype >= ELFSH_ARCHNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_TYPENUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);

  dim = alloca(sizeof(unsigned int) * 4);
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
int	elfsh_register_encodeplt1hook(unsigned char archtype, unsigned char objtype, 
				      unsigned char ostype, void *fct)
{
  vector_t	*encodeplt1;
  unsigned int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  encodeplt1 = aspect_vector_get(ELFSH_HOOK_ENCODEPLT1);

  if (archtype >= ELFSH_ARCHNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_TYPENUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);
  dim = alloca(sizeof(unsigned int) * 4);
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
int	elfsh_register_relhook(unsigned char archtype, unsigned char objtype, unsigned char ostype,
			       void *fct)
{
  vector_t	*rel;
  unsigned int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  rel = aspect_vector_get(ELFSH_HOOK_REL);
  
  if (archtype >= ELFSH_ARCHNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_TYPENUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);

  dim = alloca(sizeof(unsigned int) * 4);
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
int	elfsh_register_cflowhook(unsigned char archtype, unsigned char objtype, 
				 unsigned char ostype, void *fct)
{
  vector_t	*cflow;
  unsigned int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  cflow = aspect_vector_get(ELFSH_HOOK_CFLOW);

  if (archtype >= ELFSH_ARCHNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_TYPENUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);

  dim = alloca(sizeof(unsigned int) * 4);
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
int	elfsh_register_argchook(unsigned char archtype, unsigned char objtype, 
				unsigned char ostype, void *fct)
{
  vector_t	*argcp;
  unsigned int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  
  argcp = aspect_vector_get(ELFSH_HOOK_ARGC);

  if (archtype >= ELFSH_ARCHNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_TYPENUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);

  dim = alloca(sizeof(unsigned int) * 4);
  dim[0] = archtype;
  dim[1] = objtype;
  dim[2] = ostype;
  aspect_vectors_insert(argcp, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/**
 * Initialize hook hash table 
 * @return
 */
int		elfsh_init_vectors()
{
  unsigned int		*dims;
  char		**strdims;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 
  aspect_init();

  /* All hooks have the same dimensions here */
  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 4 * sizeof(unsigned int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,strdims, 4 * sizeof(char *), -1);
  dims[0]    = ELFSH_ARCHNUM;
  dims[1]    = ELFSH_TYPENUM;
  dims[2]    = ELFSH_OSNUM;
  strdims[0] = "ARCHTYPE";
  strdims[1] = "ELFTYPE";
  strdims[2] = "OSTYPE";

  aspect_register_vector(ELFSH_HOOK_ALTPLT, 
			elfsh_default_plthandler,
			dims, strdims, 3, ASPECT_TYPE_CADDR);
  aspect_register_vector(ELFSH_HOOK_PLT, 
			elfsh_default_plthandler,
			dims, strdims, 3, ASPECT_TYPE_CADDR);
  aspect_register_vector(ELFSH_HOOK_REL, 
			elfsh_default_relhandler,
			dims, strdims, 3, ASPECT_TYPE_CADDR);
  aspect_register_vector(ELFSH_HOOK_CFLOW, 
			elfsh_default_cflowhandler, 
			dims, strdims, 3, ASPECT_TYPE_CADDR);
  aspect_register_vector(ELFSH_HOOK_EXTPLT, 
			elfsh_default_extplthandler, 
			dims, strdims, 3, ASPECT_TYPE_CADDR);  
  aspect_register_vector(ELFSH_HOOK_ENCODEPLT, 
			 elfsh_default_encodeplthandler, 
			 dims, strdims, 3, ASPECT_TYPE_CADDR);  
  aspect_register_vector(ELFSH_HOOK_ENCODEPLT1, 
			 elfsh_default_encodeplt1handler, 
			 dims, strdims, 3, ASPECT_TYPE_CADDR);
  aspect_register_vector(ELFSH_HOOK_ARGC, 
			 elfsh_default_argchandler, 
			 dims, strdims, 3, ASPECT_TYPE_CADDR);  

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Initialize ALTPLT hijack and ET_REL injection handlers in their hook 
 */
void	elfsh_setup_hooks()
{
  //unsigned int	i, j, k;
  static int done = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Check if already initialized */
  if (done)
    PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);

  elfsh_init_vectors();

  /***************************************/
  /****** PLT hijacking handlers *********/
  /**************************************/

  /* Usual PLT targets for ET_EXEC/i386 */
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_hijack_plt_ia32);
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_hijack_plt_ia32);
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_hijack_plt_ia32);
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_hijack_plt_ia32);
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_hijack_plt_ia32);
  
  /* Usual PLT targets for ET_EXEC/Sparc32 */
  elfsh_register_plthook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_hijack_plt_sparc32);
  elfsh_register_plthook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_hijack_plt_sparc32);
  elfsh_register_plthook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_hijack_plt_sparc32);
  elfsh_register_plthook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_hijack_plt_sparc32);
  elfsh_register_plthook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_hijack_plt_sparc32);

  /* Usual PLT targets for ET_EXEC/Sparc64 */
  elfsh_register_plthook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_hijack_plt_sparc64);
  elfsh_register_plthook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_hijack_plt_sparc64);
  elfsh_register_plthook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_hijack_plt_sparc64);
  elfsh_register_plthook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_hijack_plt_sparc64);
  elfsh_register_plthook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_hijack_plt_sparc64);

  /* Usual PLT targets for ET_EXEC/IA64 */
  elfsh_register_plthook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_hijack_plt_ia64);
  elfsh_register_plthook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_hijack_plt_ia64);
  elfsh_register_plthook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_hijack_plt_ia64);
  elfsh_register_plthook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_hijack_plt_ia64);
  elfsh_register_plthook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_hijack_plt_ia64);

  /* Usual PLT targets for ET_EXEC/Alpha64 */
  elfsh_register_plthook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_hijack_plt_alpha64);
  elfsh_register_plthook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_hijack_plt_alpha64);
  elfsh_register_plthook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_hijack_plt_alpha64);
  elfsh_register_plthook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_hijack_plt_alpha64);
  elfsh_register_plthook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_hijack_plt_alpha64);

  /* Usual PLT targets for ET_EXEC/MIPS32 */
  elfsh_register_plthook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_hijack_plt_mips32);
  elfsh_register_plthook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_hijack_plt_mips32);
  elfsh_register_plthook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_hijack_plt_mips32);
  elfsh_register_plthook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_hijack_plt_mips32);
  elfsh_register_plthook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_hijack_plt_mips32);

  /* Usual PLT targets for ET_EXEC/MIPS64 */
  elfsh_register_plthook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_hijack_plt_mips64);
  elfsh_register_plthook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_hijack_plt_mips64);
  elfsh_register_plthook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_hijack_plt_mips64);
  elfsh_register_plthook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_hijack_plt_mips64);
  elfsh_register_plthook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_hijack_plt_mips64);

  /***************************************/
  /****** ALTPLT hijacking handlers ******/
  /**************************************/

  /* Usual ALTPLT targets for ET_EXEC/i386 */
  elfsh_register_altplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_LINUX, elfsh_void_altplthandler);
  elfsh_register_altplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_FREEBSD, elfsh_void_altplthandler);
  elfsh_register_altplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_NETBSD, elfsh_void_altplthandler);
  elfsh_register_altplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_OPENBSD, elfsh_void_altplthandler);
  elfsh_register_altplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_SOLARIS, elfsh_void_altplthandler);
  
  /* Usual ALTPLT targets for ET_EXEC/Sparc32 */
  elfsh_register_altplthook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_LINUX, elfsh_hijack_altplt_sparc32);
  elfsh_register_altplthook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_FREEBSD, elfsh_hijack_altplt_sparc32);
  elfsh_register_altplthook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_NETBSD, elfsh_hijack_altplt_sparc32);
  elfsh_register_altplthook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_OPENBSD, elfsh_hijack_altplt_sparc32);
  elfsh_register_altplthook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_SOLARIS, elfsh_hijack_altplt_sparc32);
  
  /* Usual ALTPLT targets for ET_EXEC/Sparc64 */
  elfsh_register_altplthook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_LINUX, elfsh_hijack_altplt_sparc64);
  elfsh_register_altplthook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_FREEBSD, elfsh_hijack_altplt_sparc64);
  elfsh_register_altplthook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_NETBSD, elfsh_hijack_altplt_sparc64);
  elfsh_register_altplthook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_OPENBSD, elfsh_hijack_altplt_sparc64);
  elfsh_register_altplthook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_SOLARIS, elfsh_hijack_altplt_sparc64);
  
  /* Usual ALTPLT targets for ET_EXEC/IA64 */
  elfsh_register_altplthook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_LINUX, elfsh_hijack_altplt_ia64);
  elfsh_register_altplthook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_FREEBSD, elfsh_hijack_altplt_ia64);
  elfsh_register_altplthook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_NETBSD, elfsh_hijack_altplt_ia64);
  elfsh_register_altplthook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_OPENBSD, elfsh_hijack_altplt_ia64);
  elfsh_register_altplthook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_SOLARIS, elfsh_hijack_altplt_ia64);
  
  /* Usual ALTPLT targets for ET_EXEC/Alpha64 */
  elfsh_register_altplthook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_LINUX, elfsh_hijack_altplt_alpha64);
  elfsh_register_altplthook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_FREEBSD, elfsh_hijack_altplt_alpha64);
  elfsh_register_altplthook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_NETBSD, elfsh_hijack_altplt_alpha64);
  elfsh_register_altplthook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_OPENBSD, elfsh_hijack_altplt_alpha64);
  elfsh_register_altplthook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_SOLARIS, elfsh_hijack_altplt_alpha64);
  
  /* Usual ALTPLT targets for ET_EXEC/MIPS32 */
  elfsh_register_altplthook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_LINUX, elfsh_hijack_altplt_mips32);
  elfsh_register_altplthook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_FREEBSD, elfsh_hijack_altplt_mips32);
  elfsh_register_altplthook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_NETBSD, elfsh_hijack_altplt_mips32);
  elfsh_register_altplthook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_OPENBSD, elfsh_hijack_altplt_mips32);
  elfsh_register_altplthook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_SOLARIS, elfsh_hijack_altplt_mips32);
  
  /* Usual ALTPLT targets for ET_EXEC/MIPS64 */
  elfsh_register_altplthook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_LINUX, elfsh_hijack_altplt_mips64);
  elfsh_register_altplthook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_FREEBSD, elfsh_hijack_altplt_mips64);
  elfsh_register_altplthook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_NETBSD, elfsh_hijack_altplt_mips64);
  elfsh_register_altplthook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_OPENBSD, elfsh_hijack_altplt_mips64);
  elfsh_register_altplthook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_SOLARIS, elfsh_hijack_altplt_mips64);
  
  /***********************************/
  /* Now register Relocation hooks */
  /***********************************/

  
  /* Usual REL targets for ET_EXEC/i386 */
  elfsh_register_relhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_relocate_ia32);
  elfsh_register_relhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_relocate_ia32);
  elfsh_register_relhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_relocate_ia32);
  elfsh_register_relhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_relocate_ia32);
  elfsh_register_relhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_relocate_ia32);
  
  /* Usual REL targets for ET_EXEC/Sparc32 */
  elfsh_register_relhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_relocate_sparc32);
  elfsh_register_relhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_relocate_sparc32);
  elfsh_register_relhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_relocate_sparc32);
  elfsh_register_relhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_relocate_sparc32);
  elfsh_register_relhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_relocate_sparc32);

  /* Usual REL targets for ET_EXEC/Sparc64 */
  elfsh_register_relhook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_relocate_sparc64);
  elfsh_register_relhook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_relocate_sparc64);
  elfsh_register_relhook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_relocate_sparc64);
  elfsh_register_relhook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_relocate_sparc64);
  elfsh_register_relhook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_relocate_sparc64);

  /* Usual REL targets for ET_EXEC/IA64 */
  elfsh_register_relhook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_relocate_ia64);
  elfsh_register_relhook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_relocate_ia64);
  elfsh_register_relhook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_relocate_ia64);
  elfsh_register_relhook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_relocate_ia64);
  elfsh_register_relhook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_relocate_ia64);

  /* Usual REL targets for ET_EXEC/Alpha64 */
  elfsh_register_relhook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_relocate_alpha64);
  elfsh_register_relhook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_relocate_alpha64);
  elfsh_register_relhook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_relocate_alpha64);
  elfsh_register_relhook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_relocate_alpha64);
  elfsh_register_relhook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_relocate_alpha64);

  /* Usual REL targets for ET_EXEC/MIPS32 */
  elfsh_register_relhook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_relocate_mips32);
  elfsh_register_relhook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_relocate_mips32);
  elfsh_register_relhook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_relocate_mips32);
  elfsh_register_relhook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_relocate_mips32);
  elfsh_register_relhook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_relocate_mips32);

  /* Usual REL targets for ET_EXEC/MIPS64 */
  elfsh_register_relhook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_relocate_mips64);
  elfsh_register_relhook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_relocate_mips64);
  elfsh_register_relhook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_relocate_mips64);
  elfsh_register_relhook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_relocate_mips64);
  elfsh_register_relhook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_relocate_mips64);
  
  /***********************************/
  /* Now register Control flow hooks */
  /***********************************/
  
  /* Usual REL targets for ET_EXEC/IA32 */
  elfsh_register_cflowhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_LINUX, elfsh_cflow_ia32);
  elfsh_register_cflowhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_FREEBSD, elfsh_cflow_ia32);
  elfsh_register_cflowhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_NETBSD, elfsh_cflow_ia32);
  elfsh_register_cflowhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_OPENBSD, elfsh_cflow_ia32);
  elfsh_register_cflowhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_SOLARIS, elfsh_cflow_ia32);

  /* Usual CFLOW targets for ET_EXEC/IA64 */
  elfsh_register_cflowhook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_LINUX, elfsh_cflow_ia64);
  elfsh_register_cflowhook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_FREEBSD, elfsh_cflow_ia64);
  elfsh_register_cflowhook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_NETBSD, elfsh_cflow_ia64);
  elfsh_register_cflowhook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_OPENBSD, elfsh_cflow_ia64);
  elfsh_register_cflowhook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_SOLARIS, elfsh_cflow_ia64);

  /* Usual CFLOW targets for ET_EXEC/SPARC32 */
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_LINUX, elfsh_cflow_sparc32);
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_FREEBSD, elfsh_cflow_sparc32);
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_NETBSD, elfsh_cflow_sparc32);
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_OPENBSD, elfsh_cflow_sparc32);
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_SOLARIS, elfsh_cflow_sparc32);

  /* Usual CFLOW targets for ET_EXEC/SPARC64 */
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_LINUX, elfsh_cflow_sparc64);
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_FREEBSD, elfsh_cflow_sparc64);
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_NETBSD, elfsh_cflow_sparc64);
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_OPENBSD, elfsh_cflow_sparc64);
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_SOLARIS, elfsh_cflow_sparc64);

  /* Usual CFLOW targets for ET_EXEC/ALPHA64 */
  elfsh_register_cflowhook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_LINUX, elfsh_cflow_alpha64);
  elfsh_register_cflowhook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_FREEBSD, elfsh_cflow_alpha64);
  elfsh_register_cflowhook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_NETBSD, elfsh_cflow_alpha64);
  elfsh_register_cflowhook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_OPENBSD, elfsh_cflow_alpha64);
  elfsh_register_cflowhook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_SOLARIS, elfsh_cflow_alpha64);

  /* Usual CFLOW targets for ET_EXEC/MIPS32 */
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_LINUX, elfsh_cflow_mips32);
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_FREEBSD, elfsh_cflow_mips32);
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_NETBSD, elfsh_cflow_mips32);
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_OPENBSD, elfsh_cflow_mips32);
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_SOLARIS, elfsh_cflow_mips32);

  /* Usual CFLOW targets for ET_EXEC/MIPS64 */
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_LINUX, elfsh_cflow_mips64);
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_FREEBSD, elfsh_cflow_mips64);
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_NETBSD, elfsh_cflow_mips64);
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_OPENBSD, elfsh_cflow_mips64);
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_SOLARIS, elfsh_cflow_mips64);
  elfsh_register_cflowhook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_LINUX, elfsh_cflow_ia32);

  /* Usual REL/PLT/ALTPLT targets for ET_DYN/x86 */
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			 ELFSH_OS_BEOS, elfsh_hijack_plt_ia32);
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			 ELFSH_OS_LINUX, elfsh_hijack_plt_ia32);
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			 ELFSH_OS_FREEBSD, elfsh_hijack_plt_ia32);
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			 ELFSH_OS_NETBSD, elfsh_hijack_plt_ia32);
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			 ELFSH_OS_SOLARIS, elfsh_hijack_plt_ia32);
  elfsh_register_relhook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN,
			 ELFSH_OS_BEOS, elfsh_relocate_ia32);
  elfsh_register_altplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_LINUX, elfsh_void_altplthandler);
  elfsh_register_relhook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			 ELFSH_OS_LINUX, elfsh_relocate_ia32);
  elfsh_register_altplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_BEOS, elfsh_void_altplthandler);  

  /***********************************/
  /* Now register encodeplt points hooks */
  /***********************************/
  
  /* Usual ENCODEPLT breakpoint targets for ET_EXEC/IA32 */
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			       ELFSH_OS_LINUX, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			       ELFSH_OS_FREEBSD, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			       ELFSH_OS_NETBSD, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			       ELFSH_OS_OPENBSD, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			       ELFSH_OS_SOLARIS, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			       ELFSH_OS_LINUX, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			       ELFSH_OS_FREEBSD, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			       ELFSH_OS_NETBSD, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			       ELFSH_OS_OPENBSD, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			       ELFSH_OS_SOLARIS, elfsh_encodeplt_ia32);

  /* Usual ENCODEPLT1 targets for ET_EXEC/IA32 */
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
				ELFSH_OS_LINUX, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
				ELFSH_OS_FREEBSD, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
				ELFSH_OS_NETBSD, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
				ELFSH_OS_OPENBSD, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
				ELFSH_OS_SOLARIS, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
				ELFSH_OS_LINUX, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
				ELFSH_OS_FREEBSD, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
				ELFSH_OS_NETBSD, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
				ELFSH_OS_OPENBSD, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
				ELFSH_OS_SOLARIS, elfsh_encodeplt1_ia32);

  /*** Now EXTPLT handlers ***/

  /* Usual EXTPLT targets for ET_EXEC/IA32 */
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_LINUX, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_FREEBSD, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_NETBSD, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_OPENBSD, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_SOLARIS, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_LINUX, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_FREEBSD, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_NETBSD, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_OPENBSD, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_SOLARIS, elfsh_extplt_ia32);

  /***************************************/
  /****** ARGC arguments counting  ******/
  /**************************************/

  /* Usual ARGC targets for ET_EXEC/i386 */
  elfsh_register_argchook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			  ELFSH_OS_LINUX, elfsh_args_count_ia32);
  elfsh_register_argchook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			  ELFSH_OS_FREEBSD, elfsh_args_count_ia32);
  elfsh_register_argchook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			  ELFSH_OS_NETBSD, elfsh_args_count_ia32);
  elfsh_register_argchook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			  ELFSH_OS_OPENBSD, elfsh_args_count_ia32);
  elfsh_register_argchook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			  ELFSH_OS_SOLARIS, elfsh_args_count_ia32);
  
  done++;
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


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
  unsigned char        archtype;
  unsigned char        elftype;
  unsigned char        ostype;
  int		ret;
  vector_t	*rel;

  //eresi_Addr	***hook;
  unsigned int		dim[3];

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
      elftype  == ELFSH_TYPE_ERROR ||
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
  unsigned char        archtype;
  unsigned char        elftype;
  unsigned char        ostype;
  int           ret;
  int		(*fct)(elfshobj_t *n, char *n2, elfsh_Sym *n3, eresi_Addr n4);
  unsigned int		dim[3];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  cflow = aspect_vector_get(ELFSH_HOOK_CFLOW);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  elftype = elfsh_get_elftype(file);
  ostype   = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      elftype == ELFSH_TYPE_ERROR ||
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
  unsigned char        archtype;
  unsigned char        elftype;
  unsigned char        ostype;
  int           ret;
  int		(*fct)(elfshobj_t *f, elfsh_Sym *s, eresi_Addr a);
  unsigned int		dim[3];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  plt = aspect_vector_get(ELFSH_HOOK_PLT);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  elftype = elfsh_get_elftype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      elftype  == ELFSH_TYPE_ERROR ||
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
				eresi_Addr diff, unsigned int off)
{
  vector_t	*encodeplt;
  unsigned char        archtype;
  unsigned char        elftype;
  unsigned char        ostype;
  int           ret;
  int		(*fct)(elfshobj_t *f, elfshsect_t *s, eresi_Addr a, unsigned int off);
  unsigned int		dim[3];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  encodeplt = aspect_vector_get(ELFSH_HOOK_ENCODEPLT);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  elftype = elfsh_get_elftype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      elftype  == ELFSH_TYPE_ERROR ||
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
  unsigned char        archtype;
  unsigned char        elftype;
  unsigned char        ostype;
  int           ret;
  int		(*fct)(elfshobj_t *f, elfshsect_t *s, elfshsect_t *s2,
		       eresi_Addr a);
  unsigned int		dim[3];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  encodeplt1 = aspect_vector_get(ELFSH_HOOK_ENCODEPLT1);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  elftype = elfsh_get_elftype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      elftype  == ELFSH_TYPE_ERROR ||
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
  unsigned char        archtype;
  unsigned char        elftype;
  unsigned char        ostype;
  int           ret;
  int		(*fct)(elfshobj_t *file, elfsh_Sym *s, eresi_Addr a);
  unsigned int		dim[3];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  altplt = aspect_vector_get(ELFSH_HOOK_ALTPLT);
 
  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  elftype = elfsh_get_elftype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      elftype  == ELFSH_TYPE_ERROR ||
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
  unsigned char        archtype;
  unsigned char        elftype;
  unsigned char        ostype;
  int           ret;
  int		(*fct)(elfshsect_t *extplt, elfshsect_t *altgot,
		       elfshsect_t *dynsym, elfshsect_t *relplt);
  unsigned int		dim[3];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  vextplt = aspect_vector_get(ELFSH_HOOK_EXTPLT);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(extplt->parent);
  elftype = elfsh_get_elftype(extplt->parent);
  ostype = elfsh_get_ostype(extplt->parent);
  if (archtype == ELFSH_ARCH_ERROR ||
      elftype  == ELFSH_TYPE_ERROR ||
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
int		  *elfsh_args_count(elfshobj_t *file, unsigned int off, eresi_Addr vaddr)
{
  vector_t	*argch;
  unsigned char        archtype;
  unsigned char        elftype;
  unsigned char        ostype;
  int		*(*fct)(elfshobj_t *file, unsigned int off, eresi_Addr vaddr);
  unsigned int		dim[3];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  argch = aspect_vector_get(ELFSH_HOOK_ARGC);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  elftype = elfsh_get_elftype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      elftype  == ELFSH_TYPE_ERROR ||
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
 * Translate ELF architecture type into ELFsh architecture type 
 *
 * @param file
 * @return
 */
unsigned char		elfsh_get_archtype(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
   PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
    "Invalid file argument!", (ELFSH_ARCH_ERROR));

  switch (elfsh_get_arch(file->hdr))
    {
    case EM_386:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_IA32));
    case EM_SPARC:
    case EM_SPARC32PLUS:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_SPARC32));
    case EM_SPARCV9:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_SPARC64));
    case EM_PARISC:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_PARISC));
    case EM_IA_64:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_IA64));
    case EM_PPC:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_PPC32));
    case EM_PPC64:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_PPC64));
    case EM_SVX:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_CRAY));
    case EM_ALPHA:
#if EM_ALPHA != EM_ALPHA_EXP
    case EM_ALPHA_EXP:
#endif
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_ALPHA64));
    case EM_MIPS:
    case EM_MIPS_RS3_LE:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_MIPS32));
    case EM_ARM:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_ARM));
    case EM_X86_64:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_AMD64));
    default:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_ERROR));
    }
}


/**
 * Translate ELF object type into ELFsh object type 
 * @param file
 * @return
 */
unsigned char		elfsh_get_elftype(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (elfsh_get_objtype(file->hdr))
    {
    case ET_EXEC:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_TYPE_EXEC));
    case ET_DYN:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_TYPE_DYN));
    default:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_TYPE_ERROR));
    }
}

      
/**
 * Retreive ELFsh OS type from ELF header 
 * @param file
 * @return
 */
unsigned char  elfsh_get_real_ostype(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (file->hdr->e_ident[EI_OSABI])
    {
    case ELFOSABI_LINUX:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_OS_LINUX));
    case ELFOSABI_FREEBSD:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_OS_FREEBSD));
    case ELFOSABI_NETBSD:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_OS_NETBSD));
    case ELFOSABI_OPENBSD:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_OS_OPENBSD));
    case ELFOSABI_SOLARIS:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_OS_SOLARIS));
    case 0:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    default:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_OS_ERROR));
    }
}


/**
 * For now, always return the type of a userland process
 * Need to be extended for the kernel memory type 
 *
 * @param file
 * @return
 */
unsigned char		elfsh_get_hosttype(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (E2DBG_HOST_PROC));
}


/**
 * Fill up ELFsh Operating System type 
 *
 * @param file
 * @return
 */
unsigned char		elfsh_get_ostype(elfshobj_t *file)
{
  unsigned char	*res;
  char		*interp;
  char		*end;
  char		r;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* First try */
  r = elfsh_get_real_ostype(file);
  switch (r)
    {
      /* Information not present */
    case 0:
      break;
    default:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (r));
    }
  
  /* Smart intermediate way, fingerprint by .interp section */
  /* Not aware of any other software using this technique */
  if (elfsh_get_interp(file))
    {
      if (!interp_hash.size)
	{
	  hash_init(&interp_hash, "interpreters", 10, ASPECT_TYPE_STR);
	  hash_add(&interp_hash, 
		   "/lib/ld-linux.so", 
		   &elfsh_ostype[ELFSH_OS_LINUX]);
	  hash_add(&interp_hash, 
		   "/usr/libexec/ld-elf.so", 
		   &elfsh_ostype[ELFSH_OS_FREEBSD]);
	  hash_add(&interp_hash, 
		   "/usr/libexec/ld-eld_so", 
		   &elfsh_ostype[ELFSH_OS_NETBSD]);
	  hash_add(&interp_hash, 
		   "??????????????", 
		   &elfsh_ostype[ELFSH_OS_OPENBSD]);
	  hash_add(&interp_hash, 
		   "/usr/lib/ld.so", 
		   &elfsh_ostype[ELFSH_OS_SOLARIS]);
	  
	}


      /* Only useful for objects with .interp section */
      /* e.g. : ET_EXEC, and some special ET_DYN like libc */
      interp = (char *) elfsh_get_raw(file->secthash[ELFSH_SECTION_INTERP]);
      end = strstr(".so", interp);
      if (end)
	*(end + 3) = 0x00;
      res  = hash_get(&interp_hash, interp);
      if (res)
	{
	  file->hdr->e_ident[EI_OSABI] = *res;
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			     (elfsh_get_real_ostype(file)));
	}
    }

  /* Fatalist */
#if defined(__linux__)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_LINUX);
#elif defined(__FreeBSD__)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_FREEBSD);
#elif defined(__NetBSD__)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_NETBSD);
#elif defined(__OpenBSD__)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_OPENBSD);
#elif defined(sun)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_SOLARIS);
#elif defined(__BEOS__)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_BEOS);
#else
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_ERROR);
#endif
}

/**
 * Return the page size 
 *
 * @param file
 * @return
 */
int       elfsh_get_pagesize(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (elfsh_get_archtype(file))
    {
    case ELFSH_ARCH_IA32:
    case ELFSH_ARCH_MIPS32:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 4096);
    case ELFSH_ARCH_SPARC32:
    case ELFSH_ARCH_SPARC64:
    case ELFSH_ARCH_PARISC:
    case ELFSH_ARCH_IA64:
    case ELFSH_ARCH_ALPHA64:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 8192);
    case ELFSH_ARCH_PPC32:
    case ELFSH_ARCH_PPC64:
    case ELFSH_ARCH_CRAY:
    case ELFSH_ARCH_ERROR:
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unsupported architecture", 0);
    }  
}

/**
 * Return the page size 
 *
 * @param file
 * @return
 */
unsigned int       elfsh_get_breaksize(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  switch (elfsh_get_archtype(file))
    {
    case ELFSH_ARCH_IA32:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
    case ELFSH_ARCH_MIPS32:
    case ELFSH_ARCH_SPARC32:
    case ELFSH_ARCH_SPARC64:
    case ELFSH_ARCH_PARISC:
    case ELFSH_ARCH_ALPHA64:
    case ELFSH_ARCH_PPC32:
    case ELFSH_ARCH_PPC64:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 4);
    case ELFSH_ARCH_IA64:
    case ELFSH_ARCH_CRAY:
    case ELFSH_ARCH_ERROR:
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unsupported architecture", 0);
    }  
}

