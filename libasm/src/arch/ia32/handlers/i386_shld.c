/**
 * @file i386_shld.c
 * @ingroup handlers_ia32
 * $Id$
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for instruction shld, opcode 0x0f 0xa4
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction.
*/

int i386_shld(asm_instr *new, u_char *opcode, u_int len,
	      asm_processor *proc)
{
  struct s_modrm        *modrm;
  new->len += 1;

  modrm = (struct s_modrm *) opcode + 1;
  new->instr = ASM_SHLD;

#if LIBASM_USE_OPERAND_VECTOR
#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1,				ASM_CONTENT_REGISTER, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1,				ASM_CONTENT_REGISTER, new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1,				ASM_CONTENT_GENERAL, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1,				ASM_CONTENT_GENERAL, new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[2], opcode + 2,				ASM_CONTENT_IMMEDIATEBYTE, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[2], opcode + 2,				ASM_CONTENT_IMMEDIATEBYTE, new);
#endif
  new->len += 1;
#else

  new->op[0].content = ASM_CONTENT_REGISTER;
  new->op[0].regset = ASM_REGSET_R32;
  new->op[0].type = ASM_OP_BASE;
  new->op[0].ptr = opcode + 1;
  new->op[0].len = 0;
  new->op[0].baser = modrm->m;

  new->op[1].content = ASM_CONTENT_GENERAL;
  new->op[1].regset = ASM_REGSET_R32;
  new->op[1].type = ASM_OP_BASE;
  new->op[1].ptr = opcode + 1;
  new->op[1].len = 1;
  new->op[1].baser = modrm->r;

  new->op[2].content = ASM_CONTENT_IMMEDIATE;
  new->op[2].type = ASM_OP_VALUE;
  new->op[2].ptr = opcode + 2;
  new->op[2].len = 1;
  new->op[2].imm = 0;
  memcpy(&new->op[2].imm, opcode + 2, 1);
  new->len += new->op[0].len + new->op[1].len + new->op[2].len;
#endif
  return (new->len);
}
