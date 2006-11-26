/*
** this is private - do not distribute
** 
** Author  : <sk at devhell dot org>
** Started : Xxx Xxx xx xx:xx:xx 2002
** Updated : Mon May  5 23:33:21 2003
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_mov_al_ref_iv" opcode="0xa0"/>
*/

int op_mov_al_ref_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->len += 5;
    new->instr = ASM_MOV;
    new->ptr_instr = opcode;
    
    new->op1.type = ASM_OTYPE_FIXED;
    new->op1.content = ASM_OP_BASE;
    new->op1.len = 0;
    new->op1.ptr = 0;
    new->op1.base_reg = ASM_REG_AL;
    new->op1.regset = ASM_REGSET_R8;
    
    new->op2.type = ASM_OTYPE_OFFSET;
    new->op2.content = ASM_OP_VALUE | ASM_OP_REFERENCE;
    new->op2.len = 4;
    new->op2.ptr = opcode + 1;
    memcpy(&new->op2.imm, opcode + 1, 4);
  return (new->len);
}

/*
  <instruction func="op_mov_eax_ref_iv" opcode="0xa1"/>
*/

int op_mov_eax_ref_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_MOV;
    new->ptr_instr = opcode;
  new->len += 5;
    
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.ptr = opcode;
  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.regset = asm_proc_oplen(proc) ? 
    ASM_REGSET_R16 : ASM_REGSET_R32;
  new->op1.base_reg = ASM_REG_EAX;
  
  new->op2.type = ASM_OTYPE_OFFSET;
  new->op2.content = ASM_OP_VALUE | ASM_OP_REFERENCE;
  new->op2.len = 4;
  new->op2.ptr = opcode + 1;
    
  memcpy(&new->op2.imm, opcode + 1, 4);
  return (new->len);
}


/*
  <instruction func="op_mov_ref_iv_al" opcode="0xa2"/>
*/

int op_mov_ref_iv_al(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_MOV;
    new->ptr_instr = opcode;
  new->len += 5;
    
  new->op1.type = ASM_OTYPE_OFFSET;
  new->op1.len = 4;
  new->op1.content = ASM_OP_VALUE | ASM_OP_REFERENCE;
  memcpy(&new->op1.imm, opcode + 1, 4);
    
  new->op2.type = ASM_OTYPE_FIXED;
  new->op2.regset = ASM_REGSET_R8;
  new->op2.content = ASM_OP_BASE;
  new->op2.base_reg = ASM_REG_AL;
  return (new->len);
}

/*
  <instruction func="op_mov_ref_iv_eax" opcode="0xa3"/>
*/

int op_mov_ref_iv_eax(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {

    new->instr = ASM_MOV;
    new->len += 1;
    new->ptr_instr = opcode;
    
    new->op1.type = ASM_OTYPE_OFFSET;
    new->op2.type = ASM_OTYPE_FIXED;

    new->op1.content = ASM_OP_VALUE | ASM_OP_REFERENCE;
    new->op1.ptr = opcode + 1;
    new->op1.len = 4;
    memcpy(&new->op1.imm, opcode + 1, 4);
    
    new->op2.content = ASM_OP_BASE | ASM_OP_FIXED;
    new->op2.len = 0;
    new->op2.base_reg = ASM_REG_EAX;
    new->op2.regset = asm_proc_oplen(proc) ? 
      ASM_REGSET_R16 : ASM_REGSET_R32;
    new->len += 4;
  return (new->len);
}


/*
  <instruction func="op_movsb" opcode="0xa4"/>
 */

int op_movsb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->instr = ASM_MOVSB;
    new->len += 1;
    new->ptr_instr = opcode;
    
    new->op1.type = ASM_OTYPE_YDEST;
    new->op2.type = ASM_OTYPE_XSRC;
    
    new->op1.content = ASM_OP_BASE | ASM_OP_REFERENCE;
    new->op1.prefix = ASM_PREFIX_ES;
    new->op1.regset = ASM_REGSET_R32;
    new->op1.base_reg = ASM_REG_EDI;
    
    new->op2.content = ASM_OP_REFERENCE | ASM_OP_BASE;
    new->op2.prefix = ASM_PREFIX_DS;
    new->op2.regset = ASM_REGSET_R32;
    new->op2.base_reg = ASM_REG_ESI;
  return (new->len);
}

