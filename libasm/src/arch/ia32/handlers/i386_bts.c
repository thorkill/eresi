/**
 * @file i386_bts.c
 * @ingroup handlers_ia32
 * $Id: i386_bts.c,v 1.7 2007/10/14 00:01:41 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_bts" opcode="0xab"/>
 */

int i386_bts(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_BTS;

  new->type = ASM_TYPE_BITTEST | ASM_TYPE_BITSET | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_CF;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,				new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,				new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_GENERAL,				new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_GENERAL,				new);
#endif

  return (new->len);
}
