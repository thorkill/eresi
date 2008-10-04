/**
 * @file op_lahf.c
 * @ingroup handlers_ia32
 * $Id: op_lahf.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for lahf instruction opcode 0x9f
 *
  <instruction func="op_lahf" opcode="0x9f"/>
*/

int op_lahf(asm_instr *new, unsigned char *opcode, unsigned int len,
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
