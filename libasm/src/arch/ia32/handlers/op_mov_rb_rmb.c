/**
 * @file op_mov_rb_rmb.c
ngroup ia-instrs/g
 * @ingroup instrs
 * $Id$
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for opcode 0x8a : instruction op_mov_rb_rmb
  <instruction func="op_mov_rb_rmb" opcode="0x8a"/>
*/

int op_mov_rb_rmb(asm_instr *new, u_char *opcode, u_int len,
		  asm_processor *proc)
{
  new->len += 1;
  new->type = ASM_TYPE_ASSIGN;
  new->instr = ASM_MOV;
  new->ptr_instr = opcode;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_GENERALBYTE,                                new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_GENERALBYTE,                                new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_ENCODEDBYTE,                                new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_ENCODEDBYTE,                                new);
#endif

  return (new->len);
}
