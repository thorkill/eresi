/*
** $Id: op_and_eax_iv.c,v 1.7 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_and_eax_iv" opcode="0x25"/>
*/

int op_and_eax_iv(asm_instr *new, unsigned char *opcode, unsigned int len,
		  asm_processor *proc)
{
  new->instr = ASM_AND;
  new->len += 1;
  new->ptr_instr = opcode;

  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_CF | ASM_FLAG_ZF | ASM_FLAG_PF |
                      ASM_FLAG_OF | ASM_FLAG_SF;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_FIXED, new, 
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_EAX,
					       asm_proc_is_protected(proc) ?
					       ASM_REGSET_R32 : ASM_REGSET_R16));

#else
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_FIXED, new);
#endif
  new->op[0].content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op[0].regset = ASM_REGSET_R32;
  new->op[0].ptr = opcode;
  new->op[0].len = 0;
  new->op[0].baser = ASM_REG_EAX;
  new->op[0].regset = asm_proc_is_protected(proc) ?
    ASM_REGSET_R32 : ASM_REGSET_R16;
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_IMMEDIATE,                                new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_IMMEDIATE,                                new);
#endif

  return (new->len);
}
