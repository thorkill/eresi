/**
 * @file asm_operand_fetch.c
 * $Id: asm_operand_fetch_shortjump.c,v 1.1 2007-05-09 17:39:35 heroine Exp $
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 *
 *
 */

int     asm_operand_fetch_shortjump(asm_operand *operand, u_char *opcode, int otype, 
				    asm_processor *proc)
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
