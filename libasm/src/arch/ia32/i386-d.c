/*
** this is private - do not distribute
** 
** Author  : <sk at devhell dot org>
** Started : Xxx Xxx xx xx:xx:xx 2002
** Updated : Thu May  8 13:16:39 2003
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_shift_rmb_1" opcode="0xd0"/>
 */

int op_shift_rmb_1(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_SHIFT;
  new->ptr_instr = opcode;
  new->len += 1;
    
  new->instr = ASM_SHR;
  new->op1.type = ASM_OTYPE_ENCODED;
  operand_rmb(&new->op1, opcode + 1, len - 1, proc);
  new->len += new->op1.len;
  // new->op1 = asm_create_operand(0, 0, 0, 0, proc);
    
  // new->op2 = asm_create_operand(0, 0, 0, 0, proc);
  // new->op2.content = OP_IMM_BYTE;
    
  return (new->len);
}

/*
  <instruction func="op_shift_rmv_1" opcode="0xd1"/>
 */

int op_shift_rmv_1(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;
  
  modrm = (struct s_modrm *) opcode + 1;
  new->ptr_instr = opcode;
  new->len += 1;
  switch(modrm->r) {
  case ASM_REG_EDI:
    new->instr = ASM_SAR;
    break;
  case ASM_REG_ESP:
    new->instr = ASM_SHL;
    break;
  default:
    new->instr = ASM_SHR;
    break;
  }
  new->op1.type = ASM_OTYPE_ENCODED;
  operand_rmv(&new->op1, opcode + 1, len - 1, proc);
  /*
    new->op1 = asm_create_operand(0, 0, 0, 0, proc);
    new->op1.len = 1;
    new->op1.content = OP_REG;
    new->op1.base_reg = modrm->m;
  */
  new->len += new->op1.len;
  return (new->len);
}

/*
  <instruction func="op_shift_rmb_cl" opcode="0xd2"/>
 */

int op_shift_rmb_cl(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;
  
  modrm = (struct s_modrm *) opcode + 1;
  new->ptr_instr = opcode;
    new->len += 1;
    switch(modrm->r) {
      case 1:
	new->instr = ASM_ROR;
	break;
    default:
      new->instr = ASM_SHIFT;
    }
    new->op1.type = ASM_OTYPE_ENCODED;
    operand_rmb(&new->op1, opcode + 1, len - 1, proc);
    new->op2.content = ASM_OP_BASE;
    new->op2.regset = ASM_REGSET_R8;
    new->op2.base_reg = ASM_REG_CL;
    new->len += new->op1.len;
  return (new->len);
}

/*
  <instruction func="op_shift_rmv_cl" opcode="0xd3"/>
*/

int op_shift_rmv_cl(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;

  new->ptr_instr = opcode;
  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
    switch(modrm->r) {
    case 0:
      new->instr = ASM_ROL;
      break;
    case 5:
      new->instr = ASM_SHR;
      break;
    case 4:
      new->instr = ASM_SHL;
      break;
    case 7:
      new->instr = ASM_SAR;
      break;
    default:
      new->instr = ASM_BAD;
    }
  
    new->op1.type = ASM_OTYPE_ENCODED;
    new->op2.type = ASM_OTYPE_FIXED;
    operand_rmv(&new->op1, opcode + 1, len - 1, proc);
    
    new->op2.content = ASM_OP_BASE | ASM_OP_FIXED;
    new->op2.regset = ASM_REGSET_R8;
    new->op2.base_reg = ASM_REG_CL;
    new->op2.len = 0;
    new->op2.ptr = 0;
    
    new->len += new->op1.len + new->op2.len;
  return (new->len);
}

/*
  <instruction opcode="0xd4" func="op_aam"/>
*/

int op_aam(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_AAM;
  return (new->len);
}


/*
  <instruction opcode="0xd5" func="op_aad"/>
*/
int op_aad(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->len += 1;
  new->ptr_instr = opcode;
    new->instr = ASM_AAD;
  return (new->len);
}

/*
  <instruction opcode="0xd6">
  bad instruction.
*/


/*
  <instruction opcode="0xd7" func="op_xlatb"/>
*/

int op_xlatb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_XLATB;
    return (new->len);
} 


/*
  <instruction func="op_esc0" opcode="0xd8"/>
*/

