#include <libasm.h>

int asm_arm_op_fetch_reg_scaled(asm_operand *operand, u_char *opcode,
                                int otype, asm_instr *ins)
{
  LIBASM_PROFILE_FIN();

  operand->content = ASM_ARM_OTYPE_REG_SCALED;
  operand->type = ASM_OPTYPE_REG;

  if (operand->indexr == ASM_ARM_REG_NUM)
    {
      if (operand->imm == 0)
        if (operand->shift_type == ASM_ARM_SHIFT_ROR)
          operand->shift_type = ASM_ARM_SHIFT_RRX;
        else
          operand->imm = 32;
    }

  LIBASM_PROFILE_FOUT(0);
}
