/**
 * @file op_unary_rmb.c
 * @ingroup handlers_ia32
 * @brief Handler for instruction unary rmb opcode 0xf6
 * $Id$
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief Handler for instruction unary rmb opcode 0xf6
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction.
 */

int op_unary_rmb(asm_instr *new, u_char *opcode, u_int len, 
                 asm_processor *proc)
{
  struct s_modrm        *modrm;
  int                    olen;

  new->ptr_instr = opcode;
  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
  new->type = ASM_TYPE_ARITH;

  switch(modrm->r) {
    case 0:
      new->instr = ASM_TEST;
      new->type = ASM_TYPE_COMPARISON | ASM_TYPE_WRITEFLAG;
      new->flagswritten = ASM_FLAG_OF | ASM_FLAG_CF | ASM_FLAG_PF |
                          ASM_FLAG_SF | ASM_FLAG_ZF;
      new->op[0].content = ASM_CONTENT_ENCODED;

      new->len += (olen = asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODEDBYTE, new));
      new->len += asm_operand_fetch(&new->op[1], opcode + 1 + olen, ASM_CONTENT_IMMEDIATEBYTE, new);

      break;
    case 1:
      return (0);
      break;
    case 2:
      new->instr = ASM_NOT;
      break;
    case 3:
      new->instr = ASM_NEG;
      new->type |= ASM_TYPE_WRITEFLAG;
      new->flagswritten = ASM_FLAG_CF;
      break;
    case 4:
      new->instr = ASM_MUL;
      new->type |= ASM_TYPE_WRITEFLAG;
      new->flagswritten = ASM_FLAG_OF | ASM_FLAG_CF;
      break;
    case 5:
      new->instr = ASM_IMUL;
      new->type |= ASM_TYPE_WRITEFLAG;
      new->flagswritten = ASM_FLAG_OF | ASM_FLAG_CF;
      break;
    case 6:
      new->instr = ASM_DIV;
      break;
    case 7:
      new->instr = ASM_IDIV;
      break;
    default:
      break;
  }

  if (!new->op[0].content)
  {
    new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODEDBYTE, new);
  }
  return (new->len);
}
