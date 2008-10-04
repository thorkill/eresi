/**
 * @file op_iret.c
 * @ingroup handlers_ia32
 * $Id: op_iret.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
 *
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 *
 * <instruction opcode="0xcf" func="op_iret"/>
 */

int op_iret(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc)
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_IRET;
  new->type = ASM_TYPE_INT | ASM_TYPE_TOUCHSP | ASM_TYPE_RETPROC |
                ASM_TYPE_READFLAG;
  new->flagsread = ASM_FLAG_NT | ASM_FLAG_VM;
  return (new->len);
}
