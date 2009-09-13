/**
* @file libasm/src/arch/ia32/handlers/op_sahf.c
 *
 * @ingroup IA32_instrs
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction opcode="0x9e" func="op_sahf"/>
 */

int op_sahf(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_SAHF;
  new->type = ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_CF | ASM_FLAG_PF | ASM_FLAG_AF |
                        ASM_FLAG_SF | ASM_FLAG_ZF;
  return (new->len);
}
