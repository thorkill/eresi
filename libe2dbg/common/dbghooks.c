/*
** dbghooks.c for e2dbg in the ERESI framework
**
** The interface hooking system
**
** Those hooks stay in the debugger and not in libelfsh.
** 
** Started   Sat Sep 24 07:17:33 2005 jfv
** $Id: dbghooks.c,v 1.1 2008-02-16 12:32:27 thor Exp $
*/
#include "libe2dbg.h"


/* Void handlers for the 2 hooks */
void		  e2dbg_default_getregs()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__); 
}

void		  e2dbg_default_setregs()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__); 
}

/* Error handler by default for them */
eresi_Addr*	  e2dbg_default_getpc()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "GETPC unimplemented on this architecture", NULL);
}

eresi_Addr*	  e2dbg_default_getfp()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "GETFP unimplemented on this architecture", NULL);
}

void		e2dbg_default_setstep()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_RERR(__FILE__, __FUNCTION__, __LINE__, 
		     "SETSTEP unimplemented on this architecture");
}

void		e2dbg_default_resetstep()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_RERR(__FILE__, __FUNCTION__, __LINE__, 
		     "RESETSTEP unimplemented on this architecture"); 
}

eresi_Addr	e2dbg_default_nextfphandler(eresi_Addr a)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}

eresi_Addr	e2dbg_default_getrethandler(eresi_Addr a)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}

int		e2dbg_default_breakhandler(elfshobj_t   *null,
					   elfshbp_t	*null3)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}



