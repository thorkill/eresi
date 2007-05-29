/*
** $Id: op_opsize.c,v 1.3 2007-05-29 00:40:27 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_opsize" opcode="0x66"/>
 */

int     op_opsize(asm_instr *new, u_char *opcode, u_int len, 
		  asm_processor *proc)
{
  asm_i386_processor    *i386p;
  
  if (!new->ptr_prefix)
    new->ptr_prefix = opcode;
  i386p = (asm_i386_processor *) proc;
  
  i386p->internals->opsize = !i386p->internals->opsize;
  new->len += 1;
  new->prefix |= ASM_PREFIX_OPSIZE;
  len = proc->fetch(new, opcode + 1, len - 1, proc);
  i386p->internals->opsize = !i386p->internals->opsize;
  return (len);
}
