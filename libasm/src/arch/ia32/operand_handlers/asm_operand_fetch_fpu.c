/**
 * @file asm_operand_fetch_fpu.c
 * @ingroup operand_handler
 * $Id: asm_operand_fetch_fpu.c 1319 2009-03-24 22:48:58Z strauss $
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * @todo
 * Implement optional parameter for FPU operand fetching.
 */
/**
 * @ingroup operand_handler
 * Decode data for operand type ASM_CONTENT_FPU
 * @param operand Pointer to operand structure to fill.
 * @param opcode Pointer to operand data
 * @param otype
 * @param ins Pointer to instruction structure.
 * @return Operand length
 */

int     asm_operand_fetch_fpu(asm_operand *operand, u_char *opcode, 
				int otype, asm_instr *ins)
{
  operand->content = ASM_CONTENT_FPU;
  return (0);
}
