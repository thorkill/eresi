/**
 * @file op_stosb.c
 * @ingroup handlers_ia32
** $Id: op_stosb.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_stosb" opcode="0xaa"/>
*/

int op_stosb(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc) 
{
  new->ptr_instr = opcode;
  new->instr = ASM_STOSB;
  new->len += 1;
  new->type = ASM_TYPE_STORE;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_YDEST, new, 0);
  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_OTYPE_XSRC, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_YDEST, new);
  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_OTYPE_XSRC, new);
#endif

  return (new->len);
}
