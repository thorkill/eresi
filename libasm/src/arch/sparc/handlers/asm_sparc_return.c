/*
**
** $Id: asm_sparc_return.c,v 1.7 2007/10/14 00:01:42 heroine Exp $
**
*/
#include "libasm.h"

/**
 * Sparc handler for the return instruction.
 * @param ins Pointer to instruction structure.
 * @param buf Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @param Return instruction length.
 */

int
asm_sparc_return(asm_instr * ins, unsigned char * buf, unsigned int len,
		 asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);

  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];

  ins->type = ASM_TYPE_RETPROC;

  ins->nb_op = 1;

  if (opcode.i) {
    ins->op[0].baser = opcode.rs1;
    ins->op[0].imm = opcode.imm;
    asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_IMM_ADDRESS, ins);
  }
  else {
    ins->op[0].baser = opcode.rs1;
    ins->op[0].indexr = opcode.rs2;
    asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_REG_ADDRESS, ins);
  }

  return 4;
}
