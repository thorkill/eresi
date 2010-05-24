/**
* @file libasm/src/arch/arm/handlers/asm_arm_swp.c
** @ingroup ARM_instrs
*/
#include "libasm.h"

int     asm_arm_swp(asm_instr * ins, u_char * buf, u_int len,
                   asm_processor * proc)
{
  struct s_arm_decode_ldst opcode;
  struct s_asm_proc_arm *inter;

  LIBASM_PROFILE_FIN();

  inter = proc->internals;
  arm_convert_swap(&opcode,buf);

  arm_decode_condition(ins, opcode.cond);

  ins->instr = inter->swap_table[(opcode.b << 4) | opcode.cond];

  ins->name = ins->proc->instr_table[ins->instr];

  MASSIGNTYPE(ins, ASM_TYPE_LOAD | ASM_TYPE_STORE);

  ins->nb_op = 3;

  /* Decode operands */
  ins->op[0].baser = opcode.rd;
  ins->op[0].destination = 1;
  asm_arm_op_fetch(&ins->op[0], buf, ASM_ARM_OTYPE_REGISTER, ins);

  ins->op[1].baser = opcode.rm;
  asm_arm_op_fetch(&ins->op[1], buf, ASM_ARM_OTYPE_REGISTER, ins);

  /* Use register offset type to print with [] surrounding 
   * the register name.
   */
  ins->op[2].baser = opcode.rn;
  ins->op[2].indexing = ASM_ARM_ADDRESSING_OFFSET;
  ins->op[2].imm = 0;
  ins->op[2].offset_added = 1;
  asm_arm_op_fetch(&ins->op[2], buf, ASM_ARM_OTYPE_REG_OFFSET, ins);

  if (MISTYPE(ins, ASM_TYPE_BRANCH)
      || MISTYPE(ins, ASM_TYPE_CALLPROC)
      || MISTYPE(ins, ASM_TYPE_RETPROC))
    {
      MASSIGNTYPE(ins, ASM_TYPE_INDCONTROL);
    }

  LIBASM_PROFILE_FOUT(4);
}
