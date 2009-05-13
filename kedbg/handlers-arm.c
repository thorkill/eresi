#include "arm-flags.h"
#include "kedbg.h"
#include "interface.h"


void            kedbg_print_reg_ARM(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();
  gdbwrap_gdbARMreg32 *reg;

  PROFILER_INQ();
  reg = &loc->reg32_ARM;
  e2dbg_register_dump("r0",           reg->r0);
  e2dbg_register_dump("r1",           reg->r1);
  e2dbg_register_dump("r2",           reg->r2);
  e2dbg_register_dump("r3",           reg->r3);
  e2dbg_register_dump("r4",           reg->r4);
  e2dbg_register_dump("r5",           reg->r5);
  e2dbg_register_dump("r6",           reg->r6);
  e2dbg_register_dump("r7",           reg->r7);
  e2dbg_register_dump("r8",           reg->r8);
  e2dbg_register_dump("r9",           reg->r9);
  e2dbg_register_dump("r10",          reg->r10);
  e2dbg_register_dump("r12",          reg->r12);
  e2dbg_register_dump("r8_fiq",       reg->r8_fiq);
  e2dbg_register_dump("r9_fiq",       reg->r9_fiq);
  e2dbg_register_dump("r10_fiq",      reg->r10_fiq);
  e2dbg_register_dump("r11_fiq",      reg->r11_fiq);
  e2dbg_register_dump("r12_fiq",      reg->r12_fiq);
  e2dbg_register_dump("r13_fiq",      reg->r13_fiq);
  e2dbg_register_dump("r14_fiq",      reg->r14_fiq);
  e2dbg_register_dump("r14_usr",      reg->r14_usr);
  e2dbg_register_dump("r13_irq",      reg->r13_fiq);
  e2dbg_register_dump("r14_irq",      reg->r14_fiq);
  e2dbg_register_dump("r13_svc",      reg->r13_fiq);
  e2dbg_register_dump("r14_svc",      reg->r14_fiq);
  e2dbg_register_dump("r13_abt",      reg->r13_fiq);
  e2dbg_register_dump("r14_abt",      reg->r14_fiq);
  e2dbg_register_dump("r13_und",      reg->r13_fiq);
  e2dbg_register_dump("r14_und",      reg->r14_fiq);
  e2dbg_register_dump("cpsr",         reg->cpsr);
  e2dbg_register_dump("spsr_fiq",     reg->spsr_fiq);
  e2dbg_register_dump("spsr_irq",     reg->spsr_irq);
  e2dbg_register_dump("spsr_svc",     reg->spsr_svc);
  e2dbg_register_dump("spsr_abt",     reg->spsr_abt);
  e2dbg_register_dump("spsr_und",     reg->spsr_und);
  e2dbg_register_dump("SP (r13_usr)", reg->r13_usr);
  e2dbg_register_dump("FP (r11)",     reg->r11);
  e2dbg_register_dump("PC (r15)",     reg->r15);

  PROFILER_OUTQ();
}


eresi_Addr      *kedbg_getpc_ARM(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  PROFILER_INQ();
  /* First update all the reg. */
  gdbwrap_readgenARMreg(loc);
  PROFILER_ROUTQ((eresi_Addr *)&loc->reg32_ARM.r15);
}


eresi_Addr	*kedbg_getfp_ARM(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  PROFILER_INQ();
  /* First update all the reg. */
  gdbwrap_readgenreg(loc);
  PROFILER_ROUTQ((eresi_Addr *)loc->reg32_ARM.r11);
}


void            *kedbg_get_latest_ret_ARM()
{
  ureg32     ptr = 0;
  ureg32     cpsr;
  gdbwrap_t  *loc = gdbwrap_current_get();

  PROFILER_INQ();
  /* First update all the reg. */
  gdbwrap_readgenreg(loc);

  cpsr = loc->reg32_ARM.cpsr;

  switch (cpsr & ARM_MODE)
    {
      case ARM_USR: ptr = loc->reg32_ARM.r14_usr; break;
      case ARM_FIQ: ptr = loc->reg32_ARM.r14_fiq; break;
      case ARM_IRQ: ptr = loc->reg32_ARM.r14_irq; break;
      case ARM_SVC: ptr = loc->reg32_ARM.r14_svc; break;
      case ARM_ABT: ptr = loc->reg32_ARM.r14_abt; break;
      case ARM_UND: ptr = loc->reg32_ARM.r14_und; break;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (void *)ptr);
}


