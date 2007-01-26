/*
** $Id: op_outsw.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_outsw" opcode="0x6f"/>
  <instruction func="op_outsd" opcode="0x6f"/>
*/

int op_outsw(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
  
  if (!asm_proc_opsize(proc))
    new->instr = ASM_OUTSW;
  else
    new->instr = ASM_OUTSD;
  
  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op1.regset = ASM_REGSET_R16;
  new->op1.base_reg = ASM_REG_DX;
  
  new->op2.type = ASM_OTYPE_XSRC;
  new->op2.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op2.base_reg = ASM_REG_ESI;
  new->op2.regset = asm_proc_addsize(proc) ? ASM_REGSET_R16 :
    ASM_REGSET_R32;
  return (new->len);
}
