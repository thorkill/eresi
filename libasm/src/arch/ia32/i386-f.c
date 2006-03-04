/*
** private - do not distribute
**
** Author  : <sk at devhell dot org>
** Started : Mon Jun 10 17:06:33 2002
** Updated : Mon Mar 15 14:04:44 2004
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_lock" opcode="0xf0"/>
*/

int op_lock(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  if (!new->ptr_prefix)
    new->ptr_prefix = opcode;
  new->len += 1;
  new->instr = ASM_LOCK;
  return (new->len);
}

/*
  <instruction func="op_lock" opcode="0xf1"/>
*/

int op_int1(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_INT1;
  return (new->len);
}

/*
  <instruction func="op_repnz" opcode="0xf2"/>
*/

int op_repnz(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  if (!new->ptr_prefix)
    new->ptr_prefix = opcode;
  new->len += 1;
  new->prefix = ASM_PREFIX_REPNE;
  return (proc->fetch(new, opcode + 1, len - 1, proc));
}

/*
  <instruction func="op_repz" opcode="0xf3"/>
 */

int op_repz(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  if (!new->ptr_prefix)
    new->ptr_prefix = opcode;
  new->len += 1;
  new->prefix = ASM_PREFIX_REP;
  return (proc->fetch(new, opcode + 1, len - 1, proc));
}

/*
  <instruction func="op_hlt" opcode="0xf4"/>
*/

int op_hlt(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_HLT;
  return (new->len);
}

/*
  <instruction func="op_cmc" opcode="0xf5"/>
*/

int op_cmc(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_CMC;
  return (new->len);
}

/*
  <instruction func="op_unary_rmb" opcode="0xf6"/>
*/

int op_unary_rmb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;

  new->ptr_instr = opcode;
  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
    switch(modrm->r) {
    case 0:
      new->instr = ASM_TEST;
      new->op1.type = ASM_OTYPE_ENCODED;

      operand_rmb(&new->op1, opcode + 1, len - 1, proc);
      new->op2.type = ASM_OTYPE_IMMEDIATE;
      new->op2.size = ASM_OSIZE_BYTE;
      new->op2.content = ASM_OP_VALUE;
      new->op2.len = 1;
      new->op2.ptr = opcode + 1 + new->op1.len;
      new->op2.imm = 0;
      memcpy(&new->op2.imm, new->op2.ptr, 1);
      new->len += new->op1.len + new->op2.len;
      break;
    case 1:
      return (0);
      break;
    case 2:
      new->instr = ASM_NOT;
      break;
    case 3:
      new->instr = ASM_NEG;
      break;
    case 4:
      new->instr = ASM_MUL;
      break;
    case 5:
      new->instr = ASM_IMUL;
      break;
    case 6:
      new->instr = ASM_DIV;
      break;
    case 7:
      new->instr = ASM_IDIV;
      break;
    default:
      break;
    }
    if (!new->op1.type) {
      new->op1.type = ASM_OTYPE_ENCODED;
      operand_rmb(&new->op1, opcode + 1, len - 1, proc);
      new->len += new->op1.len;
    }
  return (new->len);
}

/*
  <instruction func="op_unary_rmv" opcode="0xf7"/>
*/

int op_unary_rmv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;

  modrm = (struct s_modrm *) (opcode + 1);
  new->ptr_instr = opcode;
  new->len += 1;
    switch (modrm->r) {
    case 0:
      new->instr = ASM_TEST;
      break;
    case 1:
      return (0);
    case 2:
      new->instr = ASM_NOT;
      break;
    case 3:
      new->instr = ASM_NEG;
      break;
    case 4:
      new->instr = ASM_MUL;
      break;
    case 5:
      new->instr = ASM_IMUL;
      break;
    case 6:
      new->instr = ASM_DIV;
      break;
    case 7:
      new->instr = ASM_IDIV;
      break;
    } /* switch */
    new->op1.type = ASM_OTYPE_ENCODED;
    new->op1.size = ASM_OSIZE_VECTOR;
    operand_rmv(&new->op1, opcode + 1, len - 1, proc);
    new->len += new->op1.len;
    if (new->instr == ASM_TEST) {
      new->op2.type = ASM_OTYPE_IMMEDIATE;
      new->op2.len = asm_proc_vector_len(proc);
      new->op2.content = ASM_OP_VALUE;
      new->op2.ptr = opcode + 1 + new->op1.len;
      memcpy(&new->op2.imm, opcode + 1 + new->op1.len,
	     asm_proc_vector_len(proc));
      new->len += new->op2.len;
    }
  return (new->len);
}

