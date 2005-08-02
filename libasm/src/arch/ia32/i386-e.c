/*
** private - do not distribute
** 
** Author  : <sk at devhell dot org>
** Started : Xxx Xxx xx xx:xx:xx 2002
** Updated : Fri May  9 19:03:10 2003
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_loopne" opcode="0xe0"/>
*/

int op_loopne(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->instr = ASM_LOOPNE;
    new->type = ASM_TYPE_CONDBRANCH;
    new->ptr_instr = opcode;
    new->len += 2;
    new->op1.type = ASM_OTYPE_JUMP;
    new->op1.size = ASM_OSIZE_BYTE;
    new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
    new->op1.ptr = opcode + 1;
    
    new->op1.imm = 0;
    if (*(opcode + 1) >= 0x80u)
      memcpy((char *) &new->op1.imm + 1, "\ff\xff\xff", 3);
    memcpy((char *) &new->op1.imm, opcode + 1, 1);
  return (new->len);
}

/*
  <instruction func="op_loope" opcode="0xe1"/>
 */

int op_loope(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 2;
    new->ptr_instr = opcode;
    new->instr = ASM_LOOPE;
    new->type = ASM_TYPE_CONDBRANCH;
    new->op1.type = ASM_OTYPE_JUMP;
    new->op1.size = ASM_OSIZE_BYTE;
    new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
    new->op1.ptr = opcode + 1;
    
    new->op1.imm = 0;
    if (*(opcode + 1) >= 0x80u)
      memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
    memcpy((char *) &new->op1.imm, opcode + 1, 1);
  return (new->len);
}

/*
  <instruction func="op_loop" opcode="0xe2"/>
 */

int op_loop(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 2;
    new->ptr_instr = opcode;
    new->type = ASM_TYPE_CONDBRANCH;
  new->instr = ASM_LOOP;
  new->op1.type = ASM_OTYPE_JUMP;
  new->op1.size = ASM_OSIZE_BYTE;
  new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
  new->op1.ptr = opcode + 1;
    
  new->op1.imm = 0;
  if (*(opcode + 1) > 0x80u)
    memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
  memcpy((char *) &new->op1.imm, opcode + 1, 1);
  return (new->len);
}

/*
  <instruction func="op_je_cxz" opcode="0xe3"/>
 */

int op_je_cxz(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 3;
    new->ptr_instr = opcode;
    new->type = ASM_TYPE_CONDBRANCH;
  new->instr = ASM_BRANCH_CXZ;
  new->op1.type = ASM_OTYPE_JUMP;
  new->op1.size = ASM_OSIZE_BYTE;
  new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
  new->op1.ptr = opcode + 1;
    
  new->op1.imm = 0;
  if (*(opcode + 1) > 0x80u)
    memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
  memcpy((char *) &new->op1.imm, opcode + 1, 1);
  // new->type = IS_COND_BRANCH;
  return (new->len);
}

/*
  <instruction func="op_in_al_ref_ib" opcode="0xe4"/>
*/

int op_in_al_ref_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->instr = ASM_IN;
    new->ptr_instr = opcode;
    new->len += 2;

    new->op1.type = ASM_OTYPE_FIXED;
    new->op1.content = ASM_OP_BASE;
    new->op1.regset = ASM_REGSET_R8;
    new->op1.base_reg = ASM_REG_AL;

    new->op2.type = ASM_OTYPE_IMMEDIATE;
    new->op2.content = ASM_OP_VALUE;
    
    new->op2.imm = 0;
    memcpy(&new->op2.imm, opcode + 1, 1);
    
  return (new->len);
}

/*
  <instruction func="op_in_eax_ref_ib" opcode="0xe5"/>
*/

int op_in_eax_ref_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_IN;
    new->ptr_instr = opcode;
  new->len += 1;
  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.base_reg = ASM_REG_EAX;
  new->op1.regset = asm_proc_oplen(proc) ? 
    ASM_REGSET_R16 : ASM_REGSET_R32;
    
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op2.content = ASM_OP_REFERENCE | ASM_OP_VALUE;

  return (new->len);
}


/*
  <instruction func="op_out_ref_ib_al" opcode="0xe6"/>
*/


int op_out_ref_ib_al(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->instr = ASM_OUT;
    new->ptr_instr = opcode;
    new->len += 2;
    new->op1.type = ASM_OTYPE_IMMEDIATE;
    new->op1.content = ASM_OP_VALUE;
    
    new->op1.imm = 0;
    memcpy(&new->op1.imm, opcode + 1, 1);
    
    new->op2.type = ASM_OTYPE_FIXED;
    new->op2.content = ASM_OP_BASE;
    new->op2.regset = ASM_REGSET_R8;
    new->op2.base_reg = ASM_REG_AL;
    
    return (new->len);
}

/*
  <instruction func="op_out_ref_ib_eax" opcode="0xe7"/>
*/

