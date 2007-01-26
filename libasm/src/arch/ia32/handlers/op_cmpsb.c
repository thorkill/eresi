/*
** $Id: op_cmpsb.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_cmpsb" opcode="0xa6"/>
*/

int op_cmpsb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->instr = ASM_CMPSB;
    new->len += 1;
    new->ptr_instr = opcode;

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
  
  return (new->len);
}
