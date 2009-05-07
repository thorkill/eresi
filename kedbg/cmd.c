#include "kedbg.h"
#include "interface.h"


static void     kedbg_stepandprint(void)
{
  char          *instr;
  gdbwrap_t     *loc = gdbwrap_current_get();
  int           off;
  char          *name;

  gdbwrap_stepi(loc);
  kedbg_isrealmode();
  revm_clean();
  gdbwrap_readgenreg(loc);
  e2dbg_display(e2dbgworld.displaycmd, e2dbgworld.displaynbr);
  name   = revm_resolve(world.curjob->curfile,
			(eresi_Addr)(uintptr_t) loc->reg32.eip, (elfsh_SAddr *)&off);
  instr  = alloca(20);
  kedbg_readmema(NULL, (eresi_Addr)(uintptr_t)loc->reg32.eip, instr, 20);
  revm_instr_display(-1, loc->reg32.eip, 0, 20, name, off, instr);
}


int             cmd_kedbg_dump_regs(void)
{
  //FIXME: ADD ARM REGs VERSION
  PROFILER_INQ();
  e2dbg_output(" .:: Registers ::. \n\n");
  kedbg_get_regvars_ia32();
  e2dbg_printregs();
  e2dbg_output("\n");
  PROFILER_ROUTQ(0);
}


static void     kedbg_step_over_bp(eresi_Addr addr)
{
  gdbwrap_t     *loc = gdbwrap_current_get();
  elfshbp_t	*bp;
  char          addrchar[20];

  snprintf(addrchar, sizeof(addrchar), XFMT, addr);
  bp = hash_get(&e2dbgworld.bp, addrchar);

  if (bp != NULL)
    {
      e2dbg_deletebreak(bp);
      gdbwrap_stepi(loc);
      e2dbg_setbreak(world.curjob->curfile, bp);
    }
}


/* Continue command. */
int             cmd_kedbgcont(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();
  elfshbp_t	*bp;
  char          addr[20];
  int           off;
  char          *name;
  char          eip_pos;

  PROFILER_INQ();
  kedbg_set_regvars_ia32();
  kedbg_shipallreg();
  revm_output("[*] Continuing process\n");

  kedbg_step_over_bp(loc->reg32.eip);

  if (!e2dbgworld.curthread->step)
    {
      gdbwrap_continue(loc);
      if (gdbwrap_is_active(loc))
	{
	  if (kedbgworld.interrupted)
	    {
	      name = revm_resolve(world.curjob->curfile, loc->reg32.eip,
				  (elfsh_SAddr *) &off);
	      printf("\n[*] Stopped at %#x <%s + %u>\n", loc->reg32.eip, name, off);
	      kedbgworld.interrupted = FALSE;
	    }
	  else
	    if (gdbwrap_lastsignal(loc) == SIGTRAP)
	      {

		if (kedbgworld.offset)
		  {
		    eip_pos = offsetof(struct gdbwrap_gdbreg32, eip) / sizeof(ureg32);
		    kedbg_writereg(eip_pos, loc->reg32.eip - kedbgworld.offset);
		  }
		snprintf(addr, sizeof(addr), XFMT, (eresi_Addr)loc->reg32.eip);
		bp = e2dbg_breakpoint_lookup(addr);
		if (bp != NULL)
		  {
		    revm_clean();
		    if (bp->cmdnbr)
		      e2dbg_display(bp->cmd, bp->cmdnbr);
		    else
		      e2dbg_display(e2dbgworld.displaycmd, e2dbgworld.displaynbr);

		    name = revm_resolve(world.curjob->curfile, loc->reg32.eip,
					(elfsh_SAddr *)&off);
		    if (!off)
		      printf("[*] Breakpoint found at %#x <%s>\n", loc->reg32.eip, name);
		    else
		      printf("[*] Breakpoint found at %#x <%s + %u>\n", loc->reg32.eip,
			     name, off);
		  }
		else
		  printf("[W] SIGTRAP received but not defined.\n");
	      }
	  else
	    printf("Signal received: %s (IP: %#x) \n",
		   get_signal(gdbwrap_lastsignal(loc)), loc->reg32.eip);
	}
    }
  else
    kedbg_stepandprint();

  if (!gdbwrap_is_active(loc))
    cmd_quit();

  kedbg_get_regvars_ia32();
  PROFILER_ROUTQ(0);
}


int             cmd_kedbgstep(void)
{
  static Bool   enable;

  PROFILER_INQ();
  if (enable)
    {
      kedbg_resetstep();
      revm_output("[*] Disabling stepping\n");
    }
  else
    {
      kedbg_setstep();
      revm_output("[*] Enabling stepping\n");
    }
  enable = !enable;

  PROFILER_ROUTQ(0);
}


int             cmd_kedbgquit(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  PROFILER_INQ();
  gdbwrap_bye(loc);
  cmd_quit();
  PROFILER_ROUTQ(0);
}