int op_out_ref_ib_eax(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->ptr_instr = opcode;
    new->instr = ASM_OUT;
    new->op1.type = ASM_OTYPE_IMMEDIATE;
    new->op1.content = ASM_OP_VALUE;
    new->op1.len = 1;
    new->op1.imm = 0;
    memcpy(&new->op1.imm, opcode + 1, 1);
    
    new->op2.type = ASM_OTYPE_FIXED;
    new->op2.content = ASM_OP_BASE;
    new->op2.regset = ASM_REGSET_R32;
    new->op2.base_reg = ASM_REG_EAX;
    new->len += new->op1.len;

    return (new->len);
}

/*
  <instruction func="op_call_iv" opcode="0xe8"/>
*/

int op_call_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {

    new->ptr_instr = opcode;
  new->instr = ASM_CALL;
  new->type = ASM_TYPE_CALLPROC;
  new->op1.type = ASM_OTYPE_JUMP;
  new->len += 1 + asm_proc_vector_len(proc);
    
  new->op1.ptr = opcode + 1;
  new->op1.len = 4;
  new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
    
  new->op1.imm = 0;
  memcpy(&new->op1.imm, opcode + 1, 4);
  return (new->len);
}

/*
  <instruction func="op_jmp_iv" opcode="0xe9"/>
 */

int op_jmp_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {

    new->instr = ASM_BRANCH;
    new->type = ASM_TYPE_IMPBRANCH;
    new->ptr_instr = opcode;
    new->len += 5;
    // new->type = IS_JMP;
    
    new->op1.type = ASM_OTYPE_JUMP;
    new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
    new->op1.ptr = opcode + 1;
    memcpy(&new->op1.imm, opcode + 1, 4);
  return (new->len);
}

/*
  <instruction func="op_jmp_ap" opcode="0xea"/>
 */

int	op_jmp_ap(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->ptr_instr = opcode;
  new->instr = ASM_LBRANCH;
  new->len += 7;
  new->type = ASM_TYPE_IMPBRANCH;

  new->op1.type = ASM_OTYPE_ADDRESS;
  new->op1.content = ASM_OP_VALUE;
  new->op1.len = 4;
  memcpy(&new->op1.imm, opcode + 1, 4);
  
  /*
  new->op2 = &new->op2;
  new->op2.len = 2;
  new->op2.content = ASM_OP_VALUE;
  new->op2.imm = 0;
  memcpy(&new->op2.imm, opcode + 5, 2);
  */
  
  return (new->len);
}

/*
  <instruction func="op_jmp_ib" opcode="0xeb"/>
 */

int op_jmp_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->instr = ASM_BRANCH;
    
    new->type = ASM_TYPE_IMPBRANCH;
    new->ptr_instr = opcode;
    new->len += 2;
    
    new->op1.type = ASM_OTYPE_IMMEDIATE;
    new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;

    new->op1.imm = 0;
    if (*(opcode + 1) >= 0x80u)
      memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
    memcpy(&new->op1.imm, opcode + 1, 1);
  return (new->len);
}



/*
  <instruction func="op_in_al_dx" opcode="0xec"/>
 */

int	op_in_al_dx(asm_instr *new, u_char *opcode, u_int len, 
			 asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_IN;
  new->op1.type = ASM_OTYPE_FIXED;
    new->ptr_instr = opcode;
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_R8;
  new->op1.base_reg = ASM_REG_AL;

  new->op2.type = ASM_OTYPE_FIXED;
  new->op2.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op2.regset = ASM_REGSET_R16;
  new->op2.base_reg = ASM_REG_DX;
  
  return (new->len);
}


/*
  <instruction func="op_in_eax_dx" opcode="0xed"/>
 */

int	op_in_eax_dx(asm_instr *new, u_char *opcode, u_int len, 
			 asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_IN;
    new->ptr_instr = opcode;
  
  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_R32;
  new->op1.base_reg = ASM_REG_EAX;
  
  new->op2.type = ASM_OTYPE_FIXED;
  new->op2.regset = ASM_REGSET_R16;
  new->op2.base_reg = ASM_REG_DX;

  new->op2.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  
  return (new->len);
}


/*
  <instruction func="op_out_dx_al" opcode="0xee"/>
 */

int	op_out_dx_al(asm_instr *new, u_char *opcode, u_int len, 
			 asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_OUT;
  new->op1.type = ASM_OTYPE_FIXED;
    new->ptr_instr = opcode;
  new->op1.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op1.regset = ASM_REGSET_R16;
  new->op1.base_reg = ASM_REG_DX;

  new->op2.type = ASM_OTYPE_FIXED;
  new->op2.content = ASM_OP_BASE;
  new->op2.regset = ASM_REGSET_R8;
  new->op2.base_reg = ASM_REG_AL;
  
  return (new->len);
}


/*
  <instruction func="op_out_dx_eax" opcode="0xef"/>
 */

int	op_out_dx_eax(asm_instr *new, u_char *opcode, u_int len, 
			 asm_processor *proc) {
  new->len += 1;
    new->ptr_instr = opcode;
  new->instr = ASM_OUT;
  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op1.regset = ASM_REGSET_R16;
  new->op1.base_reg = ASM_REG_DX;

  new->op2.type = ASM_OTYPE_FIXED;
  new->op2.content = ASM_OP_BASE;
  new->op2.regset = ASM_REGSET_R32;
  new->op2.base_reg = ASM_REG_EAX;
  
  return (new->len);
}