/* Register a next frame-pointer handler */
int		e2dbg_register_nextfphook(unsigned char archtype, unsigned char hosttype, 
					  unsigned char ostype, void *fct)
{
  vector_t	*nextfp;
  unsigned int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  nextfp = aspect_vector_get(E2DBG_HOOK_NEXTFP);

  if (archtype >= ELFSH_ARCHNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (hosttype >= E2DBG_HOSTNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);

  //hook_nextfp[archtype][hosttype][ostype] = fct;
  dim    = alloca(sizeof(unsigned int) * 4);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(nextfp, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Register a next return-addr handler */
int		e2dbg_register_getrethook(unsigned char archtype, unsigned char hosttype, 
					  unsigned char ostype, void *fct)
{
  vector_t	*getret;
  unsigned int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  getret = aspect_vector_get(E2DBG_HOOK_GETRET);

  if (archtype >= ELFSH_ARCHNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (hosttype >= E2DBG_HOSTNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);

  dim    = alloca(sizeof(unsigned int) * 4);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(getret, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Register an SETREGS injection handler */
int	e2dbg_register_sregshook(unsigned char archtype, unsigned char hosttype, unsigned char ostype,
				 void *fct)
{
  vector_t	*setregs;
  unsigned int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  setregs = aspect_vector_get(E2DBG_HOOK_SETREGS);
  
  if (archtype >= ELFSH_ARCHNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (hosttype >= E2DBG_HOSTNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);

  dim    = alloca(sizeof(unsigned int) * 4);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(setregs, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Register an SETREGS injection handler */
int      e2dbg_register_gregshook(unsigned char archtype, unsigned char hosttype, unsigned char ostype,
				 void *fct)
{
  vector_t	*getregs;
  unsigned int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  getregs = aspect_vector_get(E2DBG_HOOK_GETREGS);

  if (archtype >= ELFSH_ARCHNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Architecture type", -1);
  if (hosttype >= E2DBG_HOSTNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Operating System type", -1);

  dim    = alloca(sizeof(unsigned int) * 4);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(getregs, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Register an GETPC injection handler */
int      e2dbg_register_getpchook(unsigned char archtype, unsigned char hosttype, unsigned char ostype,
				  void *fct)
{
  vector_t	*getpc;
  unsigned int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  getpc = aspect_vector_get(E2DBG_HOOK_GETPC);

  if (archtype >= ELFSH_ARCHNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Architecture type", -1);
  if (hosttype >= E2DBG_HOSTNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Operating System type", -1);

  dim    = alloca(sizeof(unsigned int) * 4);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(getpc, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Register an GETPC injection handler */
int      e2dbg_register_getfphook(unsigned char archtype, unsigned char hosttype, unsigned char ostype,
				  void *fct)
{
  vector_t	*getfp;
  unsigned int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  getfp = aspect_vector_get(E2DBG_HOOK_GETFP);

  if (archtype >= ELFSH_ARCHNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Architecture type", -1);
  if (hosttype >= E2DBG_HOSTNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Operating System type", -1);

  dim    = alloca(sizeof(unsigned int) * 4);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(getfp, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Register an SETSTEP injection handler */
int      e2dbg_register_setstephook(unsigned char archtype, unsigned char hosttype, unsigned char ostype,
				    void *fct)
{
  vector_t	*setstep;
  unsigned int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  setstep = aspect_vector_get(E2DBG_HOOK_SETSTEP);

  if (archtype >= ELFSH_ARCHNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Architecture type", -1);
  if (hosttype >= E2DBG_HOSTNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Operating System type", -1);

  dim    = alloca(sizeof(unsigned int) * 4);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(setstep, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Register an RESETSTEP injection handler */
int      e2dbg_register_resetstephook(unsigned char archtype, unsigned char hosttype, unsigned char ostype,
				      void *fct)
{
  vector_t	*resetstep;
  unsigned int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  resetstep = aspect_vector_get(E2DBG_HOOK_RESETSTEP);
  
  if (archtype >= ELFSH_ARCHNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Architecture type", -1);
  if (hosttype >= E2DBG_HOSTNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Operating System type", -1);

  dim    = alloca(sizeof(unsigned int) * 4);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(resetstep, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Register a breakpoint redirection handler 
 */
int		e2dbg_register_breakhook(unsigned char archtype, unsigned char hosttype, 
					 unsigned char ostype, void *fct)
{
  vector_t	*breakp;
  unsigned int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  
  breakp = aspect_vector_get(E2DBG_HOOK_BREAK);

  if (archtype >= ELFSH_ARCHNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (hosttype >= E2DBG_HOSTNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Host type", -1);
  if (ostype >= ELFSH_OSNUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);

  dim = alloca(sizeof(unsigned int) * 4);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(breakp, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Initialize libe2dbg.hook vectors */
static int	e2dbg_register_vectors()
{
  unsigned int		*dims;
  char		**strdims;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* All hooks have the same dimensions here */
  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 4 * sizeof(unsigned int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,strdims, 4 * sizeof(char *), -1);
  dims[0]    = ELFSH_ARCHNUM;
  dims[1]    = E2DBG_HOSTNUM;
  dims[2]    = ELFSH_OSNUM;
  strdims[0] = "ARCHTYPE";
  strdims[1] = "HOSTYPE";
  strdims[2] = "OSTYPE";

  /* Initialize debugger vectors */
  aspect_register_vector(E2DBG_HOOK_GETREGS, 
			 e2dbg_default_getregs,
			 dims, strdims, 3, ASPECT_TYPE_CADDR);
  aspect_register_vector(E2DBG_HOOK_SETREGS, 
			 e2dbg_default_setregs,
			 dims, strdims, 3, ASPECT_TYPE_CADDR);
  aspect_register_vector(E2DBG_HOOK_GETPC, 
			 e2dbg_default_getpc,
			 dims, strdims, 3, ASPECT_TYPE_CADDR);
  aspect_register_vector(E2DBG_HOOK_SETSTEP, 
			 e2dbg_default_setstep,
			 dims, strdims, 3, ASPECT_TYPE_CADDR);
  aspect_register_vector(E2DBG_HOOK_RESETSTEP, 
			 e2dbg_default_resetstep,
			 dims, strdims, 3, ASPECT_TYPE_CADDR);
  aspect_register_vector(E2DBG_HOOK_GETFP, 
			 e2dbg_default_getfp,
			 dims, strdims, 3, ASPECT_TYPE_CADDR);
  aspect_register_vector(E2DBG_HOOK_NEXTFP, 
			 e2dbg_default_nextfphandler,
			 dims, strdims, 3, ASPECT_TYPE_CADDR);
  aspect_register_vector(E2DBG_HOOK_GETRET, 
			 e2dbg_default_getrethandler,
			 dims, strdims, 3, ASPECT_TYPE_CADDR);
  aspect_register_vector(E2DBG_HOOK_BREAK, 
			 e2dbg_default_breakhandler, 
			 dims, strdims, 3, ASPECT_TYPE_CADDR);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Setup internal VM hooks */
void		e2dbg_setup_hooks()
{
  static int	done = 0;

  /* Check if already initialized */
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (done)
    PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
  aspect_init();
  e2dbg_register_vectors();

  /* Linux / AMD64 target */
  e2dbg_register_gregshook(ELFSH_ARCH_AMD64, E2DBG_HOST_PROC, 
			   ELFSH_OS_LINUX, e2dbg_get_regvars_amd64_sysv);
  e2dbg_register_sregshook(ELFSH_ARCH_AMD64, E2DBG_HOST_PROC, 
			   ELFSH_OS_LINUX, e2dbg_set_regvars_amd64_sysv);
  e2dbg_register_getpchook(ELFSH_ARCH_AMD64, E2DBG_HOST_PROC, 
			   ELFSH_OS_LINUX, e2dbg_getpc_sysv_amd64);
  e2dbg_register_setstephook(ELFSH_ARCH_AMD64, E2DBG_HOST_PROC, 
			     ELFSH_OS_LINUX, e2dbg_setstep_sysv_amd64);
  e2dbg_register_resetstephook(ELFSH_ARCH_AMD64, E2DBG_HOST_PROC, 
			       ELFSH_OS_LINUX, e2dbg_resetstep_sysv_amd64);
  e2dbg_register_nextfphook(ELFSH_ARCH_AMD64, ELFSH_TYPE_DYN, 
			    ELFSH_OS_LINUX, e2dbg_bt_amd64);
  e2dbg_register_getfphook(ELFSH_ARCH_AMD64, ELFSH_TYPE_DYN, 
			   ELFSH_OS_LINUX, e2dbg_getfp_sysv_amd64);
  e2dbg_register_getrethook(ELFSH_ARCH_AMD64, ELFSH_TYPE_DYN, 
			    ELFSH_OS_LINUX, e2dbg_getret_amd64);
  e2dbg_register_breakhook(ELFSH_ARCH_AMD64, ELFSH_TYPE_DYN,   
			   ELFSH_OS_LINUX, e2dbg_break_amd64);  

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


  /* For sparc32 */

  e2dbg_register_gregshook(ELFSH_ARCH_SPARC32, E2DBG_HOST_PROC, 
			   ELFSH_OS_FREEBSD, e2dbg_get_regvars_sparc32_bsd);
  e2dbg_register_gregshook(ELFSH_ARCH_SPARC32, E2DBG_HOST_PROC, 
			   ELFSH_OS_NETBSD, e2dbg_get_regvars_sparc32_bsd);
  e2dbg_register_gregshook(ELFSH_ARCH_SPARC32, E2DBG_HOST_PROC, 
			   ELFSH_OS_OPENBSD, e2dbg_get_regvars_sparc32_bsd);
  e2dbg_register_gregshook(ELFSH_ARCH_SPARC32, E2DBG_HOST_PROC, 
			   ELFSH_OS_LINUX, e2dbg_get_regvars_sparc32_sysv);
  e2dbg_register_gregshook(ELFSH_ARCH_SPARC32, E2DBG_HOST_PROC, 
			   ELFSH_OS_SOLARIS, e2dbg_get_regvars_sparc32_sysv);


  e2dbg_register_sregshook(ELFSH_ARCH_SPARC32, E2DBG_HOST_PROC, 
			   ELFSH_OS_FREEBSD, e2dbg_set_regvars_sparc32_bsd);
  e2dbg_register_sregshook(ELFSH_ARCH_SPARC32, E2DBG_HOST_PROC, 
			   ELFSH_OS_NETBSD, e2dbg_set_regvars_sparc32_bsd);
  e2dbg_register_sregshook(ELFSH_ARCH_SPARC32, E2DBG_HOST_PROC, 
			   ELFSH_OS_OPENBSD, e2dbg_set_regvars_sparc32_bsd);
  e2dbg_register_sregshook(ELFSH_ARCH_SPARC32, E2DBG_HOST_PROC, 
			   ELFSH_OS_LINUX, e2dbg_set_regvars_sparc32_sysv);
  e2dbg_register_sregshook(ELFSH_ARCH_SPARC32, E2DBG_HOST_PROC, 
			   ELFSH_OS_SOLARIS, e2dbg_set_regvars_sparc32_sysv);


  e2dbg_register_getpchook(ELFSH_ARCH_SPARC32, E2DBG_HOST_PROC, 
			   ELFSH_OS_FREEBSD, e2dbg_getpc_bsd_sparc32);
  e2dbg_register_getpchook(ELFSH_ARCH_SPARC32, E2DBG_HOST_PROC, 
			   ELFSH_OS_NETBSD, e2dbg_getpc_bsd_sparc32);
  e2dbg_register_getpchook(ELFSH_ARCH_SPARC32, E2DBG_HOST_PROC, 
			   ELFSH_OS_OPENBSD, e2dbg_getpc_bsd_sparc32);
  e2dbg_register_getpchook(ELFSH_ARCH_SPARC32, E2DBG_HOST_PROC, 
			   ELFSH_OS_LINUX, e2dbg_getpc_sysv_sparc32);
  e2dbg_register_getpchook(ELFSH_ARCH_SPARC32, E2DBG_HOST_PROC, 
			   ELFSH_OS_SOLARIS, e2dbg_getpc_sysv_sparc32);
  
  e2dbg_register_setstephook(ELFSH_ARCH_SPARC32, E2DBG_HOST_PROC, 
			     ELFSH_OS_FREEBSD, e2dbg_setstep_bsd_sparc32);
  e2dbg_register_setstephook(ELFSH_ARCH_SPARC32, E2DBG_HOST_PROC, 
			     ELFSH_OS_NETBSD, e2dbg_setstep_bsd_sparc32);
  e2dbg_register_setstephook(ELFSH_ARCH_SPARC32, E2DBG_HOST_PROC, 
			     ELFSH_OS_OPENBSD, e2dbg_setstep_bsd_sparc32);
  e2dbg_register_setstephook(ELFSH_ARCH_SPARC32, E2DBG_HOST_PROC, 
			     ELFSH_OS_LINUX, e2dbg_setstep_sysv_sparc32);
  e2dbg_register_setstephook(ELFSH_ARCH_SPARC32, E2DBG_HOST_PROC, 
			     ELFSH_OS_SOLARIS, e2dbg_setstep_sysv_sparc32);
  
  e2dbg_register_resetstephook(ELFSH_ARCH_SPARC32, E2DBG_HOST_PROC, 
			       ELFSH_OS_FREEBSD, e2dbg_resetstep_bsd_sparc32);
  e2dbg_register_resetstephook(ELFSH_ARCH_SPARC32, E2DBG_HOST_PROC, 
			       ELFSH_OS_NETBSD, e2dbg_resetstep_bsd_sparc32);
  e2dbg_register_resetstephook(ELFSH_ARCH_SPARC32, E2DBG_HOST_PROC, 
			       ELFSH_OS_OPENBSD, e2dbg_resetstep_bsd_sparc32);
  e2dbg_register_resetstephook(ELFSH_ARCH_SPARC32, E2DBG_HOST_PROC, 
			       ELFSH_OS_LINUX, e2dbg_resetstep_sysv_sparc32);
  e2dbg_register_resetstephook(ELFSH_ARCH_SPARC32, E2DBG_HOST_PROC, 
			       ELFSH_OS_SOLARIS, e2dbg_resetstep_sysv_sparc32);


  /***********************************/
  /* Now nextfp hooks                */
  /***********************************/
  
  /* Usual nextfp targets for ET_EXEC/ET_DYN on IA32 */
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_LINUX, e2dbg_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_FREEBSD, e2dbg_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_NETBSD, e2dbg_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_OPENBSD, e2dbg_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_SOLARIS, e2dbg_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_LINUX, e2dbg_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_FREEBSD, e2dbg_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_NETBSD, e2dbg_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_OPENBSD, e2dbg_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_SOLARIS, e2dbg_bt_ia32);
  
  /* Now for sparc */
  e2dbg_register_nextfphook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_LINUX, e2dbg_bt_sparc32);
  e2dbg_register_nextfphook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_FREEBSD, e2dbg_bt_sparc32);
  e2dbg_register_nextfphook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_NETBSD, e2dbg_bt_sparc32);
  e2dbg_register_nextfphook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_OPENBSD, e2dbg_bt_sparc32);
  e2dbg_register_nextfphook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_SOLARIS, e2dbg_bt_sparc32);
  e2dbg_register_nextfphook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_LINUX, e2dbg_bt_sparc32);
  e2dbg_register_nextfphook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_FREEBSD, e2dbg_bt_sparc32);
  e2dbg_register_nextfphook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_NETBSD, e2dbg_bt_sparc32);
  e2dbg_register_nextfphook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_OPENBSD, e2dbg_bt_sparc32);
  e2dbg_register_nextfphook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_SOLARIS, e2dbg_bt_sparc32);



  /************************************/
  /* Now getfp hooks	              */
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

  /* Now for sparc */
  e2dbg_register_getfphook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_LINUX, e2dbg_getfp_sysv_sparc32);
  e2dbg_register_getfphook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_FREEBSD, e2dbg_getfp_bsd_sparc32);
  e2dbg_register_getfphook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_NETBSD, e2dbg_getfp_bsd_sparc32);
  e2dbg_register_getfphook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_OPENBSD, e2dbg_getfp_bsd_sparc32);
  e2dbg_register_getfphook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_SOLARIS, e2dbg_getfp_sysv_sparc32);
  e2dbg_register_getfphook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_LINUX, e2dbg_getfp_sysv_sparc32);
  e2dbg_register_getfphook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_FREEBSD, e2dbg_getfp_bsd_sparc32);
  e2dbg_register_getfphook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_NETBSD, e2dbg_getfp_bsd_sparc32);
  e2dbg_register_getfphook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_OPENBSD, e2dbg_getfp_bsd_sparc32);
  e2dbg_register_getfphook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_SOLARIS, e2dbg_getfp_sysv_sparc32);


  /***********************************/
  /* Now register getret hooks       */
  /***********************************/
  
  /* Usual getret targets for ET_EXEC/IA32 */
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_LINUX, e2dbg_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_FREEBSD, e2dbg_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_NETBSD, e2dbg_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_OPENBSD, e2dbg_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_SOLARIS, e2dbg_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_LINUX, e2dbg_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_FREEBSD, e2dbg_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_NETBSD, e2dbg_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_OPENBSD, e2dbg_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_SOLARIS, e2dbg_getret_ia32);
  
  /* Now for sparc32 */
  e2dbg_register_getrethook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_LINUX, e2dbg_getret_sparc32);
  e2dbg_register_getrethook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_FREEBSD, e2dbg_getret_sparc32);
  e2dbg_register_getrethook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_NETBSD, e2dbg_getret_sparc32);
  e2dbg_register_getrethook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_OPENBSD, e2dbg_getret_sparc32);
  e2dbg_register_getrethook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_SOLARIS, e2dbg_getret_sparc32);
  e2dbg_register_getrethook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_LINUX, e2dbg_getret_sparc32);
  e2dbg_register_getrethook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_FREEBSD, e2dbg_getret_sparc32);
  e2dbg_register_getrethook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_NETBSD, e2dbg_getret_sparc32);
  e2dbg_register_getrethook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_OPENBSD, e2dbg_getret_sparc32);
  e2dbg_register_getrethook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_SOLARIS, e2dbg_getret_sparc32);

  /***********************************/
  /* Now register breakpoint hooks   */
  /***********************************/
  e2dbg_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC,   
			   ELFSH_OS_LINUX, e2dbg_break_ia32);  
  e2dbg_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC,   
			   ELFSH_OS_FREEBSD, e2dbg_break_ia32);  
  e2dbg_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC,   
			   ELFSH_OS_NETBSD, e2dbg_break_ia32);  
  e2dbg_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC,   
			   ELFSH_OS_OPENBSD, e2dbg_break_ia32);  
  e2dbg_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC,   
			   ELFSH_OS_SOLARIS, e2dbg_break_ia32);  
  e2dbg_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN,   
			   ELFSH_OS_LINUX, e2dbg_break_ia32);  
  e2dbg_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN,   
			   ELFSH_OS_FREEBSD, e2dbg_break_ia32);  
  e2dbg_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN,   
			   ELFSH_OS_NETBSD, e2dbg_break_ia32);  
  e2dbg_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN,   
			   ELFSH_OS_OPENBSD, e2dbg_break_ia32);  
  e2dbg_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN,   
			   ELFSH_OS_SOLARIS, e2dbg_break_ia32);

  /* Now for sparc */
  e2dbg_register_breakhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC,   
			   ELFSH_OS_LINUX, e2dbg_break_sparc32);  
  e2dbg_register_breakhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC,   
			   ELFSH_OS_FREEBSD, e2dbg_break_sparc32);  
  e2dbg_register_breakhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC,   
			   ELFSH_OS_NETBSD, e2dbg_break_sparc32);  
  e2dbg_register_breakhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC,   
			   ELFSH_OS_OPENBSD, e2dbg_break_sparc32);  
  e2dbg_register_breakhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC,   
			   ELFSH_OS_SOLARIS, e2dbg_break_sparc32);  
  e2dbg_register_breakhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_DYN,   
			   ELFSH_OS_LINUX, e2dbg_break_sparc32);  
  e2dbg_register_breakhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_DYN,   
			   ELFSH_OS_FREEBSD, e2dbg_break_sparc32);  
  e2dbg_register_breakhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_DYN,   
			   ELFSH_OS_NETBSD, e2dbg_break_sparc32);  
  e2dbg_register_breakhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_DYN,   
			   ELFSH_OS_OPENBSD, e2dbg_break_sparc32);  
  e2dbg_register_breakhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_DYN,   
			   ELFSH_OS_SOLARIS, e2dbg_break_sparc32);
  
  done = 1;
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}



