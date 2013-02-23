/**
 * @file asm_operand_fetch_shortjump.c
 * @ingroup operand_handler
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief Decode data for operand type ASM_CONTENT_SHORTJUMP
 * @param operand Pointer to operand structure to fill.
 * @param opcode Pointer to operand data
 * @param otype
 * @param ins Pointer to instruction structure.
 * @return Operand length
 */
int     asm_operand_fetch_shortjump(asm_operand *operand, u_char *opcode, 
				    int otype, asm_instr *ins)
{
  u_int	len;

  operand->type = ASM_OPTYPE_MEM;
  operand->memtype = ASM_OP_VALUE |ASM_OP_ADDRESS;
  operand->content = ASM_CONTENT_JUMP;
  operand->len = 1;
  operand->imm = 0;

  len = asm_proc_opsize(ins->proc) ? 1 : 3;
  if (*(opcode) >= 0x80u)
    memset((char *) &operand->imm + 1, 0xFF, len);

  memcpy(&operand->imm, opcode, 1);
  return (1);
}
