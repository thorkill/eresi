/**
 * @file op_lea_rv_m.c
 * @ingroup handlers_ia32
 * $Id: op_lea_rv_m.c,v 1.8 2007/10/14 00:01:41 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 *
 *
  <instruction func="op_lea_rv_m" opcode="0x8d"/>
 */

int op_lea_rv_m(asm_instr *new, unsigned char *opcode, unsigned int len,
                asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_LEA;
  new->type = ASM_TYPE_ARITH;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_GENERAL,                                new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_GENERAL,                                new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_ENCODED,                                new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_ENCODED,                                new);
#endif

  return (new->len);
}
