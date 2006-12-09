/*
** private
** 
** Author  : <sk at devhell dot org>
** Started : Tue Feb 18 12:09:31 2003
** Updated : Mon Feb 17 00:34:03 2003
*/

#include <libasm.h>
#include <libasm-int.h>

/*
<i386 func="" opcode="0xc1"/>
*/

int i386_xadd(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {

  new->instr = ASM_XADD;
  new->len += 1;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
 <i386 func="i386_bswap" opcode="0xc8"/>
 <i386 func="i386_bswap" opcode="0xc9"/>
 <i386 func="i386_bswap" opcode="0xca"/>
 <i386 func="i386_bswap" opcode="0xcb"/>
 <i386 func="i386_bswap" opcode="0xcc"/>
 <i386 func="i386_bswap" opcode="0xcd"/>
 <i386 func="i386_bswap" opcode="0xce"/>
 <i386 func="i386_bswap" opcode="0xcf"/>
*/

int i386_bswap(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
	
  struct s_modrm	*modrm;  
  modrm = (struct s_modrm *) opcode;
  
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_BSWAP;
  new->op1.type = ASM_OTYPE_OPMOD;
  new->op1.regset = ASM_REGSET_R32;
  new->op1.content = ASM_OP_BASE;
  new->op1.ptr = opcode;
    
  new->op1.base_reg = modrm->m;
  return (new->len);  
}

