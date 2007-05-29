/*
** $Id: op_stosd.c,v 1.3 2007-05-29 00:40:28 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_stosd" opcode="0xab"/>
*/

int op_stosd(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->instr = ASM_STOSD;
  new->ptr_instr = opcode;
  new->len += 1;
  
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_YDEST, new);
  new->len += asm_operand_fetch(&new->op2, opcode, ASM_OTYPE_FIXED, new);
  new->op2.content = ASM_OP_BASE;
  new->op2.len = 0;
  new->op2.regset = asm_proc_opsize(proc) ?
    ASM_REGSET_R16 : ASM_REGSET_R32;
  new->op2.base_reg = ASM_REG_EAX;
#else
  new->op1.type = ASM_OTYPE_YDEST;
  new->op2.type = ASM_OTYPE_XSRC;
  
  new->op1.content = ASM_OP_FIXED | ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.prefix = ASM_PREFIX_ES;
  new->op1.regset = ASM_REGSET_R32;
  new->op1.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op1.base_reg = ASM_REG_EDI;
  new->op1.len = 0;

  new->op2.content = ASM_OP_BASE;
  new->op2.len = 0;
  new->op2.regset = asm_proc_opsize(proc) ?
    ASM_REGSET_R16 : ASM_REGSET_R32;
  new->op2.base_reg = ASM_REG_EAX;
#endif
  return (new->len);
}
