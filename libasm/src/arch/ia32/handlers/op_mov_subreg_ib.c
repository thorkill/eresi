/*
** $Id: op_mov_subreg_ib.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_mov_subreg_ib" opcode="0xb0"/>
  <instruction func="op_mov_subreg_ib" opcode="0xb1"/>
  <instruction func="op_mov_subreg_ib" opcode="0xb2"/>
  <instruction func="op_mov_subreg_ib" opcode="0xb3"/>
  <instruction func="op_mov_subreg_ib" opcode="0xb4"/>
  <instruction func="op_mov_subreg_ib" opcode="0xb5"/>
  <instruction func="op_mov_subreg_ib" opcode="0xb6"/>
  <instruction func="op_mov_subreg_ib" opcode="0xb7"/>

*/

int op_mov_subreg_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm        *modrm;
  
  modrm = (struct s_modrm *) opcode;
  new->ptr_instr = opcode;
    new->instr = ASM_MOV;
    new->len += 2;

    new->op1.type = ASM_OTYPE_OPMOD;
    new->op2.type = ASM_OTYPE_IMMEDIATE;

    new->op1.content = ASM_OP_BASE;
    new->op1.regset = ASM_REGSET_R8;
    new->op1.ptr = opcode;

    new->op1.base_reg = modrm->m;


    new->op2.content = ASM_OP_VALUE;
    new->op2.ptr = opcode + 1;
    
    new->op2.imm = 0;
    memcpy(&new->op2.imm, opcode + 1, 1);
  return (new->len);
}
