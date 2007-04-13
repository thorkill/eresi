/*
** $Id: op_cld.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_cld" opcode="0xfc"/>
*/

int op_cld(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_CLD;
  #if LIBASM_USE_OPERAND_VECTOR
  #endif
  return (new->len);
}
