/*
** $Id: i386_jb.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_jb" opcode="0x82"/>
*/

int i386_jb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  // new->type = IS_COND_BRANCH;
    new->instr = ASM_BRANCH_U_LESS;

    new->op1.type = ASM_OTYPE_JUMP;
    new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
    new->op1.ptr = opcode + 1;
    new->op1.len = 4;
    memcpy(&new->op1.imm, opcode + 1, 4);
    new->len += 5;
  return (new->len);
}
