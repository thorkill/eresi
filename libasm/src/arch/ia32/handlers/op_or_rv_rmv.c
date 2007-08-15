/*
** $Id: op_or_rv_rmv.c,v 1.5 2007-08-15 21:30:20 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  Opcode :              0x0b
  Instruction :         OR
*/

int op_or_rv_rmv(asm_instr *new, u_char *opcode, u_int len, 
                 asm_processor *proc)
{
  new->instr = ASM_OR;
  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  new->ptr_instr = opcode;
  new->len += 1;
  new->flagswritten = ASM_FLAG_CF | ASM_FLAG_OF | ASM_FLAG_PF |
                        ASM_FLAG_ZF | ASM_FLAG_SF;

  new->len += asm_operand_fetch(&new->op1, opcode + 1,
                                ASM_OTYPE_GENERAL, new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1,
                                ASM_OTYPE_ENCODED, new);

  return (new->len);
}
