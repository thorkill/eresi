/*
** $Id: op_lodsd.c,v 1.5 2007-08-14 06:52:55 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_lodsd" opcode="0xad"/>
*/

int op_lodsd(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->len += 1;
  new->instr = ASM_LODSD;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_LOAD;

  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_YDEST, new);
  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_XSRC, new);

  return (new->len);
}
