/**
 * @file i386_bsf.c
 * @ingroup handlers_ia32
 * $Id: i386_bsf.c,v 1.8 2007/10/14 00:01:41 heroine Exp $
 *
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for instruction bsf, opcode 0x0f 0xbc
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction.
*/

int i386_bsf(asm_instr *new, unsigned char *opcode, unsigned int len, 
	     asm_processor *proc) 
{
  new->len += 1;
  new->instr = ASM_BSF;

  new->type = ASM_TYPE_BITTEST | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_ZF;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_GENERAL, 
			       new, 0);
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_ENCODED, 
				new, 0);
#else 
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_GENERAL, 
			       new);
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_ENCODED, 
				new);
#endif
  return (new->len);
}
