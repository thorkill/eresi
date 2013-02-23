/**
 * @file op_xor_eax_iv.c
 * @ingroup handlers_ia32
 * @brief Handler for instruction xor eax,iv opcode 0x35
 * $Id$
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief Handler for instruction xor eax,iv opcode 0x35
 * @param instr Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction.
 */


int op_xor_eax_iv(asm_instr *new, u_char *opcode, u_int len, 
                  asm_processor *proc)
{
  new->instr = ASM_XOR;
  new->ptr_instr = opcode;
  new->len += 1;
  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_CF | ASM_FLAG_OF | ASM_FLAG_PF |
                        ASM_FLAG_ZF | ASM_FLAG_SF;

  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_FIXED, new);
  new->op[0].ptr = opcode;
  new->op[0].len = 0;
  new->op[0].baser = ASM_REG_EAX;
  new->op[0].regset = asm_proc_is_protected(proc) ? ASM_REGSET_R32 : ASM_REGSET_R16;\

  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_IMMEDIATE, new);

  return (new->len);
}

