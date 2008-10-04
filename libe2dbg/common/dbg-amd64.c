/*
** dbg-amd64.c for e2dbg in the ERESI framework
**
** Debugger backend for the AMD64 architecture  
**
** Those hooks stay in the debugger and not in libelfsh.
**
** Vectors can also be accessed with 'vectors' cmd in any eresi interpreter 
** 
** Started   Sat Sep 24 07:17:33 2005 jfv
** $Id: dbg-ia32.c,v 1.1 2008-02-16 12:32:27 thor Exp $
*/
#include "libe2dbg.h"

/* Regvars handler for the AMD64 architecture on BSD */
void		  e2dbg_get_regvars_amd64_bsd()
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

/* Regvars handler for the AMD64 architecture on SYSV */
void		  e2dbg_get_regvars_amd64_sysv()
{ 
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(__amd64__) && defined(__linux__)
  E2DBG_GETREG(E2DBG_R8_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_R8]);
  E2DBG_GETREG(E2DBG_R9_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_R9]);
  E2DBG_GETREG(E2DBG_R10_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_R10]);
  E2DBG_GETREG(E2DBG_R11_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_R11]);
  E2DBG_GETREG(E2DBG_R12_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_R12]);
  E2DBG_GETREG(E2DBG_R13_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_R13]);
  E2DBG_GETREG(E2DBG_R14_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_R14]);
  E2DBG_GETREG(E2DBG_R15_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_R15]);
  E2DBG_GETREG(E2DBG_RDI_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_RDI]);
  E2DBG_GETREG(E2DBG_RSI_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_RSI]);
  E2DBG_GETREG(E2DBG_RBP_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_RBP]);
  E2DBG_GETREG(E2DBG_RBX_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_RBX]);
  E2DBG_GETREG(E2DBG_RDX_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_RDX]);
  E2DBG_GETREG(E2DBG_RAX_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_RAX]);
  E2DBG_GETREG(E2DBG_RCX_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_RCX]);
  E2DBG_GETREG(E2DBG_RSP_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_RSP]);
  E2DBG_GETREG(E2DBG_RIP_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_RIP]);
  E2DBG_GETREG(E2DBG_EFLAGS_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EFL]);
  E2DBG_GETREG(E2DBG_CS_VAR, ((e2dbgworld.curthread->context->uc_mcontext.gregs[REG_CSGSFS] >> 48) & 0xFFFF));
  E2DBG_GETREG(E2DBG_GS_VAR, ((e2dbgworld.curthread->context->uc_mcontext.gregs[REG_CSGSFS] >> 32) & 0xFFFF));
  E2DBG_GETREG(E2DBG_FS_VAR, ((e2dbgworld.curthread->context->uc_mcontext.gregs[REG_CSGSFS] >> 16) & 0xFFFF));
  E2DBG_GETREG(E2DBG_CR2_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_CR2]);
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/* Regvars handler for the AMD64 architecture */
void		  e2dbg_set_regvars_amd64_bsd()
{ 
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(__amd64__)

#elif !defined(sun) && defined(__FreeBSD__)
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



/* Regvars handler for the AMD64 architecture on SYSV */
void		e2dbg_set_regvars_amd64_sysv()
{ 
#if defined(__amd64__) && defined(__linux__)
  u_short	reg16;
  eresi_Addr	reg64;
  revmexpr_t	*e;				
#endif

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(__amd64__) && defined(__linux__)
  E2DBG_SETREG(E2DBG_R8_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_R8]);
  E2DBG_SETREG(E2DBG_R9_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_R9]);
  E2DBG_SETREG(E2DBG_R10_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_R10]);
  E2DBG_SETREG(E2DBG_R11_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_R11]);
  E2DBG_SETREG(E2DBG_R12_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_R12]);
  E2DBG_SETREG(E2DBG_R13_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_R13]);
  E2DBG_SETREG(E2DBG_R14_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_R14]);
  E2DBG_SETREG(E2DBG_R15_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_R15]);
  E2DBG_SETREG(E2DBG_RDI_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_RDI]);
  E2DBG_SETREG(E2DBG_RSI_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_RSI]);
  E2DBG_SETREG(E2DBG_RBP_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_RBP]);
  E2DBG_SETREG(E2DBG_RBX_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_RBX]);
  E2DBG_SETREG(E2DBG_RDX_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_RDX]);
  E2DBG_SETREG(E2DBG_RAX_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_RAX]);
  E2DBG_SETREG(E2DBG_RCX_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_RCX]);
  E2DBG_SETREG(E2DBG_RSP_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_RSP]);
  E2DBG_SETREG(E2DBG_RIP_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_RIP]);
  E2DBG_SETREG(E2DBG_EFLAGS_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EFL]);

  reg64 = e2dbgworld.curthread->context->uc_mcontext.gregs[REG_CSGSFS];

  reg16 = (reg64 & 0xFFFF000000000000) >> 48;
  e = revm_expr_get(E2DBG_CS_VAR);				
  if (e && e->value)					
    reg16 = e->value->immed_val.half;
  e2dbgworld.curthread->context->uc_mcontext.gregs[REG_CSGSFS] = 
    (reg64 & 0x0000FFFFFFFFFFFF) | ((uint64_t) reg16 << 48);

  reg16 = (reg64 & 0x0000FFFF00000000) >> 32;
  e = revm_expr_get(E2DBG_GS_VAR);				
  if (e && e->value)					
    reg16 = e->value->immed_val.half;
  e2dbgworld.curthread->context->uc_mcontext.gregs[REG_CSGSFS] = 
    (reg64 & 0xFFFF0000FFFFFFFF) | ((uint64_t) reg16 << 32);

  reg16 = (reg64 & 0x00000000FFFF0000) >> 16;
  e = revm_expr_get(E2DBG_FS_VAR);				
  if (e && e->value)					
    reg16 = e->value->immed_val.half;
  e2dbgworld.curthread->context->uc_mcontext.gregs[REG_CSGSFS] = 
    (reg64 & 0xFFFFFFFF0000FFFF) | ((uint64_t) reg16 << 16);

  E2DBG_SETREG(E2DBG_CR2_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_CR2]);
