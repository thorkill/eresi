/*
** $Id: i386_movzbl_rv_rmb.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_movzbl_rv_rmb" opcode="0xb6"/>
*/

int i386_movzbl_rv_rmb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
    if (asm_proc_opsize(proc))
      new->instr = ASM_MOVZBW;
    else
      new->instr = ASM_MOVZBL;
    new->op1.type = ASM_OTYPE_GENERAL;
    new->op1.size = ASM_OSIZE_VECTOR;
    new->op2.type = ASM_OTYPE_ENCODED;
    new->op2.size = ASM_OSIZE_BYTE;
    operand_rv_rmb(new, opcode + 1, len - 1, proc);
    return (new->len);
}
