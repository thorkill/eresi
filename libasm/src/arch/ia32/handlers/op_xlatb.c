/**
 * @file op_xlatb.c
 * @ingroup handlers_ia32
 * @brief <instruction opcode="0xd7" func="op_xlatb"/>
 * $Id: op_xlatb.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief <instruction opcode="0xd7" func="op_xlatb"/>
 */

int op_xlatb(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_XLATB;
  new->type = ASM_TYPE_LOAD | ASM_TYPE_ASSIGN;
  return (new->len);
}