int             cmd_kedbgprintivt(void)
{
  uint32_t      ivt[256];
  short         i;

  PROFILER_INQ();
  kedbg_readmema(NULL, 0, ivt, 256 * sizeof(uint32_t));
  e2dbg_output(" .:: IVT ::. \n\n");
  for (i = 0; i < 256; i++)
    {
      if (!(i % 8))
	printf("\n0x%02x | ", i);
      printf("0x%08x ", ivt[i]);
    }
  printf("\n\n");
  PROFILER_ROUTQ(0);
}


/**
 * Wrapper to cmd_disasm, but checks wether we are in realmode or
 * not. It will set internal variable in case we change mode.
 **/
int             cmd_kedbgdisasm(void)
{
  PROFILER_INQ();
  kedbg_isrealmode();
  cmd_disasm();
  PROFILER_ROUTQ(0);
}


/**
 * Hooks all the IVT entries. Since a lot of entries are the same, we
 * check before adding the breakpoint on the server side.
 **/
int             cmd_kedbghookivt(void)
{
  char          addr[50];
  uint32_t      ivt[256];
  u_short       ivtinc;
  uint32_t      finaladdr;
  elfshbp_t	*bp;

  PROFILER_INQ();
  kedbg_readmema(NULL, 0, ivt, 256 * sizeof(uint32_t));
  for (ivtinc = 0; ivtinc < 256; ivtinc++)
    {
      finaladdr  = (ivt[ivtinc] & 0xFFFF0000) >> 12;
      finaladdr += ivt[ivtinc] & 0xFFFF;
      finaladdr &= 0xFFFFF;
      snprintf(addr, sizeof(addr), XFMT, (eresi_Addr)finaladdr);
      bp = hash_get(&e2dbgworld.bp, addr);

      if (bp == NULL && finaladdr != 0x0)
	e2dbg_breakpoint_add((eresi_Addr)(uintptr_t) finaladdr);

    }

  printf("[*] Entries of the IVT have been hooked.\n");
  PROFILER_ROUTQ(0);
}


/**
 * In case you get pissed of with the itrace...
 **/
void            kedbg_itracesigint(int sig)
{
  PROFILER_INQ();
  NOT_USED(sig);
  e2dbgworld.curthread->trace = FALSE;
  PROFILER_OUTQ();
}


/**
 * Do steppig until we reach a known breakpoint.
 **/
int             cmd_kedbgitrace(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();
  elfshbp_t	*bp;
  char          addr[50];

  PROFILER_INQ();
  e2dbgworld.curthread->trace = TRUE;
  signal(SIGINT, kedbg_itracesigint);
  while (e2dbgworld.curthread->trace == TRUE)
    {
      kedbg_stepandprint();
      snprintf(addr, sizeof(addr), XFMT, (eresi_Addr)loc->reg32.eip);
      bp = hash_get(&e2dbgworld.bp, addr);

      if (bp != NULL)
	{
	  e2dbg_deletebreak(bp);
	  gdbwrap_stepi(loc);
	  e2dbg_setbreak(world.curjob->curfile, bp);
	  e2dbgworld.curthread->trace = FALSE;
	  printf("[*] Breakpoint found at %#x\n", loc->reg32.eip);
	}
    }
  signal(SIGINT, kedbg_sigint);
  PROFILER_ROUTQ(0);
}


int             cmd_kedbgproc(void)
{
  PROFILER_INQ();
  kedbg_isrealmode();
  cmd_proc();
  PROFILER_ROUTQ(0);
}


int             cmd_kedbggraph(void)
{
  PROFILER_INQ();
  kedbg_isrealmode();
  cmd_graph();
  PROFILER_ROUTQ(0);
}

int            cmd_kedbgmonitor(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();
  char          *ret = NULL;
  char          cmd_bp[30];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (world.curjob->curcmd->argc)
  {
    case 0:
      printf("\n Usage: monitor <sub_cmd>\n");
      break;

    default:
      if (!strcmp(MONITORRESUME, world.curjob->curcmd->param[0]))
        ret = gdbwrap_remotecmd(loc, MONITORRESUME);
      else if (!strcmp(CMD_MONITOR_RESETHALT, world.curjob->curcmd->param[0]))
        ret = gdbwrap_remotecmd(loc, MONITORRESETHALT);
      else if (!strcmp(MONITORSTEP, world.curjob->curcmd->param[0]))
        ret = gdbwrap_remotecmd(loc, MONITORSTEP);
      else if (!strcmp(MONITORHALT, world.curjob->curcmd->param[0]))
        ret = gdbwrap_remotecmd(loc, MONITORHALT);
      else if (!strcmp(MONITORBREAK, world.curjob->curcmd->param[0]))
      {
        if (!world.curjob->curcmd->param[1] || !world.curjob->curcmd->param[2])
  		  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			     "Correct Usage: monitor break <address> <size>", -1);
        else
        {
          snprintf(cmd_bp, sizeof(cmd_bp), "bp %s %s",
              world.curjob->curcmd->param[1], world.curjob->curcmd->param[2]);

          ret = gdbwrap_remotecmd(loc, cmd_bp);
        }
      }
      else
		PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			     "Requested Monitor <sub_cmd> not implemented.", -1);
      if (ret)
        printf("\n%s\n", ret);
      break;
  }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
