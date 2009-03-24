/**
 * @file asm_operand_fetch_address.c
 * @ingroup operand_handler
 * $Id$
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * Fetch operands of 
 *
 *
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
int     asm_operand_fetch_address(asm_operand *operand, u_char *opcode, 
				  int otype, asm_instr *ins)
{
  u_int	len;

  operand->content = ASM_CONTENT_ADDRESS;
  operand->type = ASM_OPTYPE_IMM;
  operand->ptr = opcode;
  operand->imm = 0;
  len = asm_proc_opsize(ins->proc) ? 2 : 4;
  operand->len = len;
  memcpy(&operand->imm, opcode, len);
  if (len == 2)
    operand->imm = operand->imm & 0xFFFF;
  return (len);
}
