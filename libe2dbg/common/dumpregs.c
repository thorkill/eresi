/**
* @file libe2dbg/common/dumpregs.c
** @ingroup common
** @brief Command and API for register printing.
** 
** Started on  Wed Jul 27 04:08:52 2005 ym
** $Id$
*/
#include "libe2dbg.h"



/* Resolve a register */
void		e2dbg_register_dump(char *name, ureg32 reg)
{
  elfsh_SAddr	off;
  char		*str;

  str = revm_resolve(world.curjob->curfile, reg, &off);

  if (!off)
    printf("\t [%s] %08X (%010u) <%s> \n", 
	   name, reg, reg, (str ? str : "unknown"));
  else
    printf("\t [%s] %08X (%010u) <%s + " UFMT "> \n", 
	   name, reg, reg, str, off);
}


/* The register dumping function */
/* Print the registers state just before entering the breakpoint */
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
  e2dbg_setregs();
  e2dbg_printregs();
  e2dbg_output("\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}
