#include <libasm.h>

int asm_arm_op_fetch_reg_shf_imm(asm_operand *operand, u_char *opcode,
                                 int otype, asm_instr *ins)
{
  LIBASM_PROFILE_FIN();

  operand->content = ASM_ARM_OTYPE_REG_SHIFTED_IMM;
  operand->type = ASM_OPTYPE_REG;

 if (operand->imm == 0 && operand->shift_type == ASM_ARM_SHIFT_ROR)
   operand->shift_type = ASM_ARM_SHIFT_RRX;

  LIBASM_PROFILE_FOUT(0);
}
