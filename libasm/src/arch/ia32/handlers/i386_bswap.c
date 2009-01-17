/*
** $Id$
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

  new->type = ASM_TYPE_OTHER;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_OPMOD, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_OPMOD, new);
#endif

  return (new->len);
}
