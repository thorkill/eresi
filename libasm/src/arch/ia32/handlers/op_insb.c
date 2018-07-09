/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_insb" opcode="0x6b"/>
*/

int     op_insb(asm_instr *new, u_char *opcode, u_int len,
		asm_processor *proc)
{
  new->len += 1;
  new->instr = ASM_INSB;
  new->ptr_instr = opcode;

#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_YDEST, new);
  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_CONTENT_FIXED, new);
	new->op[1].type = ASM_OPTYPE_MEM;
  new->op[1].memtype = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op[1].regset = ASM_REGSET_R16;
  new->op[1].baser = ASM_REG_EDX;
#else
  new->op[1].content = ASM_CONTENT_FIXED;
	new->op[1].type = ASM_OPTYPE_MEM;
  new->op[1].memtype = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op[1].regset = ASM_REGSET_R16;
  new->op[1].baser = ASM_REG_EDX;

  new->op[0].content = ASM_CONTENT_YDEST;
	new->op[0].type = ASM_OPTYPE_MEM;
  new->op[0].memtype = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op[0].prefix = ASM_PREFIX_DS;
  new->op[0].baser = ASM_REG_EDI;
  new->op[0].regset = asm_proc_addsize(proc) ? ASM_REGSET_R16 : ASM_REGSET_R32;
#endif
  return (new->len);
}
