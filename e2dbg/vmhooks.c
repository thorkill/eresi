/*
** vmhooks.c for e2dbg in the ELF shell framework
**
** The interface hooking system
**
** Those hooks stay in the debugger and not in libelfsh.
**
** For now they cannot be accessed or changed from scripting yet.
** 
** Started   Sat Sep 24 07:17:33 2005 mayhem
** Modified  Thu Dec 28 04:28:00 2006 mayhem
*/
#include "elfsh.h"


/* Void handlers for the 2 hooks */
void		  e2dbg_default_getregs()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__); 
}

void		  e2dbg_default_setregs()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__); 
}

/* Error handler by default for them */
elfsh_Addr*	  e2dbg_default_getpc()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "GETPC unimplemented on this architecture", NULL);
}

elfsh_Addr*	  e2dbg_default_getfp()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "GETFP unimplemented on this architecture", NULL);
}

void		e2dbg_default_setstep()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_NERR(__FILE__, __FUNCTION__, __LINE__, 
		     "SETSTEP unimplemented on this architecture");
}

void		e2dbg_default_resetstep()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_NERR(__FILE__, __FUNCTION__, __LINE__, 
		     "RESETSTEP unimplemented on this architecture"); 
}

elfsh_Addr	e2dbg_default_nextfphandler(elfsh_Addr a)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}

elfsh_Addr	e2dbg_default_getrethandler(elfsh_Addr a)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}


