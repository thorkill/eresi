/**
 * @file op_sbb_eax_iv.c
 * @ingroup handlers_ia32
** $Id: op_sbb_eax_iv.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_sbb_eax_iv" opcode="0x1d"/>
*/

int op_sbb_eax_iv(asm_instr *new, unsigned char *opcode, unsigned int len, 
                  asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_SBB;
  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG | ASM_TYPE_READFLAG;
  new->flagsread = ASM_FLAG_CF;
  new->flagswritten = ASM_FLAG_AF | ASM_FLAG_CF | ASM_FLAG_PF |
                        ASM_FLAG_OF | ASM_FLAG_SF | ASM_FLAG_ZF;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_FIXED, new,
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_EAX,
					       asm_proc_is_protected(proc) ?
					       ASM_REGSET_R32 : ASM_REGSET_R16));

  new->len += asm_operand_fetch(&new->op[1], opcode + 1,
                                ASM_OTYPE_IMMEDIATE, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_FIXED, new);
  new->op[0].size = new->op[1].size = ASM_OSIZE_VECTOR;
  new->op[0].content = ASM_OP_FIXED | ASM_OP_BASE;
  new->op[0].baser = ASM_REG_EAX;
  new->op[0].regset = asm_proc_opsize(proc) ? ASM_REGSET_R32 :
    ASM_REGSET_R16;
  new->len += asm_operand_fetch(&new->op[1], opcode + 1,
                                ASM_OTYPE_IMMEDIATE, new);
#endif
  return (new->len);
}
