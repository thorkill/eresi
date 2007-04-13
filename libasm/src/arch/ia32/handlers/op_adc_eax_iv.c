/*
** $Id: op_adc_eax_iv.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_adc_eax_iv" opcode="0x15"/>
*/

int op_adc_eax_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_ADC;
  new->ptr_instr = opcode;
  new->len += 1;
  
  #if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, proc);
  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.size = new->op2.size = asm_proc_vector_size(proc);
  
  new->op1.content = ASM_OP_FIXED | ASM_OP_BASE;
  new->op1.base_reg = ASM_REG_EAX;
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_IMMEDIATE, proc);
  #else
  new->op1.type = ASM_OTYPE_FIXED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op1.size = new->op2.size = asm_proc_vector_size(proc);
  
  new->op1.content = ASM_OP_FIXED | ASM_OP_BASE;
  new->op1.base_reg = ASM_REG_EAX;
  
  new->op2.content = ASM_OP_VALUE;
  new->op2.ptr = opcode;
  new->op2.len = asm_proc_vector_len(proc);
  memcpy(&new->op2.imm, opcode + 1, 4);
  new->len += 4;
  #endif
  return (new->len);
}
