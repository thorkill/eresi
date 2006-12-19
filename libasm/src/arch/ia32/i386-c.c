/*
** $Id: i386-c.c,v 1.2 2006-12-19 02:46:19 heroine Exp $
** 
** Author  : <sk at devhell dot org>
** Started : Xxx Xxx xx xx:xx:xx 2002
** Updated : Wed Jul  2 19:00:35 2003
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_shr_rmb_ib" opcode="0xc0"/>
*/

int op_shr_rmb_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;

  modrm = (struct s_modrm *) (opcode + 1);
  new->ptr_instr = opcode;
  new->len += 1;
  if (modrm->r == ASM_REG_EAX)
    new->instr = ASM_ROL;
  else if (modrm->r == ASM_REG_EBP)
    new->instr = ASM_SHR;
  else if (modrm->r == ASM_REG_EDI)
    new->instr = ASM_SAR;
  else if (modrm->r == ASM_REG_ECX)
      new->instr = ASM_ROR;
  else if (modrm->r == 3)
    new->instr = ASM_RCR;
  else
    new->instr = ASM_SHL;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  operand_rmb_ib(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <instruction func="op_shr_rmv_ib" opcode="0xc1"/>
*/

int op_shr_rmv_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;
  
  modrm = (struct s_modrm *) opcode + 1;
  new->ptr_instr = opcode;
  new->len += 1;
  if (modrm->r == ASM_REG_EAX)
    new->instr = ASM_ROL;
  if (modrm->r == ASM_REG_EBP)
      new->instr = ASM_SHR;
    else if (modrm->r == ASM_REG_EDI)
      new->instr = ASM_SAR;
    else if (modrm->r == ASM_REG_ECX)
      new->instr = ASM_ROR;
    else if (modrm->r == ASM_REG_EAX)
      new->instr = ASM_ROL;
    else
      new->instr = ASM_SHL;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  operand_rmv_ib(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <instruction func="op_ret_i2" opcode="0xc2"/>
*/

int op_ret_i2(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 3;
   new->type = ASM_TYPE_RETPROC;
    new->instr = ASM_RET;
  new->ptr_instr = opcode;
    
    new->op1.type = ASM_OTYPE_IMMEDIATE;
    new->op1.content = ASM_OP_VALUE;
    new->op1.ptr = opcode + 1;
    new->op1.len = 2;
    new->op1.imm = 0;
    memcpy(&new->op1.imm, opcode + 1, 2);
    
  return (new->len);
}


/*
  <instruction func="op_ret" opcode="0xc3"/>
*/


int op_ret(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_RETPROC;
  new->instr = ASM_RET;
  return (new->len);
}

/*
  <instruction func="op_les_rm_rmp" opcode="0xc4"/>
*/

int op_les_rm_rmp(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
    new->instr = ASM_LES;
    new->len += 1;
  return (new->len);
}

/*
  <instruction func="op_lds_rm_rmp" opcode="0xc5"/>
*/

int op_lds_rm_rmp(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
    new->instr = ASM_LDS;
    new->len += 1;
  return (new->len);
}



/*
  <instruction func="op_mov_rmb_ib" opcode="0xc6"/>
*/

int op_mov_rmb_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_MOV;
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op1.size = ASM_OSIZE_BYTE;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op2.size = ASM_OSIZE_BYTE;
  operand_rmb_ib(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <instruction func="op_mov_rmv_iv" opcode="0xc7"/>
*/

int op_mov_rmv_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_MOV;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op1.size = ASM_OSIZE_VECTOR;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op2.size = ASM_OSIZE_VECTOR;
  operand_rmv_iv(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <instruction func="op_enter" opcode="0xc8"/>
*/

int op_enter(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->instr = ASM_ENTER;
    new->len += 3;
  new->ptr_instr = opcode;
    new->op1.type = ASM_OTYPE_IMMEDIATE;
    new->op1.content = ASM_OP_VALUE;
    new->op1.len = 2;
    new->op1.ptr = opcode + 1;
    
    new->op1.imm = 0;
    memcpy(&new->op1.imm, opcode + 1, 2);  
  return (new->len);
}

/*
  <instruction func="op_leave" opcode="0xc9"/>
*/


int op_leave(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  //new->type = ASM_TYPE_RETPROC;
  new->instr = ASM_LEAVE;
  return (new->len);
}

/*
  <instruction func="op_retf_i2" opcode="0xca"/>
*/

int op_retf_i2(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  short	*shrt_ptr;
  
    new->instr = ASM_RETF;
  new->ptr_instr = opcode;
    new->len += 3;
    // new->type = IS_RET;

    new->op1.type = ASM_OTYPE_IMMEDIATE;
    new->op1.content = ASM_OP_VALUE;
    shrt_ptr = (short *) (opcode + 1);
    if (*shrt_ptr < 0)
      memcpy((char *) &new->op1.imm + 2, "\xff\xff", 2);
    else
      new->op1.imm = 0;
    memcpy(&new->op1.imm, opcode + 1, 2);
  return (new->len);
}

/*
  <instruction func="op_retf" opcode="0xcb"/>
*/

int op_retf(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_RETF;
   new->type = ASM_TYPE_RETPROC;
  return (new->len);
}



/*
  <instruction func="op_int_3" opcode="0xcc"/>
*/

int op_int_3(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
    new->len += 1;
    new->instr = ASM_INT3;
  return (new->len);
}

/*
  <instruction func="op_int_ib" opcode="0xcd"/>
*/

int op_int_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
    new->instr = ASM_INT;
    new->len += 2;

    new->op1.type = ASM_OTYPE_IMMEDIATE;

    new->op1.content = ASM_OP_VALUE;
    new->op1.len = 1;
    new->op1.ptr = opcode + 1;
    
    if (*(opcode + 1) > 0x80u)
      memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
    else
      new->op1.imm = 0;
    memcpy(&new->op1.imm, opcode + 1, 1);
  return (new->len);
}

/*
  <instruction opcode="0xce" func="op_into"/>
*/

int op_into(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
    new->len += 1;
    new->instr = ASM_INTO;
  return (new->len);
}

/*
  <instruction opcode="0xcf" func="op_iret"/>
*/

int op_iret(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
    new->instr = ASM_IRET;
    // new->type = IS_RET;
  return (new->len);
}







