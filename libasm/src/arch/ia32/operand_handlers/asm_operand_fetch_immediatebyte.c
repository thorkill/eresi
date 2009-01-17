/**
 * @file asm_operand_fetch_immediatebye.c
 * @ingroup operand_handler
 * $Id$
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler to fetch ASM_CONTENT_IMMEDIATEBYTE operand.
 * @ingroup operand_handler
 * @param operand Pointer to operand structure.
 * @param opcode Pointer to data to disassemble.
 * @param otype Operand type.
 * @param proc Pointer to processor structure.
 * @return Operand Length
 */

#if WIP
int     asm_operand_fetch_immediatebyte(asm_operand *operand, u_char *opcode, int otype, 
				asm_instr *ins, int opt)
#else
int     asm_operand_fetch_immediatebyte(asm_operand *operand, u_char *opcode, 
					int otype, asm_instr *ins)
#endif
{ 
  u_int	len;

  operand->content = ASM_CONTENT_IMMEDIATE;
  operand->type = ASM_OP_VALUE;
  operand->ptr = opcode;
  operand->imm = 0;
  operand->len = 1;
  if (*opcode >= 0x80)
    {
      len = asm_proc_opsize(ins->proc) ? 2 : 4;
      //memset(&operand->imm, 0xff, 4);
      memset(&operand->imm, 0xff, len);
    }
  memcpy(&operand->imm, opcode, 1);
  return (1);
}
