/*
** $Id: op_imul_gv_ev_ib.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_imul_gv_ev_ib" opcode="0x6b"/>
*/

int     op_imul_gv_ev_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  int	olen;
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_IMUL;

#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_GENERAL, proc);
  new->len += (olen = asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_ENCODED, proc));
  new->len += asm_operand_fetch(&new->op3, opcode + 1 + olen, ASM_OTYPE_IMMEDIATEBYTE, proc);
#else
  
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  
  new->op3.type = ASM_OTYPE_IMMEDIATE;
  new->op3.content = ASM_OP_VALUE;
  new->op3.ptr = opcode + 1 + new->op1.len + new->op2.len;
  new->op3.len = 1;
  
  new->len += 1;
  if (*new->op3.ptr >= 0x80)
    memcpy((char *) &new->op3.imm + 1, "\xff\xff\xff", 3);
  else
    new->op3.imm = 0;
  memcpy(&new->op3, new->op3.ptr, 1);
#endif
  return (new->len);
}