/*
  <instruction func="op_clc" opcode="0xf8"/>
*/

int op_clc(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->len += 1;
  new->ptr_instr = opcode;
    new->instr = ASM_CLC;
  return (new->len);
}

/*
  <instruction func="op_stc" opcode="0xf9"/>
*/

int op_stc(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->len += 1;
  new->ptr_instr = opcode;
    new->instr = ASM_STC;
  return (new->len);
}

/*
  <instruction func="op_cli" opcode="0xfa"/>
*/

int op_cli(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->len += 1;
  new->ptr_instr = opcode;
    new->instr = ASM_CLI;
  return (new->len);
}



/*
  <instruction func="op_sti" opcode="0xfb"/>
*/

int op_sti(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->len += 1;
  new->ptr_instr = opcode;
    new->instr = ASM_STI;
  return (new->len);
}

/*
  <instruction func="op_cld" opcode="0xfc"/>
*/

int op_cld(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_CLD;
  return (new->len);
}

/*
  <instruction func="op_std" opcode="0xfd"/>
*/

int op_std(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
    new->instr = ASM_STD;
  return (new->len);
}

/*
  <instruction func="op_incdec_rmb" opcode="0xfe"/>
*/

int op_incdec_rmb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;

  new->ptr_instr = opcode;
  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
  switch(modrm->r) {
  case 0:
    new->instr = ASM_INC;
    break;
  case 1:
    new->instr = ASM_DEC;
    break;
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
    break;
  default:
    break;
  }
  new->op1.type = ASM_OTYPE_ENCODED;
  operand_rmb(&new->op1, opcode + 1, len - 1, proc);
  new->len += new->op1.len;
  return (new->len);
}


/*
  <instruction func="op_indir_rmv" opcode="0xff"/>
*/

int op_indir_rmv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;

  new->ptr_instr = opcode;
  modrm = (struct s_modrm *) (opcode + 1);
  new->len += 1;
  switch(modrm->r) {
  case 0:
    new->instr = ASM_INC;
    new->op1.type = ASM_OTYPE_ENCODED;
    new->op1.size = ASM_OSIZE_VECTOR;
    break;
  case 1:
    new->instr = ASM_DEC;
    new->op1.type = ASM_OTYPE_ENCODED;
    new->op1.size = ASM_OSIZE_VECTOR;
    break;
  case 2:
    new->type = ASM_TYPE_CALLPROC;
    new->instr = ASM_CALL;
    new->op1.type = ASM_OTYPE_MEMORY;
    break;
  case 3:
    new->type = ASM_TYPE_CALLPROC;
    new->instr = ASM_CALL;
    new->op1.type = ASM_OTYPE_MEMORY;

    break;
  case 4:
    new->type = ASM_TYPE_IMPBRANCH;
    new->instr = ASM_BRANCH;
    new->op1.type = ASM_OTYPE_MEMORY;
    break;
  case 5:
    new->type = ASM_TYPE_IMPBRANCH;
    new->instr = ASM_BRANCH;
    new->op1.type = ASM_OTYPE_MEMORY;
    break;
  case 6:
    new->instr = ASM_PUSH;
    new->op1.type = ASM_OTYPE_ENCODED;
    new->op1.size = ASM_OSIZE_VECTOR;
    break;
  case 7:
    new->instr = ASM_BAD;
    new->op1.type = ASM_OTYPE_ENCODED;
    new->op1.size = ASM_OSIZE_VECTOR;
    break;
  default:
    break;
  } /* switch */
  if ((new->op1.type == ASM_OTYPE_ENCODED) ||
      (new->op1.type == ASM_OTYPE_MEMORY)) {
    operand_rmv(&new->op1, opcode + 1, len - 1, proc);
    if (new->op1.type == ASM_OTYPE_MEMORY)
      new->op1.content |= ASM_OP_ADDRESS;
  }
  new->len += new->op1.len;
  return (new->len);
}






