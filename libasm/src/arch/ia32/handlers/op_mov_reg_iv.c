/*
** $Id: op_mov_reg_iv.c,v 1.3 2007-05-16 18:38:13 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_mov_reg_iv" opcode="0xb8"/>
  <instruction func="op_mov_reg_iv" opcode="0xb9"/>
  <instruction func="op_mov_reg_iv" opcode="0xba"/>
  <instruction func="op_mov_reg_iv" opcode="0xbb"/>
  <instruction func="op_mov_reg_iv" opcode="0xbc"/>
  <instruction func="op_mov_reg_iv" opcode="0xbd"/>
  <instruction func="op_mov_reg_iv" opcode="0xbe"/>
  <instruction func="op_mov_reg_iv" opcode="0xbf"/>
*/

int op_mov_reg_iv(asm_instr *new, u_char *opcode, u_int len, 
asm_processor *proc) 
{
  struct s_modrm        *modrm;
  
  modrm = (struct s_modrm *) opcode;
  new->type = ASM_TYPE_ASSIGN;
  new->ptr_instr = opcode;
  new->instr = ASM_MOV;
  new->len += 1;

  #if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_OPMOD, proc);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_IMMEDIATE, proc);
  #else
  
  new->op1.type = ASM_OTYPE_OPMOD;
  new->op1.size = ASM_OSIZE_DWORD;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op2.size = ASM_OSIZE_DWORD;
  
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.regset = asm_proc_opsize(proc) ?
    ASM_REGSET_R16 : ASM_REGSET_R32;
  new->op1.ptr = opcode;
  new->op1.base_reg = modrm->m;
  
  new->op2.content = ASM_OP_VALUE;
  new->op2.ptr = opcode + 1;
  new->op2.len = asm_proc_vector_len(proc);
  memcpy(&new->op2.imm, opcode + 1, asm_proc_vector_len(proc));
  new->len += asm_proc_vector_len(proc);
#endif
  return (new->len);
}