int op_esc0(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;
  
  new->ptr_instr = opcode;
  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
  switch(modrm->r) {
  case 0:
    new->instr = ASM_FADD;
    
    break;
  case 1:
    new->instr = ASM_FMUL;
    break;
  case 2:
    new->instr = ASM_FCOM;
    break;
  case 3:
    new->instr = ASM_FCOMP;
    break;
  case 4:
    new->instr = ASM_FSUB;
    break;
  case 5:
    new->instr = ASM_FSUBR;
    break;
  case 6:
    new->instr = ASM_FDIV;
    break;
  case 7:
    new->instr = ASM_FDIVR;
    break;
  }
  if (modrm->mod < 3){
    new->op1.type = ASM_OTYPE_FIXED;
    operand_rmv(&new->op1, opcode + 1, len - 1, proc);
    new->len += new->op1.len;
  } else {
    new->len += 1;
    switch(modrm->r) {
    case 6: case 0: case 5: case 1: case 4: case 7:
      
      new->op1.type = ASM_OTYPE_FIXED;
      new->op1.content = ASM_OP_FPU | ASM_OP_BASE;
      new->op2.type = ASM_OTYPE_FIXED;
      new->op2.content = ASM_OP_FPU | ASM_OP_SCALE | ASM_OP_BASE;
      new->op2.scale = modrm->m;
      break;
    
    case 2: case 3:
      new->op1.type = ASM_OTYPE_FIXED;
      new->op1.content = ASM_OP_FPU | ASM_OP_SCALE | ASM_OP_BASE;
      new->op1.scale = modrm->m;
      break;
    }
  }
  return (new->len);
}


/*
  <instruction func="op_esc1" opcode="0xd9"/>
*/

int op_esc1(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;
  
  modrm = (struct s_modrm *) opcode + 1;
  new->ptr_instr = opcode;
  new->len += 1;
  if (modrm->mod == 3)
    switch(modrm->r) {
    case 0:
      new->instr = ASM_FLD;
      new->op1.type = ASM_OTYPE_FIXED;
      new->op1.content = ASM_OP_FPU | ASM_OP_SCALE | ASM_OP_BASE;
      new->op1.len = 1;
      new->op1.scale = modrm->m;
      break;
    case 1:
      new->instr = ASM_FXCH;
      new->op1.type = ASM_OTYPE_FIXED;
      new->op1.len = 1;
      new->op1.content = ASM_OP_FPU | ASM_OP_SCALE | ASM_OP_BASE;	
      new->op1.scale = modrm->m;
      break;
    case 2:
      new->instr = ASM_FLDZ; break;
    case 4:
      switch(modrm->m) {
      case 0: new->instr = ASM_FCHS; break;
      case 1: new->instr = ASM_FABS; break;
      default: new->instr = ASM_BAD; break;
      }
      break;
    case 5:
      switch(modrm->m) {
      case 0: new->instr = ASM_FLD1; break;
      case 6: new->instr = ASM_FLDZ; break;
      }
      break;
    case 6:
      new->instr = ASM_FPATAN; break;
    case 7:
      switch(modrm->m) {
      case 0: new->instr = ASM_FPREM; break;
      case 2: new->instr = ASM_FSQRT; break;
      case 4: new->instr = ASM_FRNDINT; break;
      case 5: new->instr = ASM_FSCALE; break;
      }
    }
  else /* modrm != 3 */
    switch(modrm->r) {
    case 0:
      new->instr = ASM_FLD;
      break;
    case 1:
      new->instr = ASM_FXCH;
      break;
    case 2:
      new->instr = ASM_FST;
      break;
    case 3:
      new->instr = ASM_FSTP;
      break;
    case 4:
      new->instr = ASM_FLDENV;
      break;
    case 5:
      new->instr = ASM_FLDCW;
      break;
    case 6:
      new->instr = ASM_FNSTENV;
      break;
    case 7:
      new->instr = ASM_FNSTCW;
      break;
    }

  if (modrm->mod < 3) {
    new->op1.type = ASM_OTYPE_ENCODED;
    operand_rmv(&new->op1, opcode + 1, len - 1, proc);
  } 
  
  if (new->op1.type)
    new->len += new->op1.len;
  else
    new->len += 1;
  return (new->len);
}

/* 
   <instruction func="op_esc2" opcode="0xda"/>
*/