/*
  <instruction func="op_movsd" opcode="0xa5"/>
*/

int op_movsd(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->len += 1;
    new->ptr_instr = opcode;
    if (asm_proc_oplen(proc))
      new->instr = ASM_MOVSW;
    else
      new->instr = ASM_MOVSD;
    
    new->op1.type = ASM_OTYPE_YDEST;
    new->op2.type = ASM_OTYPE_XSRC;

    new->op1.prefix = ASM_PREFIX_ES;
    new->op1.regset = ASM_REGSET_R32;
    new->op1.content = ASM_OP_BASE | ASM_OP_REFERENCE;
    new->op1.base_reg = ASM_REG_EDI;
    new->op1.len = 0;
  
  new->op2.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op2.len = 0;
  new->op2.prefix = ASM_PREFIX_DS;
  new->op2.regset = ASM_REGSET_R32;
  new->op2.base_reg = ASM_REG_ESI;



  return (new->len);
}

/*
  <instruction func="op_cmpsb" opcode="0xa6"/>
*/

int op_cmpsb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->instr = ASM_CMPSB;
    new->len += 1;
    new->ptr_instr = opcode;

    new->op1.type = ASM_OTYPE_XSRC;
    new->op2.type = ASM_OTYPE_YDEST;

    new->op1.content = ASM_OP_BASE | ASM_OP_REFERENCE;
    new->op1.base_reg = ASM_REG_ESI;
    new->op1.regset = ASM_REGSET_R32;
    new->op1.prefix = ASM_PREFIX_DS;
    
    new->op2.content = ASM_OP_BASE | ASM_OP_REFERENCE;
    new->op2.prefix = ASM_PREFIX_ES;
    new->op2.base_reg = ASM_REG_EDI;
    new->op2.regset = ASM_REGSET_R32;
    
  return (new->len);
}

/*
  <instruction func="op_cmpsd" opcode="0xa7"/>
*/

int op_cmpsd(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
    new->ptr_instr = opcode;
  new->instr = ASM_CMPSD;
  new->op1.type = ASM_OTYPE_XSRC;
  new->op2.type = ASM_OTYPE_YDEST;

  new->op1.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op1.base_reg = ASM_REG_ESI;
  new->op1.regset = ASM_REGSET_R32;
  new->op1.prefix = ASM_PREFIX_DS;
    
  new->op2.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op2.prefix = ASM_PREFIX_ES;
  new->op2.base_reg = ASM_REG_EDI;
  new->op2.regset = ASM_REGSET_R32;
  return (new->len);
}

/*
  <instruction func="op_test_al_rb" opcode="0xa8"/>
*/


/*
 * 
 */

int op_test_al_rb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->instr = ASM_TEST;
    new->len += 1;

    new->op1.type = ASM_OTYPE_FIXED;
    new->ptr_instr = opcode;
    new->op2.type = ASM_OTYPE_IMMEDIATE;

    new->op1.content = ASM_OP_BASE;
    new->op1.base_reg = ASM_REG_AL;
    new->op1.regset = ASM_REGSET_R8;
    
    new->op2.content = ASM_OP_VALUE;
    new->op2.len = 1;
    new->op2.imm = 0;
    memcpy(&new->op2.imm, opcode + 1, 1);
    new->len += 1;
  return (new->len);
}

/*
  <instruction opcode="0xa9" func="op_test_eax_iv"/>
*/

int op_test_eax_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->instr = ASM_TEST;
    new->len += 5;
    new->ptr_instr = opcode;

    new->op1.type = ASM_OTYPE_FIXED;
    new->op2.type = ASM_OTYPE_IMMEDIATE;

    new->op1.content = ASM_OP_BASE;
    new->op1.regset = ASM_REGSET_R32;
    new->op1.base_reg = ASM_REG_EAX;

    
    new->op2.len = 4;
    new->op2.ptr = opcode + 1;
    new->op2.content = ASM_OP_VALUE;  
    memcpy(&new->op2.imm, opcode + 1, 4);
    return (new->len);
}


/*
  <instruction func="op_stosb" opcode="0xaa"/>
*/

