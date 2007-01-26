/*
** $Id: op_aad.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction opcode="0xd5" func="op_aad"/>
*/
int op_aad(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->len += 1;
  new->ptr_instr = opcode;
    new->instr = ASM_AAD;
  return (new->len);
}
