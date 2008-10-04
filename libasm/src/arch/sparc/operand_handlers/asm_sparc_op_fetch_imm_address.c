#include <libasm.h>
#include <libasm-sparc.h>

/**
 * Decode data for operand content ASM_SP_OTYPE_IMM_ADDRESS
 * @param operand Pointer to operand structure to fill.
 * @param opcode Pointer to opcode data.
 * @param otype Operand content.
 * @param ins Pointer to instruction structure.
 *
 * @return Operand length (currently unused in libasm-sparc)
 */

int asm_sparc_op_fetch_imm_address(asm_operand *operand, unsigned char *opcode, 
                         				   int otype, asm_instr *ins)
{
  operand->content = ASM_SP_OTYPE_IMM_ADDRESS;
  operand->type    = ASM_OPTYPE_MEM;
  return (0);
}
