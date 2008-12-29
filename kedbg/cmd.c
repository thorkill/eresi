#include "kedbg.h"
#include "interface.h"


int             cmd_kedbg_dump_regs(void)
{
  PROFILER_INQ();
  e2dbg_output(" .:: Registers ::. \n\n");
  kedbg_get_regvars_ia32();
  e2dbg_printregs();
  e2dbg_output("\n");
  PROFILER_ROUTQ(0);
}


/* Continue command. */
int             cmd_kedbgcont(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();
  elfshbp_t	*bp;
  uint8_t       c;
  char          addr[20];
  int           off;
  char          *name;
  uint8_t       eip_pos;
  elfshobj_t    *parent;

  PROFILER_INQ();
  kedbg_set_regvars_ia32();
  kedbg_shipallreg();
  revm_output("[*] Continuing process\n");

  if (!e2dbgworld.curthread->step)
    {
      gdbwrap_continue(loc);
      if (gdbwrap_is_active(loc))
	{
	  name = revm_resolve(world.curjob->curfile, loc->reg32.eip, &off);
	  if (kedbgworld.interrupted)
	    {
	      printf("\n[*] Stopped at %#x <%s + %u>\n", loc->reg32.eip, name, off);
	      kedbgworld.interrupted = FALSE;
	    }
	  else if (gdbwrap_lastsignal(loc) == SIGTRAP)
	    {
	      snprintf(addr, sizeof(addr), "%#x", loc->reg32.eip - kedbgworld.offset);
	      bp = e2dbg_breakpoint_lookup(addr);
	      if (bp != NULL)
		{
		  revm_clean();
		  e2dbg_display(bp->cmd, bp->cmdnbr);

		  if (!off)
		    printf("[*] Breakpoint found at %#x <%s>\n", loc->reg32.eip, name);
		  else
		    printf("[*] Breakpoint found at %#x <%s + %u>\n", loc->reg32.eip,
			   name, off);
		  c = BPCODE;
		  eip_pos = offsetof(struct gdbwrap_gdbreg32, eip) / sizeof(ureg32);
		  e2dbg_deletebreak(bp);
		  if (kedbgworld.offset)
		    kedbg_writereg(eip_pos, loc->reg32.eip - kedbgworld.offset);
		  gdbwrap_stepi(loc);
		  e2dbg_setbreak(world.curjob->curfile, bp);
		}
	    }
	  else
	    printf("Signal received: %s (IP: %#x) \n",
		   get_signal(gdbwrap_lastsignal(loc)), loc->reg32.eip);
	}
    }
  else
    {
      char    *instr;

      gdbwrap_stepi(loc);      
      revm_clean();
      gdbwrap_readgenreg(loc);
      e2dbg_display(e2dbgworld.displaycmd, e2dbgworld.displaynbr);
      parent = e2dbg_get_parent_object((eresi_Addr)loc->reg32.eip);
      name   = revm_resolve(parent, (eresi_Addr) loc->reg32.eip, &off);
      instr  = alloca(20);
      kedbg_readmema(NULL, (eresi_Addr)loc->reg32.eip, instr, 20);
      revm_instr_display(-1, loc->reg32.eip, 0, 20, name, off, instr);
    }

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
  volatile      u_int    delay;
  
  PROFILER_INQ();
  kedbg_readmema(NULL, 0, ivt, 256 * sizeof(uint32_t));
  for (ivtinc = 0; ivtinc < 256; ivtinc++)
    {
      finaladdr  = (ivt[ivtinc] & 0xFFFF0000) >> 12; //0xe2ce3
      finaladdr += ivt[ivtinc] & 0xFFFF;
      snprintf(addr, sizeof(addr), XFMT, finaladdr);
      bp = hash_get(&e2dbgworld.bp, addr);

      if (bp == NULL && finaladdr != 0x0)
	{
	  printf("Adding: %#x\n", finaladdr);
	  e2dbg_breakpoint_add((eresi_Addr) finaladdr);
	}
    }
  PROFILER_ROUTQ(0);
}
