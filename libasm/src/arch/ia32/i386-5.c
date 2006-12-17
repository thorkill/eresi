/*
** this is private - do not distribute
** 
** Author  : <sk at devhell dot org>
** Started : Xxx Xxx xx xx:xx:xx 2002
** Updated : Mon May  5 23:18:35 2003
*/
#include <libasm.h>
#include <libasm-int.h>


/*
  <instruction func="op_push_reg" opcode="0x50"/>
  <instruction func="op_push_reg" opcode="0x51"/>
  <instruction func="op_push_reg" opcode="0x52"/>
  <instruction func="op_push_reg" opcode="0x53"/>
  <instruction func="op_push_reg" opcode="0x54"/>
  <instruction func="op_push_reg" opcode="0x55"/>
  <instruction func="op_push_reg" opcode="0x56"/>
  <instruction func="op_push_reg" opcode="0x57"/>
*/

int op_push_reg(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;
  
  modrm = (struct s_modrm *) opcode;
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_PUSH;
  new->op1.type = ASM_OTYPE_OPMOD;
  // new->type = IS_MEM_WRITE;
  new->op1.regset = asm_proc_opsize(proc) ? 
    ASM_REGSET_R16 : ASM_REGSET_R32;
  new->op1.content = ASM_OP_BASE;
  new->op1.ptr = opcode;
    
  new->op1.base_reg = modrm->m;
  return (new->len);  
}


/*
  <instruction func="op_pop_reg" opcode="0x58"/>
  <instruction func="op_pop_reg" opcode="0x59"/>
  <instruction func="op_pop_reg" opcode="0x5a"/>
  <instruction func="op_pop_reg" opcode="0x5b"/>
  <instruction func="op_pop_reg" opcode="0x5c"/>
  <instruction func="op_pop_reg" opcode="0x5d"/>
  <instruction func="op_pop_reg" opcode="0x5e"/>
  <instruction func="op_pop_reg" opcode="0x5f"/>
*/

int op_pop_reg(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;
  
  modrm = (struct s_modrm *) opcode;
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_POP;
  // new->type = IS_MEM_READ;
  new->op1.type = ASM_OTYPE_OPMOD;
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = asm_proc_opsize(proc) ?
    ASM_REGSET_R16 : ASM_REGSET_R32;
  new->op1.ptr = opcode;

  new->op1.base_reg = modrm->m; 
  return (new->len);  
}






