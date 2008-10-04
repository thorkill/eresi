/*
** dbg-ia32.c for e2dbg in the ERESI framework
**
** Debugger backend for the IA32 architecture  
**
** Those hooks stay in the debugger and not in libelfsh.
**
** Vectors can also be accessed with 'vectors' cmd in any eresi interpreter 
** 
** Started   Sat Sep 24 07:17:33 2005 jfv
** $Id: dbg-ia32.c,v 1.1 2008-02-16 12:32:27 thor Exp $
*/
#include "libe2dbg.h"






/* Regvars handler for the IA32 architecture on BSD */
void		  e2dbg_get_regvars_ia32_bsd()
{ 
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if !defined(sun) && !defined(__amd64__) && defined(__FreeBSD__) //|| defined (__OpenBSD__)
  E2DBG_GETREG(E2DBG_EAX_VAR, e2dbgworld.curthread->context->uc_mcontext.mc_eax);
  E2DBG_GETREG(E2DBG_EBX_VAR, e2dbgworld.curthread->context->uc_mcontext.mc_ebx);
  E2DBG_GETREG(E2DBG_ECX_VAR, e2dbgworld.curthread->context->uc_mcontext.mc_ecx);
  E2DBG_GETREG(E2DBG_EDX_VAR, e2dbgworld.curthread->context->uc_mcontext.mc_edx);
  E2DBG_GETREG(E2DBG_ESI_VAR, e2dbgworld.curthread->context->uc_mcontext.mc_esi);
  E2DBG_GETREG(E2DBG_EDI_VAR, e2dbgworld.curthread->context->uc_mcontext.mc_edi);
  E2DBG_GETREG(E2DBG_SP_VAR , e2dbgworld.curthread->context->uc_mcontext.mc_esp);
  //E2DBG_GETREG(E2DBG_SSPVAR, e2dbgworld.curthread->context->uc_mcontext.mc_ssp); /* XXX */
  E2DBG_GETREG(E2DBG_FP_VAR , e2dbgworld.curthread->context->uc_mcontext.mc_ebp);
  E2DBG_GETREG(E2DBG_PC_VAR , e2dbgworld.curthread->context->uc_mcontext.mc_eip);
#elif defined(__NetBSD__)
  E2DBG_GETREG(E2DBG_EAX_VAR, e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EAX]);
  E2DBG_GETREG(E2DBG_EBX_VAR, e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EBX]);
  E2DBG_GETREG(E2DBG_ECX_VAR, e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_ECX]);
  E2DBG_GETREG(E2DBG_EDX_VAR, e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EDX]);
  E2DBG_GETREG(E2DBG_ESI_VAR, e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_ESI]);
  E2DBG_GETREG(E2DBG_EDI_VAR, e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EDI]);
  E2DBG_GETREG(E2DBG_SP_VAR , e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_ESP]);
  //E2DBG_GETREG(E2DBG_SSP_VAR, e2dbgworld.curthread->context->uc_mcontext.mc_ssp); /* XXX */
  E2DBG_GETREG(E2DBG_FP_VAR , e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EBP]);
  E2DBG_GETREG(E2DBG_PC_VAR , e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EIP]);
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Regvars handler for the IA32 architecture on SYSV */
void		  e2dbg_get_regvars_ia32_sysv()
{ 
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(__i386__) && (defined(__linux__) || defined(sun))
  E2DBG_GETREG(E2DBG_EAX_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EAX]);
  E2DBG_GETREG(E2DBG_EBX_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EBX]);
  E2DBG_GETREG(E2DBG_ECX_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_ECX]);
  E2DBG_GETREG(E2DBG_EDX_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EDX]);
  E2DBG_GETREG(E2DBG_ESI_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_ESI]);
  E2DBG_GETREG(E2DBG_EDI_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EDI]);
  E2DBG_GETREG(E2DBG_SP_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_ESP]);
  E2DBG_GETREG(E2DBG_SSP_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_UESP]); 
  E2DBG_GETREG(E2DBG_FP_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EBP]);
  E2DBG_GETREG(E2DBG_PC_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EIP]);
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/* Regvars handler for the IA32 architecture */
void		  e2dbg_set_regvars_ia32_bsd()
{ 
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if !defined(sun) && !defined(__amd64__) && defined(__FreeBSD__) //|| defined (__OpenBSD__)
  E2DBG_SETREG(E2DBG_EAX_VAR, e2dbgworld.curthread->context->uc_mcontext.mc_eax);
  E2DBG_SETREG(E2DBG_EBX_VAR, e2dbgworld.curthread->context->uc_mcontext.mc_ebx);
  E2DBG_SETREG(E2DBG_ECX_VAR, e2dbgworld.curthread->context->uc_mcontext.mc_ecx);
  E2DBG_SETREG(E2DBG_EDX_VAR, e2dbgworld.curthread->context->uc_mcontext.mc_edx);
  E2DBG_SETREG(E2DBG_ESI_VAR, e2dbgworld.curthread->context->uc_mcontext.mc_esi);
  E2DBG_SETREG(E2DBG_EDI_VAR, e2dbgworld.curthread->context->uc_mcontext.mc_edi);
  E2DBG_SETREG(E2DBG_SP_VAR , e2dbgworld.curthread->context->uc_mcontext.mc_esp);
  //E2DBG_SETREG(E2DBG_SSP_VAR, e2dbgworld.curthread->context->uc_mcontext.mc_ssp); /* XXX */
  E2DBG_SETREG(E2DBG_FP_VAR , e2dbgworld.curthread->context->uc_mcontext.mc_ebp);
  E2DBG_SETREG(E2DBG_PC_VAR , e2dbgworld.curthread->context->uc_mcontext.mc_eip);
#elif defined(__NetBSD__)
  E2DBG_SETREG(E2DBG_EAX_VAR, e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EAX]);
  E2DBG_SETREG(E2DBG_EBX_VAR, e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EBX]);
  E2DBG_SETREG(E2DBG_ECX_VAR, e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_ECX]);
  E2DBG_SETREG(E2DBG_EDX_VAR, e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EDX]);
  E2DBG_SETREG(E2DBG_ESI_VAR, e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_ESI]);
  E2DBG_SETREG(E2DBG_EDI_VAR, e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EDI]);
  E2DBG_SETREG(E2DBG_SP_VAR , e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_ESP]);
  //E2DBG_SETREG(E2DBG_SSP_VAR, e2dbgworld.curthread->context->uc_mcontext.mc_ssp); /* XXX */
  E2DBG_SETREG(E2DBG_FP_VAR , e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EBP]);
  E2DBG_SETREG(E2DBG_PC_VAR , e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EIP]);
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}



