/*
** hooks.c for libelfsh (The ELF shell library)
**
** All the function pointers for all provided
** techniques.
**
** Started Jan 11 2004 02:57:03 mayhem
**
*/
#include "libelfsh.h"



hash_t	interp_hash;


u_char	elfsh_ostype[5] = {
  ELFOSABI_LINUX,		
  ELFOSABI_FREEBSD,	
  ELFOSABI_NETBSD,		
  ELFOSABI_OPENBSD,	
  ELFOSABI_SOLARIS,	
};


/* 3 dimensional matrixes for PLT infection and ET_REL injection techniques */
int	(*hook_altplt[ELFSH_ARCHNUM][ELFSH_TYPENUM][ELFSH_OSNUM])(elfshobj_t *f, 
								  elfsh_Sym  *s,
								  elfsh_Addr a);
int	(*hook_plt[ELFSH_ARCHNUM][ELFSH_TYPENUM][ELFSH_OSNUM])(elfshobj_t *f, 
							       elfsh_Sym  *s,
							       elfsh_Addr a);
int	(*hook_rel[ELFSH_ARCHNUM][ELFSH_TYPENUM][ELFSH_OSNUM])(elfshsect_t *s, 
							       elfsh_Rel   *r, 
							       elfsh_Addr  *l,
							       elfsh_Addr  a,
							       elfshsect_t *m);
int	(*hook_cflow[ELFSH_ARCHNUM][ELFSH_TYPENUM][ELFSH_OSNUM])(elfshobj_t *f,
								 char       *name,
								 elfsh_Sym   *old,
								 elfsh_Addr  new);
int	(*hook_break[ELFSH_ARCHNUM][ELFSH_TYPENUM][ELFSH_OSNUM])(elfshobj_t *f,
								 elfshbp_t *b);
								 



/* Default PLT and ALTPLT hook handlers */
int	elfsh_default_plthandler(elfshobj_t *null, 
				 elfsh_Sym  *null2, 
				 elfsh_Addr null3)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}

/* Used on architectures where altplt hijack is not required, thus induce no fatal error */
/* DO NOT use this as a default handler, unless you know exactly what you are doing */
int	elfsh_void_altplthandler(elfshobj_t *null, 
				 elfsh_Sym  *null2, 
				 elfsh_Addr null3)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int	elfsh_default_relhandler(elfshsect_t *null, 
				 elfsh_Rel  * null2, 
				 elfsh_Addr * null3, 
				 elfsh_Addr   null4,
				 elfshsect_t *null5)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}

int	elfsh_default_cflowhandler(elfshobj_t   *null,
				   char		*nulls,
				   elfsh_Sym    *null2,
				   elfsh_Addr	null3)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}

int	elfsh_default_breakhandler(elfshobj_t   *null,
				   elfshbp_t	*null3)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}


/* Register an ALTPLT handler */
int	elfsh_register_altplthook(u_char archtype, 
				  u_char objtype, 
				  u_char ostype, 
				  void *fct)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (archtype >= ELFSH_ARCHNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_TYPENUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);
  hook_altplt[archtype][objtype][ostype] = fct;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Register an PLT handler */
int	elfsh_register_plthook(u_char archtype, u_char objtype, u_char ostype, 
			       void *fct)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (archtype >= ELFSH_ARCHNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_TYPENUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);
  hook_plt[archtype][objtype][ostype] = fct;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Register an ET_REL injection handler */
int	elfsh_register_relhook(u_char archtype, u_char objtype, u_char ostype,
			       void *fct)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (archtype >= ELFSH_ARCHNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_TYPENUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);
  hook_rel[archtype][objtype][ostype] = fct;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Register a control flow redirection handler */
int	elfsh_register_cflowhook(u_char archtype, u_char objtype, 
				 u_char ostype, void *fct)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (archtype >= ELFSH_ARCHNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_TYPENUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);
  hook_cflow[archtype][objtype][ostype] = fct;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Register a control flow redirection handler */
int	elfsh_register_breakhook(u_char archtype, u_char objtype, 
				 u_char ostype, void *fct)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (archtype >= ELFSH_ARCHNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_TYPENUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);
  hook_break[archtype][objtype][ostype] = fct;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Initialize ALTPLT hijack and ET_REL injection handlers in their hook */
