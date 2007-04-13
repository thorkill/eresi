/*
** $Id: op_cwtl.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_cwtl" opcode="0x98"/>
*/

int     op_cwtl(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  if (asm_proc_opsize(proc))
    new->instr = ASM_CBTW;
  else
    new->instr = ASM_CWTL;
  #if LIBASM_USE_OPERAND_VECTOR
  #endif
  return (new->len);
}
