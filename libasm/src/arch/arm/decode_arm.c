/**
* @file libasm/src/arch/arm/decode_arm.c
** @ingroup arm
*/
#include "libasm.h"

void	arm_decode_condition(asm_instr *ins, u_char condition)
{
  ins->conditional = 0;
  if (condition < 0x0E)
    {
      ins->conditional = 1;
      MASSIGNTYPE(ins, ASM_TYPE_CONDCONTROL);
      MASSIGNTYPE(ins, ASM_TYPE_READFLAG);
      ins->flagsread = arm_cond_flagsread_table[condition];
    }
}

/* Must be called after full decode of opcode structure and operands */
void	arm_decode_dataproc_flagswritten(asm_instr *ins, struct s_arm_decode_dataproc *opcode)
{
  u_char is_pc_dest;
  int i;

  /* Verify if PC is a destination register */
  is_pc_dest = 0;
  for (i = 0; i < ins->nb_op; i++)
    {
      if (ins->op[i].destination && ins->op[i].baser == ASM_ARM_REG_PC)
        {
          is_pc_dest = 1;
        }
    }

  if (opcode->s)
    {
      /* Update instruction type */
      MASSIGNTYPE(ins, ASM_TYPE_WRITEFLAG);

      /* Determine which flags are written */
      if (is_pc_dest)
        {
          /* CPSR = SPSR, thus all flags are written */
          /* If any flag is added to e_arm_flags, please update this assignment */
          ins->flagswritten = ASM_ARM_FLAG_N | ASM_ARM_FLAG_Z | ASM_ARM_FLAG_C | ASM_ARM_FLAG_V | ASM_ARM_FLAG_Q;
        }
      else
        {
          /* Fetch the table for the written flags */
          ins->flagswritten = arm_dataproc_flagswritten_table[opcode->op2];
        }
    }
}

void	arm_decode_dataproc_shfop(asm_instr *ins, u_char *buf, u_int op_nr,
                                  struct s_arm_decode_dataproc *opcode)
{

  asm_operand *op;

  op = &ins->op[op_nr];

  /* Decode operands */
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
  if (!opcode->p)
    op->indexing = ASM_ARM_ADDRESSING_POSTINDEXED;
  else
    {
      if (opcode->w)
        op->indexing = ASM_ARM_ADDRESSING_PREINDEXED;
      else
        op->indexing = ASM_ARM_ADDRESSING_OFFSET;
    }

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
  if (!opcode->p)
    op->indexing = ASM_ARM_ADDRESSING_POSTINDEXED;
  else
    {
      if (opcode->w)
        op->indexing = ASM_ARM_ADDRESSING_PREINDEXED;
      else
        op->indexing = ASM_ARM_ADDRESSING_OFFSET;
    }

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
  ins->op[0].destination = 1;
  asm_arm_op_fetch(&ins->op[0], buf, ASM_ARM_OTYPE_REGISTER, ins);

  ins->op[1].baser = opcode->r1; /* This is RdHi */
  ins->op[1].destination = 1;
  asm_arm_op_fetch(&ins->op[1], buf, ASM_ARM_OTYPE_REGISTER, ins);

  ins->op[2].baser = opcode->r4; /* This is Rs */
  asm_arm_op_fetch(&ins->op[2], buf, ASM_ARM_OTYPE_REGISTER, ins);

  ins->op[3].baser = opcode->r3; /* This is Rm */
  asm_arm_op_fetch(&ins->op[3], buf, ASM_ARM_OTYPE_REGISTER, ins);
}

void	arm_decode_coproc_ldst_offop(asm_instr *ins, u_char *buf, u_int op_nr,
                                     struct s_arm_decode_coproc_ldst *opcode)
{
  asm_operand *op;

  op = &ins->op[op_nr];

  op->baser = opcode->rn;

  op->offset_added = opcode->u;
  if (!opcode->p)
    {
      if (opcode->w)
        op->indexing = ASM_ARM_ADDRESSING_POSTINDEXED;
      else
        op->indexing = ASM_ARM_ADDRESSING_UNINDEXED;
    }
  else
    {
      if (opcode->w)
        op->indexing = ASM_ARM_ADDRESSING_PREINDEXED;
      else
        op->indexing = ASM_ARM_ADDRESSING_OFFSET;
    }

  op->imm = opcode->offset;
  if (op->indexing != ASM_ARM_ADDRESSING_UNINDEXED)
    op->imm *= 4;

  asm_arm_op_fetch(op, buf, ASM_ARM_OTYPE_REG_OFFSET, ins);
}
