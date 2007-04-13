/*
** $Id: op_popa.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_popa" opcode="0x61"/>
*/

int op_popa(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_POPA;
#if LIBASM_USE_OPERAND_VECTOR
#endif
  return (new->len);
}
