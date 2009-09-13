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

  LIBASM_PROFILE_FOUT(0);
}
