/*
** $Id: op_aas.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_aas" opcode="0x3f"/>
 */

int     op_aas(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->len += 1;
  new->ptr_instr = opcode;
  
  new->instr = ASM_AAS;
#if LIBASM_USE_OPERAND_VECTOR
#endif
  return (new->len);
}
