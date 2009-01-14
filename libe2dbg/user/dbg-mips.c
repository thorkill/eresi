/*
** dbg-mips.c for e2dbg in the ERESI framework
**
** INCOMPLETE Debugger backend for the MIPS architecture  
**
** Those hooks stay in the debugger and not in libelfsh.
**
** Vectors can also be accessed with 'vectors' cmd in any eresi interpreter 
** 
** Started   Sat Sep 24 07:17:33 2005 jfv
** $Id$
*/
#include "libe2dbg.h"


/* Printregs handlers for the IA32 architecture on NetBSD */
void		e2dbg_print_regvars_mips_irix()
{
#if defined(IRIX) && defined(__mips)
  e2dbg_register_dump("R0", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_R0]);
  e2dbg_register_dump("AT", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_AT]);
  e2dbg_register_dump("V0", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_V0]);
  e2dbg_register_dump("V1", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_V1]);
  e2dbg_register_dump("A0", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_A0]);
  e2dbg_register_dump("A1", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_A1]);
  e2dbg_register_dump("A2", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_A2]);
  e2dbg_register_dump("A3", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_A3]);
  e2dbg_register_dump("A4", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_A4]);
  e2dbg_register_dump("A5", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_A5]);
  e2dbg_register_dump("A6", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_A6]);
  e2dbg_register_dump("A7", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_A7]);
  e2dbg_register_dump("T0", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_T0]);
  e2dbg_register_dump("T1", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_T1]);
  e2dbg_register_dump("T2", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_T2]);
  e2dbg_register_dump("T3", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_T3]);
  e2dbg_register_dump("S0", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_S0]);
  e2dbg_register_dump("S1", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_S1]);
  e2dbg_register_dump("S2", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_S2]);
  e2dbg_register_dump("S3", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_S3]);
  e2dbg_register_dump("S4", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_S4]);
  e2dbg_register_dump("S5", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_S5]);
  e2dbg_register_dump("S6", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_S6]);
  e2dbg_register_dump("S7", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_S7]);
  e2dbg_register_dump("T8", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_T8]);
  e2dbg_register_dump("T9", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_T9]);
  e2dbg_register_dump("K0", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_K0]);
  e2dbg_register_dump("K1", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_K1]);
  e2dbg_register_dump("GP", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_GP]);
  e2dbg_register_dump("SP", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_SP]);
  e2dbg_register_dump("S8", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_S8]);
  e2dbg_register_dump("RA", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_RA]); 
  e2dbg_register_dump("MDLO", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_MDLO]);
  e2dbg_register_dump("MDHI", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_MDHI]);
  e2dbg_register_dump("EPC", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_EPC]);
  e2dbg_register_dump("SR", e2dbgworld.curthread->context->uc_mcontext.__gregs[CTX_SR]);
#endif
}
