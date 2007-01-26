/*
** $Id: op_in_eax_ref_ib.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_in_eax_ref_ib" opcode="0xe5"/>
*/

int op_in_eax_ref_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_IN;
    new->ptr_instr = opcode;
  new->len += 1;
  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.base_reg = ASM_REG_EAX;
  new->op1.regset = asm_proc_opsize(proc) ?
    ASM_REGSET_R16 : ASM_REGSET_R32;
  
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op2.content = ASM_OP_REFERENCE | ASM_OP_VALUE;
  
  return (new->len);
}
