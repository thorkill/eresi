/*
** $Id: op_les_rm_rmp.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_les_rm_rmp" opcode="0xc4"/>
*/

int op_les_rm_rmp(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
    new->instr = ASM_LES;
    new->len += 1;
  return (new->len);
}
