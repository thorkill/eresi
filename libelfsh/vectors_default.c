/**
* @file libelfsh/vectors_default.c
** @ingroup libelfsh
**
** vectors_default.c for libelfsh (The ELF shell library)
** 
** Default handlers for elfsh hooked functionalities
**
** $Id$$
**
*/
#include "libelfsh.h"


void *elfsh_default_readmem(elfshsect_t *sect)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "Unsupported Arch, ELF type, or OS", NULL);
}

int elfsh_default_readmema(elfshobj_t *obj, eresi_Addr addr, void *buf, u_int size)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "Unsupported Arch, ELF type, or OS", -1);
}

int elfsh_default_writemem(elfshobj_t *file, eresi_Addr addr, void *buff, u_int size)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "Unsupported Arch, ELF type, or OS", -1);
}


int elfsh_default_writememf(elfshobj_t *file, u_int off, char *buff, int len)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "Unsupported Arch, ELF type, or OS", -1);
}

int elfsh_default_readmemf(elfshobj_t *file, u_int off, char *buff, int len)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "Unsupported Arch, ELF type, or OS", -1);
}

								
/**
 * @brief Default hooks handlers
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
 * @brief Default PLT encoding handler.
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
 * @brief Default first entry PLT encoding handler.
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
 * @brief Default relocation handler.
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
 * @brief Default control flow redirection handler.
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
 * @brief Default EXTPLT handler.
 */
int	elfsh_default_extplthandler(elfshsect_t *o, elfshsect_t *d, 
				    elfshsect_t *t, elfshsect_t *f)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}

/**
 * @brief Default ALTPLT handler.
 * Used on architectures where altplt hijack is not required, thus induces no fatal error
 * DO NOT use this as a default handler, unless you know exactly what you are doing
 */
int	elfsh_void_altplthandler(elfshobj_t *null, 
				 elfsh_Sym  *null2, 
				 eresi_Addr null3)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Default MPROTECT handler
 * Used in kernel context where mprotect is not required, thus induces no fatal error
 * DO NOT use this as a default handler, unless you know exactly what you are doing
 */
int	elfsh_void_mprotecthandler(elfshobj_t *obj, eresi_Addr addr, uint32_t sz, int prot)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Default MUNPROTECT handler.
 * Used on architectures where altplt hijack is not required, thus induces no fatal error
 * DO NOT use this as a default handler, unless you know exactly what you are doing
 */
int	elfsh_void_munprotecthandler(elfshobj_t *obj, eresi_Addr addr, uint32_t sz)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Default argument counting handler.
 */
int	elfsh_default_argchandler(eresi_Addr addr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}

/**
 * @brief Default runtime memory mapping handler.
 */
eresi_Addr elfsh_default_rmaphandler(elfshobj_t *file, size_t size, int prot)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "Unsupported Arch, ELF type, or OS", ELFSH_INVALID_ADDR);
}

