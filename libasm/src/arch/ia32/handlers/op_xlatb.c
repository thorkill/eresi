/**
 * @file op_xlatb.c
 *
 * @ingroup IA32_instrs
 * @brief <instruction opcode="0xd7" func="op_xlatb"/>
 * $Id$
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief <instruction opcode="0xd7" func="op_xlatb"/>
 */

int op_xlatb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_XLATB;
  new->type = ASM_TYPE_LOAD | ASM_TYPE_ASSIGN;
  return (new->len);
}
