/**
 * @file i386_shrd_rmv_rv_ib.c
 * @ingroup handlers_ia32
 * $Id$
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for instruction shrd, opcode 0x0f 0xac
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction.
*/

int i386_shrd_rmv_rv_ib(asm_instr *new, u_char *opcode, u_int len,
			asm_processor *proc)
{
  int		olen;
  new->instr = ASM_SHRD;
  new->len += 1;
#if LIBASM_USE_OPERAND_VECTOR
#if WIP
  new->len += (olen = asm_operand_fetch(&new->op[0], opcode + 1,					ASM_CONTENT_ENCODED, new, 0));
#else
  new->len += (olen = asm_operand_fetch(&new->op[0], opcode + 1,					ASM_CONTENT_ENCODED, new));
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1,				ASM_CONTENT_GENERAL, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1,				ASM_CONTENT_GENERAL, new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[2], opcode + 1 + olen,				ASM_CONTENT_IMMEDIATEBYTE, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[2], opcode + 1 + olen,				ASM_CONTENT_IMMEDIATEBYTE, new);
#endif
#else
    new->op[0].content = ASM_CONTENT_ENCODED;
  new->op[0].size = ASM_OSIZE_VECTOR;
  new->op[1].content = ASM_CONTENT_GENERAL;
  new->op[1].size = ASM_OSIZE_VECTOR;
  new->op[2].content = ASM_CONTENT_IMMEDIATE;
  new->op[2].size = ASM_OSIZE_BYTE;
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
  new->op[2].content = ASM_CONTENT_IMMEDIATE;
  new->op[2].type = ASM_OP_VALUE;
  new->op[2].ptr = opcode + 2;
  new->op[2].len = 1;

  new->op[2].imm = 0;
  memcpy(&new->op[2].imm, opcode + 2, 1);
#endif
  return (new->len);
}
