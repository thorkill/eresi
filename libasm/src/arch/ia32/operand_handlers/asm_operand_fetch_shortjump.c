/**
 * @file asm_operand_fetch.c
 * $Id: asm_operand_fetch_shortjump.c,v 1.2 2007-05-29 00:40:28 heroine Exp $
 */

#include <libasm.h>
#include <libasm-int.h>

/**
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

int     asm_operand_fetch_shortjump(asm_operand *operand, u_char *opcode, 
				    int otype, asm_instr *ins)
{
  operand->type = ASM_OTYPE_JUMP;
  operand->content = ASM_OP_VALUE | ASM_OP_ADDRESS;

  operand->len = 1;
  operand->imm = 0;
  if (*(opcode) >= 0x80u)
    memcpy((char *) &operand->imm + 1, "\xff\xff\xff", 3);
  memcpy(&operand->imm, opcode, 1);
  return (1);
}
