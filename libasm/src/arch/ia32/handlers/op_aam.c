/*
** $Id: op_aam.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction opcode="0xd4" func="op_aam"/>
*/

int op_aam(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_AAM;
#if LIBASM_USE_OPERAND_VECTOR
#endif
  return (new->len);
}
