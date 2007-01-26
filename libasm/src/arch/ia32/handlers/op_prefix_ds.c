/*
** $Id: op_prefix_ds.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
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
  return (proc->fetch(new, opcode + 1, len - 1, proc));
}
