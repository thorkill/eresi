/*
** $Id: i386_shld.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_shld" opcode="0xa4"/>
*/

int i386_shld(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm        *modrm;
  
  modrm = (struct s_modrm *) opcode + 1;
    new->instr = ASM_SHLD;

    new->op1.type = ASM_OTYPE_REGISTER;
    new->op1.regset = ASM_REGSET_R32;
    new->op1.content = ASM_OP_BASE;
    new->op1.ptr = opcode + 1;
    new->op1.len = 0;
    new->op1.base_reg = modrm->m;

    new->op2.type = ASM_OTYPE_GENERAL;
    new->op2.regset = ASM_REGSET_R32;
    new->op2.content = ASM_OP_BASE;
    new->op2.ptr = opcode + 1;
    new->op2.len = 1;
    new->op2.base_reg = modrm->r;

    new->op3.type = ASM_OTYPE_IMMEDIATE;
    new->op3.content = ASM_OP_VALUE;
    new->op3.ptr = opcode + 2;
    new->op3.len = 1;
    new->op3.imm = 0;
    memcpy(&new->op3.imm, opcode + 2, 1);
    new->len += new->op1.len + new->op2.len + new->op3.len + 1;
  return (new->len);
}
