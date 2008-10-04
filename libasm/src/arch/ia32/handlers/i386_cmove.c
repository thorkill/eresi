/**
 * @file i386_cmove.c
 * @ingroup handlers_ia32
 * $Id: i386_cmove.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_cmove" opcode="0x44"/>
*/


int i386_cmove(asm_instr *new, unsigned char *opcode, unsigned int len,
	       asm_processor *proc)
{
  new->len += 1;
  new->instr = ASM_CMOVE;

#if LIBASM_USE_OPERAND_VECTOR
#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_GENERAL,				new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_GENERAL,				new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_ENCODED,				new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_ENCODED,				new);
#endif
#else
  new->op[0].type = ASM_OTYPE_GENERAL;
  new->op[1].type = ASM_OTYPE_ENCODED;
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  #endif
  return (new->len);
}

