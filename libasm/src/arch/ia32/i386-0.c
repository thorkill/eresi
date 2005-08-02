/*
** Author : <sk at devhell dot org>  
** Started : Sat Jun  1 03:13:28 2002
** Updated : Thu Apr  8 01:37:10 2004
*/
#include <libasm.h>
#include <libasm-int.h>

/*
 * Opcode :		0x00
 * ADD
 * Destination is an encoded byte.
 * Source is a byte register.
 */

int op_add_rmb_rb(asm_instr *new, u_char *opcode, u_int len, 
		  asm_processor *proc) {
  new->instr = ASM_ADD;
  new->ptr_instr = opcode;
  new->len++;
  /* new->type = IS_MEM_READ */
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op1.size = ASM_OSIZE_BYTE;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op2.size = ASM_OSIZE_BYTE;
  operand_rmb_rb(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
 * Opcode :		0x01
 * Instruction :		ADD
 * Operands:		Destination: encoded vector.
 * Source: is a vector register.
 */

int op_add_rmv_rv(asm_instr *new, u_char *opcode, u_int len, 
		  asm_processor *proc) {
  new->len++;
  new->ptr_instr = opcode;
  new->instr = ASM_ADD;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.size = ASM_OSIZE_VECTOR;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op2.size = ASM_OSIZE_VECTOR;
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
 * Opcode :		0x02
 * Instruction :		ADD
 * Destination is a byte register
 * Source is a byte encoded
 */

int op_add_rb_rmb(asm_instr *new, u_char *opcode, u_int len, 
		  asm_processor *proc) {
  new->instr = ASM_ADD;
  new->ptr_instr = opcode;
  new->len++;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op1.size = ASM_OSIZE_BYTE;
  new->op2.type = ASM_OTYPE_ENCODED;
  new->op2.size = ASM_OSIZE_BYTE;

  operand_rb_rmb(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  Opcode :		0x03
  Instruction :		ADD
  */

int op_add_rv_rmv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_ADD;
  new->len++;
  new->ptr_instr = opcode;

  new->op1.type = ASM_OTYPE_GENERAL;
  new->op1.size = ASM_OSIZE_VECTOR;
  new->op2.type = ASM_OTYPE_ENCODED;
  new->op2.size = ASM_OSIZE_VECTOR;
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  
  return (new->len);
}


/*
  Opcode :		0x04
  Instruction :		ADD
*/

int op_add_al_ib(asm_instr *new, u_char *opcode, u_int len, 
		      asm_processor *proc) {
  new->instr = ASM_ADD;
  new->ptr_instr = opcode;
  new->len += 2;
  new->op1.type = ASM_OTYPE_FIXED;
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
  Opcode :		0x05
  Instruction :		ADD
*/

int op_add_eax_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    
  new->instr = ASM_ADD;
  new->ptr_instr = opcode;
  new->len += 1 + asm_proc_vector_len(proc);
  
  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.content = ASM_OP_FIXED | ASM_OP_BASE;
  new->op1.len = 0;
  new->op1.base_reg = ASM_REG_EAX;
  new->op1.regset = asm_proc_oplen(proc) ? ASM_REGSET_R16 :
    ASM_REGSET_R32;
  
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op2.size = asm_proc_vector_size(proc);
  new->op2.content = ASM_OP_VALUE;
  new->op2.ptr = opcode + 1;
  new->op2.len = asm_proc_vector_len(proc);
  memcpy(&new->op2.imm, opcode + 1, asm_proc_vector_len(proc));
  
  return (new->len);
}


/*
  Opcode :		0x06
  Instruction :		PUSH
*/

int op_push_es(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_PUSH;
  // new->type = IS_MEM_WRITE;
  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.content |= ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.regset = ASM_REGSET_SREG;
  new->op1.base_reg = ASM_REG_ES;
  return (new->len);
}

/*
  Opcode :		0x07
  Instruction :		POP
*/

int op_pop_es(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    
  new->instr = ASM_POP;
  // new->type = IS_MEM_READ;
  new->len += 1;
    
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.regset = ASM_REGSET_SREG;
  new->op1.base_reg = ASM_REG_ES;
  return (new->len);
}



/*
  Opcode :		0x08
  Instruction :		OR
*/

int op_or_rmb_rb(asm_instr *new, u_char *opcode, u_int len, 
		 asm_processor *proc) {
  new->instr = ASM_OR;
  new->ptr_instr = opcode;
  new->len += 1;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op1.size = new->op2.size = ASM_OSIZE_BYTE;
  operand_rmb_rb(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  Opcode :		0x09
  Instruction :		OR
*/

int op_or_rmv_rv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_OR;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op1.size = new->op2.size = asm_proc_vector_size(proc);
    
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  Opcode :		0x0a
  */

int op_or_rb_rmb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {

  new->instr = ASM_OR;
  new->ptr_instr = opcode;
  new->len += 1;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  new->op1.size = new->op2.size = ASM_OSIZE_BYTE;
  operand_rb_rmb(new, opcode + 1, len - 1, proc); 
    
  return (new->len);
}

/*
  Opcode :		0x0b
  Instruction :		OR
*/

int op_or_rv_rmv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_OR;
  new->ptr_instr = opcode;
  new->len += 1;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  new->op1.size = new->op2.size = asm_proc_vector_size(proc);
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  Opcode :		0x0c
  Instruction :		OR
*/

int op_or_al_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    
  new->instr = ASM_OR;
  new->ptr_instr = opcode;
  new->len += 2;
  
  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.ptr = opcode;
  
  new->op1.len = 0;
  new->op1.base_reg = ASM_REG_AL;
  new->op1.regset = ASM_REGSET_R8;

  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op2.content = ASM_OP_VALUE;
  new->op2.ptr = opcode + 1;
  new->op2.len = 1;
  new->op2.imm = 0;
  memcpy(&new->op2.imm, opcode + 1, 1);

  return (new->len);
}

/*
  Opcode :		0x0d
  Instruction :		OR
  */

int op_or_eax_iv(asm_instr *new, u_char *opcode, u_int len, 
		 asm_processor *proc) {
  new->instr = ASM_OR;
  new->ptr_instr = opcode;
  
  new->len += 1 + asm_proc_vector_len(proc);
  
  new->op1.type = ASM_OP_FIXED;
  new->op1.content = ASM_OP_FIXED | ASM_OP_BASE;
  new->op1.regset = asm_proc_oplen(proc) ? 
    ASM_REGSET_R16 : ASM_REGSET_R32;
  new->op1.base_reg = ASM_REG_EAX;
  
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op2.size = asm_proc_vector_size(proc);
  new->op2.content = ASM_OP_VALUE;
  new->op2.ptr = opcode + 1;
  new->op2.len = asm_proc_vector_len(proc);
  memcpy(&new->op2.imm, opcode + 1, asm_proc_vector_len(proc));
  
  return (new->len);
}

/*
  Opcode :		0x0e
  Instruction :		PUSH
  */

int op_push_cs(asm_instr *new, u_char *opcode, u_int len, 
	       asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_PUSH;


  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.regset = ASM_REGSET_SREG;
  new->op1.base_reg = ASM_REG_CS;
  return (new->len);
}

/*
 * Opcode :		0x0f
 * Following byte must be fetched from i386 instruction table.
 */

int op_386sp(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  asm_i386_processor	*i386p;
  
  /* this is a bridge to i386 instructions, pentium, ... instructions */
  new->ptr_instr = opcode;
  i386p = (asm_i386_processor *) proc;
  if (i386p->internals->cisc_i386[*(opcode + 1)]) {
    new->len += 1;
    return (i386p->internals->cisc_i386[*(opcode + 1)](new, opcode + 1, 
						      len - 1, proc));  
  }
  else
    return (0);
}





