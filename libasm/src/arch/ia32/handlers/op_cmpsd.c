/*
** $Id: op_cmpsd.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_cmpsd" opcode="0xa7"/>
*/

int op_cmpsd(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
    new->ptr_instr = opcode;
  new->instr = ASM_CMPSD;

#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_XSRC, proc);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_YDEST, proc);
#else
  new->op1.type = ASM_OTYPE_XSRC;
  new->op2.type = ASM_OTYPE_YDEST;
  
  new->op1.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op1.base_reg = ASM_REG_ESI;
  new->op1.regset = ASM_REGSET_R32;
  new->op1.prefix = ASM_PREFIX_DS;
  
  new->op2.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op2.prefix = ASM_PREFIX_ES;
  new->op2.base_reg = ASM_REG_EDI;
  new->op2.regset = ASM_REGSET_R32;
#endif
  return (new->len);
}
