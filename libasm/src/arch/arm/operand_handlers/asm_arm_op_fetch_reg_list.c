/**
* @file libasm/src/arch/arm/operand_handlers/asm_arm_op_fetch_reg_list.c
** @ingroup ARM_operands
*/
#include "libasm.h"

int asm_arm_op_fetch_reg_list(asm_operand *operand, u_char *opcode,
                                int otype, asm_instr *ins)
{
  LIBASM_PROFILE_FIN();

  operand->content = ASM_ARM_OTYPE_REG_LIST;
  operand->type = ASM_OPTYPE_REG;

  if (operand->destination)
    {
      if (operand->imm & (1 << 13))
        {
          /* R13 = SP */
          ins->type |= ASM_TYPE_TOUCHSP;
        }
      else if (operand->imm & (1 << 15))
        {
          if (ins->conditional)
            ins->type |= ASM_TYPE_CONDBRANCH;
          else
            ins->type |= ASM_TYPE_IMPBRANCH;
        }
    }

  LIBASM_PROFILE_FOUT(0);
}
