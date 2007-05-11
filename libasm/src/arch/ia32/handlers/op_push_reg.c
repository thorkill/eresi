/*
** $Id: op_push_reg.c,v 1.3 2007-05-11 16:40:58 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_push_reg" opcode="0x50"/>
  <instruction func="op_push_reg" opcode="0x51"/>
  <instruction func="op_push_reg" opcode="0x52"/>
  <instruction func="op_push_reg" opcode="0x53"/>
  <instruction func="op_push_reg" opcode="0x54"/>
  <instruction func="op_push_reg" opcode="0x55"/>
  <instruction func="op_push_reg" opcode="0x56"/>
  <instruction func="op_push_reg" opcode="0x57"/>
*/

int op_push_reg(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm        *modrm;
  
  modrm = (struct s_modrm *) opcode;
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_PUSH;
  new->type = ASM_TYPE_TOUCHSP;
  new->spdiff = -4;

#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_OPMOD, proc);
#else
  new->op1.type = ASM_OTYPE_OPMOD;
  new->op1.regset = asm_proc_opsize(proc) ?
    ASM_REGSET_R16 : ASM_REGSET_R32;
  new->op1.content = ASM_OP_BASE;
  new->op1.ptr = opcode;
  new->op1.base_reg = modrm->m;
#endif
  return (new->len);
}
