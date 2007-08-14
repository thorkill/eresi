/**
 * @file op_lea_rv_m
 * @ingroup handlers_ia32
 * $Id: op_lea_rv_m.c,v 1.7 2007-08-14 06:52:55 strauss Exp $
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

  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_GENERAL, 
                                new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_ENCODED, 
                                new);

  return (new->len);
}
