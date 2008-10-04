/**
 * @file asm_operand_fetch_immediate.c
 * @ingroup operand_handler
 * $Id: asm_operand_fetch_immediate.c,v 1.4 2007/10/14 00:01:41 heroine Exp $
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 *
 *
 */
/**
 * Decode data for operand type ASM_OTYPE_YDEST
 * @ingroup operand_handler
 * @param operand Pointer to operand structure to fill.
 * @param opcode Pointer to operand data
 * @param otype
 * @param ins Pointer to instruction structure.
 * @return Operand length
 */

#if WIP
int     asm_operand_fetch_immediate(asm_operand *operand, unsigned char *opcode, int otype, 
				asm_instr *ins, int opt)
#else
int     asm_operand_fetch_immediate(asm_operand *operand, unsigned char *opcode, 
				    int otype, asm_instr *ins)
#endif
{
  operand->type = ASM_OTYPE_IMMEDIATE;
  operand->content = ASM_OP_VALUE;
  operand->ptr = opcode;
  operand->imm = 0;
  operand->len = asm_proc_opsize(ins->proc) ? 2 : 4;
  memcpy(&operand->imm, opcode, asm_proc_opsize(ins->proc) ? 2 : 4);
  return (operand->len);
}