int op_esc2(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;
  
  modrm = (struct s_modrm *) opcode + 1;
  new->ptr_instr = opcode;
  new->len += 1;
  if (modrm->mod == 3) 
    switch (modrm->r) {
    case 5: new->instr = ASM_FUCOMPP; break;
    default: new->instr = ASM_BAD; break;
    }
  else
    switch(modrm->r) {
    case 0:
      new->instr = ASM_FIADD;
      break;
    case 1:
      new->instr = ASM_FIMUL;
      break;
    case 2:
      new->instr = ASM_FICOM;
      break;
    case 3:
      new->instr = ASM_FICOMP;
      break;
    case 4:
      new->instr = ASM_FISUB;
      break;
    case 5:
      new->instr = ASM_FISUBR;
      break;
    case 6:
      new->instr = ASM_FIDIV;
      break;
    case 7:
      new->instr = ASM_FIDIVR;
      break;
    }
  if (!(*(opcode + 1) == 0xe9)) {
    new->op1.type = ASM_OTYPE_ENCODED;
    operand_rmv(&new->op1, opcode + 1, len - 1, proc);
    new->len += new->op1.len;
  } else
    new->len += 1;
  return (new->len);
}




/*
  <instruction func="op_esc3" opcode="0xdb"/>
*/

int op_esc3(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;
  
  new->ptr_instr = opcode;
  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
    switch(modrm->r) {
    case 0:
      new->instr = ASM_FILD;
      break;
    case 1:
      new->instr = ASM_BAD;
      break;
    case 2:
      new->instr = ASM_FIST;
      break;
    case 3:
      new->instr = ASM_FISTP;
      break;
    case 4:
      // bad
      break;
    case 5:
      new->instr = ASM_FLD;
      break;
    case 6:
      
    case 7:
      new->instr = ASM_FSTP;
      break;
    }
    new->op1.type = ASM_OTYPE_ENCODED;
    operand_rmv(&new->op1, opcode + 1, len - 1, proc);
    new->len += new->op1.len;
  return (new->len);
}

/*
  <instruction func="op_esc4" opcode="0xdc"/>
*/

int op_esc4(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;

  new->ptr_instr = opcode;
  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
  switch(modrm->r) {
  case 0:
    new->instr = ASM_FADD;
    break;
  case 1:
    new->instr = ASM_FMUL;
    break;
  case 2:
    new->instr= ASM_FCOM;
    break;
  case 3:
    new->instr = ASM_FCOMP;
    break;
  case 4:
    new->instr = ASM_FSUB;
    break;
  case 5:
    new->instr = ASM_FSUBR;
    break;
  case 6:
    new->instr = ASM_FDIV;
    break;
  case 7:
    new->instr = ASM_FDIVR;
    break;
  }
    
  if (modrm->mod == 3) {
    new->op1.type = ASM_OTYPE_FIXED;
    new->op1.content = ASM_OP_FPU | ASM_OP_BASE | ASM_OP_SCALE;
    new->op1.len = 1;
    new->op1.scale = modrm->m;
    new->op2.type = ASM_OTYPE_FIXED;
    new->op2.content = ASM_OP_FPU | ASM_OP_BASE;
  } else {
    new->op1.type = ASM_OTYPE_FIXED;
    operand_rmv(&new->op1, opcode + 1, len - 1, proc);
  }
  if (new->op1.type)
    new->len += new->op1.len;
  return (new->len);
}

/*
  <instruction func="op_esc5" opcode="0xdd"/>
  
  fldl 0xfffffff0(%ebp) ; opcode = 'dd 45 f0'
  
  fstpl (%esp,1) ; opcode = 'dd 1c 24'
  fstpl 0xfffffff0(%ebp) ; opcode = 'dd 5d f0'
  
  fstp %st(0) ; opcode = 'dd d8'
  
  fstl (%esp,1) ; opcode = 'dd 14 24'
*/

int op_esc5(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;
  new->ptr_instr = opcode;

  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
  if (modrm->mod == 3)
    switch(modrm->r) {
    case 3:
      new->instr = ASM_FSTP;
      break;
    case 4:
      new->instr = ASM_FUCOM;
      break;
    case 5:
      new->instr = ASM_FUCOMP;
    break;
    default:
      new->instr = ASM_BAD;
      break;
    }
  else
    switch (modrm->r) {
    case 0:
      new->instr = ASM_FLD;
      break;
    case 1:
      new->instr = ASM_BAD;
      break;
    case 2:
      new->instr = ASM_FST;
      break;
    case 3:
      new->instr = ASM_FSTP;
      break;
    case 4:
      new->instr = ASM_BAD; // ASM_FRSTOR;
      break;
    case 5:
      new->instr = ASM_BAD; // ;
      break;
    case 6:
      new->instr = ASM_BAD;
      break;
    case 7:
      new->instr = ASM_BAD;
      break;
    }
  
  if (modrm->mod == 3) {
    new->op1.type = ASM_OTYPE_FIXED;
    new->op1.content = ASM_OP_FPU | ASM_OP_BASE | ASM_OP_SCALE;
    new->op1.len = 1;
    new->op1.ptr = opcode + 1;
      
    new->op1.scale = modrm->m;
  } else {
    new->op1.type = ASM_OTYPE_ENCODED;
    operand_rmv(&new->op1, opcode + 1, len - 1, proc);
    //new->op1 = asm_create_operand(0, 0, 0, 0, proc);
    //new->op->type = 
  }
  if (new->op1.type)
    new->len += new->op1.len;
  return (new->len);
}