/* Regvars handler for the IA32 architecture on SYSV */
void		  e2dbg_set_regvars_ia32_sysv()
{ 
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(__i386__) && (defined(__linux__) || defined(sun))
  E2DBG_SETREG(E2DBG_EAX_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EAX]);
  E2DBG_SETREG(E2DBG_EBX_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EBX]);
  E2DBG_SETREG(E2DBG_ECX_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_ECX]);
  E2DBG_SETREG(E2DBG_EDX_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EDX]);
  E2DBG_SETREG(E2DBG_ESI_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_ESI]);
  E2DBG_SETREG(E2DBG_EDI_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EDI]);
  E2DBG_SETREG(E2DBG_SP_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_ESP]);
  E2DBG_SETREG(E2DBG_SSP_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_UESP]); 
  E2DBG_SETREG(E2DBG_FP_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EBP]);
  E2DBG_SETREG(E2DBG_PC_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EIP]);
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/* Get Program Counter register */
eresi_Addr*	  e2dbg_getpc_bsd_ia32()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if !defined(sun) && !defined(__amd64__) && defined(__FreeBSD__) //|| defined (__OpenBSD__)
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		    &e2dbgworld.curthread->context->uc_mcontext.mc_eip);
#elif defined(__NetBSD__)
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		    &e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EIP]);
#endif
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}

