/**
** @file asm_arm_op_fetch_reg_offset.c
** @ingroup ARM_operands
*/
#include <libasm.h>

int asm_arm_op_fetch_reg_offset(asm_operand *operand, u_char *opcode,
                                int otype, asm_instr *ins)
{
  LIBASM_PROFILE_FIN();

  operand->content = ASM_ARM_OTYPE_REG_OFFSET;
  operand->type = ASM_OPTYPE_REG;

  if (operand->indexr != ASM_ARM_REG_NUM)
    {
      if (operand->imm == 0)
        {
          switch (operand->shift_type)
            {
            case ASM_ARM_SHIFT_LSL:
              /* Register offset */
              operand->shift_type = ASM_ARM_SHIFT_NUM;
              break;
            case ASM_ARM_SHIFT_ROR:
              operand->shift_type = ASM_ARM_SHIFT_RRX;
              break;
            case ASM_ARM_SHIFT_LSR:
            case ASM_ARM_SHIFT_ASR:
              operand->imm = 32;
              break;
            default:
              /* TODO: Error! */
              break;
            }
        }
    }

  LIBASM_PROFILE_FOUT(0);
}
