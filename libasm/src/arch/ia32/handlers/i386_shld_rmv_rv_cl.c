/*
** $Id: i386_shld_rmv_rv_cl.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_shld_rmv_rv_cl" opcode="0xa5"/>
*/

int i386_shld_rmv_rv_cl(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->instr = ASM_SHRD;
    new->len += 1;
    new->op1.type = ASM_OTYPE_ENCODED;
    new->op1.size = ASM_OSIZE_VECTOR;
    new->op2.type = ASM_OTYPE_GENERAL;
    new->op2.size = ASM_OSIZE_VECTOR;
    operand_rmv_rv(new, opcode + 1, len - 1, proc);
    new->op3.type = ASM_OTYPE_FIXED;
    new->op3.content = ASM_OP_BASE;
    new->op3.regset = ASM_REGSET_R8;
    new->op3.ptr = opcode;
    new->op3.len = 0;
    new->op3.base_reg = ASM_REG_CL;
  return (new->len);
}
