/*
**
** this is private - do not distribute
**
** Author  : <sk at devhell dot org>
** Started : Wed Jun 12 16:59:54 2002
** Updated : Sun Mar 21 00:36:06 2004
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_xor_rmb_rb" opcode="0x30"/>
*/

int op_xor_rmb_rb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_XOR;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op1.size = ASM_OSIZE_BYTE;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op2.size = ASM_OSIZE_BYTE;
    operand_rmb_rb(new, opcode + 1, len - 1, proc);
  return (new->len);
} 

/*
  <instruction func="op_xor_rmv_rv" opcode="0x31"/>
*/

int op_xor_rmv_rv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_XOR;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op1.size = ASM_OSIZE_VECTOR;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op2.size = ASM_OSIZE_VECTOR;
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
  return (new->len);  
}


/*
  <instruction func="op_xor_rb_rmb" opcode="0x32"/>
*/

int op_xor_rb_rmb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_XOR;
  new->len += 1;
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op1.size = ASM_OSIZE_BYTE;
  new->op2.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OSIZE_BYTE;
  operand_rb_rmb(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <instruction func="op_xor_rv_rmv" opcode="0x33"/>
*/

int op_xor_rv_rmv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {

  new->instr = ASM_XOR;
  new->ptr_instr = opcode;
  new->len += 1;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op1.size = ASM_OSIZE_VECTOR;
  new->op2.type = ASM_OTYPE_ENCODED;
  new->op2.size = ASM_OSIZE_VECTOR;
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <instruction func="op_xor_al_ib" opcode="0x34"/>
*/

int op_xor_al_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_XOR;
  new->len += 2;
  
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.size = ASM_OSIZE_BYTE;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op2.size = ASM_OSIZE_BYTE;
  
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.ptr = opcode;
  new->op1.len = 0;
  new->op1.base_reg = ASM_REG_AL;
  new->op1.regset = ASM_REGSET_R8;
    
    
  new->op2.content = ASM_OP_VALUE;
  new->op2.ptr = opcode + 1;
  new->op2.len = 1;
  new->op2.imm = 0;
  memcpy(&new->op2.imm, opcode + 1, 1);
  return (new->len);
}

/*
  <instruction func="op_xor_eax_iv" opcode="0x35"/>
*/


int op_xor_eax_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->instr = ASM_XOR;
  new->ptr_instr = opcode;
    new->len += 5;
    
    new->op1.type = ASM_OTYPE_FIXED;
    new->op2.type = ASM_OTYPE_IMMEDIATE;

    new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
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
  <instruction func="op_prefix_ss" opcode="0x36"/>
*/

int	op_prefix_ss(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->prefix |= ASM_PREFIX_SS;
  if (!new->ptr_prefix)
    new->ptr_prefix = opcode;
  return (proc->fetch(new, opcode + 1, len - 1, proc));
}

/*
  <instruction func="op_aaa" opcode="0x37"/>
*/

int	op_aaa(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_AAA;
  return (new->len);
}

/*
  <instruction func="op_cmp_rmb_rb" opcode="0x38"/>
*/

int op_cmp_rmb_rb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_CMP;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_GENERAL;

  operand_rmb_rb(new, opcode + 1, len - 1, proc);
  return (new->len);
} 

/*
  <instruction func="op_cmp_rmv_rv" opcode="0x39"/>
*/

int op_cmp_rmv_rv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_CMP;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_GENERAL;

  operand_rmv_rv(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <instruction func="op_cmp_rb_rmb" opcode="0x3a"/>
*/

int op_cmp_rb_rmb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_CMP;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;

  operand_rb_rmb(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <instruction func="op_cmp_rv_rmv" opcode="0x3b"/>
*/

int op_cmp_rv_rmv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_CMP;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;

  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <instruction func="op_cmp_al_ib" opcode="0x3c"/>
*/

int op_cmp_al_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_CMP;
  new->len += 2;
  
  new->op1.type = ASM_OTYPE_FIXED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->ptr_instr = opcode;

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
  <instruction func="op_cmp_eax_iv" opcode="0x3d"/>
*/

int op_cmp_eax_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_CMP;
  new->ptr_instr = opcode;
  new->len += 1 + asm_proc_vector_len(proc);
    
  new->op1.type = ASM_OTYPE_FIXED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;

  new->op1.content = ASM_OP_FIXED | ASM_OP_BASE;
  new->op1.base_reg = ASM_REG_EAX;
  new->op1.regset = asm_proc_oplen(proc) ?
    ASM_REGSET_R16 : ASM_REGSET_R32;
  
  new->op2.content = ASM_OP_VALUE;
  new->op2.ptr = opcode;
  new->op2.len = asm_proc_vector_len(proc);
  memcpy(&new->op2.imm, opcode + 1, asm_proc_vector_len(proc));
    
  return (new->len);
}

/*
  <instruction func="op_prefix_ds" opcode="0x3e"/>
*/

int op_prefix_ds(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  if (!new->ptr_prefix)
  new->ptr_prefix = opcode;
  new->prefix |= ASM_PREFIX_DS;
  return (proc->fetch(new, opcode + 1, len - 1, proc));
}

/*
  <instruction func="op_aas" opcode="0x3f"/>
 */

int	op_aas(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  
  new->instr = ASM_AAS;
  return (new->len);
}


