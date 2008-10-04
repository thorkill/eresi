/*
** $Id: op_adc_al_ib.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_adc_al_ib" opcode="0x14"/>
*/

int op_adc_al_ib(asm_instr *new, unsigned char *opcode, unsigned int len,
		 asm_processor *proc)
{
  new->instr = ASM_ADC;
  new->len += 1;
  new->ptr_instr = opcode;

  new->type = ASM_TYPE_ARITH | ASM_TYPE_READFLAG | ASM_TYPE_WRITEFLAG;
  new->flagsread = ASM_FLAG_CF;
  new->flagswritten = ASM_FLAG_OF | ASM_FLAG_SF | ASM_FLAG_ZF |
                       ASM_FLAG_AF | ASM_FLAG_CF | ASM_FLAG_PF;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_FIXED, new, 
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_EAX,
					       asm_proc_is_protected(proc) ?
					       ASM_REGSET_R32 : ASM_REGSET_R16));

#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_FIXED, new);
#endif
  new->op[0].content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op[0].ptr = opcode;
  new->op[0].len = 0;
  new->op[0].baser = ASM_REG_AL;
  new->op[0].regset = ASM_REGSET_R8;
#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1,			                  	ASM_OTYPE_IMMEDIATEBYTE, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1,			                  	ASM_OTYPE_IMMEDIATEBYTE, new);
#endif

  return (new->len);
}
