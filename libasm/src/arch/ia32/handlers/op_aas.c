/*
** $Id: op_aas.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_aas" opcode="0x3f"/>
 */

int     op_aas(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  
  new->instr = ASM_AAS;
  return (new->len);
}
