/**
 * @file op_mov_rmb_rb.c
 * @ingroup handlers_ia32
 * $Id$
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_mov_rmb_rb" opcode="0x88"/>
*/

int op_mov_rmb_rb(asm_instr *new, u_char *opcode, u_int len,
                  asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_ASSIGN;
  new->instr = ASM_MOV;

  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODEDBYTE, new);
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_GENERALBYTE, new);

  return (new->len);
}
