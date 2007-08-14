/*
** $Id: op_cmpsd.c,v 1.7 2007-08-14 06:52:55 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_cmpsd" opcode="0xa7"/>
*/

int op_cmpsd(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_CMPSD;

  new->type = ASM_TYPE_COMPARISON | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_AF | ASM_FLAG_CF | ASM_FLAG_PF |
                        ASM_FLAG_OF | ASM_FLAG_SF | ASM_FLAG_ZF;

  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_XSRC, new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_YDEST, new);

  return (new->len);
}
