/*
## this is private - do not distribute
##
** Author  : <sk at devhell dot org>
** Started : Xxx Xxx xx xx:xx:xx 2002
** Updated : Fri May  9 19:03:13 2003
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_adc_rmb_rb" opcode="0x10"/>
*/

int op_adc_rmb_rb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_ADC;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op1.size= ASM_OSIZE_BYTE;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op2.size= ASM_OSIZE_BYTE;
  operand_rmb_rb(new, opcode + 1, len - 1, proc);
  return (new->len);
} 

/*
  <instruction func="op_adc_rmv_rv" opcode="0x11"/>
*/

int op_adc_rmv_rv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_ADC;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op1.size= ASM_OSIZE_VECTOR;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op2.size= ASM_OSIZE_VECTOR;
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <instruction func="op_adc_rb_rmb" opcode="0x12"/>
*/

int op_adc_rb_rmb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_ADC;
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op1.size = ASM_OSIZE_BYTE;
  new->op2.type = ASM_OTYPE_ENCODED;
  new->op2.size = ASM_OSIZE_BYTE;
  operand_rb_rmb(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <instruction func="op_adc_rv_rmv" opcode="0x13"/>
*/

int op_adc_rv_rmv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_ADC;
  new->op1.size = new->op2.size = asm_proc_vector_size(proc);
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op1.size = ASM_OSIZE_BYTE;
  new->op2.type = ASM_OTYPE_ENCODED;
  new->op2.size= ASM_OSIZE_BYTE;
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <instruction func="op_adc_al_ib" opcode="0x14"/>
*/

int op_adc_al_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_ADC;
  new->len += 2;
  new->ptr_instr = opcode;

  new->op1.type = ASM_OTYPE_FIXED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op1.size = new->op2.size = ASM_OSIZE_BYTE;

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
  <instruction func="op_adc_eax_iv" opcode="0x15"/>
*/

int op_adc_eax_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_ADC;
  new->ptr_instr = opcode;
  new->len += 5;
  
  new->op1.type = ASM_OTYPE_FIXED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op1.size = new->op2.size = asm_proc_vector_size(proc);
  
  new->op1.content = ASM_OP_FIXED | ASM_OP_BASE;
  new->op1.base_reg = ASM_REG_EAX;
  
  new->op2.content = ASM_OP_VALUE;
  new->op2.ptr = opcode;
  new->op2.len = asm_proc_vector_len(proc);
  memcpy(&new->op2.imm, opcode + 1, 4);
  
  return (new->len);
}


/*
  <instruction func="op_push_ss" opcode="0x16"/>
*/

int	op_push_ss(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_PUSH;
  new->len += 1;
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_FIXED;
  
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_SREG;
  // new->type = IS_MEM_WRITE;
  return (new->len);
}

/*
  <instruction func="op_pop_es" opcode="0x17"/>
*/

int	op_pop_ss(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_POP;
  new->len += 1;
  // new->type = IS_MEM_READ;
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_FIXED;
  
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.regset = ASM_REGSET_SREG;
  new->op1.base_reg = ASM_REG_SS;
  
  return (new->len);
}

/*
  <instruction func="op_sbb_rmb_rb" opcode="0x18"/>
*/

int op_sbb_rmb_rb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_SBB;
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op2.size = new->op1.size = ASM_OSIZE_BYTE;
  operand_rmb_rb(new, opcode + 1, len - 1, proc);
  return (new->len);
} 

/*
  <instruction func="op_sbb_rmv_rv" opcode="0x19"/>
*/

int op_sbb_rmv_rv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {

  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_SBB;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op2.size = new->op1.size = ASM_OSIZE_VECTOR;
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <instruction func="op_sbb_rb_rmb" opcode="0x1a"/>
*/

int op_sbb_rb_rmb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_SBB;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  new->op2.size = new->op1.size = ASM_OSIZE_BYTE;
  operand_rb_rmb(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <instruction func="op_sbb_rv_rmv" opcode="0x1b"/>
*/

int op_sbb_rv_rmv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  
  new->len += 1;
  new->instr = ASM_SBB;
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  new->op2.size = new->op1.size = ASM_OSIZE_VECTOR;
  new->op1.size = new->op2.size = asm_proc_vector_size(proc);
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <instruction func="op_sbb_al_ib" opcode="0x1c"/>
*/

int op_sbb_al_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {

  new->instr = ASM_SBB;
  new->len += 2;
  new->ptr_instr = opcode;

  new->op1.type = ASM_OTYPE_FIXED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op1.size = new->op2.size = ASM_OSIZE_BYTE;

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
  <instruction func="op_sbb_eax_iv" opcode="0x1d"/>
*/

int op_sbb_eax_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {

  new->len += 5;
  new->ptr_instr = opcode;
  new->instr = ASM_SBB;
  new->op1.type = ASM_OTYPE_FIXED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op1.size = new->op2.size = ASM_OSIZE_VECTOR;
    

  new->op1.content = ASM_OP_FIXED | ASM_OP_BASE;
  new->op1.base_reg = ASM_REG_EAX;
  new->op1.regset = asm_proc_oplen(proc) ? ASM_REGSET_R32 : 
    ASM_REGSET_R16;
  
  new->op2.content = ASM_OP_VALUE;
  new->op2.ptr = opcode;
  new->op2.len = asm_proc_vector_len(proc);;
  memcpy(&new->op2.imm, opcode + 1, 4);
  return (new->len);
}

/*
  <instruction func="op_push_ds" opcode="0x1e"/>
*/

int op_push_ds(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_PUSH;
  // new->type = IS_MEM_WRITE;
  new->op1.type = ASM_OTYPE_FIXED;
  
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_SREG;
  new->op1.base_reg = ASM_REG_DS;
  return (new->len);
}


/*
  <instruction func="op_pop_ds" opcode="0x1f"/>
*/

int	op_pop_ds(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  
  new->instr = ASM_POP;
  new->ptr_instr = opcode;
  new->len += 1;
  // new->instr = IS_MEM_READ;
  new->op1.type = ASM_OTYPE_FIXED;
  
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.regset = ASM_REGSET_SREG;
  new->op1.base_reg = ASM_REG_DS;
  new->op1.len = 0;
  
  return (new->len);
}


