/**
 * @file asm_operand_fetch_encoded.c
 * @ingroup operand_handler
 * $Id: asm_operand_fetch_encoded.c,v 1.4 2007/10/14 00:01:41 heroine Exp $
 */

#include <libasm.h>
#include <libasm-int.h>


/**
 * @ingroup operand_handler
 * Decode data for operand type ASM_OTYPE_ENCODED
 * @param operand Pointer to operand structure to fill.
 * @param opcode Pointer to operand data
 * @param otype
 * @param ins Pointer to instruction structure.
 * @return Operand length
 */

#if WIP
int     asm_operand_fetch_encoded(asm_operand *operand, unsigned char *opcode, int otype, 
				asm_instr *ins, int opt)
#else
int     asm_operand_fetch_encoded(asm_operand *operand, unsigned char *opcode, 
				  int otype, asm_instr *ins)
#endif
{
  int	len;
  operand->type = ASM_OTYPE_ENCODED;
  len = operand_rmv(operand, opcode, 4, ins->proc);
  operand->sbaser = get_reg_intel(operand->baser, operand->regset);
  operand->sindex = get_reg_intel(operand->indexr, operand->regset);
  return (len);
}
