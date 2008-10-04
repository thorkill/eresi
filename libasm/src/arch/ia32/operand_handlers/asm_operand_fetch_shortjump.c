/**
 * @file asm_operand_fetch_shortjump.c
 * @ingroup operand_handler
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief Decode data for operand type ASM_OTYPE_SHORTJUMP
 * @param operand Pointer to operand structure to fill.
 * @param opcode Pointer to operand data
 * @param otype
 * @param ins Pointer to instruction structure.
 * @return Operand length
 */
#if WIP
int     asm_operand_fetch_shortjump(asm_operand *operand, unsigned char *opcode, 
				    int otype, asm_instr *ins, int opt)
#else
int     asm_operand_fetch_shortjump(asm_operand *operand, unsigned char *opcode, 
				    int otype, asm_instr *ins)
#endif
{
  
#if WIP
  asm_content_pack(operand, ASM_OP_VALUE | ASM_OP_ADDRESS, ASM_OTYPE_JUMP);
#else
  operand->content = ASM_OP_VALUE |ASM_OP_ADDRESS;
  operand->type = ASM_OTYPE_JUMP;
#endif
  operand->len = 1;
  operand->imm = 0;
  if (*(opcode) >= 0x80u)
    memcpy((char *) &operand->imm + 1, "\xff\xff\xff", 3);
  memcpy(&operand->imm, opcode, 1);
  return (1);
}
