/*
** dbg-sparc32.c for e2dbg in the ERESI framework
**
** Debugger backend for the SPARC32 architecture  
**
** Those hooks stay in the debugger and not in libelfsh.
**
** Vectors can also be accessed with 'vectors' cmd in any eresi interpreter 
** 
** Started   Thu Jun 07 07:17:33 2007 mayhem
** $Id: dbg-sparc32.c,v 1.1 2007-06-07 12:01:00 may Exp $
*/
#include "libe2dbg.h"






/* Regvars handler for the IA32 architecture on BSD */
void		  e2dbg_get_regvars_sparc32_bsd()
{ 
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(SPARC) && (defined(__FreeBSD__) || defined (__OpenBSD__))
  E2DBG_GETREG(ELFSH_EAXVAR, e2dbgworld.curthread->context->uc_mcontext.mc_eax);
  E2DBG_GETREG(ELFSH_EBXVAR, e2dbgworld.curthread->context->uc_mcontext.mc_ebx);
  E2DBG_GETREG(ELFSH_ECXVAR, e2dbgworld.curthread->context->uc_mcontext.mc_ecx);
  E2DBG_GETREG(ELFSH_EDXVAR, e2dbgworld.curthread->context->uc_mcontext.mc_edx);
  E2DBG_GETREG(ELFSH_ESIVAR, e2dbgworld.curthread->context->uc_mcontext.mc_esi);
  E2DBG_GETREG(ELFSH_EDIVAR, e2dbgworld.curthread->context->uc_mcontext.mc_edi);
  E2DBG_GETREG(ELFSH_SPVAR , e2dbgworld.curthread->context->uc_mcontext.mc_esp);
  //E2DBG_GETREG(ELFSH_SSPVAR, e2dbgworld.curthread->context->uc_mcontext.mc_ssp); /* XXX */
  E2DBG_GETREG(ELFSH_FPVAR , e2dbgworld.curthread->context->uc_mcontext.mc_ebp);
  E2DBG_GETREG(ELFSH_PCVAR , e2dbgworld.curthread->context->uc_mcontext.mc_eip);
#elif defined(__NetBSD__) 
  E2DBG_GETREG(ELFSH_EAXVAR, e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EAX]);
  E2DBG_GETREG(ELFSH_EBXVAR, e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EBX]);
  E2DBG_GETREG(ELFSH_ECXVAR, e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_ECX]);
  E2DBG_GETREG(ELFSH_EDXVAR, e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EDX]);
  E2DBG_GETREG(ELFSH_ESIVAR, e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_ESI]);
  E2DBG_GETREG(ELFSH_EDIVAR, e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EDI]);
  E2DBG_GETREG(ELFSH_SPVAR , e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_ESP]);
  //E2DBG_GETREG(ELFSH_SSPVAR, e2dbgworld.curthread->context->uc_mcontext.mc_ssp); /* XXX */
  E2DBG_GETREG(ELFSH_FPVAR , e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EBP]);
  E2DBG_GETREG(ELFSH_PCVAR , e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EIP]);
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Regvars handler for the IA32 architecture on SYSV */
void		  e2dbg_get_regvars_sparc32_sysv()
{ 
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(SPARC) && (defined(__linux__) || defined(sun))
  E2DBG_GETREG(ELFSH_EAXVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EAX]);
  E2DBG_GETREG(ELFSH_EBXVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EBX]);
  E2DBG_GETREG(ELFSH_ECXVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_ECX]);
  E2DBG_GETREG(ELFSH_EDXVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EDX]);
  E2DBG_GETREG(ELFSH_ESIVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_ESI]);
  E2DBG_GETREG(ELFSH_EDIVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EDI]);
  E2DBG_GETREG(ELFSH_SPVAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_ESP]);
  E2DBG_GETREG(ELFSH_SSPVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_UESP]); 
  E2DBG_GETREG(ELFSH_FPVAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EBP]);
  E2DBG_GETREG(ELFSH_PCVAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EIP]);
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/* Regvars handler for the IA32 architecture */
void		  e2dbg_set_regvars_sparc32_bsd()
{ 
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if !defined(sun) && !defined(__amd64__) && (defined(__FreeBSD__) || defined (__OpenBSD__))
  E2DBG_SETREG(ELFSH_EAXVAR, e2dbgworld.curthread->context->uc_mcontext.mc_eax);
  E2DBG_SETREG(ELFSH_EBXVAR, e2dbgworld.curthread->context->uc_mcontext.mc_ebx);
  E2DBG_SETREG(ELFSH_ECXVAR, e2dbgworld.curthread->context->uc_mcontext.mc_ecx);
  E2DBG_SETREG(ELFSH_EDXVAR, e2dbgworld.curthread->context->uc_mcontext.mc_edx);
  E2DBG_SETREG(ELFSH_ESIVAR, e2dbgworld.curthread->context->uc_mcontext.mc_esi);
  E2DBG_SETREG(ELFSH_EDIVAR, e2dbgworld.curthread->context->uc_mcontext.mc_edi);
  E2DBG_SETREG(ELFSH_SPVAR , e2dbgworld.curthread->context->uc_mcontext.mc_esp);
  //E2DBG_SETREG(ELFSH_SSPVAR, e2dbgworld.curthread->context->uc_mcontext.mc_ssp); /* XXX */
  E2DBG_SETREG(ELFSH_FPVAR , e2dbgworld.curthread->context->uc_mcontext.mc_ebp);
  E2DBG_SETREG(ELFSH_PCVAR , e2dbgworld.curthread->context->uc_mcontext.mc_eip);
#elif defined(__NetBSD__)
  E2DBG_SETREG(ELFSH_EAXVAR, e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EAX]);
  E2DBG_SETREG(ELFSH_EBXVAR, e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EBX]);
  E2DBG_SETREG(ELFSH_ECXVAR, e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_ECX]);
  E2DBG_SETREG(ELFSH_EDXVAR, e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EDX]);
  E2DBG_SETREG(ELFSH_ESIVAR, e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_ESI]);
  E2DBG_SETREG(ELFSH_EDIVAR, e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EDI]);
  E2DBG_SETREG(ELFSH_SPVAR , e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_ESP]);
  //E2DBG_SETREG(ELFSH_SSPVAR, e2dbgworld.curthread->context->uc_mcontext.mc_ssp); /* XXX */
  E2DBG_SETREG(ELFSH_FPVAR , e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EBP]);
  E2DBG_SETREG(ELFSH_PCVAR , e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EIP]);
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}



