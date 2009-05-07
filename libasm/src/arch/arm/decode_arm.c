#include "libasm.h"

void	arm_decode_dataproc_shfop(asm_instr *ins, u_char *buf, u_int op_nr,
                                  struct s_arm_decode_dataproc *opcode)
{
  asm_operand *op;

  op = &ins->op[op_nr];

  if (opcode->i)
    {
      u_int64_t temp;

      temp = opcode->immed << (32 - opcode->rotate_imm*2);
      temp = temp | (temp >> 32);
      op->imm = (u_int32_t) temp;
      asm_arm_op_fetch(op, buf, ASM_ARM_OTYPE_IMMEDIATE, ins);
    }
  else
    {
      op->baser = opcode->rm;
      op->shift_type = opcode->shift;

      if (opcode->reg_shift)
        {
          /* Scaled by register */
          op->indexr = opcode->rs;
          op->imm = 0;
          asm_arm_op_fetch(op, buf, ASM_ARM_OTYPE_REG_SCALED, ins);
        }
      else
        {
          /* Scaled by immediate */
          op->imm = opcode->shift_imm;
          op->indexr = ASM_ARM_REG_NUM;

          if (opcode->shift_imm == 0 && opcode->shift == ASM_ARM_SHIFT_LSL)
            asm_arm_op_fetch(op, buf, ASM_ARM_OTYPE_REGISTER, ins);
          else
            asm_arm_op_fetch(op, buf, ASM_ARM_OTYPE_REG_SCALED, ins);
        }
    }
}

void	arm_decode_ldst_offop(asm_instr *ins, u_char *buf, u_int op_nr,
                              struct s_arm_decode_ldst *opcode)
{
  asm_operand *op;

  op = &ins->op[op_nr]; 

  op->baser = opcode->rn;
  /* (Scaled) register offset */
  op->indexr = opcode->rm;
  op->imm = opcode->shift_imm;
  op->shift_type = opcode->shift;

  op->offset_added = opcode->u;
  op->preindexed = opcode->p;
  op->writeback = opcode->w;

  if (!opcode->reg_offset)
    /* Immediate offset */
    {
      op->imm = opcode->imm_offset;
      op->indexr = ASM_ARM_REG_NUM;
      op->shift_type = ASM_ARM_SHIFT_NUM;
    }

  asm_arm_op_fetch(op, buf, ASM_ARM_OTYPE_REG_OFFSET, ins);
}

void	arm_decode_ldst_misc_offop(asm_instr *ins, u_char *buf, u_int op_nr,
                                   struct s_arm_decode_ldst_misc *opcode)
{
  asm_operand *op;

  op = &ins->op[op_nr];

  op->baser = opcode->rn;

  op->offset_added = opcode->u;
  op->preindexed = opcode->p;
  op->writeback = opcode->w;

  if (opcode->i)
    /* Immediate offset */
    op->imm = (opcode->immedH << 4) | (opcode->immedL_rm);
  else
    op->indexr = opcode->immedL_rm;

  asm_arm_op_fetch(op, buf, ASM_ARM_OTYPE_REG_OFFSET, ins);
}


// XXX: this can become the handler for smlal, smull, umlal and umull */
void	arm_decode_multiply_long(asm_instr *ins, u_char *buf,
                                 struct s_arm_decode_multiply *opcode)
{
  ins->op[0].baser = opcode->r2; /* This is RdLo */
  asm_arm_op_fetch(&ins->op[0], buf, ASM_ARM_OTYPE_REGISTER, ins);

  ins->op[1].baser = opcode->r1; /* This is RdHi */
  asm_arm_op_fetch(&ins->op[1], buf, ASM_ARM_OTYPE_REGISTER, ins);

  ins->op[2].baser = opcode->r4; /* This is Rs */
  asm_arm_op_fetch(&ins->op[2], buf, ASM_ARM_OTYPE_REGISTER, ins);

  ins->op[3].baser = opcode->r3; /* This is Rm */
  asm_arm_op_fetch(&ins->op[3], buf, ASM_ARM_OTYPE_REGISTER, ins);
}

void    arm_decode_branch1_imm(asm_instr *ins, u_char *buf,
                               struct s_arm_decode_branch1 *opcode)
{
  /* Address = PC + 8 + (SignExt(imm) << 2) */
  if (opcode->signed_imm & 0x0800000)
    /* Sign extend */
    ins->op[0].imm |= 0xFF000000;
  ins->op[0].imm <<= 2;
  ins->op[0].imm += 8;
}
