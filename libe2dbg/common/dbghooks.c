/**
** @file libe2dbg/common/dbghooks.c
** @brief Vectors for debugging features
** @ingroup common
** 
** Started   Sat Sep 24 07:17:33 2005 jfv
** $Id$
*/
#include "libe2dbg.h"


/* Void handlers for the 3 register-related vectors */
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

void		   e2dbg_default_printregs()
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

int		e2dbg_default_deletebreak_handler(elfshbp_t *bp)

{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}



/* Register a next frame-pointer handler */
int		e2dbg_register_nextfphook(u_char archtype, u_char hosttype, 
					  u_char ostype, void *(*fct)(void *frame))
{
  vector_t	*nextfp;
  u_int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  nextfp = aspect_vector_get(E2DBG_HOOK_NEXTFP);

  if (archtype >= ELFSH_ARCH_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (hosttype >= ELFSH_HOST_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OS_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);

  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(nextfp, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Register a next return-addr handler */
int		e2dbg_register_getrethook(u_char archtype, u_char hosttype, 
					  u_char ostype, void *fct)
{
  vector_t	*getret;
  u_int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  getret = aspect_vector_get(E2DBG_HOOK_GETRET);

  if (archtype >= ELFSH_ARCH_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (hosttype >= ELFSH_HOST_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OS_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);

  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(getret, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Register an SETREGS injection handler */
int	e2dbg_register_sregshook(u_char archtype, u_char hosttype, u_char ostype,
				 void *fct)
{
  vector_t	*setregs;
  u_int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  setregs = aspect_vector_get(E2DBG_HOOK_SETREGS);
  
  if (archtype >= ELFSH_ARCH_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (hosttype >= ELFSH_HOST_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Host type", -1);
  if (ostype >= ELFSH_OS_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);

  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(setregs, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Register an SETREGS injection handler */
int      e2dbg_register_gregshook(u_char archtype, u_char hosttype, u_char ostype,
				  void *fct)
{
  vector_t	*getregs;
  u_int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  getregs = aspect_vector_get(E2DBG_HOOK_GETREGS);

  if (archtype >= ELFSH_ARCH_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Architecture type", -1);
  if (hosttype >= ELFSH_HOST_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Host type", -1);
  if (ostype >= ELFSH_OS_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Operating System type", -1);

  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(getregs, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Register an PRINTREGS handler */
int		e2dbg_register_pregshook(u_char archtype, u_char hosttype, u_char ostype, void *fct)
{
  vector_t	*printregs;
  u_int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  printregs = aspect_vector_get(E2DBG_HOOK_PRINTREGS);
  
  if (archtype >= ELFSH_ARCH_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (hosttype >= ELFSH_HOST_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Host type", -1);
  if (ostype >= ELFSH_OS_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);

  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(printregs, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Register an GETPC injection handler */
int      e2dbg_register_getpchook(u_char archtype, u_char hosttype, u_char ostype,
				  void *fct)
{
  vector_t	*getpc;
  u_int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  getpc = aspect_vector_get(E2DBG_HOOK_GETPC);

  if (archtype >= ELFSH_ARCH_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Architecture type", -1);
  if (hosttype >= ELFSH_HOST_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Object type", -1);
  if (ostype >= ELFSH_OS_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Operating System type", -1);

  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(getpc, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Register an GETPC injection handler */
int      e2dbg_register_getfphook(u_char archtype, u_char hosttype, u_char ostype,
				  eresi_Addr *(*fct)(void))
{
  vector_t	*getfp;
  u_int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  getfp = aspect_vector_get(E2DBG_HOOK_GETFP);

  if (archtype >= ELFSH_ARCH_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Architecture type", -1);
  if (hosttype >= ELFSH_HOST_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Object type", -1);
  if (ostype >= ELFSH_OS_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Operating System type", -1);

  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(getfp, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Register an SETSTEP injection handler */
int      e2dbg_register_setstephook(u_char archtype, u_char hosttype, u_char ostype,
				    void *fct)
{
  vector_t	*setstep;
  u_int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  setstep = aspect_vector_get(E2DBG_HOOK_SETSTEP);

  if (archtype >= ELFSH_ARCH_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Architecture type", -1);
  if (hosttype >= ELFSH_HOST_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Object type", -1);
  if (ostype >= ELFSH_OS_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Operating System type", -1);

  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(setstep, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Register an RESETSTEP injection handler */
int      e2dbg_register_resetstephook(u_char archtype, u_char hosttype, u_char ostype,
				      void *fct)
{
  vector_t	*resetstep;
  u_int		*dim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  resetstep = aspect_vector_get(E2DBG_HOOK_RESETSTEP);
  
  if (archtype >= ELFSH_ARCH_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Architecture type", -1);
  if (hosttype >= ELFSH_HOST_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Object type", -1);
  if (ostype >= ELFSH_OS_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid Operating System type", -1);

  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(resetstep, dim, (unsigned long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Register a breakpoint redirection handler 
 */
int		e2dbg_register_breakhook(u_char archtype, u_char hosttype, 
					 u_char ostype, void *fct)
{
  vector_t	*breakp;
  u_int		dim[4];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  
  breakp = aspect_vector_get(E2DBG_HOOK_BREAK);

  if (archtype >= ELFSH_ARCH_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (hosttype >= ELFSH_HOST_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Host type", -1);
  if (ostype >= ELFSH_OS_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);
  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  aspect_vectors_insert(breakp, dim, (u_long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Register a breakpoint deletion handler 
 */
int		e2dbg_register_delbreakhook(u_char hosttype, void *fct)
{
  vector_t	*breakp;
  u_int		dim[2];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  
  breakp = aspect_vector_get(E2DBG_HOOK_DELBREAK);

  if (hosttype >= ELFSH_HOST_NUM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Host type", -1);
  dim[0] = hosttype;
  aspect_vectors_insert(breakp, dim, (u_long) fct);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Initialize libe2dbg.hook vectors */
static int	e2dbg_register_vectors()
{
  u_int		*dims;
  char		**strdims;
  u_int		*adims;
  char		**astrdims;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* All hooks have the same dimensions here */
  XALLOC(__FILE__, __FUNCTION__, __LINE__,dims   , 4 * sizeof(u_int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,strdims, 4 * sizeof(char *), -1);
  dims[0]    = ELFSH_ARCH_NUM;
  dims[1]    = ELFSH_HOST_NUM;
  dims[2]    = ELFSH_OS_NUM;
  strdims[0] = "ARCHTYPE";
  strdims[1] = "HOSTYPE";
  strdims[2] = "OSTYPE";
  
  /* Except the breakpoint deletion hook */
  XALLOC(__FILE__, __FUNCTION__, __LINE__, adims   , 2 * sizeof(u_int) , -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, astrdims, 2 * sizeof(char *), -1);
  adims[0]    = ELFSH_HOST_NUM;
  astrdims[0] = "HOSTYPE";

  /* Initialize debugger vectors */
  aspect_register_vector(E2DBG_HOOK_GETREGS, 
			 e2dbg_default_getregs,
			 dims, strdims, 3, ASPECT_TYPE_CADDR);
  aspect_register_vector(E2DBG_HOOK_SETREGS, 
			 e2dbg_default_setregs,
			 dims, strdims, 3, ASPECT_TYPE_CADDR);
  aspect_register_vector(E2DBG_HOOK_PRINTREGS, 
			 e2dbg_default_printregs,
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
  aspect_register_vector(E2DBG_HOOK_DELBREAK, 
			 e2dbg_default_deletebreak_handler, 
			 adims, astrdims, 1, ASPECT_TYPE_CADDR);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Setup internal VM hooks */
void		e2dbg_setup_hooks()
{
  static Bool	done = FALSE;

  /* Check if already initialized */
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (done)
    PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
  aspect_init();
  e2dbg_register_vectors();
  done = TRUE;
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
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

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  getregs = aspect_vector_get(E2DBG_HOOK_GETREGS);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(world.curjob->curfile);
  hosttype = elfsh_get_hosttype(world.curjob->curfile);
  ostype = elfsh_get_ostype(world.curjob->curfile);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == ELFSH_HOST_ERROR ||
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
  u_char        archtype;
  u_char        hosttype;
  u_char        ostype;
  u_int		dim[3];
  vector_t	*setregs;
  void		(*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  setregs = aspect_vector_get(E2DBG_HOOK_SETREGS);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(world.curjob->curfile);
  hosttype = elfsh_get_hosttype(world.curjob->curfile);
  ostype = elfsh_get_ostype(world.curjob->curfile);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == ELFSH_HOST_ERROR ||
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


/* Call the setregs hook */
int		e2dbg_printregs()
{
  u_char        archtype;
  u_char        hosttype;
  u_char        ostype;
  u_int		dim[3];
  vector_t	*printregs;
  void		(*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  printregs = aspect_vector_get(E2DBG_HOOK_PRINTREGS);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(world.curjob->curfile);
  hosttype = elfsh_get_hosttype(world.curjob->curfile);
  ostype = elfsh_get_ostype(world.curjob->curfile);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == ELFSH_HOST_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "PRINTREGS handler unexistant for this ARCH/OS", -1);

  dim[0] = archtype;
  dim[1] = hosttype;
  dim[2] = ostype;
  fct    = aspect_vectors_select(printregs, dim);
 
  /* This hook is non-fatal, we just wont have printed registers if it fails. */
  fct();
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Call the getpc hook */
eresi_Addr*     e2dbg_getpc()
{
  eresi_Addr	*pc;
  u_char        archtype;
  u_char        hosttype;
  u_char        ostype;
  u_int		dim[3];
  vector_t	*getpc;
  eresi_Addr	*(*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  getpc = aspect_vector_get(E2DBG_HOOK_GETPC);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(world.curjob->curfile);
  hosttype = elfsh_get_hosttype(world.curjob->curfile);
  ostype = elfsh_get_ostype(world.curjob->curfile);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == ELFSH_HOST_ERROR ||
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
  u_char        archtype;
  u_char        hosttype;
  u_char        ostype;
  u_int		dim[3];
  vector_t	*getfp;
  eresi_Addr	*(*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  getfp = aspect_vector_get(E2DBG_HOOK_GETFP);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(world.curjob->curfile);
  hosttype = elfsh_get_hosttype(world.curjob->curfile);
  ostype = elfsh_get_ostype(world.curjob->curfile);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == ELFSH_HOST_ERROR ||
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
  u_char        archtype;
  u_char        hosttype;
  u_char        ostype;
  u_int		dim[3];
  vector_t	*setstep;
  void		(*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  setstep = aspect_vector_get(E2DBG_HOOK_SETSTEP);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(world.curjob->curfile);
  hosttype = elfsh_get_hosttype(world.curjob->curfile);
  ostype = elfsh_get_ostype(world.curjob->curfile);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == ELFSH_HOST_ERROR ||
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
  u_char        archtype;
  u_char        hosttype;
  u_char        ostype;
  u_int		dim[3];
  vector_t	*resetstep;
  void		(*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  resetstep = aspect_vector_get(E2DBG_HOOK_RESETSTEP);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(world.curjob->curfile);
  hosttype = elfsh_get_hosttype(world.curjob->curfile);
  ostype = elfsh_get_ostype(world.curjob->curfile);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == ELFSH_HOST_ERROR ||
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
  u_char        archtype;
  u_char        hosttype;
  u_char        ostype;
  int		ret;
  u_int		dim[3];
  vector_t	*nextfp;
  int		(*fct)(eresi_Addr addr);

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  nextfp = aspect_vector_get(E2DBG_HOOK_NEXTFP);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  hosttype = elfsh_get_hosttype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == ELFSH_HOST_ERROR ||
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
  u_char        archtype;
  u_char        hosttype;
  u_char        ostype;
  int		ret;
  u_int		dim[3];
  vector_t	*getret;
  int		(*fct)(eresi_Addr addr);

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  getret = aspect_vector_get(E2DBG_HOOK_GETRET);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  hosttype = elfsh_get_hosttype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == ELFSH_HOST_ERROR ||
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
  u_char        archtype;
  u_char        hosttype;
  u_char        ostype;
  int		ret;
  int		(*fct)(elfshobj_t *file, elfshbp_t *bp);
  u_int		dim[3];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  breakh = aspect_vector_get(E2DBG_HOOK_BREAK);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  hosttype = elfsh_get_hosttype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      hosttype == ELFSH_HOST_ERROR ||
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



/**
 * Call the breakpoint deletion hook 
 */
int	        e2dbg_deletebreak(elfshbp_t *bp)
{
  vector_t	*breakh;
  u_char        hosttype;
  int		ret;
  int		(*fct)(elfshbp_t *bp);
  u_int		dim[1];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  breakh = aspect_vector_get(E2DBG_HOOK_DELBREAK);

  /* Fingerprint binary */
  hosttype = elfsh_get_hosttype(bp->obj);
  if (hosttype == ELFSH_HOST_ERROR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "DELBREAK handler unexistant for this ARCH/OS", -1);
  
  dim[0] = hosttype;
  fct    = aspect_vectors_select(breakh, dim);
  ret  = fct(bp);
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Breakpoint deletion handler failed", (-1));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
