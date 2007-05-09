/**
 * @file asm_operand_fetch.c
 * $Id: asm_operand_fetch_address.c,v 1.1 2007-05-09 17:39:35 heroine Exp $
 */

#include <libasm.h>
#include <libasm-int.h>

int     asm_operand_fetch_address(asm_operand *operand, u_char *opcode, int otype, 
                                  asm_processor *proc)
{
  operand->type = ASM_OTYPE_ADDRESS;
  operand->content = ASM_OP_VALUE;
  operand->ptr = opcode;
  operand->imm = 0;
  operand->len = 4;
  memcpy(&operand->imm, opcode, 4);
  return (4);
}
