/*
** $Id: op_prefix_fs.c,v 1.3 2007-05-29 00:40:27 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_prefix_fs" opcode="0x64"/>
 */

int     op_prefix_fs(asm_instr *new, u_char *opcode, u_int len, 
		     asm_processor *proc) 
{
  new->prefix |= ASM_PREFIX_FS;
  if (!new->ptr_prefix)
    new->ptr_prefix = opcode;
  new->len += 1;
  return (proc->fetch(new, opcode + 1, len - 1, proc));
}
