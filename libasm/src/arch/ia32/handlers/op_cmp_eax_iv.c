/*
** $Id: op_cmp_eax_iv.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_cmp_eax_iv" opcode="0x3d"/>
*/

int op_cmp_eax_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_CMP;
  new->ptr_instr = opcode;
  new->len += 1 + asm_proc_vector_len(proc);
  
  new->op1.type = ASM_OTYPE_FIXED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  
  new->op1.content = ASM_OP_FIXED | ASM_OP_BASE;
  new->op1.base_reg = ASM_REG_EAX;
  new->op1.regset = asm_proc_opsize(proc) ?
    ASM_REGSET_R16 : ASM_REGSET_R32;
  
  new->op2.content = ASM_OP_VALUE;
  new->op2.ptr = opcode;
  new->op2.len = asm_proc_vector_len(proc);
  memcpy(&new->op2.imm, opcode + 1, asm_proc_vector_len(proc));
  
  return (new->len);
}
