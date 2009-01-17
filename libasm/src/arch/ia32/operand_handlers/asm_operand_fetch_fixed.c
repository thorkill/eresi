/**
 * @file asm_operand_fetch_fixed.c
 * @ingroup operand_handler
 * $Id$
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * @todo
 * Implement optional parameter for fixed operand fetching.
 */
/**
 * @ingroup operand_handler
 * Decode data for operand type ASM_CONTENT_YDEST
 * @param operand Pointer to operand structure to fill.
 * @param opcode Pointer to operand data
 * @param otype
 * @param ins Pointer to instruction structure.
 * @return Operand length
 */

#if WIP
int     asm_operand_fetch_fixed(asm_operand *operand, u_char *opcode, int otype, 
				asm_instr *ins, int opt)
#else
int     asm_operand_fetch_fixed(asm_operand *operand, u_char *opcode, 
				int otype, asm_instr *ins)
#endif
{
  operand->content = ASM_CONTENT_FIXED;
  #if WIP
  /**
   * @todo extract fields.
  operand->type = asm_fixed_unpack_content();
  operand->regset = asm_fixed_unpack_regset();
  operand->
  */
#endif
  return (0);
}
