/*
** $Id: op_lock.c,v 1.4 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_lock" opcode="0xf0"/>
*/

int op_lock(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc)
{
  if (!new->ptr_prefix)
    new->ptr_prefix = opcode;
  new->len += 1;
  new->prefix = ASM_PREFIX_LOCK;
  return (proc->fetch(new, opcode + 1, len - 1, proc));
}
