/*
** $Id: i386_movsbl_rv_rmb.c,v 1.4 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_movsbl_rv_rmb" opcode="0xbe"/>
*/

int i386_movsbl_rv_rmb(asm_instr *new, unsigned char *opcode, unsigned int len,
		       asm_processor *proc)
{
  if (asm_proc_opsize(proc))
    new->instr = ASM_MOVSBW;
  else
    new->instr = ASM_MOVSBL;
  new->len += 1;
#if LIBASM_USE_OPERAND_VECTOR
#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_GENERAL,				new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_GENERAL,				new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_ENCODEDBYTE,				new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_ENCODEDBYTE,				new);
#endif
#else
  new->op[0].type = ASM_OTYPE_GENERAL;
  new->op[1].type = ASM_OTYPE_ENCODED;
  operand_rv_rmb(new, opcode + 1, len - 1, proc);
#endif
  return (new->len);
}
