/*
** $Id: op_pop_es.c,v 1.7 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  Opcode :              0x07
  Instruction :         POP
*/

int op_pop_es(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc) 
{
  new->instr = ASM_POP;
  new->type = ASM_TYPE_TOUCHSP | ASM_TYPE_ASSIGN | ASM_TYPE_LOAD;
  new->spdiff = 4;
  new->len += 1;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_FIXED, new,
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_ES,
					       ASM_REGSET_SREG));

#else
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_FIXED, new);
  new->op[0].content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op[0].regset = ASM_REGSET_SREG;
  new->op[0].baser = ASM_REG_ES;
#endif

  return (new->len);
}
