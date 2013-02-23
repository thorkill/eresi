/**
 * @file op_lea_rv_m.c
 * @ingroup handlers_ia32
 * $Id$
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 *
 *
  <instruction func="op_lea_rv_m" opcode="0x8d"/>
 */

int op_lea_rv_m(asm_instr *new, u_char *opcode, u_int len,
                asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_LEA;
  new->type = ASM_TYPE_ARITH;

  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_GENERAL, new);
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_ENCODED, new);

  return (new->len);
}