/* Register a next frame-pointer handler */
int		e2dbg_register_nextfphook(u_char archtype, u_char hosttype, 
					  u_char ostype, void *fct)
{
  vector_t	*nextfp;
  u_int		*dim;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  nextfp = hash_get(&vector_hash, E2DBG_HOOK_NEXTFP);

  if (archtype >= ELFSH_ARCHNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (hosttype >= E2DBG_HOSTNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);

  //hook_nextfp[archtype][hosttype][ostype] = fct;
  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(nextfp, dim, (elfsh_Addr) fct);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Register a next return-addr handler */
int		e2dbg_register_getrethook(u_char archtype, u_char hosttype, 
					  u_char ostype, void *fct)
{
  vector_t	*getret;
  u_int		*dim;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  getret = hash_get(&vector_hash, E2DBG_HOOK_GETRET);

  if (archtype >= ELFSH_ARCHNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (hosttype >= E2DBG_HOSTNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);

  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(getret, dim, (elfsh_Addr) fct);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Register an SETREGS injection handler */
int	e2dbg_register_sregshook(u_char archtype, u_char hosttype, u_char ostype,
				 void *fct)
{
  vector_t	*setregs;
  u_int		*dim;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  setregs = hash_get(&vector_hash, E2DBG_HOOK_SETREGS);
  
  if (archtype >= ELFSH_ARCHNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (hosttype >= E2DBG_HOSTNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);

  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(setregs, dim, (elfsh_Addr) fct);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Register an SETREGS injection handler */
int      e2dbg_register_gregshook(u_char archtype, u_char hosttype, u_char ostype,
				 void *fct)
{
  vector_t	*getregs;
  u_int		*dim;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  getregs = hash_get(&vector_hash, E2DBG_HOOK_GETREGS);

  if (archtype >= ELFSH_ARCHNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Architecture type", -1);
  if (hosttype >= E2DBG_HOSTNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Operating System type", -1);

  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(getregs, dim, (elfsh_Addr) fct);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Register an GETPC injection handler */
int      e2dbg_register_getpchook(u_char archtype, u_char hosttype, u_char ostype,
				  void *fct)
{
  vector_t	*getpc;
  u_int		*dim;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  getpc = hash_get(&vector_hash, E2DBG_HOOK_GETPC);

  if (archtype >= ELFSH_ARCHNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Architecture type", -1);
  if (hosttype >= E2DBG_HOSTNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Operating System type", -1);

  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(getpc, dim, (elfsh_Addr) fct);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Register an GETPC injection handler */
int      e2dbg_register_getfphook(u_char archtype, u_char hosttype, u_char ostype,
				  void *fct)
{
  vector_t	*getfp;
  u_int		*dim;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  getfp = hash_get(&vector_hash, E2DBG_HOOK_GETFP);

  if (archtype >= ELFSH_ARCHNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Architecture type", -1);
  if (hosttype >= E2DBG_HOSTNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Operating System type", -1);

  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(getfp, dim, (elfsh_Addr) fct);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Register an SETSTEP injection handler */
int      e2dbg_register_setstephook(u_char archtype, u_char hosttype, u_char ostype,
				    void *fct)
{
  vector_t	*setstep;
  u_int		*dim;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  setstep = hash_get(&vector_hash, E2DBG_HOOK_SETSTEP);

  if (archtype >= ELFSH_ARCHNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Architecture type", -1);
  if (hosttype >= E2DBG_HOSTNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Operating System type", -1);

  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(setstep, dim, (elfsh_Addr) fct);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Register an RESETSTEP injection handler */
int      e2dbg_register_resetstephook(u_char archtype, u_char hosttype, u_char ostype,
				      void *fct)
{
  vector_t	*resetstep;
  u_int		*dim;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  resetstep = hash_get(&vector_hash, E2DBG_HOOK_RESETSTEP);
  
  if (archtype >= ELFSH_ARCHNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Architecture type", -1);
  if (hosttype >= E2DBG_HOSTNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Operating System type", -1);

  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(resetstep, dim, (elfsh_Addr) fct);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Initialize e2dbg hook vectors */
static int	e2dbg_register_vectors()
{
  u_int		*dims;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* All hooks have the same dimensions here */
  XALLOC(dims, 4 * sizeof(u_int), -1);
  dims[0] = ELFSH_ARCHNUM;
  dims[1] = ELFSH_TYPENUM;
  dims[2] = ELFSH_OSNUM;
  dims[3] = 0;
  
  /* Initialize debugger vectors */
  aspect_register_vector(E2DBG_HOOK_GETREGS, 
			 e2dbg_register_gregshook, 
			 e2dbg_default_getregs,
			 dims, 3);
  aspect_register_vector(E2DBG_HOOK_SETREGS, 
			 e2dbg_register_sregshook, 
			 e2dbg_default_setregs,
			 dims, 3);
  aspect_register_vector(E2DBG_HOOK_GETPC, 
			 e2dbg_register_getpchook, 
			 e2dbg_default_getpc,
			 dims, 3);
  aspect_register_vector(E2DBG_HOOK_SETSTEP, 
			 e2dbg_register_setstephook, 
			 e2dbg_default_setstep,
			 dims, 3);
  aspect_register_vector(E2DBG_HOOK_RESETSTEP, 
			 e2dbg_register_resetstephook, 
			 e2dbg_default_resetstep,
			 dims, 3);
  aspect_register_vector(E2DBG_HOOK_GETFP, 
			 e2dbg_register_getfphook, 
			 e2dbg_default_getfp,
			 dims, 3);
  aspect_register_vector(E2DBG_HOOK_NEXTFP, 
			 e2dbg_register_nextfphook, 
			 e2dbg_default_nextfphandler,
			 dims, 3);
  aspect_register_vector(E2DBG_HOOK_GETRET, 
			 e2dbg_register_getrethook, 
			 e2dbg_default_getrethandler,
			 dims, 3);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Setup internal VM hooks */
void		e2dbg_setup_hooks()
{
  static int	done = 0;

  /* Check if already initialized */
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (done)
    ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
  aspect_vectors_init();
  e2dbg_register_vectors();

  /* Initialize vectors */
  e2dbg_register_gregshook(ELFSH_ARCH_IA32, E2DBG_HOST_PROC, 
			   ELFSH_OS_FREEBSD, e2dbg_get_regvars_ia32_bsd);
  e2dbg_register_gregshook(ELFSH_ARCH_IA32, E2DBG_HOST_PROC, 
			   ELFSH_OS_NETBSD, e2dbg_get_regvars_ia32_bsd);
  e2dbg_register_gregshook(ELFSH_ARCH_IA32, E2DBG_HOST_PROC, 
			   ELFSH_OS_OPENBSD, e2dbg_get_regvars_ia32_bsd);
  e2dbg_register_gregshook(ELFSH_ARCH_IA32, E2DBG_HOST_PROC, 
			   ELFSH_OS_LINUX, e2dbg_get_regvars_ia32_sysv);
  e2dbg_register_gregshook(ELFSH_ARCH_IA32, E2DBG_HOST_PROC, 
			   ELFSH_OS_SOLARIS, e2dbg_get_regvars_ia32_sysv);

  e2dbg_register_sregshook(ELFSH_ARCH_IA32, E2DBG_HOST_PROC, 
			   ELFSH_OS_FREEBSD, e2dbg_set_regvars_ia32_bsd);
  e2dbg_register_sregshook(ELFSH_ARCH_IA32, E2DBG_HOST_PROC, 
			   ELFSH_OS_NETBSD, e2dbg_set_regvars_ia32_bsd);
  e2dbg_register_sregshook(ELFSH_ARCH_IA32, E2DBG_HOST_PROC, 
			   ELFSH_OS_OPENBSD, e2dbg_set_regvars_ia32_bsd);
  e2dbg_register_sregshook(ELFSH_ARCH_IA32, E2DBG_HOST_PROC, 
			   ELFSH_OS_LINUX, e2dbg_set_regvars_ia32_sysv);
  e2dbg_register_sregshook(ELFSH_ARCH_IA32, E2DBG_HOST_PROC, 
			   ELFSH_OS_SOLARIS, e2dbg_set_regvars_ia32_sysv);

  e2dbg_register_getpchook(ELFSH_ARCH_IA32, E2DBG_HOST_PROC, 
			   ELFSH_OS_FREEBSD, e2dbg_getpc_bsd_ia32);
  e2dbg_register_getpchook(ELFSH_ARCH_IA32, E2DBG_HOST_PROC, 
			   ELFSH_OS_NETBSD, e2dbg_getpc_bsd_ia32);
  e2dbg_register_getpchook(ELFSH_ARCH_IA32, E2DBG_HOST_PROC, 
			   ELFSH_OS_OPENBSD, e2dbg_getpc_bsd_ia32);
  e2dbg_register_getpchook(ELFSH_ARCH_IA32, E2DBG_HOST_PROC, 
			   ELFSH_OS_LINUX, e2dbg_getpc_sysv_ia32);
  e2dbg_register_getpchook(ELFSH_ARCH_IA32, E2DBG_HOST_PROC, 
			   ELFSH_OS_SOLARIS, e2dbg_getpc_sysv_ia32);
  
  e2dbg_register_setstephook(ELFSH_ARCH_IA32, E2DBG_HOST_PROC, 
			     ELFSH_OS_FREEBSD, e2dbg_setstep_bsd_ia32);
  e2dbg_register_setstephook(ELFSH_ARCH_IA32, E2DBG_HOST_PROC, 
			     ELFSH_OS_NETBSD, e2dbg_setstep_bsd_ia32);
  e2dbg_register_setstephook(ELFSH_ARCH_IA32, E2DBG_HOST_PROC, 
			     ELFSH_OS_OPENBSD, e2dbg_setstep_bsd_ia32);
  e2dbg_register_setstephook(ELFSH_ARCH_IA32, E2DBG_HOST_PROC, 
			     ELFSH_OS_LINUX, e2dbg_setstep_sysv_ia32);
  e2dbg_register_setstephook(ELFSH_ARCH_IA32, E2DBG_HOST_PROC, 
			     ELFSH_OS_SOLARIS, e2dbg_setstep_sysv_ia32);
  
  e2dbg_register_resetstephook(ELFSH_ARCH_IA32, E2DBG_HOST_PROC, 
			       ELFSH_OS_FREEBSD, e2dbg_resetstep_bsd_ia32);
  e2dbg_register_resetstephook(ELFSH_ARCH_IA32, E2DBG_HOST_PROC, 
			       ELFSH_OS_NETBSD, e2dbg_resetstep_bsd_ia32);
  e2dbg_register_resetstephook(ELFSH_ARCH_IA32, E2DBG_HOST_PROC, 
			       ELFSH_OS_OPENBSD, e2dbg_resetstep_bsd_ia32);
  e2dbg_register_resetstephook(ELFSH_ARCH_IA32, E2DBG_HOST_PROC, 
			       ELFSH_OS_LINUX, e2dbg_resetstep_sysv_ia32);
  e2dbg_register_resetstephook(ELFSH_ARCH_IA32, E2DBG_HOST_PROC, 
			       ELFSH_OS_SOLARIS, e2dbg_resetstep_sysv_ia32);

  /***********************************/
  /* Now nextfp break points hooks */
  /***********************************/
  
  /* Usual nextfp targets for ET_EXEC/ET_DYN on IA32 */
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_LINUX, elfsh_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_FREEBSD, elfsh_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_NETBSD, elfsh_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_OPENBSD, elfsh_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_SOLARIS, elfsh_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_LINUX, elfsh_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_FREEBSD, elfsh_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_NETBSD, elfsh_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_OPENBSD, elfsh_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_SOLARIS, elfsh_bt_ia32);

  /************************************/
  /* Now getfp break points hooks     */
  /***********************************/
  
  /* Usual getfp targets for ET_EXEC/ET_DYN on IA32 */
  e2dbg_register_getfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_LINUX, e2dbg_getfp_sysv_ia32);
  e2dbg_register_getfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_FREEBSD, e2dbg_getfp_bsd_ia32);
  e2dbg_register_getfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_NETBSD, e2dbg_getfp_bsd_ia32);
  e2dbg_register_getfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_OPENBSD, e2dbg_getfp_bsd_ia32);
  e2dbg_register_getfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_SOLARIS, e2dbg_getfp_sysv_ia32);
  e2dbg_register_getfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_LINUX, e2dbg_getfp_sysv_ia32);
  e2dbg_register_getfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_FREEBSD, e2dbg_getfp_bsd_ia32);
  e2dbg_register_getfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_NETBSD, e2dbg_getfp_bsd_ia32);
  e2dbg_register_getfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_OPENBSD, e2dbg_getfp_bsd_ia32);
  e2dbg_register_getfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_SOLARIS, e2dbg_getfp_sysv_ia32);


  /***********************************/
  /* Now register getret points hooks*/
  /***********************************/
  
  /* Usual getret targets for ET_EXEC/IA32 */
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_LINUX, elfsh_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_FREEBSD, elfsh_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_NETBSD, elfsh_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_OPENBSD, elfsh_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_SOLARIS, elfsh_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_LINUX, elfsh_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_FREEBSD, elfsh_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_NETBSD, elfsh_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_OPENBSD, elfsh_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_SOLARIS, elfsh_getret_ia32);
  
  done = 1;
  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}



/* Call the getregs hook */
int		  e2dbg_getregs()
{
  u_char        archtype;
  u_char        hosttype;
  u_char        ostype;
  u_int		dim[3];
  vector_t	*getregs;
  void		(*fct)();

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  getregs = (vector_t *) hash_get(&vector_hash, E2DBG_HOOK_GETREGS);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(world.curjob->current);
  hosttype = elfsh_get_hosttype(world.curjob->current);
  ostype = elfsh_get_ostype(world.curjob->current);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == E2DBG_HOST_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "GETREGS handler unexistant for this ARCH/OS", -1);

  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  fct    = aspect_vectors_select(getregs, dim);

  /* This hook is non-fatal, we just wont have regs as variables if it fails */
  fct();
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Call the setregs hook */
int		  e2dbg_setregs()
{
  u_char        archtype;
  u_char        hosttype;
  u_char        ostype;
  u_int		dim[3];
  vector_t	*setregs;
  void		(*fct)();

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  setregs = (vector_t *) hash_get(&vector_hash, E2DBG_HOOK_SETREGS);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(world.curjob->current);
  hosttype = elfsh_get_hosttype(world.curjob->current);
  ostype = elfsh_get_ostype(world.curjob->current);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == E2DBG_HOST_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "SETREGS handler unexistant for this ARCH/OS", -1);

  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  fct    = aspect_vectors_select(setregs, dim);
 
  /* This hook is non-fatal, we just wont have modified registers if it fails. */
  fct();
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Call the getpc hook */
elfsh_Addr*     e2dbg_getpc()
{
  elfsh_Addr	*pc;
  u_char        archtype;
  u_char        hosttype;
  u_char        ostype;
  u_int		dim[3];
  vector_t	*getpc;
  elfsh_Addr	*(*fct)();

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  getpc = (vector_t *) hash_get(&vector_hash, E2DBG_HOOK_GETPC);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(world.curjob->current);
  hosttype = elfsh_get_hosttype(world.curjob->current);
  ostype = elfsh_get_ostype(world.curjob->current);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == E2DBG_HOST_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "GETPC handler unexistant for this ARCH/OS", NULL);
  
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  fct    = aspect_vectors_select(getpc, dim);
  pc     = fct();
  if (pc == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Failed to get program counter register", NULL);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, pc);
}


/* Call the getfp hook */
elfsh_Addr*     e2dbg_getfp()
{
  elfsh_Addr	*fp;
  u_char        archtype;
  u_char        hosttype;
  u_char        ostype;
  u_int		dim[3];
  vector_t	*getfp;
  elfsh_Addr	*(*fct)();

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  getfp = (vector_t *) hash_get(&vector_hash, E2DBG_HOOK_GETFP);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(world.curjob->current);
  hosttype = elfsh_get_hosttype(world.curjob->current);
  ostype = elfsh_get_ostype(world.curjob->current);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == E2DBG_HOST_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "GETPC handler unexistant for this ARCH/OS", NULL);
  
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  fct    = aspect_vectors_select(getfp, dim);
  fp     = fct();
  if (fp == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Failed to get program counter register", NULL);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, fp);
}


/* Call the getregs hook */
int		  e2dbg_setstep()
{
  u_char        archtype;
  u_char        hosttype;
  u_char        ostype;
  u_int		dim[3];
  vector_t	*setstep;
  void		(*fct)();

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  setstep = (vector_t *) hash_get(&vector_hash, E2DBG_HOOK_SETSTEP);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(world.curjob->current);
  hosttype = elfsh_get_hosttype(world.curjob->current);
  ostype = elfsh_get_ostype(world.curjob->current);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == E2DBG_HOST_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "SETSTEP handler unexistant for this ARCH/OS", -1);
  
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  fct    = aspect_vectors_select(setstep, dim);
  fct();
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Call the getregs hook */
int		  e2dbg_resetstep()
{
  u_char        archtype;
  u_char        hosttype;
  u_char        ostype;
  u_int		dim[3];
  vector_t	*resetstep;
  void		(*fct)();

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  resetstep = (vector_t *) hash_get(&vector_hash, E2DBG_HOOK_RESETSTEP);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(world.curjob->current);
  hosttype = elfsh_get_hosttype(world.curjob->current);
  ostype = elfsh_get_ostype(world.curjob->current);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == E2DBG_HOST_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "GETPC handler unexistant for this ARCH/OS", -1);
  
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  fct    = aspect_vectors_select(resetstep, dim);
  fct();
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Call the getregs hook */
elfsh_Addr	e2dbg_nextfp(elfshobj_t *file, elfsh_Addr addr)
{
  u_char        archtype;
  u_char        hosttype;
  u_char        ostype;
  int		ret;
  u_int		dim[3];
  vector_t	*nextfp;
  int		(*fct)(elfsh_Addr addr);

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  nextfp = (vector_t *) hash_get(&vector_hash, E2DBG_HOOK_NEXTFP);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  hosttype = elfsh_get_hosttype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == E2DBG_HOST_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "BACKTRACE (nextfp) handler unexistant for this ARCH/OS", 
		      -1);

  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  fct    = aspect_vectors_select(nextfp, dim);
  ret = fct(addr);
  if (ret == 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Backtrace handler failed", (-1));
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/* Call the getregs hook */
elfsh_Addr	e2dbg_getret(elfshobj_t *file, elfsh_Addr addr)
{
  u_char        archtype;
  u_char        hosttype;
  u_char        ostype;
  int		ret;
  u_int		dim[3];
  vector_t	*getret;
  int		(*fct)(elfsh_Addr addr);

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  getret = (vector_t *) hash_get(&vector_hash, E2DBG_HOOK_GETRET);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  hosttype = elfsh_get_hosttype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == E2DBG_HOST_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "GETRET handler unexistant for this ARCH/OS", -1);
  
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  fct    = aspect_vectors_select(getret, dim);
  ret    = fct(addr);
  if (ret == 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "GetRet handler failed", (-1));
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}



/* Regvars handler for the IA32 architecture on BSD */
void		  e2dbg_get_regvars_ia32_bsd()
{ 
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
#if !defined(__amd64__) && defined(__FreeBSD__) || defined (__OpenBSD__) || defined(__NetBSD__)
  VM_GETREG(ELFSH_EAXVAR, e2dbgworld.curthread->context->uc_mcontext.mc_eax);
  VM_GETREG(ELFSH_EBXVAR, e2dbgworld.curthread->context->uc_mcontext.mc_ebx);
  VM_GETREG(ELFSH_ECXVAR, e2dbgworld.curthread->context->uc_mcontext.mc_ecx);
  VM_GETREG(ELFSH_EDXVAR, e2dbgworld.curthread->context->uc_mcontext.mc_edx);
  VM_GETREG(ELFSH_ESIVAR, e2dbgworld.curthread->context->uc_mcontext.mc_esi);
  VM_GETREG(ELFSH_EDIVAR, e2dbgworld.curthread->context->uc_mcontext.mc_edi);
  VM_GETREG(ELFSH_SPVAR , e2dbgworld.curthread->context->uc_mcontext.mc_esp);
  //VM_GETREG(ELFSH_SSPVAR, e2dbgworld.curthread->context->uc_mcontext.mc_ssp); /* XXX */
  VM_GETREG(ELFSH_FPVAR , e2dbgworld.curthread->context->uc_mcontext.mc_ebp);
  VM_GETREG(ELFSH_PCVAR , e2dbgworld.curthread->context->uc_mcontext.mc_eip);
#endif
  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Regvars handler for the IA32 architecture on SYSV */
void		  e2dbg_get_regvars_ia32_sysv()
{ 
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(__linux__) || defined(sun)
  VM_GETREG(ELFSH_EAXVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EAX]);
  VM_GETREG(ELFSH_EBXVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EBX]);
  VM_GETREG(ELFSH_ECXVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_ECX]);
  VM_GETREG(ELFSH_EDXVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EDX]);
  VM_GETREG(ELFSH_ESIVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_ESI]);
  VM_GETREG(ELFSH_EDIVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EDI]);
  VM_GETREG(ELFSH_SPVAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_ESP]);
  VM_GETREG(ELFSH_SSPVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_UESP]); 
  VM_GETREG(ELFSH_FPVAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EBP]);
  VM_GETREG(ELFSH_PCVAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EIP]);
#endif
  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/* Regvars handler for the IA32 architecture */
void		  e2dbg_set_regvars_ia32_bsd()
{ 
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
#if !defined(__amd64__) && defined(__FreeBSD__) || defined (__OpenBSD__) || defined(__NetBSD__)
  VM_SETREG(ELFSH_EAXVAR, e2dbgworld.curthread->context->uc_mcontext.mc_eax);
  VM_SETREG(ELFSH_EBXVAR, e2dbgworld.curthread->context->uc_mcontext.mc_ebx);
  VM_SETREG(ELFSH_ECXVAR, e2dbgworld.curthread->context->uc_mcontext.mc_ecx);
  VM_SETREG(ELFSH_EDXVAR, e2dbgworld.curthread->context->uc_mcontext.mc_edx);
  VM_SETREG(ELFSH_ESIVAR, e2dbgworld.curthread->context->uc_mcontext.mc_esi);
  VM_SETREG(ELFSH_EDIVAR, e2dbgworld.curthread->context->uc_mcontext.mc_edi);
  VM_SETREG(ELFSH_SPVAR , e2dbgworld.curthread->context->uc_mcontext.mc_esp);
  //VM_SETREG(ELFSH_SSPVAR, e2dbgworld.curthread->context->uc_mcontext.mc_ssp); /* XXX */
  VM_SETREG(ELFSH_FPVAR , e2dbgworld.curthread->context->uc_mcontext.mc_ebp);
  VM_SETREG(ELFSH_PCVAR , e2dbgworld.curthread->context->uc_mcontext.mc_eip);
#endif
  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}



/* Regvars handler for the IA32 architecture on SYSV */
void		  e2dbg_set_regvars_ia32_sysv()
{ 
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(__linux__) || defined(sun)
  VM_SETREG(ELFSH_EAXVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EAX]);
  VM_SETREG(ELFSH_EBXVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EBX]);
  VM_SETREG(ELFSH_ECXVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_ECX]);
  VM_SETREG(ELFSH_EDXVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EDX]);
  VM_SETREG(ELFSH_ESIVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_ESI]);
  VM_SETREG(ELFSH_EDIVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EDI]);
  VM_SETREG(ELFSH_SPVAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_ESP]);
  VM_SETREG(ELFSH_SSPVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_UESP]); 
  VM_SETREG(ELFSH_FPVAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EBP]);
  VM_SETREG(ELFSH_PCVAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EIP]);