/* Get Program Counter register on Sys V */
eresi_Addr*	  e2dbg_getpc_sysv_ia32()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(__i386__) && (defined(__linux__) || defined(sun))
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		     (eresi_Addr *)&e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EIP]);
#endif
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}



/* Get Program Counter register */
eresi_Addr*	  e2dbg_getfp_bsd_ia32()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if !defined(sun) && !defined(__amd64__) && defined(__FreeBSD__) //|| defined (__OpenBSD__)
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		    &e2dbgworld.curthread->context->uc_mcontext.mc_ebp);
#elif defined(__NetBSD__)
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		    &e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EBP]);

#endif
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}

/* Get Program Counter register on Sys V */
eresi_Addr*	  e2dbg_getfp_sysv_ia32()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(__i386__) && (defined(__linux__) || defined(sun))
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		     (eresi_Addr *)&e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EBP]);
#endif
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}


/* Enable stepping on BSD */
void		  e2dbg_setstep_bsd_ia32()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if !defined(sun) && !defined(__amd64__) && defined(__FreeBSD__) //|| defined (__OpenBSD__)
  e2dbgworld.curthread->context->uc_mcontext.mc_eflags |= 0x100;
#elif defined(__NetBSD__)
  e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EFL] |= 0x100;
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Enable stepping on Sys V */
void		  e2dbg_setstep_sysv_ia32()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(__i386__) && (defined(__linux__) || defined(sun))
  e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EFL] |= 0x100; 
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Disable stepping on Sys V */
void		  e2dbg_resetstep_sysv_ia32()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(__i386__) && (defined(__linux__) || defined(sun))
  e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EFL] &= ~0x100;
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Disable stepping on BSD */
void		  e2dbg_resetstep_bsd_ia32()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if !defined(sun) && !defined(__amd64__) && defined(__FreeBSD__) //|| defined (__OpenBSD__)
  e2dbgworld.curthread->context->uc_mcontext.mc_eflags &= ~0x100;
#elif defined(__NetBSD__)
  e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EFL] &= ~0x100;
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/**
 * Get the next frame pointer given the current one 
 */
void		*e2dbg_bt_ia32(void *frame)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  
  if (!frame)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (NULL));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		((void *) *(long*) frame));
}

/**
 * Get the return address giving the current frame pointer 
 */
void		*e2dbg_getret_ia32(void *frame)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  
  if (!frame)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (NULL));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		(void *) (*((long *) frame + 1)));
}


/**
 * Write a breakpoint 0xCC in memory
 * One of the 2 breakpoint technique of e2dbg 
 */
int		e2dbg_break_ia32(elfshobj_t *f,
				 elfshbp_t  *bp)
{
  int		prot;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
#if __DEBUG_BREAKPOINTS__
  fprintf(stderr, "[DEBUG_BREAKPOINTS:ia32] bp->addr %08X \n", bp->addr);
#endif
  
  bp->savedinstr[0] = *(unsigned long *) bp->addr;
  prot = elfsh_munprotect(f, bp->addr, 4);
  if (prot == (-1))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Munprotect failed", (-1));
  
#if __DEBUG_BREAKPOINTS__
  fprintf(stderr, "[DEBUG_BREAKPOINTS:ia32] after munprotect\n");
#endif
  *(unsigned char *) bp->addr = 0xCC;
#if __DEBUG_BREAKPOINTS__
  fprintf(stderr, "[DEBUG_BREAKPOINTS:ia32] after write\n");
#endif
  elfsh_mprotect(bp->addr, 4, prot);
#if __DEBUG_BREAKPOINTS__
  fprintf(stderr, "[DEBUG_BREAKPOINTS:ia32] after mprotect\n");
#endif
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}
