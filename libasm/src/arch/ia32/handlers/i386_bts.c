/**
 * @file i386_bts.c
 * @ingroup handlers_ia32
 * $Id: i386_bts.c,v 1.6 2007-07-18 15:47:10 strauss Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_bts" opcode="0xab"/>
 */

int i386_bts(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_BTS;

  new->type = ASM_TYPE_BITTEST | ASM_TYPE_BITSET | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_CF;

  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODED, 
				new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_GENERAL, 
				new);    

  return (new->len);
}
