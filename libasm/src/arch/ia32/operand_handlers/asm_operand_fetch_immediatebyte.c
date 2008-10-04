/**
 * @file asm_operand_fetch_immediatebye.c
 * @ingroup operand_handler
 * $Id: asm_operand_fetch_immediatebyte.c,v 1.4 2007/10/14 00:01:41 heroine Exp $
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler to fetch ASM_OTYPE_IMMEDIATEBYTE operand.
 * @ingroup operand_handler
 * @param operand Pointer to operand structure.
 * @param opcode Pointer to data to disassemble.
 * @param otype Operand type.
 * @param proc Pointer to processor structure.
 * @return Operand Length
 */

#if WIP
int     asm_operand_fetch_immediatebyte(asm_operand *operand, unsigned char *opcode, int otype, 
				asm_instr *ins, int opt)
#else
int     asm_operand_fetch_immediatebyte(asm_operand *operand, unsigned char *opcode, 
					int otype, asm_instr *ins)
#endif
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