#endif
  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/* Get Program Counter register */
elfsh_Addr*	  e2dbg_getpc_bsd_ia32()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
#if !defined(__amd64__) && defined(__FreeBSD__) || defined (__OpenBSD__) || defined(__NetBSD__)
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__,
		    &e2dbgworld.curthread->context->uc_mcontext.mc_eip);
#endif
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}

/* Get Program Counter register on Sys V */
elfsh_Addr*	  e2dbg_getpc_sysv_ia32()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(__linux__) || defined(sun)
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__,
		     (elfsh_Addr *)&e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EIP]);
#endif
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}



/* Get Program Counter register */
elfsh_Addr*	  e2dbg_getfp_bsd_ia32()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
#if !defined(__amd64__) && defined(__FreeBSD__) || defined (__OpenBSD__) || defined(__NetBSD__)
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__,
		    &e2dbgworld.curthread->context->uc_mcontext.mc_ebp);
#endif
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}

/* Get Program Counter register on Sys V */
elfsh_Addr*	  e2dbg_getfp_sysv_ia32()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(__linux__) || defined(sun)
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__,
		     (elfsh_Addr *)&e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EBP]);
#endif
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}


/* Enable stepping on BSD */
void		  e2dbg_setstep_bsd_ia32()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
#if !defined(__amd64__) && defined(__FreeBSD__) || defined (__OpenBSD__) || defined(__NetBSD__)
  e2dbgworld.curthread->context->uc_mcontext.mc_eflags |= 0x100;
#endif
  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Enable stepping on Sys V */
void		  e2dbg_setstep_sysv_ia32()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(__linux__) || defined(sun)
  e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EFL] |= 0x100; 
#endif
  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Disable stepping on Sys V */
void		  e2dbg_resetstep_sysv_ia32()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(__linux__) || defined(sun)
  e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EFL] &= ~0x100;
#endif
  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Disable stepping on BSD */
void		  e2dbg_resetstep_bsd_ia32()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
#if !defined(__amd64__) && defined(__FreeBSD__) || defined (__OpenBSD__) || defined(__NetBSD__)
  e2dbgworld.curthread->context->uc_mcontext.mc_eflags &= ~0x100;
#endif
  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}
