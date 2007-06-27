/*
** $Id: op_inc_reg.c,v 1.4 2007-06-27 11:25:11 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_inc_reg" opcode="0x40"/>
  <instruction func="op_inc_reg" opcode="0x41"/>
  <instruction func="op_inc_reg" opcode="0x42"/>
  <instruction func="op_inc_reg" opcode="0x43"/>
  <instruction func="op_inc_reg" opcode="0x44"/>
  <instruction func="op_inc_reg" opcode="0x45"/>
  <instruction func="op_inc_reg" opcode="0x46"/>
  <instruction func="op_inc_reg" opcode="0x47"/>
*/


int op_inc_reg(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  struct s_modrm        *modrm;
  
  modrm = (struct s_modrm *) opcode;
  new->len += 1;
  new->instr = ASM_INC;
  new->ptr_instr = opcode;

  
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_OPMOD, new);
#else
  new->op1.type = ASM_OTYPE_OPMOD;
  
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = asm_proc_opsize(proc) ?
    ASM_REGSET_R16 : ASM_REGSET_R32;
  new->op1.ptr = opcode;
  
  new->op1.baser = modrm->m;
#endif
  return (new->len);  
}
