/**
* @file librevm/api/asmproc.c
 * @ingroup librevm
 * Author: jfv
 *
 * $Id$
 *
 */
#include "revm.h"

/**
 * Synchronise the job asm_processor with the good architecture
 * @ingroup librevm
 */
void	revm_proc_init()
{
  char	logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!world.curjob->curfile)
    PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);

  /* Init proc */
  if (!world.curjob->proc)
    {
      switch (elfsh_get_arch(world.curjob->curfile->hdr))
        {
        case EM_386:
          world.curjob->proc = &world.proc_ia32;
          break;
        case EM_SPARC:
        case EM_SPARC32PLUS:
        case EM_SPARCV9:
          world.curjob->proc = &world.proc_sparc;
          break;
        case EM_MIPS:
        case EM_MIPS_RS3_LE:
        case EM_MIPS_X:
          world.curjob->proc = &world.proc_mips;
          break;
        case EM_ARM:
          world.curjob->proc = &world.proc_arm;
          break;
        default:
          snprintf(logbuf, sizeof (logbuf),
                   "Architecture %s not supported. No flowjack available.\n",
                   elfsh_get_machine_string(elfsh_get_arch(world.curjob->curfile->hdr)));
          revm_output(logbuf);
          PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
        }
    }

  asm_set_resolve_handler(world.curjob->proc, asm_do_resolve, world.curjob->curfile);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}
