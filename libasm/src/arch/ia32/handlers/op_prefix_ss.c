/*
** $Id: op_prefix_ss.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_prefix_ss" opcode="0x36"/>
*/

int     op_prefix_ss(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->prefix |= ASM_PREFIX_SS;
  if (!new->ptr_prefix)
    new->ptr_prefix = opcode;
#if LIBASM_USE_OPERAND_VECTOR
#endif
  return (proc->fetch(new, opcode + 1, len - 1, proc));
}
