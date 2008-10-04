/**
 * @file op_mov_rb_rmb.c
 * @ingroup handlers_ia32
 * $Id: op_mov_rb_rmb.c,v 1.7 2007/10/14 00:01:41 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for opcode 0x8a : instruction op_mov_rb_rmb
  <instruction func="op_mov_rb_rmb" opcode="0x8a"/>
*/

int op_mov_rb_rmb(asm_instr *new, unsigned char *opcode, unsigned int len,
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
