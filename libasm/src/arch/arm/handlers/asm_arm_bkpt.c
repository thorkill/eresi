/**
* @file libasm/src/arch/arm/handlers/asm_arm_bkpt.c
** @ingroup ARM_instrs
*/
#include "libasm.h"

int     asm_arm_bkpt(asm_instr * ins, u_char * buf, u_int len,
                    asm_processor * proc)
{
  struct s_arm_decode_bkpt opcode;

  LIBASM_PROFILE_FIN();

  arm_convert_bkpt(&opcode, buf);

  ins->conditional = 0;

  ins->instr = ASM_ARM_BKPT;

  ins->name = ins->proc->instr_table[ins->instr];

  MASSIGNTYPE(ins, ASM_TYPE_INT);

  ins->nb_op = 1;

  /* Decode operands */
  ins->op[0].imm = (opcode.immed1 << 4) | opcode.immed2;
  asm_arm_op_fetch(&ins->op[0], buf, ASM_ARM_OTYPE_IMMEDIATE, ins);

  // TODO: try to understand if we shall flag as READFLAG/WRITEFLAG

  LIBASM_PROFILE_FOUT(4);
}
