/**
 * @file op_mov_rmv_iv.c
 * @ingroup handlers_ia32
 * $Id: op_mov_rmv_iv.c,v 1.7 2007/10/14 00:01:41 heroine Exp $
 * ChangeLog:
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for opcode 0xc7
 * @param new
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction.
 */

int op_mov_rmv_iv(asm_instr *new, unsigned char *opcode, unsigned int len,
                  asm_processor *proc)
{
  int olen;
  new->len += 1;
  new->type = ASM_TYPE_ASSIGN;
  new->ptr_instr = opcode;
  new->instr = ASM_MOV;

#if WIP
  new->len += (olen = asm_operand_fetch(&new->op[0], opcode + 1,                                        ASM_OTYPE_ENCODED, new, 0));
#else
  new->len += (olen = asm_operand_fetch(&new->op[0], opcode + 1,                                        ASM_OTYPE_ENCODED, new));
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1 + olen,                                ASM_OTYPE_IMMEDIATE, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1 + olen,                                ASM_OTYPE_IMMEDIATE, new);
#endif

  if (asm_instruction_is_prefixed(new, ASM_PREFIX_OPSIZE))
  {
    if (asm_operand_is_reference(&new->op[0]))
    {
      new->instr = ASM_MOVW;
    }
  }
  else
    new->instr = ASM_MOV;

 return (new->len);
}
