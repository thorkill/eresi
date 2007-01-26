/*
** $Id: op_js.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_js" opcode="0x78"/>
*/

int  op_js(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 2;
  new->type = ASM_TYPE_CONDBRANCH;
  new->instr = ASM_BRANCH_SIGNED;
  new->op1.type = ASM_OTYPE_JUMP;
  new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
  
  new->op1.imm = 0;
  if (*(opcode + 1) >= 0x80u)
    memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
  memcpy(&new->op1.imm, opcode + 1, 1);
  return (new->len);
}
