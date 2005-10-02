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
#else
  vm_dumpreg("EAX", e2dbgworld.context->uc_mcontext.gregs[REG_EBX]);
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
