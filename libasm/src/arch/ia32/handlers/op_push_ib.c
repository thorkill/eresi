/*
** $Id: op_push_ib.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_push_ib" opcode="0x6a"/>
*/

int op_push_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_PUSH;
  new->len += 1;
  new->ptr_instr = opcode;
  
  new->op1.type = ASM_OTYPE_IMMEDIATE;
  new->op1.content = ASM_OP_VALUE;
  new->op1.ptr = opcode + 1;
  new->op1.len = 1;
  
  new->op1.imm = 0;
  if (*(opcode + 1) > 0x80u)
    memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
  memcpy(&new->op1.imm, opcode + 1, 1);
  new->len += 1;
  return (new->len);
}
