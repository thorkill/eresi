/*
** $Id: i386_bswap.c,v 1.5 2007-06-27 11:25:11 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

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

int i386_bswap(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  
  struct s_modrm        *modrm;
  modrm = (struct s_modrm *) opcode;
  
  new->len += 1;
  
  new->ptr_instr = opcode;
  new->instr = ASM_BSWAP;

#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_OPMOD, new);
#else
  new->op1.type = ASM_OTYPE_OPMOD;
  new->op1.regset = ASM_REGSET_R32;
  new->op1.content = ASM_OP_BASE;
  new->op1.ptr = opcode;
  
  new->op1.baser = modrm->m;
#endif
  return (new->len);
}
