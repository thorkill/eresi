/**
 * @file op_mov_ref_iv_eax.c
 * @ingroup handlers_ia32
 * $Id: op_mov_ref_iv_eax.c,v 1.7 2007-08-14 06:52:55 strauss Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_mov_ref_iv_eax" opcode="0xa3"/>
*/

int op_mov_ref_iv_eax(asm_instr *new, u_char *opcode, u_int len, 
                      asm_processor *proc)
{
  new->instr = ASM_MOV;
  new->type = ASM_TYPE_ASSIGN;
  new->len += 1;
  new->ptr_instr = opcode;

  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_OFFSET, 
                                new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_GENERAL, 
                                new);

  return (new->len);
}
