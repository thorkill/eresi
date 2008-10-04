/**
 * @file op_xor_al_ib.c
 * @ingroup handlers_ia32
 * @brief Handler for instruction xor al,ib opcode 0x34
 * 
 * $Id: op_xor_al_ib.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief Handler for instruction xor al,ib opcode 0x34
 * @param instr Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Return length of instruction.
 */

int op_xor_al_ib(asm_instr *instr, unsigned char *opcode, unsigned int len, asm_processor *proc) {
  instr->instr = ASM_XOR;
  instr->len += 1;
  instr->ptr_instr = opcode;
  instr->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  instr->flagswritten = ASM_FLAG_CF | ASM_FLAG_OF | ASM_FLAG_PF |
    ASM_FLAG_ZF | ASM_FLAG_SF;

#if WIP
  instr->len += asm_operand_fetch(&instr->op[0], opcode, ASM_OTYPE_FIXED, instr,
				  asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_AL,
						 ASM_REGSET_R8));
  instr->len += asm_operand_fetch(&instr->op[1], opcode + 1,
				  ASM_OTYPE_IMMEDIATEBYTE, instr, 0);
#else
  instr->len += asm_operand_fetch(&instr->op[0], opcode, ASM_OTYPE_FIXED, instr);
  instr->op[0].ptr = opcode;
  instr->op[0].len = 0;
  instr->op[0].baser = ASM_REG_AL;
  instr->op[0].regset = ASM_REGSET_R8;
  instr->len += asm_operand_fetch(&instr->op[1], opcode + 1,
				  ASM_OTYPE_IMMEDIATEBYTE, instr);
#endif

  return (instr->len);
}
