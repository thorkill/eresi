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
  kedbg_get_regvars_ia32();
  //  fprintf(stderr, "YOUPLA: %#x\n", *kedbg_getpc_ia32());
  return 0;
}


int             cmd_kedbg_dump_regs(void)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  e2dbg_output(" .:: Registers ::. \n\n");
  e2dbg_setregs();
  e2dbg_printregs();
  e2dbg_output("\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}


/* Continue command. */
int             cmd_kedbgcont(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();
  elfshbp_t	*bp;
  char          c;
  char          addr[20];
  int           off;

  if (!e2dbgworld.curthread->step)
    gdbwrap_continue(loc);
  else
    {
      gdbwrap_stepi(loc);
      printf(" STEPPING ACTIVATED \n");
    }
  if (gdbwrap_lastsignal(loc) == SIGTRAP)
    {
      kedbg_readmema(NULL, loc->reg32.eip - 1, &c, 1);
      snprintf(addr, sizeof(addr), "%#x", loc->reg32.eip - 1);
      bp = e2dbg_breakpoint_lookup(addr);
      if (bp != NULL)
	{
	  c = BPCODE;
	  kedbg_writemem(NULL, bp->addr, bp->savedinstr, 1);
	  gdbwrap_stepi(loc);
	  kedbg_writemem(NULL, bp->addr, &c, 1);
	}
      else fprintf(stderr, "An error has occured when trying to find the bp.");
    }
  else
    printf("The value returned was: %d- %#x \n",
	   gdbwrap_lastsignal(loc), loc->reg32.eip);

  printf("eip: %#x (%s) ", loc->reg32.eip,
	 revm_resolve(world.curjob->curfile, loc->reg32.eip, &off));
  printf("offset: %#x\n", off);
  return 0;
}
