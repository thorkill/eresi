/*
** $Id: op_loopne.c,v 1.5 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_loopne" opcode="0xe0"/>
*/

int op_loopne(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc)
{
  new->instr = ASM_LOOPNE;
  new->type = ASM_TYPE_CONDBRANCH | ASM_TYPE_READFLAG;
  new->ptr_instr = opcode;
  new->len += 1;
  new->flagsread = ASM_FLAG_ZF;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_SHORTJUMP,                                new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_SHORTJUMP,                                new);
#endif

  return (new->len);
}
