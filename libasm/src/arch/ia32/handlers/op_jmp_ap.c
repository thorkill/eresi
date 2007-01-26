/*
** $Id: op_jmp_ap.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_jmp_ap" opcode="0xea"/>
 */

int     op_jmp_ap(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->ptr_instr = opcode;
  new->instr = ASM_LBRANCH;
  new->len += 7;
  new->type = ASM_TYPE_IMPBRANCH;
  
  new->op1.type = ASM_OTYPE_ADDRESS;
  new->op1.content = ASM_OP_VALUE;
  new->op1.len = 4;
  memcpy(&new->op1.imm, opcode + 1, 4);
  
  /*
  new->op2 = &new->op2;
  new->op2.len = 2;
  new->op2.content = ASM_OP_VALUE;
  new->op2.imm = 0;
  memcpy(&new->op2.imm, opcode + 5, 2);
  */
  
  return (new->len);
}
