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
int		cmd_dumpregs()
{
  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  vm_output(" .:: Registers ::. \n\n");
  vm_dumpreg("EAX", e2dbgworld.context->uc_mcontext.gregs[REG_EAX]);
  vm_dumpreg("EBX", e2dbgworld.context->uc_mcontext.gregs[REG_EBX]);
  vm_dumpreg("ECX", e2dbgworld.context->uc_mcontext.gregs[REG_ECX]);
  vm_dumpreg("EDX", e2dbgworld.context->uc_mcontext.gregs[REG_EDX]);
  vm_dumpreg("ESI", e2dbgworld.context->uc_mcontext.gregs[REG_ESI]);
  vm_dumpreg("EDI", e2dbgworld.context->uc_mcontext.gregs[REG_EDI]);
  vm_dumpreg("ESP", e2dbgworld.context->uc_mcontext.gregs[REG_ESP]);
  vm_dumpreg("EBP", e2dbgworld.context->uc_mcontext.gregs[REG_EBP]);
  vm_dumpreg("EIP", e2dbgworld.context->uc_mcontext.gregs[REG_EIP]);
  
  printf("\n");

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}
