/*
** $Id: op_prefix_es.c,v 1.4 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_prefix_es" opcode="0x26"/>
*/

int     op_prefix_es(asm_instr *new, unsigned char *opcode, unsigned int len,
		     asm_processor *proc)
{
  new->prefix |= ASM_PREFIX_ES;
  if (!new->ptr_prefix)
    new->ptr_prefix = opcode;
  new->len += 1;
  return (proc->fetch(new, opcode + 1, len - 1, proc));
}
