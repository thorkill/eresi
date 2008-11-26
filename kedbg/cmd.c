#include "kedbg.h"
#include "interface.h"


int             cmd_kedbg_dump_regs(void)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  e2dbg_output(" .:: Registers ::. \n\n");
  //  gdbwrap_readgenreg(loc);
  kedbg_set_regvars_ia32();
  e2dbg_printregs();
  e2dbg_output("\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
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

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  kedbg_set_regvars_ia32();
  gdbwrap_shipallreg(loc);
  revm_output("[*] Continuing process\n");

  if (!e2dbgworld.curthread->step)
    {
      gdbwrap_continue(loc);
      if (gdbwrap_is_active(loc))
	{
	  if (gdbwrap_lastsignal(loc) == SIGTRAP)
	    {
/* 	      kedbg_readmema(NULL, loc->reg32.eip - 1, &c, 1); */
/* 	      ASSERT(c == BPCODE); */
	      u_char bpoff;

	      bpoff = kedbg_isvmrunning() ? 0 : 1;
	      snprintf(addr, sizeof(addr), "%#x", loc->reg32.eip - bpoff);
	      bp = e2dbg_breakpoint_lookup(addr);
	      if (bp != NULL)
		{
		  revm_clean();
		  e2dbg_display(bp->cmd, bp->cmdnbr);

		  printf("[*] Breakpoint found at %#x <%s>\n", loc->reg32.eip,
			 revm_resolve(world.curjob->curfile, loc->reg32.eip, &off));

		  c = BPCODE;
		  eip_pos = offsetof(struct gdbwrap_gdbreg32, eip) / sizeof(ureg32);
		  //	      kedbg_writemem(NULL, bp->addr, bp->savedinstr, 1);
		  e2dbg_deletebreak(bp);
		  gdbwrap_writereg2(eip_pos, loc->reg32.eip - bpoff, loc);
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
      e2dbg_display(e2dbgworld.displaycmd, e2dbgworld.displaynbr);
      parent = e2dbg_get_parent_object((eresi_Addr)loc->reg32.eip);
      name   = revm_resolve(parent, (eresi_Addr) loc->reg32.eip, &off);

      instr = alloca(20 + off);
      printf("Eip: %#x\n", loc->reg32.eip);
      kedbg_readmema(NULL, (eresi_Addr)loc->reg32.eip - off, instr, 20 + off);
      revm_instr_display(-1, off, loc->reg32.eip, 0, 20, name, off, instr);
    }

  if (!gdbwrap_is_active(loc))
    cmd_quit();

  kedbg_get_regvars_ia32();
  
  PROFILER_ROUTQ(0);
}


int             cmd_kedbgstep(void)
{
  static Bool   enable;

  if (enable)
    {
      kedbg_resetstep_ia32();
      revm_output("[*] Disabling stepping\n");
    }
  else
    {
      kedbg_setstep_ia32();
      revm_output("[*] Enabling stepping\n");
    }
  enable = !enable;

  return 0;
}


int             cmd_kedbgquit(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get(); 

  gdbwrap_bye(loc);
  cmd_quit();
  return 0;
}
