/**
 * @file asm_operand_fetch.c
 * $Id: asm_operand_fetch_immediatebyte.c,v 1.3 2007-05-29 00:40:28 heroine Exp $
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler to fetch ASM_OTYPE_IMMEDIATEBYTE operand.
 * @param operand Pointer to operand structure.
 * @param opcode Pointer to data to disassemble.
 * @param otype Operand type.
 * @param proc Pointer to processor structure.
 * @return Operand Length
 */

int     asm_operand_fetch_immediatebyte(asm_operand *operand, u_char *opcode, 
					int otype, asm_instr *ins)
{ 
  operand->type = ASM_OTYPE_IMMEDIATE;
  operand->content = ASM_OP_VALUE;
  operand->ptr = opcode;
  operand->imm = 0;
  operand->len = 1;
  if (*opcode >= 0x80)
    memset(&operand->imm, 0xff, 4);
  memcpy(&operand->imm, opcode, 1);
  return (1);
}
