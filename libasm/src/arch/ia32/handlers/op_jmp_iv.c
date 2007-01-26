/*
** $Id: op_jmp_iv.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_jmp_iv" opcode="0xe9"/>
 */

int op_jmp_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {

    new->instr = ASM_BRANCH;
    new->type = ASM_TYPE_IMPBRANCH;
    new->ptr_instr = opcode;
    new->len += 5;
    // new->type = IS_JMP;

    new->op1.type = ASM_OTYPE_JUMP;
    new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
    new->op1.ptr = opcode + 1;
    memcpy(&new->op1.imm, opcode + 1, 4);
  return (new->len);
}
