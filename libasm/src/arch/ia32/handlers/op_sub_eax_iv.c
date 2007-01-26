/*
** $Id: op_sub_eax_iv.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_sub_eax_iv" opcode="0x2d"/>
*/

int op_sub_eax_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  
  new->instr = ASM_SUB;
  new->len += 1 + asm_proc_vector_len(proc);
  new->ptr_instr = opcode;
  
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
  memcpy(&new->op2.imm, opcode + 1, asm_proc_vector_len(proc));
  return (new->len);
}
