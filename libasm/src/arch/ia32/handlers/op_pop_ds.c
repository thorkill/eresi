/*
** $Id: op_pop_ds.c,v 1.7 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_pop_ds" opcode="0x1f"/>
*/

int     op_pop_ds(asm_instr *new, unsigned char *opcode, unsigned int len, 
                  asm_processor *proc) 
{
  new->instr = ASM_POP;
  new->ptr_instr = opcode;
  new->len += 1;
  new->type = ASM_TYPE_TOUCHSP | ASM_TYPE_LOAD | ASM_TYPE_ASSIGN;
  new->spdiff = 4;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_FIXED, new,
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_DS,
					       ASM_REGSET_SREG));

#else
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_FIXED, new);
  new->op[0].content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op[0].regset = ASM_REGSET_SREG;
  new->op[0].baser = ASM_REG_DS;
#endif

  return (new->len);
}
