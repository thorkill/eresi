/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
 * Opcode :             0x00
 * ADD
 * Destination is an encoded byte.
 * Source is a byte register.
 */

int op_add_rmb_rb(asm_instr *new, u_char *opcode, u_int len,
                  asm_processor *proc)
{
  new->instr = ASM_ADD;
  new->ptr_instr = opcode;
  new->len++;

  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_CF | ASM_FLAG_ZF | ASM_FLAG_PF |
                      ASM_FLAG_OF | ASM_FLAG_AF | ASM_FLAG_SF;

  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODEDBYTE, new);
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_GENERALBYTE, new);

  return (new->len);
}
