/**
 * @brief Handler for instruction push ds opcode 0x1e
 * @ingroup handlers_ia32
** $Id: op_push_ds.c,v 1.7 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for instruction push ds opcode 0x1e
 *
*/

int op_push_ds(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc) 
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_PUSH;
  new->type = ASM_TYPE_TOUCHSP | ASM_TYPE_STORE;
  new->spdiff = -4;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_FIXED, new,
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_DS,
					       ASM_REGSET_SREG));

  //asm_fixed_pack(content, type, regset, baser)
#else
  new->op[0].content = ASM_OP_BASE;
  new->op[0].regset = ASM_REGSET_SREG;
  new->op[0].baser = ASM_REG_DS;
#endif

  return (new->len);
}
