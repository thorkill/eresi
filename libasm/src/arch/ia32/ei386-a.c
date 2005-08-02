/*
** private - do not distribute
** 
** Author  : <sk at devhell dot org>
** Started : Fri Aug  2 03:04:41 2002
** Updated : Tue May  6 21:32:36 2003
*/

#include <libasm.h>
#include <libasm-int.h>

/*
  Opcode :		0x0f 0xa0
  Instruction :		PUSH
  <i386 func="i386_push_fs" opcode="0xa0"/>
*/

int i386_push_fs(asm_instr *new, u_char *opcode, u_int len, 
		 asm_processor *proc) {
    new->instr = ASM_PUSH;
    new->len += 1;
    new->op1.type = ASM_OTYPE_FIXED;
    new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
    new->op1.regset = ASM_REGSET_SREG;
    new->op1.len = 0;
    new->op1.ptr = 0;
    new->op1.base_reg = ASM_REG_FS;
    return (new->len);
}

/*
  Opcode :		0x0f 0xa1
  Instruction :		POP
*/

int i386_pop_fs(asm_instr *new, u_char *opcode, u_int len, 
		asm_processor *proc) {
    new->instr = ASM_POP;
    new->len += 1;
    new->op1.type = ASM_OTYPE_FIXED;
    new->op1.regset = ASM_REGSET_SREG;
    new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
    new->op1.len = 0;
    new->op1.ptr = 0;
    new->op1.base_reg = ASM_REG_FS;
  return (new->len);
}

/*
  Opcode :		0x0f 0xa2
  Instruction :		CPUID
 */

int i386_cpuid(asm_instr *new, u_char *opcode, u_int len, 
	       asm_processor *proc) {
    new->len += 1;
    new->instr = ASM_CPUID;
  return (new->len);
}

/*
  <i386 func="i386_bt_rm_r" opcode="0xa3"/>
*/

int i386_bt_rm_r(asm_instr *new, u_char *opcode, u_int len, 
		 asm_processor *proc) {
  struct s_modrm	*modrm;
  
  modrm = (struct s_modrm *) opcode + 1;
  new->instr = ASM_BT;
  new->len += 1;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_GENERAL;
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
    /*
    new->op1 = asm_create_operand(0, 0, 0, 0, proc);
    new->op1.content = OP_REG;
    new->op1.ptr = opcode + 1;
    new->op1.len = 1;
    new->op1.base_reg = modrm->m;
    
    
    new->op2 = asm_create_operand(0, 0, 0, 0, proc);
    new->op2.content = OP_REG;
    new->op2.ptr = opcode + 1;
    new->op2.len = 0;
    new->op2.base_reg = modrm->r;
    
    new->len += new->op1.len + new->op2.len;
    */
  return (new->len);
}


/*
  <i386 func="i386_shld" opcode="0xa4"/>
*/

int i386_shld(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;
  
  modrm = (struct s_modrm *) opcode + 1;
    new->instr = ASM_SHLD;

    new->op1.type = ASM_OTYPE_REGISTER;
    new->op1.regset = ASM_REGSET_R32;
    new->op1.content = ASM_OP_BASE;
    new->op1.ptr = opcode + 1;
    new->op1.len = 0;
    new->op1.base_reg = modrm->m;

    new->op2.type = ASM_OTYPE_GENERAL;
    new->op2.regset = ASM_REGSET_R32;
    new->op2.content = ASM_OP_BASE;
    new->op2.ptr = opcode + 1;
    new->op2.len = 1;
    new->op2.base_reg = modrm->r;
      
    new->op3.type = ASM_OTYPE_IMMEDIATE;
    new->op3.content = ASM_OP_VALUE;
    new->op3.ptr = opcode + 2;
    new->op3.len = 1;
    new->op3.imm = 0;
    memcpy(&new->op3.imm, opcode + 2, 1);
    new->len += new->op1.len + new->op2.len + new->op3.len + 1;
  return (new->len);
}


/*
  <i386 func="i386_bts" opcode="0xab"/>
 */

int i386_bts(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->len += 1;
    new->instr = ASM_BTS;
    new->op1.type = ASM_OTYPE_ENCODED;
    new->op2.type = ASM_OTYPE_GENERAL;
    operand_rmv_rv(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
 <i386 func="i386_shrd_rmv_rv_ib" opcode="0xac"/>
*/

int i386_shrd_rmv_rv_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->instr = ASM_SHRD;
    new->len += 1;
    new->op1.type = ASM_OTYPE_ENCODED;
    new->op1.size = ASM_OSIZE_VECTOR;
    new->op2.type = ASM_OTYPE_GENERAL;
    new->op2.size = ASM_OSIZE_VECTOR;
    new->op3.type = ASM_OTYPE_IMMEDIATE;
    new->op3.size = ASM_OSIZE_BYTE;
    operand_rmv_rv(new, opcode + 1, len - 1, proc);
    new->op3.type = ASM_OTYPE_IMMEDIATE;
    new->op3.content = ASM_OP_VALUE;
    new->op3.ptr = opcode + 2;
    new->op3.len = 1;
    
    new->op3.imm = 0;
    memcpy(&new->op3.imm, opcode + 2, 1);
    new->len += 1;
  return (new->len);
}

/*
 <i386 func="i386_shrd_rmv_rv_cl" opcode="0xad"/>
*/

int i386_shrd_rmv_rv_cl(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->instr = ASM_SHRD;
    new->len += 1;
    new->op1.type = ASM_OTYPE_ENCODED;
    new->op1.size = ASM_OSIZE_VECTOR;
    new->op2.type = ASM_OTYPE_GENERAL;
    new->op2.size = ASM_OSIZE_VECTOR;
    operand_rmv_rv(new, opcode + 1, len - 1, proc);
    new->op3.type = ASM_OTYPE_FIXED;
    new->op3.content = ASM_OP_BASE;
    new->op3.regset = ASM_REGSET_R8;
    new->op3.ptr = opcode;
    new->op3.len = 0;
    new->op3.base_reg = ASM_REG_CL;
  return (new->len);
}



/*
  <i386 func="i386_imul_rv_rmv" opcode="0xaf"/>
*/

int i386_imul_rv_rmv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->instr = ASM_IMUL;
    new->len += 1;
    new->op1.type = ASM_OTYPE_GENERAL;
    new->op1.size = ASM_OSIZE_VECTOR;
    new->op2.type = ASM_OTYPE_ENCODED;
    new->op2.size = ASM_OSIZE_VECTOR;
    operand_rv_rmv(new, opcode + 1, len - 1, proc);
    return (new->len);
}





