/**
 *
 * @file op_mov_rmb_ib.c
 * @ingroup handlers_ia32
 * $Id$
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 *
 *
 <instruction func="op_mov_rmb_ib" opcode="0xc6"/>
*/

int op_mov_rmb_ib(asm_instr *new, u_char *opcode, u_int len,
                  asm_processor *proc)
{
  int olen;
  new->type = ASM_TYPE_ASSIGN;
  new->len += 1;
  new->instr = ASM_MOV;
  new->ptr_instr = opcode;

  new->len += (olen = asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODEDBYTE, new));
  new->len += asm_operand_fetch(&new->op[1], opcode + 1 + olen , ASM_CONTENT_IMMEDIATEBYTE, new);

  return (new->len);
}
