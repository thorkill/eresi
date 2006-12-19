/*
** private - do not distribute
** 
** Author  : <sk at devhell dot org>
** Started : Xxx Xxx xx xx:xx:xx 2002
** Updated : Mon May  5 22:35:36 2003
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_immed_rmb_ib" opcode="0x80"/>
  <instruction func="op_immed_rmb_ib" opcode="0x82"/>
*/

int op_immed_rmb_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;

  new->ptr_instr = opcode;
  modrm = (struct s_modrm *) opcode + 1;
  new->len+= 1;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  operand_rmb_ib(new, opcode + 1, len - 1, proc);
  switch(modrm->r) {
  case 0:
    new->instr = ASM_ADD;
    break;
    case 1:
      new->instr = ASM_ORB;
      new->op2.imm &= 0xff;
      break;
    case 2:
      new->instr = ASM_ADC;
      break;
    case 3:
      new->instr = ASM_SBB;
      break;
    case 4:
      new->instr = ASM_AND;
      new->op2.imm &= 0xff;
      break;
    case 5:
      new->instr = ASM_SUB;
      break;
    case 6:
      new->instr = ASM_XOR;
      break;
    case 7:
      new->instr = ASM_CMP;
      new->op2.imm &= 0xff;
      break;
  }
  return (new->len);
}

/*
  <instruction func="op_immed_rmv_iv" opcode="0x81"/>
 */

int op_immed_rmv_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;
  new->ptr_instr = opcode;
  
  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
  switch(modrm->r) {
    case 0:
      new->instr = ASM_ADD;
      break;
    case 1:
      new->instr = ASM_OR;
      break;
    case 2:
      new->instr = ASM_ADC;
      break;
    case 3:
      new->instr = ASM_SBB;
      break;
    case 4:
      new->instr = ASM_AND;
      break;
    case 5:
      new->instr = ASM_SUB;
      break;
    case 6:
      new->instr = ASM_XOR;
      break;
    case 7:
      new->instr = ASM_CMP;
      break;
  }
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  operand_rmv_iv(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <instruction func="" opcode="0x82"> <- missing xml ending / 
  to skip parsing.
  bad instruction
*/

/*
  <instruction func="op_immed_rmv_ib" opcode="0x83"/>
*/

int op_immed_rmv_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;

  modrm = (struct s_modrm *) (opcode + 1);
  new->ptr_instr = opcode;
  new->len += 1;
  switch(modrm->r) {
    case 0:
      new->instr = ASM_ADD;
      break;
    case 1:
      new->instr = ASM_OR;
      break;
    case 2:
      new->instr = ASM_ADC;
      break;
    case 3:
      new->instr = ASM_SBB;
      break;
    case 4:
      new->instr = ASM_AND;
      break;
    case 5:
      new->instr = ASM_SUB;
      break;
    case 6:
      new->instr = ASM_XOR;
      break;
    case 7:
      new->instr = ASM_CMP;
      break;
    } 
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op1.size = ASM_OSIZE_VECTOR;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op2.size = ASM_OSIZE_BYTE;
  operand_rmv_ib(new, opcode + 1, len - 1, proc);
  return (new->len);
}


/*
  <instruction func="op_test_rmb_rb" opcode="0x84"/>
*/


int op_test_rmb_rb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc){
  new->len += 1;
  new->instr = ASM_TEST;
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.size = ASM_OSIZE_BYTE;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op2.size = ASM_OSIZE_BYTE;
  operand_rmb_rb(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <instruction func="op_test_rmv_rv" opcode="0x85"/>
*/

int op_test_rmv_rv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc){
  new->len += 1;
    new->instr = ASM_TEST;
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op1.size = ASM_OSIZE_VECTOR;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op2.size = ASM_OSIZE_VECTOR;
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <instruction func="op_xchg_rmb_rb" opcode="0x86"/>
*/

int op_xchg_rmb_rb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->len += 1;
  new->ptr_instr = opcode;
    new->instr = ASM_XCHG;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op1.size = ASM_OSIZE_BYTE;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op2.size = ASM_OSIZE_BYTE;
  operand_rmb_rb(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <instruction func="op_xchg_rmv_rv" opcode="0x87"/>
 */

int op_xchg_rmv_rv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_XCHG;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op1.size = ASM_OSIZE_VECTOR;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op2.size = ASM_OSIZE_VECTOR;
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
  
  return (new->len);
}

/*
  <instruction func="op_mov_rmb_rb" opcode="0x88"/>
*/

int op_mov_rmb_rb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
    new->instr = ASM_MOV;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op1.size = ASM_OSIZE_BYTE;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op2.size = ASM_OSIZE_BYTE;
  operand_rmb_rb(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <instruction func="op_mov_rmv_rv" opcode="0x89"/>
*/

int op_mov_rmv_rv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_MOV;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op1.size = ASM_OSIZE_VECTOR;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op1.size = ASM_OSIZE_VECTOR;
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <instruction func="op_mov_rb_rmb" opcode="0x8a"/>
*/

int op_mov_rb_rmb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
    new->instr = ASM_MOV;
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op1.size = ASM_OSIZE_BYTE;
  new->op2.type = ASM_OTYPE_ENCODED;
  new->op2.size = ASM_OSIZE_BYTE;
  operand_rb_rmb(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <instruction func="op_mov_rv_rmv" opcode="0x8b"/>
*/

int op_mov_rv_rmv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_MOV;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op1.size = ASM_OSIZE_VECTOR;
  new->op2.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OSIZE_VECTOR;
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <instruction func="op_mov_rm_segr" opcode="0x8c"/>
*/

int op_mov_rm_segr(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;

  modrm = (struct s_modrm *) opcode + 1;
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_MOV;

  new->op1.type = ASM_OTYPE_ENCODED;
  new->op1.size = ASM_OSIZE_VECTOR;
  operand_rmv(&new->op1, opcode + 1, len - 1, proc);
  new->len += new->op1.len;

  new->op2.type = ASM_OTYPE_SEGMENT;
  new->op2.content = ASM_OP_BASE;
  new->op2.regset = ASM_REGSET_SREG;
  new->op2.base_reg = modrm->r;

  return (new->len);
}


/*
  <instruction func="op_lea_rv_m" opcode="0x8d"/>
*/

int op_lea_rv_m(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_LEA;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op1.size = ASM_OSIZE_VECTOR;
  new->op2.type = ASM_OTYPE_MEMORY;
  
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <instruction func="op_mov_segr_rm" opcode="0x8e"/>
*/

int op_mov_segr_rm(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;
  
  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_MOV;
  
  new->op1.type = ASM_OTYPE_SEGMENT;
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_SREG;
  new->op1.base_reg = modrm->r;
  
  new->op2.type = ASM_OTYPE_GENERAL;
  operand_rmv(&new->op2, opcode + 1, len - 1, proc);
  new->len += new->op2.len;
  //new->op2.content = ASM_OP_BASE;
  //new->op2.regset = ASM_REGSET_R32;
  //new->op2.base_reg = modrm->m;
  
  return (new->len);
}

/*
  <instruction func="op_pop_rmv" opcode="0x8f"/>
*/

int op_pop_rmv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_POP;
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_ENCODED;
  operand_rmv(&new->op1, opcode + 1, len - 1, proc);
  new->len += new->op1.len;
  return (new->len);
}









