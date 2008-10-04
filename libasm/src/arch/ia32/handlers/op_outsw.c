/*
** $Id: op_outsw.c,v 1.5 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_outsw" opcode="0x6f"/>
  <instruction func="op_outsd" opcode="0x6f"/>
*/

int op_outsw(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc) 
{
  new->ptr_instr = opcode;
  new->len += 1;
  
  if (!asm_proc_opsize(proc))
    new->instr = ASM_OUTSW;
  else
    new->instr = ASM_OUTSD;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_FIXED, new,
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_DX,
					       ASM_REGSET_R16));

  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_OTYPE_XSRC, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_FIXED, new);
  new->op[0].content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op[0].regset = ASM_REGSET_R16;
  new->op[0].baser = ASM_REG_DX;
  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_OTYPE_XSRC, new);
#endif
  return (new->len);
}