/* Call the getregs hook */
int		  e2dbg_getregs()
{
  unsigned char        archtype;
  unsigned char        hosttype;
  unsigned char        ostype;
  unsigned int		dim[3];
  vector_t	*getregs;
  void		(*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  getregs = aspect_vector_get(E2DBG_HOOK_GETREGS);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(world.curjob->curfile);
  hosttype = elfsh_get_hosttype(world.curjob->curfile);
  ostype = elfsh_get_ostype(world.curjob->curfile);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == E2DBG_HOST_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "GETREGS handler unexistant for this ARCH/OS", -1);

  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  fct    = aspect_vectors_select(getregs, dim);

  /* This hook is non-fatal, we just wont have regs as variables if it fails */
  fct();
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Call the setregs hook */
int		  e2dbg_setregs()
{
  unsigned char        archtype;
  unsigned char        hosttype;
  unsigned char        ostype;
  unsigned int		dim[3];
  vector_t	*setregs;
  void		(*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  setregs = aspect_vector_get(E2DBG_HOOK_SETREGS);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(world.curjob->curfile);
  hosttype = elfsh_get_hosttype(world.curjob->curfile);
  ostype = elfsh_get_ostype(world.curjob->curfile);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == E2DBG_HOST_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "SETREGS handler unexistant for this ARCH/OS", -1);

  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  fct    = aspect_vectors_select(setregs, dim);
 
  /* This hook is non-fatal, we just wont have modified registers if it fails. */
  fct();
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Call the getpc hook */
eresi_Addr*     e2dbg_getpc()
{
  eresi_Addr	*pc;
  unsigned char        archtype;
  unsigned char        hosttype;
  unsigned char        ostype;
  unsigned int		dim[3];
  vector_t	*getpc;
  eresi_Addr	*(*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  getpc = aspect_vector_get(E2DBG_HOOK_GETPC);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(world.curjob->curfile);
  hosttype = elfsh_get_hosttype(world.curjob->curfile);
  ostype = elfsh_get_ostype(world.curjob->curfile);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == E2DBG_HOST_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "GETPC handler unexistant for this ARCH/OS", NULL);
  
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  fct    = aspect_vectors_select(getpc, dim);
  pc     = fct();
  if (pc == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Failed to get program counter register", NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, pc);
}


/* Call the getfp hook */
eresi_Addr*     e2dbg_getfp()
{
  eresi_Addr	*fp;
  unsigned char        archtype;
  unsigned char        hosttype;
  unsigned char        ostype;
  unsigned int		dim[3];
  vector_t	*getfp;
  eresi_Addr	*(*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  getfp = aspect_vector_get(E2DBG_HOOK_GETFP);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(world.curjob->curfile);
  hosttype = elfsh_get_hosttype(world.curjob->curfile);
  ostype = elfsh_get_ostype(world.curjob->curfile);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == E2DBG_HOST_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "GETPC handler unexistant for this ARCH/OS", NULL);
  
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  fct    = aspect_vectors_select(getfp, dim);
  fp     = fct();
  if (fp == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Failed to get program counter register", NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, fp);
}


/* Call the getregs hook */
int		  e2dbg_setstep()
{
  unsigned char        archtype;
  unsigned char        hosttype;
  unsigned char        ostype;
  unsigned int		dim[3];
  vector_t	*setstep;
  void		(*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  setstep = aspect_vector_get(E2DBG_HOOK_SETSTEP);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(world.curjob->curfile);
  hosttype = elfsh_get_hosttype(world.curjob->curfile);
  ostype = elfsh_get_ostype(world.curjob->curfile);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == E2DBG_HOST_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "SETSTEP handler unexistant for this ARCH/OS", -1);
  
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  fct    = aspect_vectors_select(setstep, dim);
  fct();
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Call the getregs hook */
int		  e2dbg_resetstep()
{
  unsigned char        archtype;
  unsigned char        hosttype;
  unsigned char        ostype;
  unsigned int		dim[3];
  vector_t	*resetstep;
  void		(*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  resetstep = aspect_vector_get(E2DBG_HOOK_RESETSTEP);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(world.curjob->curfile);
  hosttype = elfsh_get_hosttype(world.curjob->curfile);
  ostype = elfsh_get_ostype(world.curjob->curfile);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == E2DBG_HOST_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "GETPC handler unexistant for this ARCH/OS", -1);
  
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  fct    = aspect_vectors_select(resetstep, dim);
  fct();
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Call the getregs hook */
eresi_Addr	e2dbg_nextfp(elfshobj_t *file, eresi_Addr addr)
{
  unsigned char        archtype;
  unsigned char        hosttype;
  unsigned char        ostype;
  int		ret;
  unsigned int		dim[3];
  vector_t	*nextfp;
  int		(*fct)(eresi_Addr addr);

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  nextfp = aspect_vector_get(E2DBG_HOOK_NEXTFP);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  hosttype = elfsh_get_hosttype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == E2DBG_HOST_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "BACKTRACE (nextfp) handler unexistant for this ARCH/OS", 
		      -1);

  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  fct    = aspect_vectors_select(nextfp, dim);
  ret = fct(addr);
  if (ret == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Backtrace handler failed", (-1));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/* Call the getregs hook */
eresi_Addr	e2dbg_getret(elfshobj_t *file, eresi_Addr addr)
{
  unsigned char        archtype;
  unsigned char        hosttype;
  unsigned char        ostype;
  int		ret;
  unsigned int		dim[3];
  vector_t	*getret;
  int		(*fct)(eresi_Addr addr);

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  getret = aspect_vector_get(E2DBG_HOOK_GETRET);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  hosttype = elfsh_get_hosttype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == E2DBG_HOST_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "GETRET handler unexistant for this ARCH/OS", -1);
  
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  fct    = aspect_vectors_select(getret, dim);
  ret    = fct(addr);
  if (ret == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "GetRet handler failed", (-1));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}





/**
 * Call the breakpoint hook 
 */
int		  e2dbg_setbreak(elfshobj_t *file, elfshbp_t *bp)
{
  vector_t	*breakh;
  unsigned char        archtype;
  unsigned char        hosttype;
  unsigned char        ostype;
  int		ret;
  int		(*fct)(elfshobj_t *file, elfshbp_t *bp);
  unsigned int		dim[3];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  breakh = aspect_vector_get(E2DBG_HOOK_BREAK);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  hosttype = elfsh_get_hosttype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == E2DBG_HOST_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "SETBREAK handler unexistant for this ARCH/OS", -1);
  
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  fct    = aspect_vectors_select(breakh, dim);
  ret  = fct(file, bp);
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Breakpoint handler failed", (-1));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