void	elfsh_setup_hooks()
{
  u_int	i, j, k;
  static int done = 0;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Check if already initialized */
  if (done)
    ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);

  /* Initialize hooks to void */
  for (i = 0; i < ELFSH_ARCHNUM; i++)
    for (j = 0; j < ELFSH_TYPENUM; j++)
      for (k = 0; k < ELFSH_OSNUM; k++)
	{
	  hook_altplt[i][j][k] = elfsh_default_plthandler;
	  hook_plt[i][j][k]    = elfsh_default_plthandler;
	  hook_rel[i][j][k]    = elfsh_default_relhandler;
	  hook_cflow[i][j][k]  = elfsh_default_cflowhandler;
	  hook_break[i][j][k]  = elfsh_default_breakhandler;
	}
  

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


  /* Usual REL/PLT/ALTPLT targets for ET_DYN/x86 */
  elfsh_register_relhook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN,
			 ELFSH_OS_BEOS, elfsh_relocate_ia32);
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

  elfsh_register_cflowhook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_LINUX, elfsh_cflow_ia32);

  elfsh_register_altplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_LINUX, elfsh_void_altplthandler);

  elfsh_register_relhook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			 ELFSH_OS_LINUX, elfsh_relocate_ia32);
  
  elfsh_register_altplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_BEOS, elfsh_void_altplthandler);  
  

  /***********************************/
  /* Now register break points hooks */
  /***********************************/
  
  /* Usual REL targets for ET_EXEC/IA32 */
  elfsh_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_LINUX, elfsh_break_ia32);
  elfsh_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_FREEBSD, elfsh_break_ia32);
  elfsh_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_NETBSD, elfsh_break_ia32);
  elfsh_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_OPENBSD, elfsh_break_ia32);
  elfsh_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_SOLARIS, elfsh_break_ia32);
  elfsh_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_LINUX, elfsh_break_ia32);
  elfsh_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_FREEBSD, elfsh_break_ia32);
  elfsh_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_NETBSD, elfsh_break_ia32);
  elfsh_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_OPENBSD, elfsh_break_ia32);
  elfsh_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_SOLARIS, elfsh_break_ia32);

  
  done++;

  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/* Translate ELF architecture type into ELFsh architecture type */
u_char		elfsh_get_archtype(elfshobj_t *file)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (elfsh_get_arch(file->hdr))
    {
    case EM_386:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_IA32));
    case EM_SPARC:
    case EM_SPARC32PLUS:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_SPARC32));
    case EM_SPARCV9:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_SPARC64));
    case EM_PARISC:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_PARISC));
    case EM_IA_64:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_IA64));
    case EM_PPC:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_PPC32));
    case EM_PPC64:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_PPC64));
    case EM_SVX:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_CRAY));
    case EM_ALPHA:
#if EM_ALPHA != EM_ALPHA_EXP
    case EM_ALPHA_EXP:
#endif
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_ALPHA64));
    case EM_MIPS:
    case EM_MIPS_RS3_LE:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_MIPS32));
    default:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_ERROR));
    }
}


/* Translate ELF object type into ELFsh object type */
u_char		elfsh_get_elftype(elfshobj_t *file)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (elfsh_get_objtype(file->hdr))
    {
    case ET_EXEC:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_TYPE_EXEC));
    case ET_DYN:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_TYPE_DYN));
    default:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_TYPE_ERROR));
    }
}

      
/* Retreive ELFsh OS type from ELF header */
u_char  elfsh_get_real_ostype(elfshobj_t *file)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (file->hdr->e_ident[EI_OSABI])
    {
    case ELFOSABI_LINUX:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_OS_LINUX));
    case ELFOSABI_FREEBSD:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_OS_FREEBSD));
    case ELFOSABI_NETBSD:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_OS_NETBSD));
    case ELFOSABI_OPENBSD:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_OS_OPENBSD));
    case ELFOSABI_SOLARIS:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_OS_SOLARIS));
    case 0:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    default:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_OS_ERROR));
    }
}


/* For now, always return the type of a userland process */
/* Need to be extended for the kernel memory type */
u_char		elfsh_get_hosttype(elfshobj_t *file)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (E2DBG_HOST_PROC));
}


/* Fill up ELFsh Operating System type */
u_char		elfsh_get_ostype(elfshobj_t *file)
{
  u_char	*res;
  char		*interp;
  char		*end;
  char		r;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* First try */
  r = elfsh_get_real_ostype(file);
  switch (r)
    {
      /* Information not present */
    case 0:
      break;
    default:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (r));
    }
  
  /* Smart intermediate way, fingerprint by .interp section */
  /* Not aware of any other software using this technique */
  if (elfsh_get_interp(file))
    {
      if (!interp_hash.size)
	{
	  hash_init(&interp_hash, 10);
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
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (elfsh_get_real_ostype(file)));
	}
    }

  /* Fatalist */
#if defined(__linux__)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_LINUX);
#elif defined(__FreeBSD__)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_FREEBSD);
#elif defined(__NetBSD__)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_NETBSD);
#elif defined(__OpenBSD__)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_OPENBSD);
#elif defined(sun)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_SOLARIS);
#elif defined(__BEOS__)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_BEOS);
#else
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_ERROR);
#endif
}

/* Return the page size */
int       elfsh_get_pagesize(elfshobj_t *file)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (elfsh_get_archtype(file))
    {
    case ELFSH_ARCH_IA32:
    case ELFSH_ARCH_MIPS32:
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 4096);
    case ELFSH_ARCH_SPARC32:
    case ELFSH_ARCH_SPARC64:
    case ELFSH_ARCH_PARISC:
    case ELFSH_ARCH_IA64:
    case ELFSH_ARCH_ALPHA64:
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 8192);
    case ELFSH_ARCH_PPC32:
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    case ELFSH_ARCH_PPC64:
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    case ELFSH_ARCH_CRAY:
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    case ELFSH_ARCH_ERROR:
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    default:
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }  
}

