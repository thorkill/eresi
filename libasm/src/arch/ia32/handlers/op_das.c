/*
** $Id: op_das.c,v 1.4 2007/08/14 06:52:55 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_das" opcode="0x2f"/>
 */

int     op_das(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc)
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_DAS;
  new->type = ASM_TYPE_ARITH | ASM_TYPE_READFLAG | ASM_TYPE_WRITEFLAG;
  new->flagsread = ASM_FLAG_AF | ASM_FLAG_CF;
  new->flagswritten = ASM_FLAG_AF | ASM_FLAG_CF | ASM_FLAG_PF | ASM_FLAG_SF | ASM_FLAG_ZF;
  return (new->len);
}
