/*
** this is private - do not distribute
** 
** Author  : <sk at devhell dot org>
** Started : Xxx Xxx xx xx:xx:xx 2002
** Updated : Sun Mar 21 00:37:27 2004
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_jo" opcode="0x70"/>
*/

int  op_jo(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {    
  new->len += 2;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_CONDBRANCH;
  new->op1.type = ASM_OTYPE_JUMP;
  new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;

  new->op1.imm = 0;
  if (*(opcode + 1) >= 0x80u)
    memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
  memcpy(&new->op1.imm, opcode + 1, 1);
  new->instr = ASM_BRANCH_OVERFLOW;
  return (new->len);
}

/*
  <instruction func="op_jno" opcode="0x71"/>
*/

                       
int  op_jno(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {    
  new->len += 2;
  new->type = ASM_TYPE_CONDBRANCH;
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_JUMP;
  new->op1.size = ASM_OSIZE_BYTE;
  new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;

  new->op1.imm = 0;
  if (*(opcode + 1) >= 0x80u)
    memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
  memcpy(&new->op1.imm, opcode + 1, 1);
  new->instr = ASM_BRANCH_NOT_OVERFLOW;
  return (new->len);
}

/*
  <instruction func="op_jb" opcode="0x72"/>
*/

   
int  op_jb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {    
  new->len += 2;
  new->type = ASM_TYPE_CONDBRANCH;
  new->op1.type = ASM_OTYPE_JUMP;
  new->ptr_instr = opcode;
  new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;

  new->op1.imm = 0;
  if (*(opcode + 1) >= 0x80u)
    memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
  memcpy(&new->op1.imm, opcode + 1, 1);
  new->instr = ASM_BRANCH_U_LESS;
  return (new->len);
}

/*
  <instruction func="op_jae" opcode="0x73"/>
*/

    
int  op_jae(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {    
  new->len += 2;
  new->type = ASM_TYPE_CONDBRANCH;
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_JUMP;
  new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;

  new->op1.imm = 0;
  if (*(opcode + 1) >= 0x80u)
    memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
  memcpy(&new->op1.imm, opcode + 1, 1);
  new->instr = ASM_BRANCH_U_GREATER_EQUAL;
  return (new->len);
}

/*
  <instruction func="op_je" opcode="0x74"/>
*/


int  op_je(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {    
  new->len += 2;
  new->type = ASM_TYPE_CONDBRANCH;
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_JUMP;
  new->instr = ASM_BRANCH_EQUAL;
  new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;

  new->op1.imm = 0;
  if (*(opcode + 1) >= 0x80u)
    memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
  memcpy(&new->op1.imm, opcode + 1, 1);
  return (new->len);
}


/*
  <instruction func="op_jne" opcode="0x75"/>
*/


int  op_jne(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {    
  new->len += 2;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_CONDBRANCH;
  new->instr = ASM_BRANCH_NOT_EQUAL;

  new->op1.type = ASM_OTYPE_JUMP;
  new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;

  new->op1.imm = 0;
  if (*(opcode + 1) >= 0x80u)
    memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
  memcpy(&new->op1.imm, opcode + 1, 1);
    
    
  return (new->len);
}


/*
  <instruction func="op_jbe" opcode="0x76"/>
*/
                       
int  op_jbe(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {    
  new->len += 2;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_CONDBRANCH;
  new->instr = ASM_BRANCH_U_LESS_EQUAL;
  new->op1.type = ASM_OTYPE_JUMP;
  new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;

  new->op1.imm = 0;
  if (*(opcode + 1) >= 0x80u)
    memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
  memcpy(&new->op1.imm, opcode + 1, 1);
  return (new->len);
}

/*
  <instruction func="op_ja" opcode="0x77"/>
*/
                       
int  op_ja(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {    
  new->len += 2;
  new->type = ASM_TYPE_CONDBRANCH;
  new->instr = ASM_BRANCH_U_GREATER;
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_JUMP;
  new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;

  new->op1.imm = 0;
  if (*(opcode + 1) >= 0x80u)
    memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
  memcpy(&new->op1.imm, opcode + 1, 1);
  return (new->len);
}

/*
  <instruction func="op_js" opcode="0x78"/>
*/

int  op_js(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {    
  new->ptr_instr = opcode;
  new->len += 2;
  new->type = ASM_TYPE_CONDBRANCH;
  new->instr = ASM_BRANCH_SIGNED;
  new->op1.type = ASM_OTYPE_JUMP;
  new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;

  new->op1.imm = 0;
  if (*(opcode + 1) >= 0x80u)
    memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
  memcpy(&new->op1.imm, opcode + 1, 1);
  return (new->len);
}

/*
  <instruction func="op_jns" opcode="0x79"/>
*/
                       
int  op_jns(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {    
  new->ptr_instr = opcode;
  new->len += 2;
  new->type = ASM_TYPE_CONDBRANCH;
  new->instr = ASM_BRANCH_NOT_SIGNED;
  new->op1.type = ASM_OTYPE_JUMP;
  new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;

  new->op1.imm = 0;
  if (*(opcode + 1) >= 0x80u)
    memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
  memcpy(&new->op1.imm, opcode + 1, 1);
  return (new->len);
}
                       

/*
  <instruction func="op_jp" opcode="0x7a"/>
*/

int  op_jp(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {    
  new->ptr_instr = opcode;
  new->len += 2;
  new->type = ASM_TYPE_CONDBRANCH;
  new->instr = ASM_BRANCH_PARITY;
  new->op1.type = ASM_OTYPE_JUMP;
  new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;

  new->op1.imm = 0;
  if (*(opcode + 1) >= 0x80u)
    memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
  memcpy(&new->op1.imm, opcode + 1, 1);
  return (new->len);
}
/*
  <instruction func="op_jnp" opcode="0x7b"/>
*/
                       
int  op_jnp(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {    
  new->len += 2;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_CONDBRANCH;
  new->instr = ASM_BRANCH_NOT_PARITY;
  new->op1.type = ASM_OTYPE_JUMP;
  new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;

  new->op1.imm = 0;
  if (*(opcode + 1) >= 0x80u)
    memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
  memcpy(&new->op1.imm, opcode + 1, 1);
  return (new->len);
}

/*
  <instruction func="op_jl" opcode="0x7c"/>
*/
                       
int  op_jl(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {    
  new->ptr_instr = opcode;
  new->len += 2;
  new->type = ASM_TYPE_CONDBRANCH;
  new->instr = ASM_BRANCH_S_LESS;
  new->op1.type = ASM_OTYPE_JUMP;
  new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;

  new->op1.imm = 0;
  if (*(opcode + 1) >= 0x80u)
    memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
  memcpy(&new->op1.imm, opcode + 1, 1);
  return (new->len);
}
/*
  <instruction func="op_jge" opcode="0x7d"/>
*/
                       
int  op_jge(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {    
  new->ptr_instr = opcode;
  new->len += 2;
  new->type = ASM_TYPE_CONDBRANCH;
  new->instr = ASM_BRANCH_S_GREATER_EQUAL;
  new->op1.type = ASM_OTYPE_JUMP;
  new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;

  new->op1.imm = 0;
  if (*(opcode + 1) >= 0x80u)
    memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
  memcpy(&new->op1.imm, opcode + 1, 1);
  return (new->len);
}

/*
  <instruction func="op_jle" opcode="0x7e"/>
*/
                       
int  op_jle(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {    
  new->len += 2;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_CONDBRANCH;
  new->instr = ASM_BRANCH_S_LESS_EQUAL;
  new->op1.type = ASM_OTYPE_JUMP;
  new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;

  new->op1.imm = 0;
  if (*(opcode + 1) >= 0x80u)
    memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
  memcpy(&new->op1.imm, opcode + 1, 1);
  return (new->len);
}

/*
  <instruction func="op_jg" opcode="0x7f"/>
*/

int  op_jg(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {    
  new->ptr_instr = opcode;
  new->len += 2;
  new->type = ASM_TYPE_CONDBRANCH;
  new->instr = ASM_BRANCH_S_GREATER;
  new->op1.type = ASM_OTYPE_JUMP;
  new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;

  new->op1.imm = 0;
  if (*(opcode + 1) >= 0x80u)
    memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
  memcpy(&new->op1.imm, opcode + 1, 1);
  return (new->len);
}





