/*
** $Id: op_sahf.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction opcode="0x9e" func="op_sahf"/>
 */

int op_sahf(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
    new->instr = ASM_SAHF;
  return (new->len);
}
