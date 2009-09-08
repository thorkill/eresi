/**
 * @file op_lahf.c
ngroup ia-instrs/g
 * @ingroup instrs
 * $Id$
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for lahf instruction opcode 0x9f
 *
  <instruction func="op_lahf" opcode="0x9f"/>
*/

int op_lahf(asm_instr *new, u_char *opcode, u_int len,
            asm_processor *proc)
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_LAHF;
  new->type = ASM_TYPE_READFLAG;
  new->flagsread = ASM_FLAG_SF | ASM_FLAG_ZF | ASM_FLAG_AF |
                        ASM_FLAG_PF | ASM_FLAG_CF;
  return (new->len);
}
