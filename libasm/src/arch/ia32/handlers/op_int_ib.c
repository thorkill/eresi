/*
** $Id: op_int_ib.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_int_ib" opcode="0xcd"/>
*/

int op_int_ib(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc)
{
  new->ptr_instr = opcode;
  new->instr = ASM_INT;
  new->type = ASM_TYPE_INT | ASM_TYPE_TOUCHSP;
  new->len += 1;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1,                                ASM_OTYPE_IMMEDIATEBYTE, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1,                                ASM_OTYPE_IMMEDIATEBYTE, new);
#endif

  return (new->len);
}