#endif

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/* Get Program Counter register */
eresi_Addr*	  e2dbg_getpc_bsd_amd64()
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
eresi_Addr*	  e2dbg_getpc_sysv_amd64()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(__amd64__) && defined(__linux__)
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		     (eresi_Addr *)&e2dbgworld.curthread->context->uc_mcontext.gregs[REG_RIP]);
#endif
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}



/* Get Program Counter register */
eresi_Addr*	  e2dbg_getfp_bsd_amd64()
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
eresi_Addr*	  e2dbg_getfp_sysv_amd64()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(__amd64__) && defined(__linux__)
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		     (eresi_Addr *)&e2dbgworld.curthread->context->uc_mcontext.gregs[REG_RBP]);
#endif
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}


/* Enable stepping on BSD */
void		  e2dbg_setstep_bsd_amd64()
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
void		  e2dbg_setstep_sysv_amd64()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(__amd64__) && defined(__linux__)
  e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EFL] |= 0x100; 
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Disable stepping on Sys V */
void		  e2dbg_resetstep_sysv_amd64()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(__amd64__) && defined(__linux__)
  e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EFL] &= ~0x100;
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Disable stepping on BSD */
void		  e2dbg_resetstep_bsd_amd64()
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
void		*e2dbg_bt_amd64(void *frame)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  
  if (!frame)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (NULL));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		((void *) *(eresi_Addr*) frame));
}

/**
 * Get the return address giving the current frame pointer 
 */
void		*e2dbg_getret_amd64(void *frame)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  
  if (!frame)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (NULL));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		(void *) (*((eresi_Addr *) frame + 1)));
}


/**
 * Write a breakpoint 0xCC in memory
 * One of the 2 breakpoint technique of e2dbg 
 */
int		e2dbg_break_amd64(elfshobj_t *f,
				 elfshbp_t  *bp)
{
  int		prot;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
#if __DEBUG_BREAKPOINTS__
  fprintf(stderr, "[DEBUG_BREAKPOINTS:amd64] bp->addr %08X \n", bp->addr);
#endif
  
  bp->savedinstr[0] = *(eresi_Addr *) bp->addr;
  prot = elfsh_munprotect(f, bp->addr, 4);
  if (prot == (-1))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Munprotect failed", (-1));
  
#if __DEBUG_BREAKPOINTS__
  fprintf(stderr, "[DEBUG_BREAKPOINTS:amd64] after munprotect\n");
#endif
  *(unsigned char *) bp->addr = 0xCC;
#if __DEBUG_BREAKPOINTS__
  fprintf(stderr, "[DEBUG_BREAKPOINTS:amd64] after write\n");
#endif
  elfsh_mprotect(bp->addr, 4, prot);
#if __DEBUG_BREAKPOINTS__
  fprintf(stderr, "[DEBUG_BREAKPOINTS:amd64] after mprotect\n");
#endif
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}
