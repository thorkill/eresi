/*
** $Id: op_mov_rmv_iv.c,v 1.4 2007-05-29 00:40:27 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_mov_rmv_iv" opcode="0xc7"/>
*/

int op_mov_rmv_iv(asm_instr *new, u_char *opcode, u_int len, 
		  asm_processor *proc) 
{
  int	olen;
  new->len += 1;
  new->type = ASM_TYPE_ASSIGN;
  new->ptr_instr = opcode;
  new->instr = ASM_MOV;
#if LIBASM_USE_OPERAND_VECTOR
  new->len += (olen = asm_operand_fetch(&new->op1, opcode + 1, 
					ASM_OTYPE_ENCODED, new));
  new->len += asm_operand_fetch(&new->op2, opcode + 1 + olen, 
				ASM_OTYPE_IMMEDIATE, new);
#else
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op1.size = ASM_OSIZE_VECTOR;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op2.size = ASM_OSIZE_VECTOR;
  operand_rmv_iv(new, opcode + 1, len - 1, proc);
#endif
  return (new->len);
}
