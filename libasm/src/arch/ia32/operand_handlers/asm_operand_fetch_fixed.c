/**
 * @file asm_operand_fetch_fixed.c
 * @ingroup libasm/ia32/operands/handlers
 * $Id$
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * @todo
 * Implement optional parameter for fixed operand fetching.
 */
/**
 * @ingroup libasm/ia32/operands/handlers
 * Decode data for operand type ASM_OTYPE_YDEST
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
