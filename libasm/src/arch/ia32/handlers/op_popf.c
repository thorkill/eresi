/*
** $Id: op_popf.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction name="popf" func="op_popf" opcode="0x9d"/>
*/

int op_popf(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
    new->len += 1;
    new->instr = ASM_POPF;
    // new->type = IS_MEM_READ;
  return (new->len);
}
