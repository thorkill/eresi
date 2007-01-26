/*
** $Id: i386_mov_rm_cr.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

int i386_mov_rm_cr(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm        *modrm;
  
  modrm = (struct s_modrm *) (opcode + 1);
  new->len += 2;
    new->instr = ASM_MOV;
    new->op1.type = ASM_OTYPE_REGISTER;
    new->op1.content = ASM_OP_BASE;
    new->op1.regset = ASM_REGSET_R32;
    new->op1.base_reg = modrm->m;
    new->op2.type = ASM_OTYPE_SEGMENT;
    new->op2.content = ASM_OP_BASE;
    new->op2.regset = ASM_REGSET_CREG;
    new->op2.base_reg = modrm->r;
  return (new->len);
}
