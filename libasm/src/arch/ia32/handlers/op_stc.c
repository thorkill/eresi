/**
 * @file op_stc.c
 * @ingroup handlers_ia32
** $Id: op_stc.c,v 1.5 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief
 * <instruction func="op_stc" opcode="0xf9"/>
*/

int op_stc(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_STC;
  new->type = ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_CF;
  return (new->len);
}
