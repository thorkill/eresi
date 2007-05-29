/*
** $Id: i386_movsbl_rv_rmb.c,v 1.3 2007-05-29 00:40:27 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_movsbl_rv_rmb" opcode="0xbe"/>
*/

int i386_movsbl_rv_rmb(asm_instr *new, u_char *opcode, u_int len, 
		       asm_processor *proc) 
{  
  if (asm_proc_opsize(proc))
    new->instr = ASM_MOVSBW;
  else
    new->instr = ASM_MOVSBL;
  new->len += 1;
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_GENERAL, 
				new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_ENCODEDBYTE,
				new);
#else
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rv_rmb(new, opcode + 1, len - 1, proc);
#endif
  return (new->len);
}
