/**
 * notes.c for elfsh
 * @file notes.c
 * Started on Fri Jan 26 21:00:33 BRST 2007
 *
 *
 * $Id: core.c,v 1.2 2008-02-16 12:32:27 thor Exp $
 *
 */
#include "libstderesi.h"

/**
 * Return string associated to a signal
 * @param sig
 * @return A string describing a signal.
 */
char *get_signal(int sig)
{
  switch(sig) 
    {
    case 1: return  "SIGHUP";  case 2: return "SIGINT";
    case 3: return  "SIGQUIT"; case 4: return "SIGILL";
    case 5: return  "SIGTRAP"; case 6: return "SIGABRT";
    case 7: return  "SIGBUS";  case 8: return "SIGFPE";
    case 9: return  "SIGKILL"; case 10: return "SIGUSR1";
    case 11: return "SIGSEGV"; case 12: return "SIGUSR2";
    case 13: return "SIGPIPE"; case 14: return "SIGALRM";
    case 15: return "SIGTERM"; case 16: return "SIGSTKFLT";
    case 17: return "SIGCHLD"; case 18: return "SIGCONT";
    case 19: return "SIGSTOP"; case 20: return "SIGTSTP";
    case 21: return "SIGTTIN"; case 22: return "SIGTTOU";
    case 23: return "SUGURG";  case 24: return "SIGXCPU";
    case 25: return "SIGXFSZ"; case 26: return "SIGVTALRM";
    case 27: return "SIGPROF"; case 28: return "SIGWINCH";
    case 29: return "SIGIO";   case 30: return "SIGPWR";
    case 31: return "SIGSYS"; default: return "SIG_UNKNOWN";
    }
  return NULL;
}

/** 
 * display info about the core file 
 */
int 		cmd_coreinfo()
{
  char	buff[256];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  snprintf(buff,sizeof(buff)," [CORE FILE NOTES_ENTRY]\n [Object %s | %s]\n\n",
	   world.curjob->curfile->name, 
	   world.curjob->curfile->coretype == ELFSH_CORE_LINUX ?
	   "Linux":"FreeBSD");

  revm_output(buff);

  switch(world.curjob->curfile->coretype) 
    {
    case ELFSH_CORE_LINUX:
      {
	elfsh_prstatus_t	*ps;
	elfsh_prpsinfo_t	*pr;
	elfsh_uregset_t		regs;

	ps = &world.curjob->curfile->core.prstatus;
	pr = &world.curjob->curfile->core.prpsinfo;

	snprintf(buff, sizeof(buff), " Received Signal: %d (%s)\n\n", 
		 ps->pr_cursig, get_signal(ps->pr_cursig));
	revm_output(buff);

	regs = *((elfsh_uregset_t *)(&ps->pr_reg));

	snprintf(buff, sizeof(buff), " [ FILE INFO ]\n");
	revm_output(buff);

	snprintf(buff, sizeof(buff), 
		 " Executable filename: %s\n UID: %d\tGID: %d\n PID: %d\n\n",
		 pr->pr_fname, pr->pr_uid, pr->pr_gid, ps->pr_pid);
	revm_output(buff);

	snprintf(buff, sizeof(buff), " [ REGISTERS INFO ]\n");
	revm_output(buff);

	snprintf(buff, sizeof(buff), 
		 " EAX: %08x\n EBX: %08x\n ECX: %08x\n EDX: %08x\n",
		 (unsigned int)regs.eax, (unsigned int)regs.ebx, (unsigned int)regs.ecx, 
		 (unsigned int)regs.edx);
	revm_output(buff);

	snprintf(buff, sizeof(buff), 
		 " ESP: %08x\n ESI: %08x\n EDI: %08x\n EIP: %08x\n"
		 " EFLAGS: %08x\n\n",
		 (unsigned int)regs.esp, (unsigned int)regs.esi, (unsigned int)regs.edi, 
		 (unsigned int)regs.eip, (unsigned int)regs.eflags);

	revm_output(buff);
      }
      break;

    case ELFSH_CORE_FREEBSD:
      {
	elfsh_bsdprstatus_t		*ps;
	elfsh_bsdprpsinfo_t		*pr;
	elfsh_bsduregset_t		regs;

	ps = &world.curjob->curfile->bsdcore.prstatus;
	pr = &world.curjob->curfile->bsdcore.prpsinfo;

	snprintf(buff, sizeof(buff), " Received Signal: %d (%s)\n\n", 
		 ps->pr_cursig, get_signal(ps->pr_cursig));
	revm_output(buff);

	regs = *((elfsh_bsduregset_t *)(&ps->pr_reg));
	snprintf(buff, sizeof(buff), " [ FILE INFO ]\n");
	revm_output(buff);

	snprintf(buff, sizeof(buff), " Executable filename: %s\n PID: %d\n\n",
		 pr->pr_fname, ps->pr_pid);
	revm_output(buff);

	snprintf(buff, sizeof(buff), " [ REGISTERS INFO ]\n");
	revm_output(buff);
      }
      break;
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
