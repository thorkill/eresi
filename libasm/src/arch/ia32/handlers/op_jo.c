/**
 * @file op_jo.c
 * @ingroup handlers_ia32
 * $Id: op_jo.c,v 1.5 2007-08-14 06:52:55 strauss Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * Instructiono jo handler for opcode 0x70
 * <instruction func="op_jo" opcode="0x70"/>
 */

int  op_jo(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_CONDBRANCH;

  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_SHORTJUMP, 
                                new);

  return (new->len);
}
