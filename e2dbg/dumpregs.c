/*
** dumpregs.c for elfsh
** 
** Started on  Wed Jul 27 04:08:52 2005 ym
** 
*/
#include "elfsh.h"



/* Resolve a register */
void		vm_dumpreg(char *name, uint32_t reg)
{
  int		off;
  char		*str;

  str = vm_resolve(world.curjob->current, reg, &off);

  if (!off)
    printf("\t [%s] %08X (%010u) <%s> \n", 
	   name, reg, reg, (str ? str : "unknown"));
  else
    printf("\t [%s] %08X (%010u) <%s + %u> \n", 
	   name, reg, reg, str, off);
}


/* The register dumping function */
/* Print the registers state just before entering the breakpoint */
/* Small BSD patch submitted by Rod */
int		cmd_dumpregs()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!e2dbgworld.context)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "No context avialable", (-1));
  
  vm_output(" .:: Registers ::. \n\n");

#if defined(__FreeBSD__)
  vm_dumpreg("EAX", e2dbgworld.context->uc_mcontext.mc_eax);
  vm_dumpreg("EBX", e2dbgworld.context->uc_mcontext.mc_ebx);
  vm_dumpreg("ECX", e2dbgworld.context->uc_mcontext.mc_ecx);
  vm_dumpreg("EDX", e2dbgworld.context->uc_mcontext.mc_edx);
  vm_dumpreg("ESI", e2dbgworld.context->uc_mcontext.mc_esi);
  vm_dumpreg("EDI", e2dbgworld.context->uc_mcontext.mc_edi);
  vm_dumpreg("ESP", e2dbgworld.context->uc_mcontext.mc_esp);
  vm_dumpreg("EBP", e2dbgworld.context->uc_mcontext.mc_ebp);
  vm_dumpreg("EIP", e2dbgworld.context->uc_mcontext.mc_eip);
#elif defined(IRIX)
  vm_dumpreg("R0", e2dbgworld.context->uc_mcontext.__gregs[CTX_R0]);
  vm_dumpreg("AT", e2dbgworld.context->uc_mcontext.__gregs[CTX_AT]);
  vm_dumpreg("V0", e2dbgworld.context->uc_mcontext.__gregs[CTX_V0]);
  vm_dumpreg("V1", e2dbgworld.context->uc_mcontext.__gregs[CTX_V1]);
  vm_dumpreg("A0", e2dbgworld.context->uc_mcontext.__gregs[CTX_A0]);
  vm_dumpreg("A1", e2dbgworld.context->uc_mcontext.__gregs[CTX_A1]);
  vm_dumpreg("A2", e2dbgworld.context->uc_mcontext.__gregs[CTX_A2]);
  vm_dumpreg("A3", e2dbgworld.context->uc_mcontext.__gregs[CTX_A3]);
  vm_dumpreg("A4", e2dbgworld.context->uc_mcontext.__gregs[CTX_A4]);
  vm_dumpreg("A5", e2dbgworld.context->uc_mcontext.__gregs[CTX_A5]);
  vm_dumpreg("A6", e2dbgworld.context->uc_mcontext.__gregs[CTX_A6]);
  vm_dumpreg("A7", e2dbgworld.context->uc_mcontext.__gregs[CTX_A7]);
  vm_dumpreg("T0", e2dbgworld.context->uc_mcontext.__gregs[CTX_T0]);
  vm_dumpreg("T1", e2dbgworld.context->uc_mcontext.__gregs[CTX_T1]);
  vm_dumpreg("T2", e2dbgworld.context->uc_mcontext.__gregs[CTX_T2]);
  vm_dumpreg("T3", e2dbgworld.context->uc_mcontext.__gregs[CTX_T3]);
  vm_dumpreg("S0", e2dbgworld.context->uc_mcontext.__gregs[CTX_S0]);
  vm_dumpreg("S1", e2dbgworld.context->uc_mcontext.__gregs[CTX_S1]);
  vm_dumpreg("S2", e2dbgworld.context->uc_mcontext.__gregs[CTX_S2]);
  vm_dumpreg("S3", e2dbgworld.context->uc_mcontext.__gregs[CTX_S3]);
  vm_dumpreg("S4", e2dbgworld.context->uc_mcontext.__gregs[CTX_S4]);
  vm_dumpreg("S5", e2dbgworld.context->uc_mcontext.__gregs[CTX_S5]);
  vm_dumpreg("S6", e2dbgworld.context->uc_mcontext.__gregs[CTX_S6]);
  vm_dumpreg("S7", e2dbgworld.context->uc_mcontext.__gregs[CTX_S7]);
  vm_dumpreg("T8", e2dbgworld.context->uc_mcontext.__gregs[CTX_T8]);
  vm_dumpreg("T9", e2dbgworld.context->uc_mcontext.__gregs[CTX_T9]);
  vm_dumpreg("K0", e2dbgworld.context->uc_mcontext.__gregs[CTX_K0]);
  vm_dumpreg("K1", e2dbgworld.context->uc_mcontext.__gregs[CTX_K1]);
  vm_dumpreg("GP", e2dbgworld.context->uc_mcontext.__gregs[CTX_GP]);
  vm_dumpreg("SP", e2dbgworld.context->uc_mcontext.__gregs[CTX_SP]);
  vm_dumpreg("S8", e2dbgworld.context->uc_mcontext.__gregs[CTX_S8]);
  vm_dumpreg("RA", e2dbgworld.context->uc_mcontext.__gregs[CTX_RA]); 
  vm_dumpreg("MDLO", e2dbgworld.context->uc_mcontext.__gregs[CTX_MDLO]);
  vm_dumpreg("MDHI", e2dbgworld.context->uc_mcontext.__gregs[CTX_MDHI]);
  vm_dumpreg("EPC", e2dbgworld.context->uc_mcontext.__gregs[CTX_EPC]);
  vm_dumpreg("SR", e2dbgworld.context->uc_mcontext.__gregs[CTX_SR]);
#else
  vm_dumpreg("EAX", e2dbgworld.context->uc_mcontext.gregs[REG_EAX]);
  vm_dumpreg("EBX", e2dbgworld.context->uc_mcontext.gregs[REG_EBX]);
  vm_dumpreg("ECX", e2dbgworld.context->uc_mcontext.gregs[REG_ECX]);
  vm_dumpreg("EDX", e2dbgworld.context->uc_mcontext.gregs[REG_EDX]);
  vm_dumpreg("ESI", e2dbgworld.context->uc_mcontext.gregs[REG_ESI]);
  vm_dumpreg("EDI", e2dbgworld.context->uc_mcontext.gregs[REG_EDI]);
  vm_dumpreg("ESP", e2dbgworld.context->uc_mcontext.gregs[REG_ESP]);
  vm_dumpreg("EBP", e2dbgworld.context->uc_mcontext.gregs[REG_EBP]);
  vm_dumpreg("EIP", e2dbgworld.context->uc_mcontext.gregs[REG_EIP]);
#endif
  
  printf("\n");

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}