void            kedbg_set_regvars_ARM(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();
  gdbwrap_gdbARMreg32   *reg;

  PROFILER_INQ();
  reg =  gdbwrap_readgenARMreg(loc);
  if (reg != NULL)
    {
      E2DBG_SETREG(E2DBG_R0_VAR,       reg->r0);
      E2DBG_SETREG(E2DBG_R1_VAR,       reg->r1);
      E2DBG_SETREG(E2DBG_R2_VAR,       reg->r2);
      E2DBG_SETREG(E2DBG_R3_VAR,       reg->r3);
      E2DBG_SETREG(E2DBG_R4_VAR,       reg->r4);
      E2DBG_SETREG(E2DBG_R5_VAR,       reg->r5);
      E2DBG_SETREG(E2DBG_R6_VAR,       reg->r6);
      E2DBG_SETREG(E2DBG_R7_VAR,       reg->r7);
      E2DBG_SETREG(E2DBG_R8_VAR,       reg->r8);
      E2DBG_SETREG(E2DBG_R9_VAR,       reg->r9);
      E2DBG_SETREG(E2DBG_R10_VAR,      reg->r10);
      E2DBG_SETREG(E2DBG_R11_VAR,      reg->r11);
      E2DBG_SETREG(E2DBG_FP_VAR,       reg->r11);
      E2DBG_SETREG(E2DBG_R12_VAR,      reg->r12);
      E2DBG_SETREG(E2DBG_SP_VAR,       reg->r13_usr);
      E2DBG_SETREG(E2DBG_R13_USR_VAR,  reg->r13_usr);
      E2DBG_SETREG(E2DBG_R14_USR_VAR,  reg->r14_usr);
      E2DBG_SETREG(E2DBG_R15_VAR,      reg->r15);
      E2DBG_SETREG(E2DBG_PC_VAR,       reg->r15);
      E2DBG_SETREG(E2DBG_R8_FIQ_VAR,   reg->r8_fiq);
      E2DBG_SETREG(E2DBG_R9_FIQ_VAR,   reg->r9_fiq);
      E2DBG_SETREG(E2DBG_R10_FIQ_VAR,  reg->r10_fiq);
      E2DBG_SETREG(E2DBG_R11_FIQ_VAR,  reg->r11_fiq);
      E2DBG_SETREG(E2DBG_R12_FIQ_VAR,  reg->r12_fiq);
      E2DBG_SETREG(E2DBG_R13_FIQ_VAR,  reg->r13_fiq);
      E2DBG_SETREG(E2DBG_R14_FIQ_VAR,  reg->r14_fiq);
      E2DBG_SETREG(E2DBG_R13_IRQ_VAR,  reg->r13_irq);
      E2DBG_SETREG(E2DBG_R14_IRQ_VAR,  reg->r14_irq);
      E2DBG_SETREG(E2DBG_R13_SVC_VAR,  reg->r13_svc);
      E2DBG_SETREG(E2DBG_R14_SVC_VAR,  reg->r14_svc);
      E2DBG_SETREG(E2DBG_R13_ABT_VAR,  reg->r13_abt);
      E2DBG_SETREG(E2DBG_R14_ABT_VAR,  reg->r14_abt);
      E2DBG_SETREG(E2DBG_R13_UND_VAR,  reg->r13_und);
      E2DBG_SETREG(E2DBG_R14_UND_VAR,  reg->r14_und);
      E2DBG_SETREG(E2DBG_CPSR_VAR,     reg->cpsr);
      E2DBG_SETREG(E2DBG_SPSR_FIQ_VAR, reg->spsr_fiq);
      E2DBG_SETREG(E2DBG_SPSR_IRQ_VAR, reg->spsr_irq);
      E2DBG_SETREG(E2DBG_SPSR_SVC_VAR, reg->spsr_svc);
      E2DBG_SETREG(E2DBG_SPSR_ABT_VAR, reg->spsr_abt);
      E2DBG_SETREG(E2DBG_SPSR_UND_VAR, reg->spsr_und);
    }
  PROFILER_OUTQ();
}


