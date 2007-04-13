/*
** $Id: op_sub_eax_iv.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_sub_eax_iv" opcode="0x2d"/>
*/

int op_sub_eax_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  
  new->instr = ASM_SUB;
  new->len += 1;
  new->ptr_instr = opcode;
  
  #if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, proc);
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.ptr = opcode;
  new->op1.len = 0;
  new->op1.regset = ASM_REGSET_R32;
  new->op1.base_reg = ASM_REG_EAX;
  new->len += asm_operand_fetch(&new->op2, opcode, ASM_OTYPE_IMMEDIATE, proc);
  #else
  new->op1.type = ASM_OTYPE_FIXED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.ptr = opcode;
  new->op1.len = 0;
  new->op1.regset = ASM_REGSET_R32;
  new->op1.base_reg = ASM_REG_EAX;
  
  new->op2.content = ASM_OP_VALUE;
  new->op2.ptr = opcode + 1;
  new->op2.len = asm_proc_vector_len(proc);
  new->op2.imm = 0;
  new->len +=  asm_proc_vector_len(proc);
  memcpy(&new->op2.imm, opcode + 1, asm_proc_vector_len(proc));
  #endif
  return (new->len);
}
