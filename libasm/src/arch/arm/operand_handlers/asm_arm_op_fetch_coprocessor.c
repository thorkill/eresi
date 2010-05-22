/**
* @file libasm/src/arch/arm/operand_handlers/asm_arm_op_fetch_coprocessor.c
** @ingroup ARM_operands
*/
#include <libasm.h>

int asm_arm_op_fetch_coprocessor(asm_operand *operand, u_char *opcode,
                                 int otype, asm_instr *ins)
{
  LIBASM_PROFILE_FIN();

  operand->content = ASM_ARM_OTYPE_COPROC;
  operand->type = ASM_OPTYPE_IMM;

  LIBASM_PROFILE_FOUT(0);
}
