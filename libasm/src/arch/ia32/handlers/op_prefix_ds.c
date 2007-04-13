/*
** $Id: op_prefix_ds.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_prefix_ds" opcode="0x3e"/>
*/

int op_prefix_ds(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  if (!new->ptr_prefix)
  new->ptr_prefix = opcode;
  new->prefix |= ASM_PREFIX_DS;
#if LIBASM_USE_OPERAND_VECTOR
#endif
  return (proc->fetch(new, opcode + 1, len - 1, proc));
}
