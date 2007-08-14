/*
** $Id: op_jmp_iv.c,v 1.4 2007-08-14 06:52:55 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_jmp_iv" opcode="0xe9"/>
 */

int op_jmp_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->instr = ASM_BRANCH;
  new->type = ASM_TYPE_IMPBRANCH;
  new->ptr_instr = opcode;
  new->len += 1;

  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_JUMP, new);

  return (new->len);
}
