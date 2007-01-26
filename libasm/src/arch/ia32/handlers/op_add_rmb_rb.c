/*
** $Id: op_add_rmb_rb.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
 * Opcode :             0x00
 * ADD
 * Destination is an encoded byte.
 * Source is a byte register.
 */

int op_add_rmb_rb(asm_instr *new, u_char *opcode, u_int len,
                  asm_processor *proc) {
  new->instr = ASM_ADD;
  new->ptr_instr = opcode;
  new->len++;
  /* new->type = IS_MEM_READ */
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op1.size = ASM_OSIZE_BYTE;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op2.size = ASM_OSIZE_BYTE;
  operand_rmb_rb(new, opcode + 1, len - 1, proc);
  return (new->len);
}