void            kedbg_get_regvars_ARM(void)
{
  gdbwrap_gdbARMreg32   *reg;
  gdbwrap_t     *loc = gdbwrap_current_get();

  PROFILER_INQ();
  reg = gdbwrap_readgenARMreg(loc);

  E2DBG_GETREG(E2DBG_R0_VAR,       reg->r0);
  E2DBG_GETREG(E2DBG_R1_VAR,       reg->r1);
  E2DBG_GETREG(E2DBG_R2_VAR,       reg->r2);
  E2DBG_GETREG(E2DBG_R3_VAR,       reg->r3);
  E2DBG_GETREG(E2DBG_R4_VAR,       reg->r4);
  E2DBG_GETREG(E2DBG_R5_VAR,       reg->r5);
  E2DBG_GETREG(E2DBG_R6_VAR,       reg->r6);
  E2DBG_GETREG(E2DBG_R7_VAR,       reg->r7);
  E2DBG_GETREG(E2DBG_R8_VAR,       reg->r8);
  E2DBG_GETREG(E2DBG_R9_VAR,       reg->r9);
  E2DBG_GETREG(E2DBG_R10_VAR,      reg->r10);
  E2DBG_GETREG(E2DBG_R11_VAR,      reg->r11);
  E2DBG_GETREG(E2DBG_FP_VAR,       reg->r11);
  E2DBG_GETREG(E2DBG_R12_VAR,      reg->r12);
  E2DBG_GETREG(E2DBG_SP_VAR,       reg->r13_usr);
  E2DBG_GETREG(E2DBG_R13_USR_VAR,  reg->r13_usr);
  E2DBG_GETREG(E2DBG_R14_USR_VAR,  reg->r14_usr);
  E2DBG_GETREG(E2DBG_R15_VAR,      reg->r15);
  E2DBG_GETREG(E2DBG_PC_VAR,       reg->r15);
  E2DBG_GETREG(E2DBG_R8_FIQ_VAR,   reg->r8_fiq);
  E2DBG_GETREG(E2DBG_R9_FIQ_VAR,   reg->r9_fiq);
  E2DBG_GETREG(E2DBG_R10_FIQ_VAR,  reg->r10_fiq);
  E2DBG_GETREG(E2DBG_R11_FIQ_VAR,  reg->r11_fiq);
  E2DBG_GETREG(E2DBG_R12_FIQ_VAR,  reg->r12_fiq);
  E2DBG_GETREG(E2DBG_R13_FIQ_VAR,  reg->r13_fiq);
  E2DBG_GETREG(E2DBG_R14_FIQ_VAR,  reg->r14_fiq);
  E2DBG_GETREG(E2DBG_R13_IRQ_VAR,  reg->r13_irq);
  E2DBG_GETREG(E2DBG_R14_IRQ_VAR,  reg->r14_irq);
  E2DBG_GETREG(E2DBG_R13_SVC_VAR,  reg->r13_svc);
  E2DBG_GETREG(E2DBG_R14_SVC_VAR,  reg->r14_svc);
  E2DBG_GETREG(E2DBG_R13_ABT_VAR,  reg->r13_abt);
  E2DBG_GETREG(E2DBG_R14_ABT_VAR,  reg->r14_abt);
  E2DBG_GETREG(E2DBG_R13_UND_VAR,  reg->r13_und);
  E2DBG_GETREG(E2DBG_R14_UND_VAR,  reg->r14_und);
  E2DBG_GETREG(E2DBG_CPSR_VAR,     reg->cpsr);
  E2DBG_GETREG(E2DBG_SPSR_FIQ_VAR, reg->spsr_fiq);
  E2DBG_GETREG(E2DBG_SPSR_IRQ_VAR, reg->spsr_irq);
  E2DBG_GETREG(E2DBG_SPSR_SVC_VAR, reg->spsr_svc);
  E2DBG_GETREG(E2DBG_SPSR_ABT_VAR, reg->spsr_abt);
  E2DBG_GETREG(E2DBG_SPSR_UND_VAR, reg->spsr_und);

  PROFILER_OUTQ();
}

