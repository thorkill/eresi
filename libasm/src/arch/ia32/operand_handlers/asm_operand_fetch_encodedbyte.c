/**
 * @file asm_operand_fetch.c
 * $Id: asm_operand_fetch_encodedbyte.c,v 1.3 2007-06-27 11:25:12 heroine Exp $
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * Fetch ASM_OTYPE_ENCODED operand
 *
 *
 */
/**
 * Decode data for operand type ASM_OTYPE_YDEST
 * @param operand Pointer to operand structure to fill.
 * @param opcode Pointer to operand data
 * @param otype
 * @param ins Pointer to instruction structure.
 * @return Operand length
 */

int     asm_operand_fetch_encodedbyte(asm_operand *operand, u_char *opcode, 
				      int otype, asm_instr *ins)
{ 
  int	len;
  
  operand->type = ASM_OTYPE_ENCODED;
  len = operand_rmb(operand, opcode, 5, ins->proc);
  operand->sbaser = get_reg_intel(operand->baser, operand->regset);
  operand->sindex = get_reg_intel(operand->indexr, operand->regset);
  return (len);
}
