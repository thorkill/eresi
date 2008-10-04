/**
 * @file asm_operand_fetch_pmmx.c
 * $Id: asm_operand_fetch_pmmx.c,v 1.4 2007/10/14 00:01:41 heroine Exp $
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * Fetch ASM_OTYPE_PMMX operand
 *
 *
 */
/**
 * Decode data for operand type ASM_OTYPE_PMMX
 * @ingroup operand_handler
 * @param operand Pointer to operand structure to fill.
 * @param opcode Pointer to operand data
 * @param otype
 * @param ins Pointer to instruction structure.
 * @return Operand length
 */

#if WIP
int     asm_operand_fetch_pmmx(asm_operand *operand, unsigned char *opcode, int otype, 
				asm_instr *ins, int opt)
#else
int     asm_operand_fetch_pmmx(asm_operand *operand, unsigned char *opcode, 
			       int otype, asm_instr *ins)
#endif
{
  int           len;
  operand->type = ASM_OTYPE_PMMX;
  len = operand_rmv(operand, opcode, 4, ins->proc);
  asm_content_pack(operand, operand->content, operand->type);
  operand->regset = ASM_REGSET_MM;
  operand->sbaser = get_reg_intel(operand->baser, operand->regset);
  return (len);
}
