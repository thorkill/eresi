/**
 *
 * @file op_mov_rmb_ib.c
 * $Id: op_mov_rmb_ib.c,v 1.3 2007-05-16 18:38:13 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 *
 *
 <instruction func="op_mov_rmb_ib" opcode="0xc6"/>
*/

int op_mov_rmb_ib(asm_instr *new, u_char *opcode, u_int len, 
		  asm_processor *proc)
{
  int	olen;
  new->type = ASM_TYPE_ASSIGN;
  new->len += 1;
  new->instr = ASM_MOV;
  new->ptr_instr = opcode;
#if LIBASM_USE_OPERAND_VECTOR
  new->len += (olen = asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODEDBYTE, proc));
  new->len += asm_operand_fetch(&new->op2, opcode + 1 + olen , ASM_OTYPE_IMMEDIATEBYTE, proc);
#else
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op1.size = ASM_OSIZE_BYTE;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op2.size = ASM_OSIZE_BYTE;
  operand_rmb_ib(new, opcode + 1, len - 1, proc);
#endif
  return (new->len);
}
