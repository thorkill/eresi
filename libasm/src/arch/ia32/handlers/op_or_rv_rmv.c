/*
** $Id: op_or_rv_rmv.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  Opcode :              0x0b
  Instruction :         OR
*/

int op_or_rv_rmv(asm_instr *new, unsigned char *opcode, unsigned int len,
                 asm_processor *proc)
{
  new->instr = ASM_OR;
  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  new->ptr_instr = opcode;
  new->len += 1;
  new->flagswritten = ASM_FLAG_CF | ASM_FLAG_OF | ASM_FLAG_PF |
                        ASM_FLAG_ZF | ASM_FLAG_SF;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1,                                ASM_OTYPE_GENERAL, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1,                                ASM_OTYPE_GENERAL, new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1,                                ASM_OTYPE_ENCODED, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1,                                ASM_OTYPE_ENCODED, new);
#endif

  return (new->len);
}
