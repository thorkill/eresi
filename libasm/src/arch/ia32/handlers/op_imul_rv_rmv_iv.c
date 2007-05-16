/*
** $Id: op_imul_rv_rmv_iv.c,v 1.3 2007-05-16 18:38:13 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_imul_rv_rmv_iv" opcode="0x69"/>
 */

int     op_imul_rv_rmv_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  int		olen;
  new->instr = ASM_IMUL;
  new->type = ASM_TYPE_ARITH;
  new->ptr_instr = opcode;
  new->len += 1;
  
  #if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_GENERAL, proc);
  new->len += (olen = asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_ENCODED, proc));
  new->len += asm_operand_fetch(&new->op3, opcode + 1 + olen, ASM_OTYPE_IMMEDIATE, proc);
  #else
  
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  
  new->op3.type = ASM_OTYPE_IMMEDIATE;
  new->op3.ptr = opcode + new->len;
  new->op3.content = ASM_OP_VALUE;
  memcpy(&new->op3.imm, new->op3.ptr, 4);
  new->op3.len = 4;
  new->len += 4;
#endif
  return (new->len);
}
