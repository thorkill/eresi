/*
** this is private - do not distribute
** 
** Author  : <sk at devhell dot org>
** Started : Xxx Xxx xx xx:xx:xx 2002
** Updated : Sun Nov 30 06:52:38 2003
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_pusha" opcode="0x60"/>
*/

int op_pusha(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->len += 1;
    new->ptr_instr = opcode;
    new->instr = ASM_PUSHA;
  return (new->len);
}

/*
  <instruction func="op_popa" opcode="0x61"/>
*/

int op_popa(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->len += 1;
    new->ptr_instr = opcode;
    new->instr = ASM_POPA;
    return (new->len);
}


/*
  <instruction func="op_bound_gv_ma" opcode="0x62"/>
*/

int	op_bound_gv_ma(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;
  
  new->instr = ASM_BOUND;
  new->len += 1;
  new->ptr_instr = opcode;

  modrm = (struct s_modrm *) (opcode + 1);
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_MEMORY;
  operand_rmv(&new->op1, opcode + 1, len - 1, proc);
    
  new->op2.content = ASM_OP_BASE;
  new->op2.regset = ASM_REGSET_R32;
  new->op2.base_reg = modrm->r;
  new->len += new->op1.len;
  return (new->len);
}

/*
  <instruction func="op_arpl_ew_rw" opcode="0x63"/>
*/

int	op_arpl_ew_rw(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->instr = ASM_ARPL;
  new->len += 1;
  
  /*
    new->op1 = &new->op1;
  new->op1.type = ;
  new->op1.
  
  new->op2 = &new->op2;
  new->op2.content = ;
  new->op2.len = ;
  new->op2.ptr = ;
  */
  return (new->len);
}

/*
  <instruction func="op_prefix_fs" opcode="0x64"/>
 */

int	op_prefix_fs(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->prefix |= ASM_PREFIX_FS;
  if (!new->ptr_prefix)
    new->ptr_prefix = opcode;
  new->len += 1;
  return (proc->fetch(new, opcode + 1, len - 1, proc));
}

/*
  <instruction func="op_prefix_gs" opcode="0x65"/>
 */

int	op_prefix_gs(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  if (!new->ptr_prefix)
    new->ptr_prefix = opcode;
  new->prefix |= ASM_PREFIX_GS;
  new->len += 1;
  return (proc->fetch(new, opcode + 1, len - 1, proc));
}

/*
  <instruction func="op_oplen" opcode="0x66"/>
 */

int	op_oplen(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  asm_i386_processor	*i386p;

  if (!new->ptr_prefix)
    new->ptr_prefix = opcode;
  i386p = (asm_i386_processor *) proc;

  i386p->internals->oplen = !i386p->internals->oplen;
  new->len += 1;
  new->prefix |= ASM_PREFIX_OPSIZE;
  len = proc->fetch(new, opcode + 1, len - 1, proc);
  i386p->internals->oplen = !i386p->internals->oplen;
  return (len);
}

int	op_addsize(asm_instr *new, u_char *opcode, u_int len, 
		   asm_processor *proc) {
  asm_i386_processor	*i386p;
  
  if (!new->ptr_prefix)
    new->ptr_prefix = opcode;
  i386p = (asm_i386_processor *) proc;
  new->prefix |= ASM_PREFIX_ADDSIZE;
    i386p->internals->addlen = !i386p->internals->addlen;
  len = proc->fetch(new, opcode + 1, len - 1, proc);
  i386p->internals->addlen = !i386p->internals->addlen;
  return (len);
}

/*
  <instruction func="op_push_iv" opcode="0x68"/>
*/

int op_push_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_PUSH;
  new->len += 5;
  new->ptr_instr = opcode;

  new->op1.type = ASM_OTYPE_IMMEDIATE;
  new->op1.content = ASM_OP_VALUE;
  new->op1.ptr = opcode + 1;
  new->op1.len = 4;

  memcpy((char *) &new->op1.imm, opcode + 1, 4);
    
  return (new->len);
}


/*
  <instruction func="op_imul_rv_rmv_iv" opcode="0x69"/>
 */

int	op_imul_rv_rmv_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {

  new->instr = ASM_IMUL;
  new->ptr_instr = opcode;
  new->len += 1;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  
  new->op3.type = ASM_OTYPE_IMMEDIATE;
  new->op3.ptr = opcode + new->len;
  new->op3.content = ASM_OP_VALUE;
  memcpy(&new->op3.imm, new->op3.ptr, 4);
  new->op3.len = 4;
  new->len += 4;
  return (new->len);
}

/*
  <instruction func="op_push_ib" opcode="0x6a"/>
*/

int op_push_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_PUSH;
  new->len += 1;
  new->ptr_instr = opcode;

  new->op1.type = ASM_OTYPE_IMMEDIATE;
  new->op1.content = ASM_OP_VALUE;
  new->op1.ptr = opcode + 1;
  new->op1.len = 1;
    
  new->op1.imm = 0;
  if (*(opcode + 1) > 0x80u)
    memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
  memcpy(&new->op1.imm, opcode + 1, 1);
  new->len += 1;
  return (new->len);
}

/*
  <instruction func="op_imul_gv_ev_ib" opcode="0x6b"/>
*/

int	op_imul_gv_ev_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {

  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_IMUL;
  
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  
  new->op3.type = ASM_OTYPE_IMMEDIATE;
  new->op3.content = ASM_OP_VALUE;
  new->op3.ptr = opcode + 1 + new->op1.len + new->op2.len;
  new->op3.len = 1;
  
  new->len += 1;
  if (*new->op3.ptr >= 0x80)
    memcpy((char *) &new->op3.imm + 1, "\xff\xff\xff", 3);
  else
    new->op3.imm = 0;
  memcpy(&new->op3, new->op3.ptr, 1);
  
  return (new->len);
}

/*
  <instruction func="op_outsb" opcode="0x6e"/>
*/

int op_outsb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_OUTSB;
  new->ptr_instr = opcode;

  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.regset = ASM_REGSET_R16;
  new->op1.base_reg = ASM_REG_EDX;

  new->op2.type = ASM_OTYPE_XSRC;
  new->op2.regset = ASM_REGSET_R32;
  new->op2.prefix = ASM_PREFIX_DS;
  new->op2.base_reg = ASM_REG_ESI;
  return (new->len);
} 

/*

 */

int op_outsw(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
  if (!asm_proc_oplen(proc))
    new->instr = ASM_OUTSW;
  else
    new->instr = ASM_OUTSD;

  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_R16;
  new->op1.base_reg = ASM_REG_DX;

  new->op2.type = ASM_OTYPE_XSRC;
  new->op2.content = ASM_OP_BASE;
  new->op2.base_reg = ASM_REG_EDI;
  new->op2.regset = asm_proc_oplen(proc) ? ASM_REGSET_R32 : 
    ASM_REGSET_R16;
  return (new->len);
}
