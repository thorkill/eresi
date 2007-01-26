/*
** $Id: op_scasd.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_scasd" opcode="0xaf"/>
*/

int op_scasd(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  if (asm_proc_opsize(proc))
    new->instr = ASM_SCASW;
  else
    new->instr = ASM_SCASD;
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_FIXED;
  new->op2.type = ASM_OTYPE_YDEST;
  
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_R8;
  new->op1.base_reg = ASM_REG_EAX;
  
  new->op2.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op2.regset = ASM_REGSET_R32;
  new->op2.prefix = ASM_PREFIX_ES;
  new->op2.base_reg = ASM_REG_EDI;
  new->instr = ASM_SCASD;
  return (new->len);
}
