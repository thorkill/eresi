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
** $Id: dbg-sparc32.c,v 1.1 2008-02-16 12:32:27 thor Exp $
*/
#include "libe2dbg.h"






/* Regvars handler for the IA32 architecture on BSD */
void		  e2dbg_get_regvars_sparc32_bsd()
{ 
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(SPARC) && (defined(__FreeBSD__) || defined (__OpenBSD__))
  //E2DBG_GETREG(ELFSH_EAXVAR, e2dbgworld.curthread->context->uc_mcontext.mc_eax);
#elif defined(SPARC) && defined(__NetBSD__) 
  //E2DBG_GETREG(ELFSH_EAXVAR, e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EAX]);
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}



/* Regvars handler for the IA32 architecture on SYSV */
void		  e2dbg_get_regvars_sparc32_sysv()
{ 
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(SPARC) && (defined(__linux__) || defined(sun))
  E2DBG_GETREG(E2DBG_PC_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_PC]);
  E2DBG_GETREG(E2DBG_NPC_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_nPC]);
  E2DBG_GETREG(E2DBG_Y_VAR  , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_Y]);
  E2DBG_GETREG(E2DBG_G1_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_G1]);
  E2DBG_GETREG(E2DBG_G2_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_G2]);
  E2DBG_GETREG(E2DBG_G3_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_G3]);
  E2DBG_GETREG(E2DBG_G4_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_G4]); 
  E2DBG_GETREG(E2DBG_G5_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_G5]);
  E2DBG_GETREG(E2DBG_G5_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_G6]);
  E2DBG_GETREG(E2DBG_G7_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_G7]);
  E2DBG_GETREG(E2DBG_O0_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_O0]);
  E2DBG_GETREG(E2DBG_O1_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_O1]);
  E2DBG_GETREG(E2DBG_O2_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_O2]);
  E2DBG_GETREG(E2DBG_O3_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_O3]);
  E2DBG_GETREG(E2DBG_O4_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_O4]);
  E2DBG_GETREG(E2DBG_O5_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_O5]);
  E2DBG_GETREG(E2DBG_O6_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_O6]);
  E2DBG_GETREG(E2DBG_O7_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_O7]);
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/* Regvars handler for the IA32 architecture */
void		  e2dbg_set_regvars_sparc32_bsd()
{ 
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(SPARC) && (defined(__FreeBSD__) || defined (__OpenBSD__))
  //E2DBG_SETREG(ELFSH_EAXVAR, e2dbgworld.curthread->context->uc_mcontext.mc_eax);
#elif defined(SPARC) && defined(__NetBSD__)
  //E2DBG_SETREG(ELFSH_EAXVAR, e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EAX]);
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}



/* Regvars handler for the IA32 architecture on SYSV */
void		  e2dbg_set_regvars_sparc32_sysv()
{ 
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(SPARC) && (defined(__linux__) || defined(sun))
  E2DBG_SETREG(E2DBG_PC_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_PC]);
  E2DBG_SETREG(E2DBG_NPC_VAR, e2dbgworld.curthread->context->uc_mcontext.gregs[REG_nPC]);
  E2DBG_SETREG(E2DBG_Y_VAR  , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_Y]);
  E2DBG_SETREG(E2DBG_G1_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_G1]);
  E2DBG_SETREG(E2DBG_G2_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_G2]);
  E2DBG_SETREG(E2DBG_G3_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_G3]);
  E2DBG_SETREG(E2DBG_G4_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_G4]); 
  E2DBG_SETREG(E2DBG_G5_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_G5]);
  E2DBG_SETREG(E2DBG_G5_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_G6]);
  E2DBG_SETREG(E2DBG_G7_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_G7]);
  E2DBG_SETREG(E2DBG_O0_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_O0]);
  E2DBG_SETREG(E2DBG_O1_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_O1]);
  E2DBG_SETREG(E2DBG_O2_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_O2]);
  E2DBG_SETREG(E2DBG_O3_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_O3]);
  E2DBG_SETREG(E2DBG_O4_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_O4]);
  E2DBG_SETREG(E2DBG_O5_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_O5]);
  E2DBG_SETREG(E2DBG_O6_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_O6]);
  E2DBG_SETREG(E2DBG_O7_VAR , e2dbgworld.curthread->context->uc_mcontext.gregs[REG_O7]);
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/* Get Program Counter register */
eresi_Addr*	  e2dbg_getpc_bsd_sparc32()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(SPARC) && (defined(__FreeBSD__) || defined (__OpenBSD__))
  //PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
  //&e2dbgworld.curthread->context->uc_mcontext.mc_eip);
#elif defined(SPARC) && defined(__NetBSD__)
  //PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
  //&e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EIP]);
#endif
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}


/* Get Program Counter register on Sys V */
eresi_Addr*	  e2dbg_getpc_sysv_sparc32()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(SPARC) && (defined(__linux__) || defined(sun))
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		(eresi_Addr *) &e2dbgworld.curthread->context->uc_mcontext.gregs[REG_PC]);
#endif
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}



/* Get Program Counter register */
eresi_Addr*	  e2dbg_getfp_bsd_sparc32()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(SPARC) && (defined(__FreeBSD__) || defined (__OpenBSD__))
  //PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
  //&e2dbgworld.curthread->context->uc_mcontext.mc_ebp);
#elif defined(__NetBSD__)
  //PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
  //&e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EBP]);
#endif
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}


/* Get Program Stack Pointer register on Sys V */
/* On SPARC, we use SP and not FP in that handler */
eresi_Addr*	  e2dbg_getfp_sysv_sparc32()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(SPARC) && (defined(__linux__) || defined(sun))
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		(eresi_Addr *)&e2dbgworld.curthread->context->uc_mcontext.gregs[REG_O6]);
#endif
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}


