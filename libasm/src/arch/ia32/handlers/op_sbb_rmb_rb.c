/*
** $Id: op_sbb_rmb_rb.c,v 1.3 2007-05-29 00:40:28 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_sbb_rmb_rb" opcode="0x18"/>
*/

int op_sbb_rmb_rb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->len += 1;
  new->instr = ASM_SBB;
  new->ptr_instr = opcode;

#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, 
				ASM_OTYPE_ENCODEDBYTE, new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, 
				ASM_OTYPE_GENERALBYTE, new);
#else
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op2.size = new->op1.size = ASM_OSIZE_BYTE;
  operand_rmb_rb(new, opcode + 1, len - 1, proc);
#endif
  return (new->len);
}
