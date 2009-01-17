/**
 * @file i386_group16.c
 * @ingroup handlers_ia32
 * $Id$
 *
 * Changelog
 * 2007-05-29: operand type fixed.
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for instruction group 16, opcode 0x0f 0xae
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction
*/

int i386_group16(asm_instr *new, u_char *opcode, u_int len,
		 asm_processor *proc)
{
  struct s_modrm	*modrm;
  if (new->ptr_instr != 0)
    new->ptr_instr = opcode - 1;
  new->len += 1;
  new->instr = ASM_BAD;


  modrm = (struct s_modrm *) opcode + 1;
  switch(modrm->r)
    {
    case 0:
      new->instr = ASM_FXSAVE;
#if WIP
      new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODED,				    new, 0);
#else
      new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODED,				    new);
#endif
      break;
    case 1:
      new->instr = ASM_FXRSTORE;
#if WIP
      new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODED,				    new, 0);
#else
      new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODED,				    new);
#endif
      break;
    case 2:
      new->instr = ASM_LDMXCSR;
#if WIP
      new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODED,				    new, 0);
#else
      new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODED,				    new);
#endif
      break;
    case 3:
      new->instr = ASM_STMXCSR;
#if WIP
      new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODED,				    new, 0);
#else
      new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODED,				    new);
#endif
      break;
    case 4:
      new->instr = ASM_BAD;
      break;
    case 5:
      new->instr = ASM_LFENCE;
      break;
    case 6:
      new->instr = ASM_MFENCE;
      break;
    case 7:
      new->instr = ASM_SFENCE;
      ///CLFUSH
      break;
    }

#if LIBASM_USE_OPERAND_VECTOR
#endif
  return (new->len);
}
