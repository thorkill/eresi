/*
** $Id: op_insb.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_insb" opcode="0x6b"/>
*/

int     op_insb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_INSB;
  new->ptr_instr = opcode;
  
  
  new->op2.type = ASM_OTYPE_FIXED;
  new->op2.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op2.regset = ASM_REGSET_R16;
  new->op2.base_reg = ASM_REG_EDX;
  
  new->op1.type = ASM_OTYPE_YDEST;
  new->op1.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op1.prefix = ASM_PREFIX_DS;
  new->op1.base_reg = ASM_REG_EDI;
  new->op1.regset = asm_proc_addsize(proc) ? ASM_REGSET_R16 :
    ASM_REGSET_R32;
  return (new->len);
}
