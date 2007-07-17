/*
** dumpregs.c for elfsh
** 
** Started on  Wed Jul 27 04:08:52 2005 ym
** 
**
** $Id: dumpregs.c,v 1.4 2007-07-17 18:11:24 may Exp $
**
*/
#include "libe2dbg.h"



/* Resolve a register */
void		e2dbg_register_dump(char *name, uint32_t reg)
{
  elfsh_SAddr	off;
  char		*str;

  str = revm_resolve(world.curjob->current, reg, &off);

  if (!off)
    printf("\t [%s] %08X (%010u) <%s> \n", 
	   name, reg, reg, (str ? str : "unknown"));
  else
    printf("\t [%s] %08X (%010u) <%s + " UFMT "> \n", 
	   name, reg, reg, str, off);
}


/* The register dumping function */
/* Print the registers state just before entering the breakpoint */
/* Small BSD patch submitted by Rod */
int		cmd_dumpregs()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!e2dbgworld.curthread)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No current thread available", (-1));
  if (!e2dbgworld.curthread->context)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No context available", (-1));
  
  e2dbg_output(" .:: Registers ::. \n\n");

#if defined(__amd64__) && defined(__FreeBSD__)
  #error "Register context not filled on FreeBSD / AMD64"
#elif defined(__FreeBSD__) || defined(__OpenBSD__)
  e2dbg_register_dump("EAX", e2dbgworld.curthread->context->uc_mcontext.mc_eax);
  e2dbg_register_dump("EBX", e2dbgworld.curthread->context->uc_mcontext.mc_ebx);
  e2dbg_register_dump("ECX", e2dbgworld.curthread->context->uc_mcontext.mc_ecx);
  e2dbg_register_dump("EDX", e2dbgworld.curthread->context->uc_mcontext.mc_edx);
  e2dbg_register_dump("ESI", e2dbgworld.curthread->context->uc_mcontext.mc_esi);
  e2dbg_register_dump("EDI", e2dbgworld.curthread->context->uc_mcontext.mc_edi);
  e2dbg_register_dump("ESP", e2dbgworld.curthread->context->uc_mcontext.mc_esp);
  e2dbg_register_dump("EBP", e2dbgworld.curthread->context->uc_mcontext.mc_ebp);
  e2dbg_register_dump("EIP", e2dbgworld.curthread->context->uc_mcontext.mc_eip);
#elif defined(__NetBSD__)
  e2dbg_register_dump("EAX", e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EAX]);
  e2dbg_register_dump("EBX", e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EBX]);
  e2dbg_register_dump("ECX", e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_ECX]);
  e2dbg_register_dump("EDX", e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EDX]);
  e2dbg_register_dump("ESI", e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_ESI]);
  e2dbg_register_dump("EDI", e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EDI]);
  e2dbg_register_dump("ESP", e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_ESP]);
  e2dbg_register_dump("EBP", e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EBP]);
  e2dbg_register_dump("EIP", e2dbgworld.curthread->context->uc_mcontext.__gregs[_REG_EIP]);
#elif defined(IRIX)
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
#elif defined(__i386__)
  e2dbg_register_dump("EAX", e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EAX]);
  e2dbg_register_dump("EBX", e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EBX]);
  e2dbg_register_dump("ECX", e2dbgworld.curthread->context->uc_mcontext.gregs[REG_ECX]);
  e2dbg_register_dump("EDX", e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EDX]);
  e2dbg_register_dump("ESI", e2dbgworld.curthread->context->uc_mcontext.gregs[REG_ESI]);
  e2dbg_register_dump("EDI", e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EDI]);
  e2dbg_register_dump("ESP", e2dbgworld.curthread->context->uc_mcontext.gregs[REG_ESP]);
  e2dbg_register_dump("EBP", e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EBP]);
  e2dbg_register_dump("EIP", e2dbgworld.curthread->context->uc_mcontext.gregs[REG_EIP]);
#endif
  
  printf("\n");

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}
