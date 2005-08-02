/*
## this is private - do not distribute
** 
** Author  : <sk at devhell dot org>
** Started : Wed Jul 24 18:14:37 2002
** Updated : Sat Mar 20 05:42:38 2004
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_and_rmb_rb" opcode="0x20"/>
*/

int op_and_rmb_rb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_AND;
  
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_GENERAL;
  operand_rmb_rb(new, opcode + 1, len -1, proc);
  return (new->len);
} 


/*
  <instruction func="op_and_rmv_rv" opcode="0x21"/>
*/

int op_and_rmv_rv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_AND;
  
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_GENERAL;
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
  return (new->len);
}
/*
  <instruction func="op_and_rb_rmb" opcode="0x22"/>
*/

int op_and_rb_rmb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_AND;
  
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rb_rmb(new, opcode + 1, len - 1, proc);
  return (new->len);
}
/*
  <instruction func="op_and_rv_rmv" opcode="0x23"/>
*/

int op_and_rv_rmv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_AND;
  
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rv_rmv(new, opcode + 1, len -1, proc);
  return (new->len);
}
/*
  <instruction func="op_and_al_ib" opcode="0x24"/>
*/

int op_and_al_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 2;
  new->instr = ASM_AND;
  new->ptr_instr = opcode;
  
  new->op1.type = ASM_OTYPE_FIXED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;

  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.regset = ASM_REGSET_R8;
  new->op1.ptr = opcode;
  new->op1.len = 0;
  new->op1.base_reg = ASM_REG_AL;

    new->op2.content = ASM_OP_VALUE;
    new->op2.ptr = opcode + 1;
    new->op2.len = 1;
    new->op2.imm = 0;
    memcpy(&new->op2.imm, opcode + 1, 1);
    return (new->len);
}
/*
  <instruction func="op_and_eax_iv" opcode="0x25"/>
*/

int op_and_eax_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_AND;
  new->len += 1 + asm_proc_vector_len(proc);
  new->ptr_instr = opcode;

  new->op1.type = ASM_OTYPE_FIXED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;

  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.regset = ASM_REGSET_R32;
  new->op1.ptr = opcode;
  new->op1.len = 0;
  new->op1.base_reg = ASM_REG_EAX;
  new->op1.regset = asm_proc_is_protected(proc) ? 
    ASM_REGSET_R32 : ASM_REGSET_R16;

  new->op2.content = ASM_OP_VALUE;
  new->op2.ptr = opcode + 1;
  new->op2.len = asm_proc_vector_len(proc);
  new->op2.imm = 0;
  memcpy(&new->op2.imm, opcode + 1, asm_proc_vector_len(proc));
  return (new->len);
}

/*
  <instruction func="op_prefix_es" opcode="0x26"/>
*/

int	op_prefix_es(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->prefix |= ASM_PREFIX_ES;
  if (!new->ptr_prefix)
    new->ptr_prefix = opcode;
  new->len += 1;
  return (proc->fetch(new, opcode + 1, len - 1, proc));
}

/*
  <instruction func="op_daa" opcode="0x27"/>
*/

int	op_daa(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_DAA;
  return (new->len);
}

/*
  <instruction func="op_sub_rmb_rb" opcode="0x28"/>
*/



int op_sub_rmb_rb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_SUB;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->ptr_instr = opcode;
  new->op2.type = ASM_OTYPE_GENERAL;
  operand_rmb_rb(new, opcode + 1, len - 1, proc);
  return (new->len);
} 
/*
  <instruction func="op_sub_rmv_rv" opcode="0x29"/>
*/

int op_sub_rmv_rv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_SUB;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_GENERAL;
    operand_rmv_rv(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <instruction func="op_sub_rb_rmb" opcode="0x2a"/>
*/

int op_sub_rb_rmb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_SUB;
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
    operand_rb_rmb(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <instruction func="op_sub_rv_rmv" opcode="0x2b"/>
*/

int op_sub_rv_rmv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_SUB;
  new->ptr_instr = opcode;
  
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  return (new->len);
}


/*
  <instruction func="op_sub_al_ib" opcode="0x2c"/>
*/

int op_sub_al_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 2;
  new->instr = ASM_SUB;
  
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_FIXED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;

  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.ptr = opcode;
  new->op1.len = 0;
  new->op1.regset = ASM_REGSET_R8;
  new->op1.base_reg = ASM_REG_AL;
  
  new->op2.content = ASM_OP_VALUE;
  new->op2.ptr = opcode + 1;
  new->op2.len = 1;
  new->op2.imm = 0;
  memcpy(&new->op2.imm, opcode + 1, 1);
  return (new->len);
}

/*
  <instruction func="op_sub_eax_iv" opcode="0x2d"/>
*/

int op_sub_eax_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {

  new->instr = ASM_SUB;
  new->len += 1 + asm_proc_vector_len(proc);
  new->ptr_instr = opcode;

  new->op1.type = ASM_OTYPE_FIXED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.ptr = opcode;
  new->op1.len = 0;
  new->op1.regset = ASM_REGSET_R32;
  new->op1.base_reg = ASM_REG_EAX;

  new->op2.content = ASM_OP_VALUE;
  new->op2.ptr = opcode + 1;
  new->op2.len = asm_proc_vector_len(proc);
  new->op2.imm = 0;
  memcpy(&new->op2.imm, opcode + 1, asm_proc_vector_len(proc));
  return (new->len);
}

/*
  <instruction func="op_prefix_cs" opcode="0x2e"/>
*/

int	op_prefix_cs(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->prefix |= ASM_PREFIX_CS;
  if (!new->ptr_prefix)
    new->ptr_prefix = opcode;
  new->len += 1;
  return (proc->fetch(new, opcode + 1, len - 1, proc));
}


/*
  <instruction func="op_das" opcode="0x2f"/>
 */

int	op_das(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_DAS;
  return (new->len);
}

