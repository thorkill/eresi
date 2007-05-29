/*
** $Id: op_pop_reg.c,v 1.4 2007-05-29 00:40:27 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_pop_reg" opcode="0x58"/>
  <instruction func="op_pop_reg" opcode="0x59"/>
  <instruction func="op_pop_reg" opcode="0x5a"/>
  <instruction func="op_pop_reg" opcode="0x5b"/>
  <instruction func="op_pop_reg" opcode="0x5c"/>
  <instruction func="op_pop_reg" opcode="0x5d"/>
  <instruction func="op_pop_reg" opcode="0x5e"/>
  <instruction func="op_pop_reg" opcode="0x5f"/>
*/

int op_pop_reg(asm_instr *new, u_char *opcode, u_int len, 
	       asm_processor *proc)
{
  struct s_modrm        *modrm;
  
  modrm = (struct s_modrm *) opcode;
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_POP;
  new->type = ASM_TYPE_TOUCHSP;
  new->spdiff = 4;

  #if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_OPMOD, new);
  #else
  new->op1.type = ASM_OTYPE_OPMOD;
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = asm_proc_opsize(proc) ?
    ASM_REGSET_R16 : ASM_REGSET_R32;
  new->op1.ptr = opcode;  
  new->op1.base_reg = modrm->m;
  #endif
  
  return (new->len);
}
