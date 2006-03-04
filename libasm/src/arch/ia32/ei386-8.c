/*
** private - do not distribute
**
** Author  : <sk at devhell dot org>
** Started : Mon Jul  1 01:11:28 2002
** Updated : Wed Apr 30 18:37:46 2003
*/

#include <libasm.h>

/*
  <i386 func="i386_jb" opcode="0x82"/>
*/

int i386_jb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  // new->type = IS_COND_BRANCH;
    new->instr = ASM_BRANCH_U_LESS;

    new->op1.type = ASM_OTYPE_JUMP;
    new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
    new->op1.ptr = opcode + 1;
    new->op1.len = 4;
    memcpy(&new->op1.imm, opcode + 1, 4);
    new->len += 5;
  return (new->len);
}


/*
  <i386 func="i386_jae" opcode="0x83"/>
*/

int i386_jae(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  // new->type = IS_COND_BRANCH;
    new->instr = ASM_BRANCH_U_GREATER_EQUAL;

    new->op1.type = ASM_OTYPE_JUMP;
    new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
    new->op1.ptr = opcode + 1;
    new->op1.len = 4;
    memcpy(&new->op1.imm, opcode + 1, 4);
    new->len += 5;
  return (new->len);
}


/*
  <i386 func="i386_je" opcode="0x84"/>
*/


int i386_je(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  // new->type = IS_COND_BRANCH;
    new->instr = ASM_BRANCH_EQUAL;

    new->op1.type = ASM_OTYPE_JUMP;
    new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
    new->op1.ptr = opcode + 1;
    new->op1.len = 4;
    memcpy(&new->op1.imm, opcode + 1, 4);
    new->len += 5;
  return (new->len);
}

/*
  <i386 func="i386_jne" opcode="0x85"/>
*/


int i386_jne(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  // new->type = IS_COND_BRANCH;
    new->instr = ASM_BRANCH_NOT_EQUAL;

    new->op1.type = ASM_OTYPE_JUMP;
    new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
    new->op1.ptr = opcode + 1;
    new->op1.len = 4;
    memcpy(&new->op1.imm, opcode + 1, 4);
    new->len += 5;
  return (new->len);
}


/*
  <i386 func="i386_jbe" opcode="0x86"/>
*/


int i386_jbe(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  // new->type = IS_COND_BRANCH;
    new->instr = ASM_BRANCH_U_LESS_EQUAL;

    new->op1.type = ASM_OTYPE_JUMP;
    new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
    new->op1.ptr = opcode + 1;
    new->op1.len = 4;
    memcpy(&new->op1.imm, opcode + 1, 4);
    new->len += 5;
  return (new->len);
}


/*
  <i386 func="i386_ja" opcode="0x87"/>
*/


int i386_ja(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  // new->type = IS_COND_BRANCH;
    new->instr = ASM_BRANCH_U_GREATER;

    new->op1.type = ASM_OTYPE_JUMP;
    new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
    new->op1.ptr = opcode + 1;
    new->op1.len = 4;
    memcpy(&new->op1.imm, opcode + 1, 4);
    new->len += 5;
  return (new->len);
}

/*
  <i386 func="i386_js" opcode="0x88"/>
*/


int i386_js(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  // new->type = IS_COND_BRANCH;
    new->instr = ASM_BRANCH_SIGNED;

    new->op1.type = ASM_OTYPE_JUMP;
    new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
    new->op1.ptr = opcode + 1;
    new->op1.len = 4;
    memcpy(&new->op1.imm, opcode + 1, 4);
    new->len += 5;
  return (new->len);
}

/*
  <i386 func="i386_jns" opcode="0x89"/>
*/


int i386_jns(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  // new->type = IS_COND_BRANCH;
    new->instr = ASM_BRANCH_NOT_SIGNED;

    new->op1.type = ASM_OTYPE_JUMP;
    new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
    new->op1.ptr = opcode + 1;
    new->op1.len = 4;
    memcpy(&new->op1.imm, opcode + 1, 4);
    new->len += 5;
  return (new->len);
}


/*
<i386 func="i386_jp" opcode="0x8a"/>
*/


int i386_jp(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
// new->type = IS_COND_BRANCH;
  new->instr = ASM_BRANCH_NOT_SIGNED;

  new->op1.type = ASM_OTYPE_JUMP;
  new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
  new->op1.ptr = opcode + 1;
  new->op1.len = 4;
  memcpy(&new->op1.imm, opcode + 1, 4);
  new->len += 5;
return (new->len);
}

/*
  <i386 func="i386_jl" opcode="0x8c"/>
*/


int i386_jl(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  // new->type = IS_COND_BRANCH;
    new->instr = ASM_BRANCH_S_LESS;

    new->op1.type = ASM_OTYPE_JUMP;
    new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
    new->op1.ptr = opcode + 1;
    new->op1.len = 4;
    memcpy(&new->op1.imm, opcode + 1, 4);
    new->len += 5;
  return (new->len);
}

/*
  <i386 func="i386_jge" opcode="0x8d"/>
*/


int i386_jge(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  // new->type = IS_COND_BRANCH;
    new->instr = ASM_BRANCH_S_GREATER_EQUAL;

    new->op1.type = ASM_OTYPE_JUMP;
    new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
    new->op1.ptr = opcode + 1;
    new->op1.len = 4;
    memcpy(&new->op1.imm, opcode + 1, 4);
    new->len += 5;
  return (new->len);
}

/*
  <i386 func="i386_jle" opcode="0x8e"/>
*/


int i386_jle(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  //     new->type = IS_COND_BRANCH;
    new->instr = ASM_BRANCH_S_LESS_EQUAL;

    new->op1.type = ASM_OTYPE_JUMP;
    new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
    new->op1.ptr = opcode + 1;
    new->op1.len = 4;
    memcpy(&new->op1.imm, opcode + 1, 4);
    new->len += 5;
  return (new->len);
}

/*
  <i386 func="i386_jg" opcode="0x8f"/>
*/


int i386_jg(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_BRANCH_S_GREATER;

  new->op1.type = ASM_OTYPE_JUMP;
  new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
    new->op1.ptr = opcode + 1;
    new->op1.len = 4;
    memcpy(&new->op1.imm, opcode + 1, 4);
    new->len += 5;
  return (new->len);
}


