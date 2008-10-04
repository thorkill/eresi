/**
 * @file asm_operand_fetch_fixed.c
 * @ingroup operand_handler
 * $Id: asm_operand_fetch_fixed.c,v 1.3 2007/10/14 00:01:41 heroine Exp $
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * @todo
 * Implement optional parameter for fixed operand fetching.
 */
/**
 * @ingroup operand_handler
 * Decode data for operand type ASM_OTYPE_YDEST
 * @param operand Pointer to operand structure to fill.
 * @param opcode Pointer to operand data
 * @param otype
 * @param ins Pointer to instruction structure.
 * @return Operand length
 */

#if WIP
int     asm_operand_fetch_fixed(asm_operand *operand, unsigned char *opcode, int otype, 
				asm_instr *ins, int opt)
#else
int     asm_operand_fetch_fixed(asm_operand *operand, unsigned char *opcode, 
				int otype, asm_instr *ins)
#endif
{
  operand->type = ASM_OTYPE_FIXED;
  #if WIP
  /**
   * @todo extract fields.
  operand->content = asm_fixed_unpack_content();
  operand->regset = asm_fixed_unpack_regset();
  operand->
  */
#endif
  return (0);
}
