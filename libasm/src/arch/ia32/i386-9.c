/*
** this is private - do not distribute
** 
** Author  : <sk at devhell dot org>
** Started : Xxx Xxx xx xx:xx:xx 2002
** Updated : Mon May  5 23:32:04 2003
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction name="nop" func="op_nop" opcode="0x90"/>
*/

int op_nop(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
    new->instr = ASM_NOP;
    return (new->len);
}

/*
  <instruction func="op_xchg_eax_reg" opcode="0x91"/>
  <instruction func="op_xchg_eax_reg" opcode="0x92"/>
  <instruction func="op_xchg_eax_reg" opcode="0x93"/>
  <instruction func="op_xchg_eax_reg" opcode="0x94"/>
  <instruction func="op_xchg_eax_reg" opcode="0x95"/>
  <instruction func="op_xchg_eax_reg" opcode="0x96"/>
  <instruction func="op_xchg_eax_reg" opcode="0x97"/>
*/

int op_xchg_eax_reg(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;

    modrm = (struct s_modrm *) opcode;
    new->instr = ASM_XCHG;
  new->ptr_instr = opcode;
    new->len += 1;
    
    new->op1.type = ASM_OTYPE_FIXED;
    new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
    new->op1.len = 0;
    new->op1.base_reg = ASM_REG_EAX;
    new->op1.regset = asm_proc_oplen(proc) ? 
      ASM_REGSET_R16 : ASM_REGSET_R32;

    new->op2.type = ASM_OTYPE_OPMOD;
    new->op2.content = ASM_OP_BASE | ASM_OP_FIXED;
    new->op2.len = 0;
    new->op2.base_reg = modrm->m;
    new->op2.regset = asm_proc_is_protected(proc) ? 
      ASM_REGSET_R32 : ASM_REGSET_R16;
  return (new->len);
}

/*
  <instruction func="op_cwtl" opcode="0x98"/>
*/

int	op_cwtl(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  if (asm_proc_oplen(proc))
    new->instr = ASM_CBTW;
  else
    new->instr = ASM_CWTL;
  return (new->len);
}

/*
 <instruction func="op_cltd" opcode="0x99"/>
*/

int op_cltd(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
  if (asm_proc_oplen(proc))
    new->instr = ASM_CWTD;
  else
  new->instr = ASM_CLTD;
  return (new->len);
}

/*
  <instruction func="op_fwait" opcode="0x9b"/>
 */

int op_fwait(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->len += 1;
  new->ptr_instr = opcode;
    new->instr = ASM_FWAIT;
  return (new->len);
}

/*
  <instruction name="pushf" func="op_pushf" opcode="0x9c"/>
*/


int op_pushf(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->len += 1;
  new->ptr_instr = opcode;
    new->instr = ASM_PUSHF;
    // new->type = IS_MEM_WRITE;
  return (new->len);
}

/*
  <instruction name="popf" func="op_popf" opcode="0x9d"/>
*/

int op_popf(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
    new->len += 1;
    new->instr = ASM_POPF;
    // new->type = IS_MEM_READ;
  return (new->len);
}

/*
  <instruction opcode="0x9e" func="op_sahf"/>
 */

int op_sahf(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
    new->instr = ASM_SAHF;
  return (new->len);
}

/*
  <instruction func="op_lahf" opcode="0x9f"/>
*/

int op_lahf(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_LAHF;
  return (new->len);
}







