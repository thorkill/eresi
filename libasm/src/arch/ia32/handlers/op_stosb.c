/**
 * @file op_stosb.c
 * @ingroup handlers_ia32
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_stosb" opcode="0xaa"/>
*/

int op_stosb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->ptr_instr = opcode;
  new->instr = ASM_STOSB;
  new->len += 1;
  new->type = ASM_TYPE_STORE;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_YDEST, new, 0);
  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_CONTENT_XSRC, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_YDEST, new);
  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_CONTENT_XSRC, new);
#endif

  return (new->len);
}
