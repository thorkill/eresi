/**
 * @file asm_operand_fetch_pmmx.c
 * $Id$
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * Fetch ASM_CONTENT_PMMX operand
 *
 *
 */
/**
 * Decode data for operand type ASM_CONTENT_PMMX
 * @ingroup operand_handler
 * @param operand Pointer to operand structure to fill.
 * @param opcode Pointer to operand data
 * @param otype
 * @param ins Pointer to instruction structure.
 * @return Operand length
 */

int     asm_operand_fetch_pmmx(asm_operand *operand, u_char *opcode, 
			       int otype, asm_instr *ins)
{
  int len;
  operand->content = ASM_CONTENT_PMMX;
  len = operand_rmv(operand, opcode, ins->proc);
  asm_content_pack(operand, operand->type, operand->content);
  operand->regset = ASM_REGSET_MM;
  operand->sbaser = get_reg_intel(operand->baser, operand->regset);
  return (len);
}
