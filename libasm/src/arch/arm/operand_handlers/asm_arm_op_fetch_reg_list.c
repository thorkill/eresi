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
          MASSIGNTYPE(ins, ASM_TYPE_TOUCHSP);
        }
      if (operand->imm & (1 << 15))
        {
          /* PC */

          if (ins->instr == ASM_ARM_LDMIA
              && ins->op[0].baser == ASM_ARM_REG_R13
              && ins->op[0].indexing == ASM_ARM_ADDRESSING_POSTINDEXED)
            {
              /* LDMIA SP!, {Reg list} (POP) is usually used as return of a function */
              MASSIGNTYPE(ins, ASM_TYPE_RETPROC);
            }
          else
            {
              MASSIGNTYPE(ins, ASM_TYPE_BRANCH);
            }
        }
    }

  LIBASM_PROFILE_FOUT(0);
}
