/*
** $Id: op_sub_rmb_rb.c,v 1.4 2007-05-29 00:40:28 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_sub_rmb_rb" opcode="0x28"/>
*/



int op_sub_rmb_rb(asm_instr *new, u_char *opcode, u_int len, 
		  asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_SUB;
  new->type = ASM_TYPE_ARITH;
  #if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODEDBYTE, 
				new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_GENERALBYTE, 
				new);
  #else
  new->op1.type = ASM_OTYPE_ENCODED;
  new->ptr_instr = opcode;
  new->op2.type = ASM_OTYPE_GENERAL;
  operand_rmb_rb(new, opcode + 1, len - 1, proc);
  #endif
  return (new->len);
}
