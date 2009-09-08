/**
 * @file op_repz.c
ngroup ia-instrs/g
 * @ingroup instrs
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_repz" opcode="0xf3"/>
 */

int op_repz(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  if (!new->ptr_prefix)
    new->ptr_prefix = opcode;
  new->len += 1;
  new->prefix = ASM_PREFIX_REP;
  return (proc->fetch(new, opcode + 1, len - 1, proc));
}