int op_stosb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->ptr_instr = opcode;
  new->instr = ASM_STOSB;
  new->len += 1;

  new->op1.type = ASM_OTYPE_YDEST;
  new->op2.type = ASM_OTYPE_XSRC;

  new->op1.prefix = ASM_PREFIX_ES;
  new->op1.regset = ASM_REGSET_R32;
  new->op1.content = ASM_OP_FIXED | ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op1.base_reg = ASM_REG_EDI;
  new->op1.len = 0;
    
  new->op2.len = 0;
  new->op2.content = ASM_OP_BASE;
  new->op2.regset = ASM_REGSET_R8;
  new->op2.base_reg = ASM_REG_AL;
  
  return (new->len);
}

/*
  <instruction func="op_stosd" opcode="0xab"/>
*/

int op_stosd(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->instr = ASM_STOSD;
    new->ptr_instr = opcode;
    new->len += 1;

    new->op1.type = ASM_OTYPE_YDEST;
    new->op2.type = ASM_OTYPE_XSRC;

    new->op1.content = ASM_OP_FIXED | ASM_OP_BASE | ASM_OP_FIXED;
    new->op1.prefix = ASM_PREFIX_ES;
    new->op1.regset = ASM_REGSET_R32;
    new->op1.content = ASM_OP_BASE | ASM_OP_REFERENCE;
    new->op1.base_reg = ASM_REG_EDI;
    new->op1.len = 0;
  
    new->op2.content = ASM_OP_BASE;
    new->op2.len = 0;
    new->op2.regset = asm_proc_oplen(proc) ?
      ASM_REGSET_R16 : ASM_REGSET_R32;
    new->op2.base_reg = ASM_REG_EAX;
  return (new->len);
}

/*
  <instruction func="op_lodsb" opcode="0xac"/>
*/

int op_lodsb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->instr = ASM_LODSB;
    new->len += 1;

    new->ptr_instr = opcode;
    new->op1.type = ASM_OTYPE_YDEST;
    new->op2.type = ASM_OTYPE_XSRC;

    new->op1.regset = ASM_REGSET_R32;
    new->op1.prefix = ASM_PREFIX_ES;
    new->op1.base_reg = ASM_REG_EDI;
    
    new->op2.content = ASM_OP_BASE | ASM_OP_FIXED;
    new->op2.regset = asm_proc_oplen(proc) ? 
      ASM_REGSET_R16 : ASM_REGSET_R32;
    new->op2.base_reg = ASM_REG_EAX;
      
  return (new->len);
}

/*
  <instruction func="op_lodsd" opcode="0xad"/>
*/

int op_lodsd(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_LODSD;
  new->ptr_instr = opcode;
  
  new->op1.type = ASM_OTYPE_YDEST;
  new->op2.type = ASM_OTYPE_XSRC;
  
  new->op1.regset = ASM_REGSET_R32;
  new->op1.prefix = ASM_PREFIX_ES;
  new->op1.base_reg = ASM_REG_EDI;
  
  new->op2.regset = ASM_REGSET_R32;
  new->op2.prefix = ASM_PREFIX_DS;
  new->op2.base_reg = ASM_REG_ESI;
  
  return (new->len);
}

/*
  <instruction func="op_scasb" opcode="0xae"/>
*/

int op_scasb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {

  new->instr = ASM_SCASB;
  new->ptr_instr = opcode;
  new->len += 1;
  new->op1.type = ASM_OTYPE_FIXED;
  new->op2.type = ASM_OTYPE_YDEST;

  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_R8;
  new->op1.base_reg = ASM_REG_EAX;
  
  new->op2.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op2.regset = ASM_REGSET_R32;
  new->op2.prefix = ASM_PREFIX_ES;
  new->op2.base_reg = ASM_REG_EDI;
  
  
  return (new->len);
}


/*
  <instruction func="op_scasd" opcode="0xaf"/>
*/

int op_scasd(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  if (asm_proc_oplen(proc))
    new->instr = ASM_SCASW;
  else
    new->instr = ASM_SCASD;
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_FIXED;
  new->op2.type = ASM_OTYPE_YDEST;

  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_R8;
  new->op1.base_reg = ASM_REG_EAX;
  
  new->op2.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op2.regset = ASM_REGSET_R32;
  new->op2.prefix = ASM_PREFIX_ES;
  new->op2.base_reg = ASM_REG_EDI;
  new->instr = ASM_SCASD;
  return (new->len);
}





