/**
 * @file asm_operand_fetch.c
 * $Id: asm_operand_fetch_immediatebyte.c,v 1.1 2007-05-09 17:39:35 heroine Exp $
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 *
 *
 */

int     asm_operand_fetch_immediatebyte(asm_operand *operand, u_char *opcode, int otype,
					asm_processor *proc)
{ 
  operand->type = ASM_OTYPE_IMMEDIATE;
  operand->content = ASM_OP_VALUE;
  operand->ptr = opcode;
  operand->imm = 0;
  operand->len = 1;
  //
  // Add sign support : putting 0xffffff if byte < 0
  //
  if (*opcode >= 0x80)
    memset(&operand->imm, 0xff, 4);
  memcpy(&operand->imm, opcode, 1);
  return (1);
}
