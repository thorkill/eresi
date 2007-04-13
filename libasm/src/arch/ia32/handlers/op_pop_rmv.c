/*
** $Id: op_pop_rmv.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_pop_rmv" opcode="0x8f"/>
*/

int op_pop_rmv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->len += 1;
  new->instr = ASM_POP;
  new->ptr_instr = opcode;

#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODED, proc);
#else
  new->op1.type = ASM_OTYPE_ENCODED;
  operand_rmv(&new->op1, opcode + 1, len - 1, proc);
  new->len += new->op1.len;
#endif
  return (new->len);
}
