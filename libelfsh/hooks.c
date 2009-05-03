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
** $Id$
**
*/
#include "libelfsh.h"



/**
 * Initialize hook hash table
 * @return
 */
int		elfsh_init_vectors()
{
  u_int		*dims;
  char		**strdims;
  u_int		*tdims;
  char		**tstrdims;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  aspect_init();

  /* All hooks have the same dimensions here */
  XALLOC(__FILE__, __FUNCTION__, __LINE__, dims    , 4 * sizeof(u_int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, strdims , 4 * sizeof(char *), -1);

  dims[0]     = ELFSH_ARCH_NUM;
  dims[1]     = ELFSH_FILE_NUM;
  dims[2]     = ELFSH_OS_NUM;
  strdims[0]  = "ARCHTYPE";
  strdims[1]  = "ELFTYPE";
  strdims[2]  = "OSTYPE";

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

  /* Now register IO vectors */
  XALLOC(__FILE__, __FUNCTION__, __LINE__, tdims   , 3 * sizeof(u_int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, tstrdims, 3 * sizeof(char *), -1);
  tdims[0]    = ELFSH_OS_NUM;
  tdims[1]    = ELFSH_IOTYPE_NUM;
  tstrdims[0] = "OSTYPE";
  tstrdims[1] = "IOTYPE";

  /* Put our default handlers for userland */
  aspect_register_vector(ELFSH_HOOK_READMEM, elfsh_default_readmem,
			 tdims, tstrdims, 2, ASPECT_TYPE_CADDR);
  aspect_register_vector(ELFSH_HOOK_WRITEMEM, elfsh_default_writemem,
			 tdims, tstrdims, 2, ASPECT_TYPE_CADDR);
  aspect_register_vector(ELFSH_HOOK_READMEMA, elfsh_default_readmema,
			 tdims, tstrdims, 2, ASPECT_TYPE_CADDR);

  XALLOC(__FILE__, __FUNCTION__, __LINE__, dims    , 3 * sizeof(u_int), -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, strdims , 3 * sizeof(char *), -1);
  dims[0]     = ELFSH_HOST_NUM;
  dims[1]     = LIBELFSH_MODE_NUM;
  strdims[0]  = "HOSTTYPE";
  strdims[1]  = "EXECMODE";

  aspect_register_vector(ELFSH_HOOK_READMEMF, elfsh_default_readmemf,
			 dims, strdims, 2, ASPECT_TYPE_CADDR);
  aspect_register_vector(ELFSH_HOOK_WRITEMEMF, elfsh_default_writememf,
			 dims, strdims, 2, ASPECT_TYPE_CADDR);

  /* Now the allocation, mprotect and munprotect vectors */
  XALLOC(__FILE__, __FUNCTION__, __LINE__, dims    , 2 * sizeof(u_int), -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, strdims , 2 * sizeof(char *), -1);
  dims[0]     = ELFSH_HOST_NUM;
  strdims[0]  = "HOSTTYPE";

  aspect_register_vector(ELFSH_HOOK_ALLOC, elfsh_default_rmaphandler,
			 dims, strdims, 1, ASPECT_TYPE_CADDR);
  aspect_register_vector(ELFSH_HOOK_MPROTECT, elfsh_void_mprotecthandler,
			 dims, strdims, 1, ASPECT_TYPE_CADDR);
  aspect_register_vector(ELFSH_HOOK_MUNPROTECT, elfsh_void_munprotecthandler,
			 dims, strdims, 1, ASPECT_TYPE_CADDR);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * @brief Initialize all vectors of libelfsh
 */
void		elfsh_setup_hooks()
{
  static int	done = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Check if already initialized */
  if (done)
    PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);

  elfsh_init_vectors();

  /***************************************/
  /****** PLT hijacking handlers *********/
  /**************************************/

  /* Usual PLT targets for ET_EXEC/i386 */
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			 ELFSH_OS_LINUX, elfsh_hijack_plt_ia32);
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			 ELFSH_OS_FREEBSD, elfsh_hijack_plt_ia32);
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			 ELFSH_OS_NETBSD, elfsh_hijack_plt_ia32);
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			 ELFSH_OS_OPENBSD, elfsh_hijack_plt_ia32);
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			 ELFSH_OS_SOLARIS, elfsh_hijack_plt_ia32);

  /* Usual PLT targets for ET_EXEC/Sparc32 */
  elfsh_register_plthook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC,
			 ELFSH_OS_LINUX, elfsh_hijack_plt_sparc32);
  elfsh_register_plthook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC,
			 ELFSH_OS_FREEBSD, elfsh_hijack_plt_sparc32);
  elfsh_register_plthook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC,
			 ELFSH_OS_NETBSD, elfsh_hijack_plt_sparc32);
  elfsh_register_plthook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC,
			 ELFSH_OS_OPENBSD, elfsh_hijack_plt_sparc32);
  elfsh_register_plthook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC,
			 ELFSH_OS_SOLARIS, elfsh_hijack_plt_sparc32);

  /* Usual PLT targets for ET_EXEC/Sparc64 */
  elfsh_register_plthook(ELFSH_ARCH_SPARC64, ELFSH_FILE_EXEC,
			 ELFSH_OS_LINUX, elfsh_hijack_plt_sparc64);
  elfsh_register_plthook(ELFSH_ARCH_SPARC64, ELFSH_FILE_EXEC,
			 ELFSH_OS_FREEBSD, elfsh_hijack_plt_sparc64);
  elfsh_register_plthook(ELFSH_ARCH_SPARC64, ELFSH_FILE_EXEC,
			 ELFSH_OS_NETBSD, elfsh_hijack_plt_sparc64);
  elfsh_register_plthook(ELFSH_ARCH_SPARC64, ELFSH_FILE_EXEC,
			 ELFSH_OS_OPENBSD, elfsh_hijack_plt_sparc64);
  elfsh_register_plthook(ELFSH_ARCH_SPARC64, ELFSH_FILE_EXEC,
			 ELFSH_OS_SOLARIS, elfsh_hijack_plt_sparc64);

  /* Usual PLT targets for ET_EXEC/IA64 */
  elfsh_register_plthook(ELFSH_ARCH_IA64, ELFSH_FILE_EXEC,
			 ELFSH_OS_LINUX, elfsh_hijack_plt_ia64);
  elfsh_register_plthook(ELFSH_ARCH_IA64, ELFSH_FILE_EXEC,
			 ELFSH_OS_FREEBSD, elfsh_hijack_plt_ia64);
  elfsh_register_plthook(ELFSH_ARCH_IA64, ELFSH_FILE_EXEC,
			 ELFSH_OS_NETBSD, elfsh_hijack_plt_ia64);
  elfsh_register_plthook(ELFSH_ARCH_IA64, ELFSH_FILE_EXEC,
			 ELFSH_OS_OPENBSD, elfsh_hijack_plt_ia64);
  elfsh_register_plthook(ELFSH_ARCH_IA64, ELFSH_FILE_EXEC,
			 ELFSH_OS_SOLARIS, elfsh_hijack_plt_ia64);

  /* Usual PLT targets for ET_EXEC/Alpha64 */
  elfsh_register_plthook(ELFSH_ARCH_ALPHA64, ELFSH_FILE_EXEC,
			 ELFSH_OS_LINUX, elfsh_hijack_plt_alpha64);
  elfsh_register_plthook(ELFSH_ARCH_ALPHA64, ELFSH_FILE_EXEC,
			 ELFSH_OS_FREEBSD, elfsh_hijack_plt_alpha64);
  elfsh_register_plthook(ELFSH_ARCH_ALPHA64, ELFSH_FILE_EXEC,
			 ELFSH_OS_NETBSD, elfsh_hijack_plt_alpha64);
  elfsh_register_plthook(ELFSH_ARCH_ALPHA64, ELFSH_FILE_EXEC,
			 ELFSH_OS_OPENBSD, elfsh_hijack_plt_alpha64);
  elfsh_register_plthook(ELFSH_ARCH_ALPHA64, ELFSH_FILE_EXEC,
			 ELFSH_OS_SOLARIS, elfsh_hijack_plt_alpha64);

  /* Usual PLT targets for ET_EXEC/MIPS32 */
  elfsh_register_plthook(ELFSH_ARCH_MIPS32, ELFSH_FILE_EXEC,
			 ELFSH_OS_LINUX, elfsh_hijack_plt_mips32);
  elfsh_register_plthook(ELFSH_ARCH_MIPS32, ELFSH_FILE_EXEC,
			 ELFSH_OS_FREEBSD, elfsh_hijack_plt_mips32);
  elfsh_register_plthook(ELFSH_ARCH_MIPS32, ELFSH_FILE_EXEC,
			 ELFSH_OS_NETBSD, elfsh_hijack_plt_mips32);
  elfsh_register_plthook(ELFSH_ARCH_MIPS32, ELFSH_FILE_EXEC,
			 ELFSH_OS_OPENBSD, elfsh_hijack_plt_mips32);
  elfsh_register_plthook(ELFSH_ARCH_MIPS32, ELFSH_FILE_EXEC,
			 ELFSH_OS_SOLARIS, elfsh_hijack_plt_mips32);

  /* Usual PLT targets for ET_EXEC/MIPS64 */
  elfsh_register_plthook(ELFSH_ARCH_MIPS64, ELFSH_FILE_EXEC,
			 ELFSH_OS_LINUX, elfsh_hijack_plt_mips64);
  elfsh_register_plthook(ELFSH_ARCH_MIPS64, ELFSH_FILE_EXEC,
			 ELFSH_OS_FREEBSD, elfsh_hijack_plt_mips64);
  elfsh_register_plthook(ELFSH_ARCH_MIPS64, ELFSH_FILE_EXEC,
			 ELFSH_OS_NETBSD, elfsh_hijack_plt_mips64);
  elfsh_register_plthook(ELFSH_ARCH_MIPS64, ELFSH_FILE_EXEC,
			 ELFSH_OS_OPENBSD, elfsh_hijack_plt_mips64);
  elfsh_register_plthook(ELFSH_ARCH_MIPS64, ELFSH_FILE_EXEC,
			 ELFSH_OS_SOLARIS, elfsh_hijack_plt_mips64);

  /***************************************/
  /****** ALTPLT hijacking handlers ******/
  /**************************************/

  /* Usual ALTPLT targets for ET_EXEC/i386 */
  elfsh_register_altplthook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			    ELFSH_OS_LINUX, elfsh_void_altplthandler);
  elfsh_register_altplthook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			    ELFSH_OS_FREEBSD, elfsh_void_altplthandler);
  elfsh_register_altplthook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			    ELFSH_OS_NETBSD, elfsh_void_altplthandler);
  elfsh_register_altplthook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			    ELFSH_OS_OPENBSD, elfsh_void_altplthandler);
  elfsh_register_altplthook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			    ELFSH_OS_SOLARIS, elfsh_void_altplthandler);

  /* Usual ALTPLT targets for ET_EXEC/Sparc32 */
  elfsh_register_altplthook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC,
			    ELFSH_OS_LINUX, elfsh_hijack_altplt_sparc32);
  elfsh_register_altplthook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC,
			    ELFSH_OS_FREEBSD, elfsh_hijack_altplt_sparc32);
  elfsh_register_altplthook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC,
			    ELFSH_OS_NETBSD, elfsh_hijack_altplt_sparc32);
  elfsh_register_altplthook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC,
			    ELFSH_OS_OPENBSD, elfsh_hijack_altplt_sparc32);
  elfsh_register_altplthook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC,
			    ELFSH_OS_SOLARIS, elfsh_hijack_altplt_sparc32);

  /* Usual ALTPLT targets for ET_EXEC/Sparc64 */
  elfsh_register_altplthook(ELFSH_ARCH_SPARC64, ELFSH_FILE_EXEC,
			    ELFSH_OS_LINUX, elfsh_hijack_altplt_sparc64);
  elfsh_register_altplthook(ELFSH_ARCH_SPARC64, ELFSH_FILE_EXEC,
			    ELFSH_OS_FREEBSD, elfsh_hijack_altplt_sparc64);
  elfsh_register_altplthook(ELFSH_ARCH_SPARC64, ELFSH_FILE_EXEC,
			    ELFSH_OS_NETBSD, elfsh_hijack_altplt_sparc64);
  elfsh_register_altplthook(ELFSH_ARCH_SPARC64, ELFSH_FILE_EXEC,
			    ELFSH_OS_OPENBSD, elfsh_hijack_altplt_sparc64);
  elfsh_register_altplthook(ELFSH_ARCH_SPARC64, ELFSH_FILE_EXEC,
			    ELFSH_OS_SOLARIS, elfsh_hijack_altplt_sparc64);

  /* Usual ALTPLT targets for ET_EXEC/IA64 */
  elfsh_register_altplthook(ELFSH_ARCH_IA64, ELFSH_FILE_EXEC,
			    ELFSH_OS_LINUX, elfsh_hijack_altplt_ia64);
  elfsh_register_altplthook(ELFSH_ARCH_IA64, ELFSH_FILE_EXEC,
			    ELFSH_OS_FREEBSD, elfsh_hijack_altplt_ia64);
  elfsh_register_altplthook(ELFSH_ARCH_IA64, ELFSH_FILE_EXEC,
			    ELFSH_OS_NETBSD, elfsh_hijack_altplt_ia64);
  elfsh_register_altplthook(ELFSH_ARCH_IA64, ELFSH_FILE_EXEC,
			    ELFSH_OS_OPENBSD, elfsh_hijack_altplt_ia64);
  elfsh_register_altplthook(ELFSH_ARCH_IA64, ELFSH_FILE_EXEC,
			    ELFSH_OS_SOLARIS, elfsh_hijack_altplt_ia64);

  /* Usual ALTPLT targets for ET_EXEC/Alpha64 */
  elfsh_register_altplthook(ELFSH_ARCH_ALPHA64, ELFSH_FILE_EXEC,
			    ELFSH_OS_LINUX, elfsh_hijack_altplt_alpha64);
  elfsh_register_altplthook(ELFSH_ARCH_ALPHA64, ELFSH_FILE_EXEC,
			    ELFSH_OS_FREEBSD, elfsh_hijack_altplt_alpha64);
  elfsh_register_altplthook(ELFSH_ARCH_ALPHA64, ELFSH_FILE_EXEC,
			    ELFSH_OS_NETBSD, elfsh_hijack_altplt_alpha64);
  elfsh_register_altplthook(ELFSH_ARCH_ALPHA64, ELFSH_FILE_EXEC,
			    ELFSH_OS_OPENBSD, elfsh_hijack_altplt_alpha64);
  elfsh_register_altplthook(ELFSH_ARCH_ALPHA64, ELFSH_FILE_EXEC,
			    ELFSH_OS_SOLARIS, elfsh_hijack_altplt_alpha64);

  /* Usual ALTPLT targets for ET_EXEC/MIPS32 */
  elfsh_register_altplthook(ELFSH_ARCH_MIPS32, ELFSH_FILE_EXEC,
			    ELFSH_OS_LINUX, elfsh_hijack_altplt_mips32);
  elfsh_register_altplthook(ELFSH_ARCH_MIPS32, ELFSH_FILE_EXEC,
			    ELFSH_OS_FREEBSD, elfsh_hijack_altplt_mips32);
  elfsh_register_altplthook(ELFSH_ARCH_MIPS32, ELFSH_FILE_EXEC,
			    ELFSH_OS_NETBSD, elfsh_hijack_altplt_mips32);
  elfsh_register_altplthook(ELFSH_ARCH_MIPS32, ELFSH_FILE_EXEC,
			    ELFSH_OS_OPENBSD, elfsh_hijack_altplt_mips32);
  elfsh_register_altplthook(ELFSH_ARCH_MIPS32, ELFSH_FILE_EXEC,
			    ELFSH_OS_SOLARIS, elfsh_hijack_altplt_mips32);

  /* Usual ALTPLT targets for ET_EXEC/MIPS64 */
  elfsh_register_altplthook(ELFSH_ARCH_MIPS64, ELFSH_FILE_EXEC,
			    ELFSH_OS_LINUX, elfsh_hijack_altplt_mips64);
  elfsh_register_altplthook(ELFSH_ARCH_MIPS64, ELFSH_FILE_EXEC,
			    ELFSH_OS_FREEBSD, elfsh_hijack_altplt_mips64);
  elfsh_register_altplthook(ELFSH_ARCH_MIPS64, ELFSH_FILE_EXEC,
			    ELFSH_OS_NETBSD, elfsh_hijack_altplt_mips64);
  elfsh_register_altplthook(ELFSH_ARCH_MIPS64, ELFSH_FILE_EXEC,
			    ELFSH_OS_OPENBSD, elfsh_hijack_altplt_mips64);
  elfsh_register_altplthook(ELFSH_ARCH_MIPS64, ELFSH_FILE_EXEC,
			    ELFSH_OS_SOLARIS, elfsh_hijack_altplt_mips64);

  /***********************************/
  /* Now register Relocation hooks */
  /***********************************/


  /* Usual REL targets for ET_EXEC/i386 */
  elfsh_register_relhook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			 ELFSH_OS_LINUX, elfsh_relocate_ia32);
  elfsh_register_relhook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			 ELFSH_OS_FREEBSD, elfsh_relocate_ia32);
  elfsh_register_relhook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			 ELFSH_OS_NETBSD, elfsh_relocate_ia32);
  elfsh_register_relhook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			 ELFSH_OS_OPENBSD, elfsh_relocate_ia32);
  elfsh_register_relhook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			 ELFSH_OS_SOLARIS, elfsh_relocate_ia32);

  /* Usual REL targets for ET_EXEC/Sparc32 */
  elfsh_register_relhook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC,
			 ELFSH_OS_LINUX, elfsh_relocate_sparc32);
  elfsh_register_relhook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC,
			 ELFSH_OS_FREEBSD, elfsh_relocate_sparc32);
  elfsh_register_relhook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC,
			 ELFSH_OS_NETBSD, elfsh_relocate_sparc32);
  elfsh_register_relhook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC,
			 ELFSH_OS_OPENBSD, elfsh_relocate_sparc32);
  elfsh_register_relhook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC,
			 ELFSH_OS_SOLARIS, elfsh_relocate_sparc32);

  /* Usual REL targets for ET_EXEC/Sparc64 */
  elfsh_register_relhook(ELFSH_ARCH_SPARC64, ELFSH_FILE_EXEC,
			 ELFSH_OS_LINUX, elfsh_relocate_sparc64);
  elfsh_register_relhook(ELFSH_ARCH_SPARC64, ELFSH_FILE_EXEC,
			 ELFSH_OS_FREEBSD, elfsh_relocate_sparc64);
  elfsh_register_relhook(ELFSH_ARCH_SPARC64, ELFSH_FILE_EXEC,
			 ELFSH_OS_NETBSD, elfsh_relocate_sparc64);
  elfsh_register_relhook(ELFSH_ARCH_SPARC64, ELFSH_FILE_EXEC,
			 ELFSH_OS_OPENBSD, elfsh_relocate_sparc64);
  elfsh_register_relhook(ELFSH_ARCH_SPARC64, ELFSH_FILE_EXEC,
			 ELFSH_OS_SOLARIS, elfsh_relocate_sparc64);

  /* Usual REL targets for ET_EXEC/IA64 */
  elfsh_register_relhook(ELFSH_ARCH_IA64, ELFSH_FILE_EXEC,
			 ELFSH_OS_LINUX, elfsh_relocate_ia64);
  elfsh_register_relhook(ELFSH_ARCH_IA64, ELFSH_FILE_EXEC,
			 ELFSH_OS_FREEBSD, elfsh_relocate_ia64);
  elfsh_register_relhook(ELFSH_ARCH_IA64, ELFSH_FILE_EXEC,
			 ELFSH_OS_NETBSD, elfsh_relocate_ia64);
  elfsh_register_relhook(ELFSH_ARCH_IA64, ELFSH_FILE_EXEC,
			 ELFSH_OS_OPENBSD, elfsh_relocate_ia64);
  elfsh_register_relhook(ELFSH_ARCH_IA64, ELFSH_FILE_EXEC,
			 ELFSH_OS_SOLARIS, elfsh_relocate_ia64);

  /* Usual REL targets for ET_EXEC/Alpha64 */
  elfsh_register_relhook(ELFSH_ARCH_ALPHA64, ELFSH_FILE_EXEC,
			 ELFSH_OS_LINUX, elfsh_relocate_alpha64);
  elfsh_register_relhook(ELFSH_ARCH_ALPHA64, ELFSH_FILE_EXEC,
			 ELFSH_OS_FREEBSD, elfsh_relocate_alpha64);
  elfsh_register_relhook(ELFSH_ARCH_ALPHA64, ELFSH_FILE_EXEC,
			 ELFSH_OS_NETBSD, elfsh_relocate_alpha64);
  elfsh_register_relhook(ELFSH_ARCH_ALPHA64, ELFSH_FILE_EXEC,
			 ELFSH_OS_OPENBSD, elfsh_relocate_alpha64);
  elfsh_register_relhook(ELFSH_ARCH_ALPHA64, ELFSH_FILE_EXEC,
			 ELFSH_OS_SOLARIS, elfsh_relocate_alpha64);

  /* Usual REL targets for ET_EXEC/MIPS32 */
  elfsh_register_relhook(ELFSH_ARCH_MIPS32, ELFSH_FILE_EXEC,
			 ELFSH_OS_LINUX, elfsh_relocate_mips32);
  elfsh_register_relhook(ELFSH_ARCH_MIPS32, ELFSH_FILE_EXEC,
			 ELFSH_OS_FREEBSD, elfsh_relocate_mips32);
  elfsh_register_relhook(ELFSH_ARCH_MIPS32, ELFSH_FILE_EXEC,
			 ELFSH_OS_NETBSD, elfsh_relocate_mips32);
  elfsh_register_relhook(ELFSH_ARCH_MIPS32, ELFSH_FILE_EXEC,
			 ELFSH_OS_OPENBSD, elfsh_relocate_mips32);
  elfsh_register_relhook(ELFSH_ARCH_MIPS32, ELFSH_FILE_EXEC,
			 ELFSH_OS_SOLARIS, elfsh_relocate_mips32);

  /* Usual REL targets for ET_EXEC/MIPS64 */
  elfsh_register_relhook(ELFSH_ARCH_MIPS64, ELFSH_FILE_EXEC,
			 ELFSH_OS_LINUX, elfsh_relocate_mips64);
  elfsh_register_relhook(ELFSH_ARCH_MIPS64, ELFSH_FILE_EXEC,
			 ELFSH_OS_FREEBSD, elfsh_relocate_mips64);
  elfsh_register_relhook(ELFSH_ARCH_MIPS64, ELFSH_FILE_EXEC,
			 ELFSH_OS_NETBSD, elfsh_relocate_mips64);
  elfsh_register_relhook(ELFSH_ARCH_MIPS64, ELFSH_FILE_EXEC,
			 ELFSH_OS_OPENBSD, elfsh_relocate_mips64);
  elfsh_register_relhook(ELFSH_ARCH_MIPS64, ELFSH_FILE_EXEC,
			 ELFSH_OS_SOLARIS, elfsh_relocate_mips64);

  /***********************************/
  /* Now register Control flow hooks */
  /***********************************/

  /* Usual REL targets for ET_EXEC/IA32 */
  elfsh_register_cflowhook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			   ELFSH_OS_LINUX, elfsh_cflow_ia32);
  elfsh_register_cflowhook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			   ELFSH_OS_FREEBSD, elfsh_cflow_ia32);
  elfsh_register_cflowhook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			   ELFSH_OS_NETBSD, elfsh_cflow_ia32);
  elfsh_register_cflowhook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			   ELFSH_OS_OPENBSD, elfsh_cflow_ia32);
  elfsh_register_cflowhook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			   ELFSH_OS_SOLARIS, elfsh_cflow_ia32);

  /* Usual CFLOW targets for ET_EXEC/IA64 */
  elfsh_register_cflowhook(ELFSH_ARCH_IA64, ELFSH_FILE_EXEC,
			   ELFSH_OS_LINUX, elfsh_cflow_ia64);
  elfsh_register_cflowhook(ELFSH_ARCH_IA64, ELFSH_FILE_EXEC,
			   ELFSH_OS_FREEBSD, elfsh_cflow_ia64);
  elfsh_register_cflowhook(ELFSH_ARCH_IA64, ELFSH_FILE_EXEC,
			   ELFSH_OS_NETBSD, elfsh_cflow_ia64);
  elfsh_register_cflowhook(ELFSH_ARCH_IA64, ELFSH_FILE_EXEC,
			   ELFSH_OS_OPENBSD, elfsh_cflow_ia64);
  elfsh_register_cflowhook(ELFSH_ARCH_IA64, ELFSH_FILE_EXEC,
			   ELFSH_OS_SOLARIS, elfsh_cflow_ia64);

  /* Usual CFLOW targets for ET_EXEC/SPARC32 */
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC,
			   ELFSH_OS_LINUX, elfsh_cflow_sparc32);
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC,
			   ELFSH_OS_FREEBSD, elfsh_cflow_sparc32);
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC,
			   ELFSH_OS_NETBSD, elfsh_cflow_sparc32);
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC,
			   ELFSH_OS_OPENBSD, elfsh_cflow_sparc32);
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC,
			   ELFSH_OS_SOLARIS, elfsh_cflow_sparc32);

  /* Usual CFLOW targets for ET_EXEC/SPARC64 */
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC64, ELFSH_FILE_EXEC,
			   ELFSH_OS_LINUX, elfsh_cflow_sparc64);
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC64, ELFSH_FILE_EXEC,
			   ELFSH_OS_FREEBSD, elfsh_cflow_sparc64);
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC64, ELFSH_FILE_EXEC,
			   ELFSH_OS_NETBSD, elfsh_cflow_sparc64);
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC64, ELFSH_FILE_EXEC,
			   ELFSH_OS_OPENBSD, elfsh_cflow_sparc64);
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC64, ELFSH_FILE_EXEC,
			   ELFSH_OS_SOLARIS, elfsh_cflow_sparc64);

  /* Usual CFLOW targets for ET_EXEC/ALPHA64 */
  elfsh_register_cflowhook(ELFSH_ARCH_ALPHA64, ELFSH_FILE_EXEC,
			   ELFSH_OS_LINUX, elfsh_cflow_alpha64);
  elfsh_register_cflowhook(ELFSH_ARCH_ALPHA64, ELFSH_FILE_EXEC,
			   ELFSH_OS_FREEBSD, elfsh_cflow_alpha64);
  elfsh_register_cflowhook(ELFSH_ARCH_ALPHA64, ELFSH_FILE_EXEC,
			   ELFSH_OS_NETBSD, elfsh_cflow_alpha64);
  elfsh_register_cflowhook(ELFSH_ARCH_ALPHA64, ELFSH_FILE_EXEC,
			   ELFSH_OS_OPENBSD, elfsh_cflow_alpha64);
  elfsh_register_cflowhook(ELFSH_ARCH_ALPHA64, ELFSH_FILE_EXEC,
			   ELFSH_OS_SOLARIS, elfsh_cflow_alpha64);

  /* Usual CFLOW targets for ET_EXEC/MIPS32 */
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS32, ELFSH_FILE_EXEC,
			   ELFSH_OS_LINUX, elfsh_cflow_mips32);
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS32, ELFSH_FILE_EXEC,
			   ELFSH_OS_FREEBSD, elfsh_cflow_mips32);
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS32, ELFSH_FILE_EXEC,
			   ELFSH_OS_NETBSD, elfsh_cflow_mips32);
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS32, ELFSH_FILE_EXEC,
			   ELFSH_OS_OPENBSD, elfsh_cflow_mips32);
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS32, ELFSH_FILE_EXEC,
			   ELFSH_OS_SOLARIS, elfsh_cflow_mips32);

  /* Usual CFLOW targets for ET_EXEC/MIPS64 */
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS64, ELFSH_FILE_EXEC,
			   ELFSH_OS_LINUX, elfsh_cflow_mips64);
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS64, ELFSH_FILE_EXEC,
			   ELFSH_OS_FREEBSD, elfsh_cflow_mips64);
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS64, ELFSH_FILE_EXEC,
			   ELFSH_OS_NETBSD, elfsh_cflow_mips64);
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS64, ELFSH_FILE_EXEC,
			   ELFSH_OS_OPENBSD, elfsh_cflow_mips64);
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS64, ELFSH_FILE_EXEC,
			   ELFSH_OS_SOLARIS, elfsh_cflow_mips64);
  elfsh_register_cflowhook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
			   ELFSH_OS_LINUX, elfsh_cflow_ia32);
  elfsh_register_cflowhook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
			   ELFSH_OS_BEOS, elfsh_cflow_ia32);

  /* Usual REL/PLT/ALTPLT targets for ET_DYN/x86 */
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
			 ELFSH_OS_BEOS, elfsh_hijack_plt_ia32);
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
			 ELFSH_OS_LINUX, elfsh_hijack_plt_ia32);
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
			 ELFSH_OS_FREEBSD, elfsh_hijack_plt_ia32);
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
			 ELFSH_OS_NETBSD, elfsh_hijack_plt_ia32);
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
			 ELFSH_OS_SOLARIS, elfsh_hijack_plt_ia32);
  elfsh_register_relhook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
			 ELFSH_OS_BEOS, elfsh_relocate_ia32);
  elfsh_register_altplthook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
			    ELFSH_OS_LINUX, elfsh_void_altplthandler);
  elfsh_register_relhook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
			 ELFSH_OS_LINUX, elfsh_relocate_ia32);
  elfsh_register_altplthook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
			    ELFSH_OS_BEOS, elfsh_void_altplthandler);

  /***********************************/
  /* Now register encodeplt points hooks */
  /***********************************/

  /* Usual ENCODEPLT breakpoint targets for ET_EXEC/IA32 */
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			       ELFSH_OS_LINUX, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			       ELFSH_OS_FREEBSD, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			       ELFSH_OS_NETBSD, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			       ELFSH_OS_OPENBSD, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			       ELFSH_OS_SOLARIS, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
			       ELFSH_OS_LINUX, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
			       ELFSH_OS_FREEBSD, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
			       ELFSH_OS_NETBSD, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
			       ELFSH_OS_OPENBSD, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
			       ELFSH_OS_SOLARIS, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
			       ELFSH_OS_BEOS, elfsh_encodeplt_ia32);

  /* Usual ENCODEPLT1 targets for ET_EXEC/IA32 */
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
				ELFSH_OS_LINUX, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
				ELFSH_OS_FREEBSD, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
				ELFSH_OS_NETBSD, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
				ELFSH_OS_OPENBSD, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
				ELFSH_OS_SOLARIS, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
				ELFSH_OS_LINUX, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
				ELFSH_OS_FREEBSD, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
				ELFSH_OS_NETBSD, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
				ELFSH_OS_OPENBSD, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
				ELFSH_OS_SOLARIS, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
				ELFSH_OS_BEOS, elfsh_encodeplt1_ia32);


  /*** Now EXTPLT handlers ***/

  /* Usual EXTPLT targets for ET_EXEC/IA32 */
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			    ELFSH_OS_LINUX, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			    ELFSH_OS_FREEBSD, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			    ELFSH_OS_NETBSD, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			    ELFSH_OS_OPENBSD, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			    ELFSH_OS_SOLARIS, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
			    ELFSH_OS_LINUX, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
			    ELFSH_OS_FREEBSD, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
			    ELFSH_OS_NETBSD, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
			    ELFSH_OS_OPENBSD, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
			    ELFSH_OS_SOLARIS, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,
			    ELFSH_OS_BEOS, elfsh_extplt_ia32);


  /***************************************/
  /****** ARGC arguments counting  ******/
  /**************************************/
  /* Usual ARGC targets for ET_EXEC/i386 */
  elfsh_register_argchook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			  ELFSH_OS_LINUX, elfsh_args_count_ia32);
  elfsh_register_argchook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			  ELFSH_OS_FREEBSD, elfsh_args_count_ia32);
  elfsh_register_argchook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			  ELFSH_OS_NETBSD, elfsh_args_count_ia32);
  elfsh_register_argchook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			  ELFSH_OS_OPENBSD, elfsh_args_count_ia32);
  elfsh_register_argchook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			  ELFSH_OS_SOLARIS, elfsh_args_count_ia32);
  elfsh_register_argchook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,
			  ELFSH_OS_BEOS, elfsh_args_count_ia32);

  /* Register handlers for READMEM hook */
  elfsh_register_readmem(ELFSH_OS_LINUX, ELFSH_IOTYPE_EMBEDDED  , elfsh_get_raw);
  elfsh_register_readmem(ELFSH_OS_LINUX, ELFSH_IOTYPE_STATIC    , elfsh_get_raw);
  elfsh_register_readmem(ELFSH_OS_FREEBSD, ELFSH_IOTYPE_EMBEDDED, elfsh_get_raw);
  elfsh_register_readmem(ELFSH_OS_FREEBSD, ELFSH_IOTYPE_STATIC  , elfsh_get_raw);
  elfsh_register_readmem(ELFSH_OS_NETBSD, ELFSH_IOTYPE_EMBEDDED , elfsh_get_raw);
  elfsh_register_readmem(ELFSH_OS_NETBSD, ELFSH_IOTYPE_STATIC   , elfsh_get_raw);
  elfsh_register_readmem(ELFSH_OS_OPENBSD, ELFSH_IOTYPE_EMBEDDED, elfsh_get_raw);
  elfsh_register_readmem(ELFSH_OS_OPENBSD, ELFSH_IOTYPE_STATIC  , elfsh_get_raw);
  elfsh_register_readmem(ELFSH_OS_SOLARIS, ELFSH_IOTYPE_EMBEDDED, elfsh_get_raw);
  elfsh_register_readmem(ELFSH_OS_SOLARIS, ELFSH_IOTYPE_STATIC  , elfsh_get_raw);
  elfsh_register_readmem(ELFSH_OS_BEOS, ELFSH_IOTYPE_EMBEDDED   , elfsh_get_raw);
  elfsh_register_readmem(ELFSH_OS_BEOS, ELFSH_IOTYPE_STATIC     , elfsh_get_raw);
  elfsh_register_readmem(ELFSH_OS_IRIX, ELFSH_IOTYPE_EMBEDDED   , elfsh_get_raw);
  elfsh_register_readmem(ELFSH_OS_IRIX, ELFSH_IOTYPE_STATIC     , elfsh_get_raw);
  elfsh_register_readmem(ELFSH_OS_HPUX, ELFSH_IOTYPE_EMBEDDED   , elfsh_get_raw);
  elfsh_register_readmem(ELFSH_OS_HPUX, ELFSH_IOTYPE_STATIC     , elfsh_get_raw);
  /* It appears that the OS field of some binary is set to 'ELF_OSABI_ARM' ... */
  elfsh_register_readmem(ELFSH_OS_ARM, ELFSH_IOTYPE_EMBEDDED   , elfsh_get_raw);
  elfsh_register_readmem(ELFSH_OS_ARM, ELFSH_IOTYPE_STATIC     , elfsh_get_raw);

  /* Register handlers for READMEMA hook */
  elfsh_register_readmema(ELFSH_OS_LINUX, ELFSH_IOTYPE_EMBEDDED  , elfsh_get_raw_by_addr);
  elfsh_register_readmema(ELFSH_OS_LINUX, ELFSH_IOTYPE_STATIC    , elfsh_get_raw_by_addr);
  elfsh_register_readmema(ELFSH_OS_FREEBSD, ELFSH_IOTYPE_EMBEDDED, elfsh_get_raw_by_addr);
  elfsh_register_readmema(ELFSH_OS_FREEBSD, ELFSH_IOTYPE_STATIC  , elfsh_get_raw_by_addr);
  elfsh_register_readmema(ELFSH_OS_NETBSD, ELFSH_IOTYPE_EMBEDDED , elfsh_get_raw_by_addr);
  elfsh_register_readmema(ELFSH_OS_NETBSD, ELFSH_IOTYPE_STATIC   , elfsh_get_raw_by_addr);
  elfsh_register_readmema(ELFSH_OS_OPENBSD, ELFSH_IOTYPE_EMBEDDED, elfsh_get_raw_by_addr);
  elfsh_register_readmema(ELFSH_OS_OPENBSD, ELFSH_IOTYPE_STATIC  , elfsh_get_raw_by_addr);
  elfsh_register_readmema(ELFSH_OS_SOLARIS, ELFSH_IOTYPE_EMBEDDED, elfsh_get_raw_by_addr);
  elfsh_register_readmema(ELFSH_OS_SOLARIS, ELFSH_IOTYPE_STATIC  , elfsh_get_raw_by_addr);
  elfsh_register_readmema(ELFSH_OS_BEOS, ELFSH_IOTYPE_EMBEDDED   , elfsh_get_raw_by_addr);
  elfsh_register_readmema(ELFSH_OS_BEOS, ELFSH_IOTYPE_STATIC     , elfsh_get_raw_by_addr);
  elfsh_register_readmema(ELFSH_OS_IRIX, ELFSH_IOTYPE_EMBEDDED   , elfsh_get_raw_by_addr);
  elfsh_register_readmema(ELFSH_OS_IRIX, ELFSH_IOTYPE_STATIC     , elfsh_get_raw_by_addr);
  elfsh_register_readmema(ELFSH_OS_HPUX, ELFSH_IOTYPE_EMBEDDED   , elfsh_get_raw_by_addr);
  elfsh_register_readmema(ELFSH_OS_HPUX, ELFSH_IOTYPE_STATIC     , elfsh_get_raw_by_addr);
  elfsh_register_readmema(ELFSH_OS_ARM, ELFSH_IOTYPE_EMBEDDED   , elfsh_get_raw_by_addr);
  elfsh_register_readmema(ELFSH_OS_ARM, ELFSH_IOTYPE_STATIC     , elfsh_get_raw_by_addr);

  /* Register handlers for WRITEMEM hook */
  elfsh_register_writemem(ELFSH_OS_LINUX, ELFSH_IOTYPE_EMBEDDED  , elfsh_memcpy);
  elfsh_register_writemem(ELFSH_OS_LINUX, ELFSH_IOTYPE_STATIC    , elfsh_memcpy);
  elfsh_register_writemem(ELFSH_OS_FREEBSD, ELFSH_IOTYPE_EMBEDDED, elfsh_memcpy);
  elfsh_register_writemem(ELFSH_OS_FREEBSD, ELFSH_IOTYPE_STATIC  , elfsh_memcpy);
  elfsh_register_writemem(ELFSH_OS_NETBSD, ELFSH_IOTYPE_EMBEDDED , elfsh_memcpy);
  elfsh_register_writemem(ELFSH_OS_NETBSD, ELFSH_IOTYPE_STATIC   , elfsh_memcpy);
  elfsh_register_writemem(ELFSH_OS_OPENBSD, ELFSH_IOTYPE_EMBEDDED, elfsh_memcpy);
  elfsh_register_writemem(ELFSH_OS_OPENBSD, ELFSH_IOTYPE_STATIC  , elfsh_memcpy);
  elfsh_register_writemem(ELFSH_OS_SOLARIS, ELFSH_IOTYPE_EMBEDDED, elfsh_memcpy);
  elfsh_register_writemem(ELFSH_OS_SOLARIS, ELFSH_IOTYPE_STATIC  , elfsh_memcpy);
  elfsh_register_writemem(ELFSH_OS_BEOS, ELFSH_IOTYPE_EMBEDDED   , elfsh_memcpy);
  elfsh_register_writemem(ELFSH_OS_BEOS, ELFSH_IOTYPE_STATIC     , elfsh_memcpy);
  elfsh_register_writemem(ELFSH_OS_IRIX, ELFSH_IOTYPE_EMBEDDED   , elfsh_memcpy);
  elfsh_register_writemem(ELFSH_OS_IRIX, ELFSH_IOTYPE_STATIC     , elfsh_memcpy);
  elfsh_register_writemem(ELFSH_OS_HPUX, ELFSH_IOTYPE_EMBEDDED   , elfsh_memcpy);
  elfsh_register_writemem(ELFSH_OS_HPUX, ELFSH_IOTYPE_STATIC     , elfsh_memcpy);
  elfsh_register_writemem(ELFSH_OS_ARM, ELFSH_IOTYPE_EMBEDDED   , elfsh_memcpy);
  elfsh_register_writemem(ELFSH_OS_ARM, ELFSH_IOTYPE_STATIC     , elfsh_memcpy);

  /* Register handlers for READMEMF/WRITEMEMF hook */
  elfsh_register_readmemf(ELFSH_HOST_USER, LIBELFSH_MODE_STATIC, elfsh_raw_read);
  elfsh_register_readmemf(ELFSH_HOST_USER, LIBELFSH_MODE_RUNTIME, elfsh_raw_read);
  elfsh_register_writememf(ELFSH_HOST_USER, LIBELFSH_MODE_STATIC, elfsh_raw_write);
  elfsh_register_writememf(ELFSH_HOST_USER, LIBELFSH_MODE_RUNTIME, elfsh_raw_write);

  /* Register handlers for the ALLOC hook */
  elfsh_register_allochook(ELFSH_HOST_USER, elfsh_map_userland);

  /* Register handlers for the ALLOC hook */
  elfsh_register_mprotecthook(ELFSH_HOST_USER, elfsh_mprotect_userland);

  /* Register handlers for the ALLOC hook */
  elfsh_register_munprotecthook(ELFSH_HOST_USER, elfsh_munprotect_userland);

  done++;
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}
