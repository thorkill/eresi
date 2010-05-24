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

  if (operand->destination
      || operand->indexing == ASM_ARM_ADDRESSING_POSTINDEXED
      || operand->indexing == ASM_ARM_ADDRESSING_PREINDEXED)
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
        case ASM_ARM_REG_CPSR:
          if (operand->imm & 0x08)
            {
              MASSIGNTYPE(ins, ASM_TYPE_WRITEFLAG);
              /* If any flag is added to e_arm_flags, please update this assignment */
              ins->flagswritten = ASM_ARM_FLAG_N | ASM_ARM_FLAG_Z | ASM_ARM_FLAG_C | ASM_ARM_FLAG_V | ASM_ARM_FLAG_Q;
            }
          break;
        }
    }

  LIBASM_PROFILE_FOUT(0);
}
