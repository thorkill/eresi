/**
** @file asm_mips_operand_noop.c
** @ingroup m-operands
*/
/*
 * - Adam 'pi3' Zabrocki
 *
 */

#include <libasm.h>

void    asm_mips_operand_noop(asm_operand *op, u_char *opcode, int otype,
                                          asm_instr *ins)
{
   op->type = ASM_MIPS_OTYPE_NOOP;
}