/* Enable stepping on BSD */
void		  e2dbg_setstep_bsd_sparc32()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(SPARC) && (defined(__FreeBSD__) || defined (__OpenBSD__))
  //e2dbgworld.curthread->context->uc_mcontext.mc_eflags |= 0x100;
#elif defined(SPARC) && defined(__NetBSD__)
  //e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EFL] |= 0x100;
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Enable stepping on Sys V */
void		  e2dbg_setstep_sysv_sparc32()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(SPARC) && (defined(__linux__) || defined(sun))
  //e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EFL] |= 0x100; 
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Disable stepping on Sys V */
void		  e2dbg_resetstep_sysv_sparc32()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(SPARC) && (defined(__linux__) || defined(sun))
  //e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EFL] &= ~0x100;
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/* Disable stepping on BSD */
void		  e2dbg_resetstep_bsd_sparc32()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(SPARC) && (defined(__FreeBSD__) || defined (__OpenBSD__))
  //e2dbgworld.curthread->context->uc_mcontext.mc_eflags &= ~0x100;
#elif defined(SPARC) && defined(__NetBSD__)
  //e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EFL] &= ~0x100;
#endif
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}



/**                                                                                                                                                                                                              
 * Get the next frame pointer given the current one                                                                                                                                                              
 */
void            *e2dbg_bt_sparc32(void *frame)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!frame)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (NULL));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		((void *) *(long*) frame + 14));
}


/**                                                                                                                                                                                                              
 * Get the return address giving the current frame pointer
 */
void            *e2dbg_getret_sparc32(void *frame)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!frame)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (NULL));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
                (void *) (*((long *) frame + 15)));
}


/**
 * Write a sparc breakpoint in memory
 * One of the 2 breakpoint technique of e2dbg
 */
#if !defined(SPARC)
int                     e2dbg_break_sparc32(elfshobj_t *f, elfshbp_t *bp)
{
  return (0);
}
#else
int                     e2dbg_break_sparc32(elfshobj_t *f, elfshbp_t *bp)
{
  int                   prot;
  unsigned long		addr;
  unsigned long long	addr64;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_BREAKPOINTS__
  fprintf(stderr, "[DEBUG_BREAKPOINTS:sparc32] bp->addr %016x \n", bp->addr);
#endif

  memcpy(bp->savedinstr, (unsigned char *) bp->addr, 8);

  prot = elfsh_munprotect(f, bp->addr, 8);
  if (prot == (-1))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Munprotect failed", (-1));

#if __DEBUG_BREAKPOINTS__
  fprintf(stderr, "[DEBUG_BREAKPOINTS:sparc32] after munprotect\n");
#endif

  //*(unsigned long *)  bp->addr      = 0x91d02001;		/* opcode for ta 1 */

  *((unsigned long *) bp->addr)     = 0x01000000;		/* put nop here */
  *((unsigned long *) bp->addr + 1) = 0x01000000;		/* put nop here */

  addr = (bp->addr & (~7));

#if __DEBUG_BREAKPOINTS__
  fprintf(stderr, "[DEBUG_BREAKPOINTS:sparc32] Address to be flushed: %016lx \n", addr);
#endif
  
  /* Testing .... */
  __asm__ __volatile__("iflush %0 + %1 \n\t" ::
		       "r" ((unsigned long) addr), 
		       "r" (0));

  __asm__ __volatile__("iflush %0 + %1 \n\t" ::
		       "r" ((unsigned long) addr), 
		       "r" (8));

  __asm__ __volatile__("iflush %0 + %1 \n\t" ::
		       "r" ((unsigned long) addr), 
		       "r" (16));

  __asm__ __volatile__("flush %0 + %1 \n\t" ::
		       "r" ((unsigned long) addr), 
		       "r" (0));

  __asm__ __volatile__("flush %0 + %1 \n\t" ::
		       "r" ((unsigned long) addr), 
		       "r" (8));

  __asm__ __volatile__("flush %0 + %1 \n\t" ::
		       "r" ((unsigned long) addr), 
		       "r" (16));

  addr64 = 0xffffffff00000000 + addr;

#if __DEBUG_BREAKPOINTS__
  fprintf(stderr, "[DEBUG_BREAKPOINTS:sparc32] Address64 to be flushed: %016llx \n", addr64);
#endif

  __asm__ __volatile__("iflush %0 + %1 \n\t" ::
		       "r" ((unsigned long) addr64), 
		       "r" (0));

  __asm__ __volatile__("iflush %0 + %1 \n\t" ::
		       "r" ((unsigned long) addr64), 
		       "r" (8));

  __asm__ __volatile__("iflush %0 + %1 \n\t" ::
		       "r" ((unsigned long) addr64), 
		       "r" (16));

  __asm__ __volatile__("flush %0 + %1 \n\t" ::
		       "r" ((unsigned long) addr64), 
		       "r" (0));

  __asm__ __volatile__("flush %0 + %1 \n\t" ::
		       "r" ((unsigned long) addr64), 
		       "r" (8));

  __asm__ __volatile__("flush %0 + %1 \n\t" ::
		       "r" ((unsigned long) addr64), 
		       "r" (16));


#if __DEBUG_BREAKPOINTS__
  fprintf(stderr, "[DEBUG_BREAKPOINTS:sparc32] after sleep & write\n");
#endif

  elfsh_mprotect(bp->addr, 8, prot);

  sleep(2);

#if __DEBUG_BREAKPOINTS__
  fprintf(stderr, "[DEBUG_BREAKPOINTS:sparc32] after mprotect\n");
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}
#endif /* defined(SPARC) */


