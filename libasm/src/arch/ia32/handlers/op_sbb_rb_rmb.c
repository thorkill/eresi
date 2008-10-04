/**
 * @file op_sbb_rb_rmb.c
 * @ingroup handlers_ia32
** $Id: op_sbb_rb_rmb.c,v 1.5 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_sbb_rb_rmb" opcode="0x1a"/>
*/

int op_sbb_rb_rmb(asm_instr *new, unsigned char *opcode, unsigned int len, 
                  asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_SBB;
  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG | ASM_TYPE_READFLAG;
  new->flagsread = ASM_FLAG_CF;
  new->flagswritten = ASM_FLAG_AF | ASM_FLAG_CF | ASM_FLAG_PF |
                        ASM_FLAG_OF | ASM_FLAG_SF | ASM_FLAG_ZF;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1,
                                ASM_OTYPE_GENERALBYTE, new, 0);
  new->len += asm_operand_fetch(&new->op[1], opcode + 1,
                                ASM_OTYPE_ENCODEDBYTE, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1,
                                ASM_OTYPE_GENERALBYTE, new);
  new->len += asm_operand_fetch(&new->op[1], opcode + 1,
                                ASM_OTYPE_ENCODEDBYTE, new);
#endif
  return (new->len);
}
