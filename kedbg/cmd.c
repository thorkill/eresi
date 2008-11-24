#include "kedbg.h"
#include "interface.h"

/* Debugging purpose. */
int             cmd_com1(void)
{

  void          *(*fct)();
  u_char        archtype;
  u_char        iotype;
  u_char        ostype;
  u_int         dim[2];
  vector_t      *mem;

  elfsh_register_readmem(ELFSH_OS_LINUX,  ELFSH_IOTYPE_GDBPROT, kedbg_readmem);
  archtype = elfsh_get_archtype(world.curjob->curfile);
  iotype = elfsh_get_iotype(world.curjob->curfile);
  ostype = elfsh_get_ostype(world.curjob->curfile);
  fprintf(stderr, "Value --: %d - %d - %d\n",archtype, iotype, ostype);
  fflush(stderr);
  mem = aspect_vector_get(ELFSH_HOOK_READMEM);
  dim[0] = ostype;
  dim[1] = iotype;
  fprintf(stderr, "Value2 --: %d \n", mem->arraysz);
  fflush(stderr);
  
  fct = aspect_vectors_select(mem, dim);
  fct(NULL);
  fprintf(stderr, "Value --: %d - %d\n", world.curjob->curfile->hostype,
	  world.curjob->curfile->iotype);

  /*   kedbg_get_regvars_ia32(); */
  //  fprintf(stderr, "YOUPLA: %#x\n", *kedbg_getpc_ia32());
  return 0;
}


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

  kedbg_set_regvars_ia32();
  gdbwrap_shipallreg(loc);

  revm_output("[*] Continuing process\n");

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!e2dbgworld.curthread->step)
    {
      gdbwrap_continue(loc);
      if (gdbwrap_is_active(loc))
	{
	  if (gdbwrap_lastsignal(loc) == SIGTRAP)
	    {

	      /* All instances of readmema must be changed to elfsh_... */
	  
	      kedbg_readmema(NULL, loc->reg32.eip - 1, &c, 1);
	      ASSERT(c == BPCODE);
	      snprintf(addr, sizeof(addr), "%#x", loc->reg32.eip - 1);
	      bp = e2dbg_breakpoint_lookup(addr);
	      if (bp != NULL)
		{
		  revm_clean();
		  e2dbg_display(bp->cmd, bp->cmdnbr);

		  c = BPCODE;
		  eip_pos = offsetof(struct gdbwrap_gdbreg32, eip) / sizeof(ureg32);
		  //	      kedbg_writemem(NULL, bp->addr, bp->savedinstr, 1);
		  kedbg_delbp(bp);
		  gdbwrap_writereg2(eip_pos, loc->reg32.eip - 1, loc);
		  gdbwrap_stepi(loc);
		  kedbg_setbp(world.curjob->curfile, bp);
		}
	      else
		fprintf(stderr, "An error has occured when trying to find the bp.");
	    }
	  else
	    printf("The value returned was: %d- %#x \n",
		   gdbwrap_lastsignal(loc), loc->reg32.eip);
      
	  printf("[*] Breakpoint found at %#x <%s>\n", loc->reg32.eip,
		 revm_resolve(world.curjob->curfile, loc->reg32.eip, &off));
	}
    }
  else
    {
      char    instr[30];

      gdbwrap_stepi(loc);      
      revm_clean();
      e2dbg_display(e2dbgworld.displaycmd, e2dbgworld.displaynbr);
      parent = e2dbg_get_parent_object((eresi_Addr)loc->reg32.eip);
      name   = revm_resolve(parent, (eresi_Addr) loc->reg32.eip, &off);
      kedbg_readmema(NULL, (eresi_Addr)loc->reg32.eip + off, instr, sizeof(instr));

      revm_instr_display(-1, 0, loc->reg32.eip, off, 20, name, 0, instr);
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
