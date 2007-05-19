/**
 * @file asm_operand_fetch.c
 * $Id: asm_operand_fetch_immediate.c,v 1.2 2007-05-19 23:59:12 heroine Exp $
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 *
 *
 */

int     asm_operand_fetch_immediate(asm_operand *operand, u_char *opcode, int otype, 
				    asm_processor *proc)
{
  operand->type = ASM_OTYPE_IMMEDIATE;
  operand->content = ASM_OP_VALUE;
  operand->ptr = opcode;
  operand->imm = 0;
  operand->len = asm_proc_opsize(proc) ? 2 : 4;
  memcpy(&operand->imm, opcode, asm_proc_opsize(proc) ? 2 : 4);
  return (operand->len);
}
