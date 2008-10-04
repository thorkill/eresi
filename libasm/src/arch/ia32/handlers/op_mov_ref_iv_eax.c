/**
 * @file op_mov_ref_iv_eax.c
 * @ingroup handlers_ia32
 * $Id: op_mov_ref_iv_eax.c,v 1.8 2007/10/14 00:01:41 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_mov_ref_iv_eax" opcode="0xa3"/>
*/

int op_mov_ref_iv_eax(asm_instr *new, unsigned char *opcode, unsigned int len,
                      asm_processor *proc)
{
  new->instr = ASM_MOV;
  new->type = ASM_TYPE_ASSIGN;
  new->len += 1;
  new->ptr_instr = opcode;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_OFFSET,                                new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_OFFSET,                                new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_GENERAL,                                new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_GENERAL,                                new);
#endif

  return (new->len);
}
