/*
* @file libstderesi/cmd/proc.c
** @ingroup cmd
**
** Made by jfv <vanegue@eresi-project.org>
** $Id$
*/
#include "libstderesi.h"

/** @brief Print information about currently analyzed processor
 *
 */
int		cmd_proc()
{
  char		logbuf[BUFSIZ];
  int		mode;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!world.curjob->proc)
    {
      revm_output(" [*] No processor initialized for this architecture (unsupported ?) \n");
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  switch (world.curjob->proc->type)
    {
    case ASM_PROC_IA32:
      mode = asm_ia32_get_mode(world.curjob->proc);
      snprintf(logbuf, sizeof(logbuf), " [*] INTEL processor (%s mode)\n",
	       (mode == INTEL_PROT ? "Protected" : "Real"));
      break;
    case ASM_PROC_SPARC:
      snprintf(logbuf, sizeof(logbuf), " [*] SPARC processor\n");
      break;
    case ASM_PROC_MIPS:
      snprintf(logbuf, sizeof(logbuf), " [*] MIPS processor\n");
      break;
    case ASM_PROC_ARM:
      snprintf(logbuf, sizeof(logbuf), " [*] ARM processor\n");
      break;
    default:
      snprintf(logbuf, sizeof(logbuf), " [*] UNKNOWN processor\n");
    }

  revm_output(logbuf);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
