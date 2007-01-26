/*
** $Id: op_sbb_eax_iv.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_sbb_eax_iv" opcode="0x1d"/>
*/

int op_sbb_eax_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  
  new->len += 5;
  new->ptr_instr = opcode;
  new->instr = ASM_SBB;
  new->op1.type = ASM_OTYPE_FIXED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op1.size = new->op2.size = ASM_OSIZE_VECTOR;
  
  
  new->op1.content = ASM_OP_FIXED | ASM_OP_BASE;
  new->op1.base_reg = ASM_REG_EAX;
  new->op1.regset = asm_proc_opsize(proc) ? ASM_REGSET_R32 :
    ASM_REGSET_R16;
  
  new->op2.content = ASM_OP_VALUE;
  new->op2.ptr = opcode;
  new->op2.len = asm_proc_vector_len(proc);;
  memcpy(&new->op2.imm, opcode + 1, 4);
  return (new->len);
}
