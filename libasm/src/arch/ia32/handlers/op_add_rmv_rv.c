/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
 * Opcode :             0x01
 * Instruction :                ADD
 * Operands:            Destination: encoded vector.
 * Source: is a vector register.
 */

int op_add_rmv_rv(asm_instr *new, u_char *opcode, u_int len,
                  asm_processor *proc)
{
  new->len++;
  new->ptr_instr = opcode;
  new->instr = ASM_ADD;

  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_CF | ASM_FLAG_ZF | ASM_FLAG_PF |
                      ASM_FLAG_OF | ASM_FLAG_AF | ASM_FLAG_SF;

  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODED, new);
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_GENERAL, new);

  return (new->len);
}
