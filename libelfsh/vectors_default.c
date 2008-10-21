/**
** @file vectors_default.c
** @ingroup libelfsh
**
** vectors_default.c for libelfsh (The ELF shell library)
** 
** Default handlers for elfsh hooked functionalities
**
** $Id$
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


