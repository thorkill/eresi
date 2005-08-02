/*
** this is private - dot not distribute
** 
** Author  : <sk at devhell dot org>
** Started : Sat Jun  1 04:12:15 2002
** Updated : Sun Mar 14 23:58:17 2004
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_mov_subreg_ib" opcode="0xb0"/>
  <instruction func="op_mov_subreg_ib" opcode="0xb1"/>
  <instruction func="op_mov_subreg_ib" opcode="0xb2"/>
  <instruction func="op_mov_subreg_ib" opcode="0xb3"/>
  <instruction func="op_mov_subreg_ib" opcode="0xb4"/>
  <instruction func="op_mov_subreg_ib" opcode="0xb5"/>
  <instruction func="op_mov_subreg_ib" opcode="0xb6"/>
  <instruction func="op_mov_subreg_ib" opcode="0xb7"/>

*/

int op_mov_subreg_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;
  
  modrm = (struct s_modrm *) opcode;
  new->ptr_instr = opcode;
    new->instr = ASM_MOV;
    new->len += 2;
    
    new->op1.type = ASM_OTYPE_OPMOD;
    new->op2.type = ASM_OTYPE_IMMEDIATE;
    
    new->op1.content = ASM_OP_BASE;
    new->op1.regset = ASM_REGSET_R8;
    new->op1.ptr = opcode;
    
    new->op1.base_reg = modrm->m;
    
    
    new->op2.content = ASM_OP_VALUE;
    new->op2.ptr = opcode + 1;
    
    new->op2.imm = 0;
    memcpy(&new->op2.imm, opcode + 1, 1);
  return (new->len);
}


/*
  <instruction func="op_mov_reg_iv" opcode="0xb8"/>
  <instruction func="op_mov_reg_iv" opcode="0xb9"/>
  <instruction func="op_mov_reg_iv" opcode="0xba"/>
  <instruction func="op_mov_reg_iv" opcode="0xbb"/>
  <instruction func="op_mov_reg_iv" opcode="0xbc"/>
  <instruction func="op_mov_reg_iv" opcode="0xbd"/>
  <instruction func="op_mov_reg_iv" opcode="0xbe"/>
  <instruction func="op_mov_reg_iv" opcode="0xbf"/>
*/

int op_mov_reg_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;

  modrm = (struct s_modrm *) opcode;
  new->ptr_instr = opcode;
  new->instr = ASM_MOV;
  new->len += 1 + asm_proc_vector_len(proc);
    
  new->op1.type = ASM_OTYPE_OPMOD;
  new->op1.size = ASM_OSIZE_DWORD;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op2.size = ASM_OSIZE_DWORD;

  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.regset = asm_proc_oplen(proc) ?
    ASM_REGSET_R16 : ASM_REGSET_R32;
  new->op1.ptr = opcode;
  new->op1.base_reg = modrm->m;
    
  new->op2.content = ASM_OP_VALUE;
  new->op2.ptr = opcode + 1;
  new->op2.len = asm_proc_vector_len(proc);

  memcpy(&new->op2.imm, opcode + 1, asm_proc_vector_len(proc));
  return (new->len);
}
