/*
** $Id: op_arpl_ew_rw.c,v 1.4 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_arpl_ew_rw" opcode="0x63"/>
*/

int     op_arpl_ew_rw(asm_instr *new, unsigned char *opcode, unsigned int len,
		      asm_processor *proc)
{
  new->ptr_instr = opcode;
  new->instr = ASM_ARPL;
  new->len += 1;
  return (new->len);
}
