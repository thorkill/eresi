/*
** $Id: op_or_rv_rmv.c,v 1.4 2007-05-29 00:40:27 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  Opcode :              0x0b
  Instruction :         OR
*/

int op_or_rv_rmv(asm_instr *new, u_char *opcode, u_int len, 
		 asm_processor *proc) 
{
  new->instr = ASM_OR;
  new->type = ASM_TYPE_ARITH;
  new->ptr_instr = opcode;
  new->len += 1;
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, 
				ASM_OTYPE_GENERAL, new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, 
				ASM_OTYPE_ENCODED, new);
#else
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  new->op1.size = new->op2.size = asm_proc_vector_size(proc);
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
#endif
  return (new->len);
}
