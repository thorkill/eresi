/*
** private
** 
** Author  : <sk at devhell dot org>
** Started : Sat Nov 23 05:03:22 2002
** Updated : Wed Apr 30 18:35:44 2003
*/

#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_mov_rm_cr" opcode="0x20"/>
*/

int i386_mov_rm_cr(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;
  
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


/*
  <i386 func="i386_mov_cr_rm" opcode="0x22"/>
*/

int i386_mov_cr_rm(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;
  
  modrm = (struct s_modrm *) (opcode + 1);
  new->len += 2;
    new->instr = ASM_MOV;
    new->op1.type = ASM_OTYPE_CONTROL;
    new->op1.content = ASM_OP_BASE;
    new->op1.regset = ASM_REGSET_CREG;
    new->op1.base_reg = modrm->r;
    new->op2.type = ASM_OTYPE_REGISTER;
    new->op2.content = ASM_OP_BASE;
    new->op2.regset = ASM_REGSET_R32;
    new->op2.base_reg = modrm->m;
  return (new->len);
}

/*
  <i386 func="i386_mov_dr_rm" opcode="0x23"/>
*/

int i386_mov_dr_rm(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;
  
  modrm = (struct s_modrm *) (opcode + 1);
    new->len += 2;
    new->instr = ASM_MOV;
    new->op1.type = ASM_OTYPE_DEBUG;
    new->op1.content = ASM_OP_BASE;
    new->op1.regset = ASM_REGSET_DREG;
    new->op1.base_reg = modrm->r;
    new->op2.type = ASM_OTYPE_REGISTER;
    new->op2.content = ASM_OP_BASE;
    new->op2.regset = ASM_REGSET_R32;
    new->op2.base_reg = modrm->m;

  return (new->len);
}