/* Regvars handler for the IA32 architecture on SYSV */
void		  e2dbg_set_regvars_sparc32_sysv()
{ 
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(SPARC) && (defined(__linux__) || defined(sun))
  E2DBG_SETREG(ELFSH_EAXVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EAX]);
  E2DBG_SETREG(ELFSH_EBXVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EBX]);
  E2DBG_SETREG(ELFSH_ECXVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_ECX]);
  E2DBG_SETREG(ELFSH_EDXVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EDX]);
  E2DBG_SETREG(ELFSH_ESIVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_ESI]);
  E2DBG_SETREG(ELFSH_EDIVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EDI]);
  E2DBG_SETREG(ELFSH_SPVAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_ESP]);
  E2DBG_SETREG(ELFSH_SSPVAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_UESP]); 
  E2DBG_SETREG(ELFSH_FPVAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EBP]);
  E2DBG_SETREG(ELFSH_PCVAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EIP]);
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/* Get Program Counter register */
elfsh_Addr*	  e2dbg_getpc_bsd_sparc32()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if !defined(sun) && !defined(__amd64__) && (defined(__FreeBSD__) || defined (__OpenBSD__))
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		    &e2dbgworld.curthread->context->uc_mcontext.mc_eip);
#elif defined(__NetBSD__)
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		    &e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EIP]);
#endif
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}

/* Get Program Counter register on Sys V */
elfsh_Addr*	  e2dbg_getpc_sysv_sparc32()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(SPARC) && (defined(__linux__) || defined(sun))
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		     (elfsh_Addr *)&e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EIP]);
#endif
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}



/* Get Program Counter register */
elfsh_Addr*	  e2dbg_getfp_bsd_sparc32()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if !defined(SPARC) && !defined(__amd64__) && (defined(__FreeBSD__) || defined (__OpenBSD__))
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		    &e2dbgworld.curthread->context->uc_mcontext.mc_ebp);
#elif defined(__NetBSD__)
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		    &e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EBP]);

#endif
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}

/* Get Program Counter register on Sys V */
elfsh_Addr*	  e2dbg_getfp_sysv_sparc32()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(SPARC) && (defined(__linux__) || defined(sun))
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		     (elfsh_Addr *)&e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EBP]);
#endif
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}


/* Enable stepping on BSD */
void		  e2dbg_setstep_bsd_sparc32()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if !defined(sun) && !defined(__amd64__) && (defined(__FreeBSD__) || defined (__OpenBSD__))
  e2dbgworld.curthread->context->uc_mcontext.mc_eflags |= 0x100;
#elif defined(__NetBSD__)
  e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EFL] |= 0x100;
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Enable stepping on Sys V */
void		  e2dbg_setstep_sysv_sparc32()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(SPARC) && (defined(__linux__) || defined(sun))
  e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EFL] |= 0x100; 
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Disable stepping on Sys V */
void		  e2dbg_resetstep_sysv_sparc32()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(SPARC) && (defined(__linux__) || defined(sun))
  e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EFL] &= ~0x100;
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Disable stepping on BSD */
void		  e2dbg_resetstep_bsd_sparc32()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if !defined(sun) && !defined(__amd64__) && (defined(__FreeBSD__) || defined (__OpenBSD__))
  e2dbgworld.curthread->context->uc_mcontext.mc_eflags &= ~0x100;
#elif defined(__NetBSD__)
  e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EFL] &= ~0x100;
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}
