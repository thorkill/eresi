/*
** $Id: op_and_rmv_rv.c,v 1.4 2007-05-29 00:40:27 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_and_rmv_rv" opcode="0x21"/>
*/

int op_and_rmv_rv(asm_instr *new, u_char *opcode, u_int len, 
		  asm_processor *proc) 
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_ARITH;
  new->instr = ASM_AND;
  
  #if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, 
				ASM_OTYPE_ENCODED, new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, 
				ASM_OTYPE_GENERAL, new);
  #else
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_GENERAL;
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
#endif
  return (new->len);
}
