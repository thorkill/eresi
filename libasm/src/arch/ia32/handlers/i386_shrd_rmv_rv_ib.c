/*
** $Id: i386_shrd_rmv_rv_ib.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
 <i386 func="i386_shrd_rmv_rv_ib" opcode="0xac"/>
*/

int i386_shrd_rmv_rv_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->instr = ASM_SHRD;
    new->len += 1;
    new->op1.type = ASM_OTYPE_ENCODED;
    new->op1.size = ASM_OSIZE_VECTOR;
    new->op2.type = ASM_OTYPE_GENERAL;
    new->op2.size = ASM_OSIZE_VECTOR;
    new->op3.type = ASM_OTYPE_IMMEDIATE;
    new->op3.size = ASM_OSIZE_BYTE;
    operand_rmv_rv(new, opcode + 1, len - 1, proc);
    new->op3.type = ASM_OTYPE_IMMEDIATE;
    new->op3.content = ASM_OP_VALUE;
    new->op3.ptr = opcode + 2;
    new->op3.len = 1;
    
    new->op3.imm = 0;
    memcpy(&new->op3.imm, opcode + 2, 1);
    new->len += 1;
  return (new->len);
}