/*
  <instruction func="op_esc6" opcode="0xde"/>
 */

int op_esc6(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;
  new->ptr_instr = opcode;
  
  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
  if (modrm->mod <= 2) {
    switch(modrm->r) {
      case 0:
	new->instr = ASM_FIADD;
	break;
      case 1:
	new->instr = ASM_FIMUL;
	break;
      case 2:
	new->instr = ASM_FICOM;
	break;
      case 3:
	new->instr = ASM_FICOMP;
	break;
      case 4:
	new->instr = ASM_FISUB;
	break;
      case 5:
	new->instr = ASM_FISUBR;
	break;
      case 6:
	new->instr = ASM_FIDIV;
	
	break;
      case 7:
	new->instr = ASM_FIDIVR;
	break;
      } 
    } else {
      switch(modrm->r) {
      case 0: new->instr = ASM_FADDP; break;
      case 1: 
	new->instr = ASM_FMULP;
	break;
      case 2: 
	new->instr = ASM_FCOMPS;
	break;
      case 3: new->instr = ASM_FCOMPP;
	break;
      case 4: new->instr = ASM_FSUBP;
	break;
      case 5: new->instr = ASM_FSUBRP;
	break;
      case 6: 
	switch(modrm->m) {
	case 3: case 5: case 2: case 4: case 6: case 1: new->instr = ASM_FDIVP; break;
	default: case 0: new->instr = ASM_FDIVP; break;
	}
	break;
      case 7: 
	switch(modrm->m) {
	case 3: new->instr = ASM_FDIVR; break;
	default: new->instr = ASM_FDIV; break;
	}	
	break;
      }
    }
      
    if (!(*(opcode + 1) == 0xd9)) {
      new->op1.type = ASM_OTYPE_FIXED;
      new->op1.content = ASM_OP_FPU | ASM_OP_BASE | ASM_OP_SCALE;
      new->op1.len = 1;
      new->op1.scale = modrm->m;
      new->op2.type = ASM_OTYPE_FIXED;
      new->op2.content = ASM_OP_FPU | ASM_OP_BASE;
      new->op2.len = 0;
    } else
      new->len++;
    if (new->op1.type)
      new->len += new->op1.len;
  return (new->len);
}

/*
  <instruction func="op_esc7" opcode="0xdf"/>
*/

int op_esc7(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;
  
  modrm = (struct s_modrm *) opcode + 1;
  new->ptr_instr = opcode;
  new->len += 1;
  if (*(opcode + 1) == 0xe0) {
    new->instr = ASM_FNSTSW;
    new->op1.type = ASM_OTYPE_FIXED;
    new->op1.content = ASM_OP_BASE;
    new->op1.regset = ASM_REGSET_R16;
    new->op1.base_reg = ASM_REG_EAX;
  } else
    switch (modrm->r) {
    case 0:
      new->instr = ASM_FILD;
      break;
    case 1:
      // bad new->instr = ASM_;
      break;
    case 2:
      new->instr = ASM_FIST;
      break;
    case 3:
      new->instr = ASM_FISTP;
      break;
    case 4:   
      new->instr = ASM_FBLD;
      break;
    case 5:    
      new->instr = ASM_FILD;
      break;
    case 6:      
      new->instr = ASM_FBSTP;
      break;
    case 7:
      new->instr = ASM_FISTP;
      break;
    }
  if (*(opcode + 1) != 0xe0) {
    new->op1.type = ASM_OTYPE_ENCODED;
    operand_rmv(&new->op1, opcode + 1, len - 1, proc);
    new->len += new->op1.len;
  } else
    new->len++;
  return (new->len);
}




