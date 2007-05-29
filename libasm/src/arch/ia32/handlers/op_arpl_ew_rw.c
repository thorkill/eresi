/*
** $Id: op_arpl_ew_rw.c,v 1.3 2007-05-29 00:40:27 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_arpl_ew_rw" opcode="0x63"/>
*/

int     op_arpl_ew_rw(asm_instr *new, u_char *opcode, u_int len, 
		      asm_processor *proc) 
{
  new->ptr_instr = opcode;
  new->instr = ASM_ARPL;
  new->len += 1;
  return (new->len);
}
