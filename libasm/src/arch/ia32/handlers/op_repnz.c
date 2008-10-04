/**
 * @file op_repnz.c
 * @ingroup handlers_ia32
** $Id: op_repnz.c,v 1.5 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_repnz" opcode="0xf2"/>
*/

int op_repnz(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc)
{
  if (!new->ptr_prefix)
    new->ptr_prefix = opcode;
  new->len += 1;
  new->prefix = ASM_PREFIX_REPNE;

  return (proc->fetch(new, opcode + 1, len - 1, proc));
}
