/*
** this is private - do not distribute
** 
** Author  : <sk at devhell dot org>
** Started : Wed Jul 24 18:16:17 2002
** Updated : Mon May  5 23:30:04 2003
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_inc_reg" opcode="0x40"/>
  <instruction func="op_inc_reg" opcode="0x41"/>
  <instruction func="op_inc_reg" opcode="0x42"/>
  <instruction func="op_inc_reg" opcode="0x43"/>
  <instruction func="op_inc_reg" opcode="0x44"/>
  <instruction func="op_inc_reg" opcode="0x45"/>
  <instruction func="op_inc_reg" opcode="0x46"/>
  <instruction func="op_inc_reg" opcode="0x47"/>
*/


int op_inc_reg(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;
  
  modrm = (struct s_modrm *) opcode;
  new->len += 1;
  new->instr = ASM_INC;
  new->ptr_instr = opcode;
  
  new->op1.type = ASM_OTYPE_OPMOD;

  new->op1.content = ASM_OP_BASE;
  new->op1.regset = asm_proc_opsize(proc) ?
    ASM_REGSET_R16 : ASM_REGSET_R32;
  new->op1.ptr = opcode;
  
  new->op1.base_reg = modrm->m;
  return (new->len);  
}

/*
  <instruction func="op_dec_reg" opcode="0x48"/>
  <instruction func="op_dec_reg" opcode="0x49"/>
  <instruction func="op_dec_reg" opcode="0x4a"/>
  <instruction func="op_dec_reg" opcode="0x4b"/>
  <instruction func="op_dec_reg" opcode="0x4c"/>
  <instruction func="op_dec_reg" opcode="0x4d"/>
  <instruction func="op_dec_reg" opcode="0x4e"/>
  <instruction func="op_dec_reg" opcode="0x4f"/>
*/

int op_dec_reg(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;
  
  modrm = (struct s_modrm *) opcode;
  new->ptr_instr = opcode;
  new->instr = ASM_DEC;
  new->len += 1;  
  new->op1.type = ASM_OTYPE_OPMOD;
    new->op1.content = ASM_OP_BASE;
    new->op1.ptr = opcode;
    new->op1.regset = asm_proc_opsize(proc) ?
      ASM_REGSET_R16 : ASM_REGSET_R32;    
    new->op1.base_reg = modrm->m;
  return (new->len);  
}







