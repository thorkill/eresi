/*
** $Id: op_movsd.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_movsd" opcode="0xa5"/>
*/

int op_movsd(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  if (asm_proc_opsize(proc))
    new->instr = ASM_MOVSW;
  else
    new->instr = ASM_MOVSD;

  new->type = ASM_TYPE_LOAD | ASM_TYPE_STORE | ASM_TYPE_ASSIGN;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_YDEST, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_YDEST, new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_OTYPE_XSRC, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_OTYPE_XSRC, new);
#endif

  return (new->len);
}
