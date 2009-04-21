/**
 * @file op_scasd.c
 * @ingroup handlers_ia32
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_scasd" opcode="0xaf"/>
*/

int op_scasd(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->len += 1;
  if (asm_proc_opsize(proc))
    new->instr = ASM_SCASW;
  else
    new->instr = ASM_SCASD;

  new->ptr_instr = opcode;
  new->type = ASM_TYPE_COMPARISON | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_AF | ASM_FLAG_CF | ASM_FLAG_PF |
                        ASM_FLAG_OF | ASM_FLAG_SF | ASM_FLAG_ZF;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_FIXED, new,
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_AL,
					       ASM_REGSET_R8));

  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_CONTENT_YDEST, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_FIXED, new);
  new->op[0].type = ASM_OPTYPE_REG;
  new->op[0].regset = ASM_REGSET_R8;
  new->op[0].baser = ASM_REG_EAX;

  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_CONTENT_YDEST, new);
#endif
  return (new->len);
}
