/*
** $Id: op_iret.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction opcode="0xcf" func="op_iret"/>
*/

int op_iret(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
    new->instr = ASM_IRET;
    // new->type = IS_RET;
  return (new->len);
}
