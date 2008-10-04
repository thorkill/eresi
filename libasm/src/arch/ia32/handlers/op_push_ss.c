/*
** $Id: op_push_ss.c,v 1.8 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_push_ss" opcode="0x16"/>
*/

int     op_push_ss(asm_instr *new, unsigned char *opcode, unsigned int len, 
                   asm_processor *proc)
{
  new->instr = ASM_PUSH;
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_TOUCHSP | ASM_TYPE_STORE;
  new->spdiff = -4;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_FIXED, new, 
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_SS,
					       ASM_REGSET_SREG));

#else
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_FIXED, new);
  new->op[0].content = ASM_OP_BASE;
  new->op[0].regset = ASM_REGSET_SREG;
  new->op[0].baser = ASM_REG_SS;
#endif

  return (new->len);
}
