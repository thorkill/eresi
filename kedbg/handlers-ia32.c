#include "kedbg.h"
#include "interface.h"

void		*kedbg_bt_ia32(void *frame)
{
  la32          ptr = 0;

  PROFILER_INQ();

  if (kedbg_isrealmode())
    kedbg_readmema(NULL,(eresi_Addr)(uintptr_t)frame, &ptr, WORD_IN_BYTE);
  else
    kedbg_readmema(NULL,(eresi_Addr)(uintptr_t)frame, &ptr, DWORD_IN_BYTE);

  PROFILER_ROUTQ((void *)ptr);
}


eresi_Addr	*kedbg_getfp_ia32(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  PROFILER_INQ();
  /* First update all the reg. */
  gdbwrap_readgenreg(loc);
  PROFILER_ROUTQ((eresi_Addr *)loc->reg32.ebp);
}


void            *kedbg_getret_ia32(void *frame)
{
  la32          ptr = 0;

  PROFILER_INQ();
  if (kedbg_isrealmode())
    kedbg_readmema(NULL, (la32)((la32 *)frame + 1), &ptr, WORD_IN_BYTE);
  else
    kedbg_readmema(NULL, (la32)((la32 *)frame + 1), &ptr, DWORD_IN_BYTE);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (void *)ptr);
}


void            kedbg_print_reg_ia32(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();
  gdbwrap_gdbreg32 *reg;

  PROFILER_INQ();
  reg = &loc->reg32;
  e2dbg_register_dump("EAX", reg->eax);
  e2dbg_register_dump("EBX", reg->ebx);
  e2dbg_register_dump("ECX", reg->ecx);
  e2dbg_register_dump("EDX", reg->edx);
  e2dbg_register_dump("ESI", reg->esi);
  e2dbg_register_dump("EDI", reg->edi);
  e2dbg_register_dump("ESP", reg->esp);
  e2dbg_register_dump("EBP", reg->ebp);
  e2dbg_register_dump("EIP", reg->eip);
  e2dbg_register_dump("EFLAGS", reg->eflags);
  e2dbg_register_dump("CS", reg->cs);
  e2dbg_register_dump("DS", reg->ds);
  e2dbg_register_dump("SS", reg->ss);
  e2dbg_register_dump("ES", reg->es);
  e2dbg_register_dump("FS", reg->fs);
  e2dbg_register_dump("GS", reg->gs);

  PROFILER_OUTQ();
}


eresi_Addr      *kedbg_getpc_ia32(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  PROFILER_INQ();
  /* First update all the reg. */
  gdbwrap_readgenreg(loc);
  PROFILER_ROUTQ((eresi_Addr *)&loc->reg32.eip);
}


/**
 * We first sync the registers with the server, then we write the new
 * set ones. They'll be sent to the server when we'll do a "cont".
 */
void            kedbg_set_regvars_ia32(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();
  gdbwrap_gdbreg32   *reg;

  PROFILER_INQ();
  reg =  gdbwrap_readgenreg(loc);
  if (reg != NULL)
    {

      E2DBG_SETREG(E2DBG_EAX_VAR, reg->eax);
      E2DBG_SETREG(E2DBG_EBX_VAR, reg->ebx);
      E2DBG_SETREG(E2DBG_ECX_VAR, reg->ecx);
      E2DBG_SETREG(E2DBG_EDX_VAR, reg->edx);
      E2DBG_SETREG(E2DBG_ESI_VAR, reg->esi);
      E2DBG_SETREG(E2DBG_EDI_VAR, reg->edi);
      E2DBG_SETREG(E2DBG_SP_VAR,  reg->esp);
      E2DBG_SETREG(E2DBG_FP_VAR,  reg->ebp);
      E2DBG_SETREG(E2DBG_PC_VAR,  reg->eip);
    }
  PROFILER_OUTQ();
}


void            kedbg_get_regvars_ia32(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  PROFILER_INQ();
  gdbwrap_readgenreg(loc);


  E2DBG_GETREG(E2DBG_EAX_VAR, loc->reg32.eax);
  E2DBG_GETREG(E2DBG_EBX_VAR, loc->reg32.ebx);
  E2DBG_GETREG(E2DBG_ECX_VAR, loc->reg32.ecx);
  E2DBG_GETREG(E2DBG_EDX_VAR, loc->reg32.edx);
  E2DBG_GETREG(E2DBG_ESI_VAR, loc->reg32.esi);
  E2DBG_GETREG(E2DBG_EDI_VAR, loc->reg32.edi);
  E2DBG_GETREG(E2DBG_SP_VAR,  loc->reg32.esp);
  //E2DBG_GETREG(E2DBG_SSP_VAR, loc->reg32.eax);
  E2DBG_GETREG(E2DBG_FP_VAR,  loc->reg32.ebp);
  E2DBG_GETREG(E2DBG_PC_VAR,  loc->reg32.eip);
  PROFILER_OUTQ();
}
