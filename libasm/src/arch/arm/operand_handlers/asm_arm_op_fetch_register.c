/**
* @file libasm/src/arch/arm/operand_handlers/asm_arm_op_fetch_register.c
** @ingroup ARM_operands
*/
#include <libasm.h>

int asm_arm_op_fetch_register(asm_operand *operand, u_char *opcode,
                              int otype, asm_instr *ins)
{
  LIBASM_PROFILE_FIN();

  operand->content = ASM_ARM_OTYPE_REGISTER;
  operand->type = ASM_OPTYPE_REG;

  if (operand->destination || operand->writeback)
    {
      /* PC shouldn't be used as base address with writeback */
      /* If by any chance someone uses it this way, we will flag the instruction as a branch */
      switch (operand->baser)
        {
        case ASM_ARM_REG_R13:
          /* R13 = SP */
          MASSIGNTYPE(ins, ASM_TYPE_TOUCHSP);
          break;
        case ASM_ARM_REG_PC:
          MASSIGNTYPE(ins, ASM_TYPE_BRANCH);
          break;
        }
    }

  LIBASM_PROFILE_FOUT(0);
}
