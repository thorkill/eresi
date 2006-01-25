/*
**
** Author  : <sk at devhell dot org>
** Started : Tue May 28 13:06:39 2002
** Updated : Sun Mar 21 00:39:09 2004
*/

#include <libasm.h>
#include <libasm-int.h>

/*
  CommonType:int
  CommonProto:(struct s_instr *, u_char *, int, struct dis_i386 *)
  FunctionName:operand_rv_m

  destination operand is a register.
  
  @param ...
  @param length of opcode
  @param a pointer to a processor structure.
*/


int	operand_rv_m(asm_instr *ins, u_char *opcode, int len, 
		     asm_processor *proc) {
  asm_modrm	*modrm;
  asm_sidbyte	*sidbyte;

  modrm = (struct s_modrm *) opcode;
  sidbyte = (struct s_sidbyte *) opcode + 1;
  
  switch(modrm->mod) {
  case 0:

    
    if (modrm->m == ASM_REG_ESP) {
      if (sidbyte->base != ASM_REG_EBP) {
	ins->op2.content = 
	  ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
	ins->op2.regset = ASM_REGSET_R32;
	ins->op2.len = 2;
	
	ins->op2.base_reg  = sidbyte->base;
	ins->op2.index_reg  = sidbyte->index;
	ins->op2.scale = asm_int_pow2(sidbyte->sid);
      } else {
	// lea 0x0(,%eax,4),%ebx 8d 1c 85 00 00 00 00
	ins->op2.content = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_INDEX | ASM_OP_SCALE;
	ins->op2.regset = ASM_REGSET_R32;
	ins->op2.ptr = opcode;
	ins->op2.len = 6;
	
	ins->op2.index_reg = sidbyte->index;
	ins->op2.scale = asm_int_pow2(sidbyte->sid);
	
	memcpy(&ins->op2.imm, opcode + 2, 4);
      }     
    } else {
      ins->op2.content = ASM_OP_REFERENCE | ASM_OP_BASE;
      ins->op2.ptr = opcode + 2;
	ins->op2.regset = ASM_REGSET_R32;
      ins->op2.len = 4;
      
      ins->op2.base_reg = modrm->m;
    }
    break;
    
  case 1:
    
    
    if (modrm->m == ASM_REG_ESP) {
      if (sidbyte->index == ASM_REG_ESP) {
	ins->op2.content = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_SCALE;
	ins->op2.ptr = opcode + 1;
	ins->op2.regset = ASM_REGSET_R32;
	ins->op2.len = 3;
	
	if (*(opcode + 2) > 0x80u)
	  memcpy((char *) &ins->op2.imm + 1, "\xff\xff\xff", 3);
	else
	  ins->op2.imm = 0;
	memcpy(&ins->op2.imm, opcode + 2, 1);
	
	ins->op2.base_reg = sidbyte->base;
	ins->op2.scale = 1;
	
	
      } else {
	ins->op2.content = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
	ins->op2.len = 3;
	ins->op2.regset = ASM_REGSET_R32;
	ins->op2.ptr = opcode + 1;

	ins->op2.base_reg = sidbyte->base;
	ins->op2.index_reg = sidbyte->index;
	ins->op2.scale = asm_int_pow2(sidbyte->sid);

	if (*(opcode + 2) > 0x80u)
	  memcpy((char *) &ins->op2.imm + 1, "\xff\xff\xff", 3);
	else
	  ins->op2.imm = 0;
	memcpy(&ins->op2.imm, opcode + 2, 1);
      }
    } else {
      // lea 0xffffff80(%ebp),%ebx ; opcode = '8d 5d 80'      
      ins->op2.content = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_VALUE;
      ins->op2.len = 2;
	ins->op2.regset = ASM_REGSET_R32;
      ins->op2.ptr = opcode;
	
      ins->op2.base_reg = modrm->m;

	
      ins->op2.imm = 0;
      if (*(opcode + 1) >= 0x80u)
	memcpy((char *) &ins->op2.imm + 1, "\xff\xff\xff", 3);
      memcpy(&ins->op2.imm, opcode + 1, 1);
    }
    break;
    
  case 2:
    
    

    if (modrm->m == ASM_REG_ESP) {
      if (sidbyte->index == ASM_REG_ESP) {
	
	ins->op2.content = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_SCALE;
	ins->op2.len = 6;
	
	ins->op2.base_reg = sidbyte->base;
	ins->op2.regset = ASM_REGSET_R32;
	ins->op2.scale = asm_int_pow2(sidbyte->sid);
	memcpy(&ins->op2.imm, opcode + 2, 4);
	

      } else {
      
	ins->op2.ptr = opcode;
	ins->op2.content = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
	ins->op2.len = 6;

	ins->op2.regset = ASM_REGSET_R32;
	ins->op2.content = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;

	ins->op2.imm = 0;
	memcpy(&ins->op2.imm, opcode + 2, 4);
      
	ins->op2.base_reg = sidbyte->base;
	ins->op2.index_reg = sidbyte->index;
	ins->op2.scale = asm_int_pow2(sidbyte->sid); 
      
      }    
    } else /* modrm->m != ASM_REG_ESP */ {
      
      ins->op2.content = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_VALUE;
      ins->op2.ptr = opcode;
	ins->op2.regset = ASM_REGSET_R32;
      ins->op2.len = 5;
      ins->op2.base_reg = modrm->m;

      
      ins->op2.imm = 0;
      memcpy(&ins->op2.imm, opcode + 1, 4);
      
    }
    break;
  case 3:
    
    ins->op2.ptr = opcode;
    ins->op2.content = ASM_OP_REFERENCE | ASM_OP_BASE;
	ins->op2.regset = ASM_REGSET_R32;
    ins->op2.len = 1;

    ins->op2.base_reg = modrm->m;
    break;
  } 
	
  ins->op1.ptr = opcode;
	ins->op1.regset = asm_proc_oplen(proc) ? 
	  ASM_REGSET_R16 : ASM_REGSET_R32;
  ins->op1.content = ASM_OP_BASE;
  ins->op1.len = 0;

  ins->op1.base_reg = modrm->r;
  
  ins->len += ins->op1.len + ins->op2.len;
  
  return (1);
}

/*
  FunctionName:operand_rb_rmb
  
*/

int	operand_rb_rmb(asm_instr *ins, u_char *opcode, int len, 
		       asm_processor *proc) {
  struct s_modrm	*modrm;

  modrm = (struct s_modrm *) opcode;
  operand_rmb(&ins->op2, opcode, len, proc);
  
  /*
  operand_rb(&ins->op1, opcode, len, proc);
  */
  ins->op1.content = ASM_OP_BASE;
  ins->op1.regset = ASM_REGSET_R8;
  ins->op1.len = 0;
  ins->op1.ptr= opcode;
  ins->op1.base_reg = (modrm->r);
  
  ins->len += ins->op1.len + ins->op2.len;
  
  return (1);
}

/*
  FunctionName:operand_rmb_rb
  
  in any case, source is a byte register.
  if ModRM is 00, destination is a byte register.
  if ModRM is 01, destination reference and a byte offset
  if ModRM is 02, is a register reference and a vector offset
  if ModRM is 03, destination is a register reference

 */
// to correct
int	operand_rmb_rb(asm_instr *ins, u_char *opcode, int len, 
		       asm_processor *proc) {
  struct s_modrm	*modrm;

  modrm = (struct s_modrm *) opcode;
  
  operand_rmb(&ins->op1, opcode, len, proc);
  
  ins->op2.ptr = opcode;
  ins->op2.len = 0;
  
  ins->op2.content = ASM_OP_BASE;
  ins->op2.regset = ASM_REGSET_R8;
  ins->op2.base_reg = (modrm->r);
  
  ins->len += ins->op1.len + ins->op2.len;
  
  return (1);
  
}

/*
  FunctionName:operand_rmv_rv

  
*/

int	operand_rmv_rv(asm_instr *ins, u_char *opcode, int len, 
		       asm_processor *proc) {
  struct s_modrm	*modrm;
  
  modrm = (struct s_modrm *) opcode;
  operand_rmv(&ins->op1, opcode, len, proc);
  
  ins->op2.content = ASM_OP_BASE;
  ins->op2.regset = asm_proc_oplen(proc) ?
      ASM_REGSET_R16 : ASM_REGSET_R32;
  ins->op2.ptr = opcode;
  ins->op2.len = 0;
  ins->op2.base_reg = modrm->r;
  
  ins->len += ins->op1.len + ins->op2.len;
  return (1);
}

/*
  FunctionName:operand_rv_rmv
    ModRM byte
    Mod:2
    R:3
    M:3
    Mod = 0b00 -> dst is a register
    Mod = 0b01 -> dst a register followed by a byte offset
    Mod = 0b10 -> dst a register reference followed by a vector offset
    Mod = 0b11 -> dst is a register reference


 */


int	operand_rv_rmv(asm_instr *ins, u_char *opcode, int len, 
		       asm_processor *proc) {
  struct s_modrm	*modrm;
  
  modrm = (struct s_modrm *) opcode;
  operand_rmv(&ins->op2, opcode, len, proc);
  
  ins->op1.content = ASM_OP_BASE;
  ins->op1.regset = asm_proc_oplen(proc) ? 
    ASM_REGSET_R16 : ASM_REGSET_R32;
  ins->op1.ptr = opcode;
  ins->op1.len = 0;
  
  ins->op1.base_reg = modrm->r;
  ins->len += ins->op1.len + ins->op2.len;
  
  return (1);
}


/*
  FunctionName:operand_rv_rmb

 */

int	operand_rv_rmb(asm_instr *ins, u_char *opcode, int len, 
		       asm_processor *proc) {
  struct s_modrm	*modrm;

  modrm = (struct s_modrm *) opcode;

  
  ins->op1.content = ASM_OP_BASE;
  ins->op1.regset = asm_proc_oplen(proc) ? 
    ASM_REGSET_R16 : ASM_REGSET_R32;
  ins->op1.len = 0;
  ins->op1.ptr = opcode;
  ins->op1.base_reg = modrm->r;
  operand_rmb(&ins->op2, opcode, len, proc);
  ins->len += ins->op2.len ? ins->op2.len : 1;
  return (1);
}


/*
  FunctionName:operand_rv_rm2

 */

int	operand_rv_rm2(asm_instr *ins, u_char *opcode, int len, 
		       asm_processor *proc) {
  struct s_modrm	*modrm;
  struct s_sidbyte	*sidbyte;
  
  modrm = (struct s_modrm *) opcode;
  sidbyte = (struct s_sidbyte *) opcode + 1;

  switch(modrm->mod) {
  case 0:
    ins->op1.content = ASM_OP_BASE;
    ins->op1.regset = ASM_REGSET_R32;
    ins->op1.len = 0;
    ins->op1.ptr = opcode;
    ins->op1.base_reg = modrm->r;

    if (modrm->m == ASM_REG_EBP) {
      // movzwl 0x807a446,%eax ; opcode = '0f b7 05 46 a4 07 08'
      ins->op2.len = 5;
      ins->op2.content = ASM_OP_REFERENCE | ASM_OP_VALUE;
      ins->op2.ptr = opcode + 1;
      
      memcpy(&ins->op2.imm, opcode + 1, 4);
    } else if (modrm->m == ASM_REG_ESP) {
      if (sidbyte->base == ASM_REG_EBP) {
	ins->op2.content = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_INDEX | ASM_OP_SCALE;
	ins->op2.len = 6;
	ins->op2.ptr = opcode;
	ins->op2.regset = ASM_REGSET_R32;
	
	ins->op2.index_reg = sidbyte->index;
	ins->op2.scale = asm_int_pow2(sidbyte->sid);
	memcpy(&ins->op2.imm, opcode + 2, 4);

      } else {
	ins->op2.content = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
	ins->op2.len = 2;
	ins->op2.ptr = opcode;
	ins->op2.regset = ASM_REGSET_R32;
	ins->op2.base_reg = sidbyte->base;
	ins->op2.index_reg = sidbyte->index;
	ins->op2.scale = asm_int_pow2(sidbyte->sid);
      }
    } else {
  
      ins->op2.content = ASM_OP_REFERENCE | ASM_OP_BASE;
      ins->op2.len = 1;
      ins->op2.ptr = opcode;
      ins->op2.regset = ASM_REGSET_R32;

      ins->op2.base_reg = modrm->m;
    }
    ins->len += ins->op1.len + ins->op2.len; 
    break;
  case 1:
    ins->op1.content = ASM_OP_BASE;
    ins->op1.regset = ASM_REGSET_R32;
    ins->op1.len = 0;
    ins->op1.ptr = opcode;
    ins->op1.base_reg = modrm->r;
    
    if (modrm->m == ASM_REG_ESP) {

      ins->op2.regset = ASM_REGSET_R32;
      ins->op2.content = ASM_OP_REFERENCE | ASM_OP_VALUE | 
	ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
      ins->op2.ptr = opcode;
      ins->op2.len = 2;
    
    
      if (*(opcode + 2) >= 0x80u)
	memcpy((char *) &ins->op2.imm + 1, "\xff\xff\xff", 3);
      else
	ins->op2.imm = 0;
      memcpy(&ins->op2.imm, opcode + 2, 1);
      ins->op2.base_reg = sidbyte->base;
      ins->op2.index_reg = sidbyte->index;
      ins->op2.scale = asm_int_pow2(sidbyte->sid);
      ins->len += 3;

    } else {
      ins->op2.regset = ASM_REGSET_R32;
      ins->op2.content = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE;
      ins->op2.ptr = opcode;
      ins->op2.len = 2;
    
    
      if (*(opcode + 1) >= 0x80u)
	memcpy((char *) &ins->op2.imm + 1, "\xff\xff\xff", 3);
      else
	ins->op2.imm = 0;
      memcpy(&ins->op2.imm, opcode + 1, 1);
      ins->op2.base_reg = modrm->m;

      ins->len += 2;
    }
    break;
  case 2:

    ins->op1.regset = ASM_REGSET_R32;
    ins->op1.content = ASM_OP_BASE;
    ins->op1.len = 0;
    ins->op1.ptr = opcode;
    ins->op1.base_reg = modrm->r;
    
    ins->op2.regset = ASM_REGSET_R32;
    switch(modrm->m) {
    case ASM_REG_ESP:
      // movzwl 0x888(%ebx,%esi,1),%eax ; opcode = '0f b7 84 33 88 08 00 00'
      ins->op2.content = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
      ins->op2.base_reg = sidbyte->base;
      ins->op2.index_reg = sidbyte->index;
      ins->op2.scale = asm_int_pow2(sidbyte->sid);
      ins->op2.len = 6;
      memcpy(&ins->op2.imm, opcode + 2, 4);
      break;
    default:
      ins->op2.content = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_VALUE;
      ins->op2.ptr = opcode;
      ins->op2.len = 5;
      memcpy(&ins->op2.imm, opcode + 1, 4);
      ins->op2.base_reg = modrm->m;
    }
    ins->len += ins->op1.len + ins->op2.len;
    break;
  case 3:
    ins->op1.content = ASM_OP_BASE;
    ins->op1.regset = ASM_REGSET_R32;
    ins->op1.len = 1;
    ins->op1.ptr = opcode;
    ins->op1.base_reg = modrm->r;

    ins->op2.content = ASM_OP_BASE;
    ins->op2.regset = ASM_REGSET_R16;
    ins->op2.len = 0;
    ins->op2.ptr = opcode;
    ins->op2.base_reg = modrm->m;

    ins->len += 1;
    
    break;
  }
  
  return (1);
}

/**
 * operand_rmv_iv
 *
 */

int	operand_rmv_iv(asm_instr *ins, u_char *opcode, int len, 
		       asm_processor *proc) {
  struct s_modrm	*modrm;
  
  modrm = (struct s_modrm *)opcode;
  operand_rmv(&ins->op1, opcode, len, proc);

  ins->op2.ptr = opcode + ins->op1.len;
  ins->op2.len = asm_proc_vector_len(proc);
  ins->op2.content = ASM_OP_VALUE;
  ins->op2.imm = 0;
  if (((unsigned char) *(opcode + 1)) >= 0x80)
    memcpy((char *) &ins->op2.imm + 2, "\xff\xff", 2);
  memcpy(&ins->op2.imm, ins->op2.ptr, asm_proc_vector_len(proc));
  ins->len += ins->op1.len + ins->op2.len;
  return (1);
}

/*
  FunctionName:operand_rmv_ib

  
  00: dst: imm vect  src: imm byte
  01: 
  02:
  03:
 */

int	operand_rmv_ib(asm_instr *ins, u_char *opcode, int len, 
		       asm_processor *proc) {
  struct s_modrm	*modrm;

  modrm = (struct s_modrm *) opcode;

  operand_rmv(&ins->op1, opcode, len, proc);
  ins->op2.content = ASM_OP_VALUE;
  ins->op2.ptr = opcode + 1;
  ins->op2.imm = 0;
  ins->op2.len = 1;
  if (*(opcode + ins->op1.len) >= 0x80u)
    memcpy((char *) &ins->op2.imm + 1, "\xff\xff\xff", 3);
  memcpy(&ins->op2.imm, opcode + ins->op1.len, 1);
  
  ins->len += ins->op1.len + ins->op2.len;
  
  return (1);
}

/*
  FunctionName:operand_rmb_ib
*/

int	operand_rmb_ib(asm_instr *ins, u_char *opcode, int len, 
		       asm_processor *proc) {
  struct s_modrm	*modrm;
  
  modrm = (struct s_modrm *) opcode;
  operand_rmb(&ins->op1, opcode, len, proc);
  ins->op2.content = ASM_OP_VALUE;
  ins->op2.len = 1;
  ins->op2.ptr = opcode + (ins->op1.len ? ins->op1.len : 1);
  // if (*(opcode + ins->op1.len) >= 0x80u)
  //   memcpy((char *) &ins->op2.imm + 1, "\xff\xff\xff", 3);
  // else
  ins->op2.imm = 0;
  memcpy(&ins->op2.imm, opcode + (ins->op1.len ? ins->op1.len : 1), 1);
    
  ins->len += (ins->op1.len ? ins->op1.len : 1) + ins->op2.len;
  return (1);
}

/*
  FunctionName:operand_rv_m

  destination is a register.

  if Mod == 00, source operand is a memory reference.
  
  if Mod == 01, source operand
    if source reg is ESP, reference is scalar.
  
  if Mod == 02, source operand is
    if source reg is ESP, reference is scalar.
    else source operand is a register reference with a
    
  if Mod == 03, source operand is


 */
/*
  FunctionName:operand_iv

  
*/

int operand_rmb(asm_operand *op, u_char *opcode, u_int len, 
				     asm_processor *proc) {
  struct s_modrm	*modrm;
  struct s_sidbyte	*sidbyte;

  modrm = (struct s_modrm *) (opcode);
  sidbyte = (struct s_sidbyte *) (opcode + 1);
  
  switch(modrm->mod) {
  case 0:
    
    /**
     * mod : 00
     * operand base
     * 
     */
    
    if (modrm->m == ASM_REG_ESP) { 
      if (sidbyte->base == ASM_REG_EBP) {
	op->content = ASM_OP_REFERENCE | 
	  ASM_OP_VALUE | ASM_OP_INDEX | ASM_OP_SCALE;
	op->regset = ASM_REGSET_R32;
	op->len = 6;
	op->ptr = opcode;
	op->scale = asm_int_pow2(sidbyte->sid);
	
	if (((unsigned char)*(opcode + 2)) >= 0x80)
	  memcpy((char *) &op->imm + 2, "\xff\xff", 2);
	else
	  op->imm = 0;
	memcpy((char *) &op->imm, opcode + 2, 4);
	
	op->index_reg = sidbyte->index;
      } else {
	
	if ((op->index_reg = sidbyte->index) != ASM_REG_ESP)
	  op->content = ASM_OP_REFERENCE | 
	    ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
	else
	  op->content = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_SCALE;
	op->len = 2;
      op->ptr = opcode;
	op->regset = ASM_REGSET_R32;
      
      op->base_reg = sidbyte->base;
      op->scale = asm_int_pow2(sidbyte->sid);
      
      }
    } else { 
      if (modrm->m == ASM_REG_EBP) {
	op->content = ASM_OP_REFERENCE | ASM_OP_VALUE;
	op->ptr = opcode;
	op->len = 5;
	memcpy(&op->imm, opcode + 1, 4);
      } else {
	
	op->ptr = opcode;
	op->len = 1;
	op->content = ASM_OP_REFERENCE | ASM_OP_BASE;
	
	op->base_reg = modrm->m;
	op->regset = ASM_REGSET_R32;
      }
    }
    break;
    
    /**
     * mod : 01
     * operand : base + sbyte
     * 
     */
    
  case 1:
    if (modrm->m == ASM_REG_ESP) {
      
      if ((op->index_reg = sidbyte->index) != ASM_REG_ESP)
	op->content = ASM_OP_REFERENCE | 
	  ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
      else
	op->content = ASM_OP_REFERENCE | 
	  ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_SCALE;
      op->ptr = opcode;
      op->len = 3;
	op->regset = ASM_REGSET_R32;
      op->base_reg = sidbyte->base;
      op->index_reg = sidbyte->index;
      op->scale = asm_int_pow2(sidbyte->sid);
      if (*(opcode + 2) >= 0x80u)
	memcpy((char *) &op->imm + 1, "\xff\xff\xff", 3);
      else
	op->imm = 0;
      memcpy(&op->imm, opcode + 2, 1);
    } else {
    op->content = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_VALUE;
    op->len = 2;
    op->base_reg = modrm->m;
    
	op->regset = ASM_REGSET_R32;
    if (*(opcode + 1) >= 0x80u)
      memcpy((char *) &op->imm + 1, "\xff\xff\xff", 3);
    else
      op->imm = 0;
    memcpy(&op->imm, opcode + 1, 1);
    }
    break;
  case 2:

    /**
     * mod : value 10
     * base + sdword
     *
     */
    
    op->ptr = opcode;
    
    if (modrm->m == ASM_REG_ESP) {
      if (sidbyte->base == ASM_REG_ESP)
	op->content = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
      else
	op->content = ASM_OP_REFERENCE | ASM_OP_VALUE | 
	  ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
      
      op->regset = ASM_REGSET_R32;
      op->base_reg = sidbyte->base;
      op->index_reg = sidbyte->index;
      op->scale = asm_int_pow2(sidbyte->sid);
      op->len = 6;
	
      memcpy(&op->imm, opcode + 2, 4);
      
    } else {
      
      op->content = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_VALUE;
      op->len = 5;
      
      op->regset = ASM_REGSET_R32;
      op->base_reg = modrm->m;
      op->imm = 0;
      memcpy(&op->imm, opcode + 1, 4);
    }
    
    break;
    
    /**************/

    if (modrm->m == ASM_REG_ESP)
    op->content = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_VALUE;
    op->len = 5;
    
    op->base_reg = modrm->m;
    op->regset = ASM_REGSET_R32;
    memcpy(&op->imm, opcode + 1, 4);
    
    /*
    op->base_reg;
    op->imm;
    */
    break;
  case 3:
    op->content = ASM_OP_BASE;
    op->regset = ASM_REGSET_R8;
    op->len = 1;
    op->ptr = opcode;
    
    op->base_reg = modrm->m;
    break;
  }

  return (op->len);
}


/***
 *
 *
 *
 */

int        operand_rmv(asm_operand *op, u_char *opcode, u_int len, 
		       asm_processor *proc) {
  struct s_modrm	*modrm;
  struct s_sidbyte	*sidbyte;
  
  modrm = (struct s_modrm *) (opcode);
  sidbyte = (struct s_sidbyte *) (opcode + 1);
  
  op->regset = ASM_REGSET_R32;
  switch(modrm->mod) {
  case 0:
    if (modrm->m == ASM_REG_ESP) { 
      if (sidbyte->base == ASM_REG_EBP) {
	/* pushl 0x8050fe0(,%eax,4) ; opcode = 'ff 34 85 e0 0f 05 08' */
	op->content = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_INDEX | ASM_OP_SCALE;
	op->regset = ASM_REGSET_R32;
	op->len = 6;
	op->ptr = opcode;
	op->scale = asm_int_pow2(sidbyte->sid);
	
	memcpy((char *) &op->imm, opcode + 2, 4);
	
	op->base_reg = -1;
	op->index_reg = sidbyte->index;
      } else if (sidbyte->base == ASM_REG_ESP) {
	op->content = ASM_OP_REFERENCE | ASM_OP_SCALE | ASM_OP_BASE;
	op->len = 2;
	op->base_reg = sidbyte->base;
	op->scale = asm_int_pow2(sidbyte->sid);

      } else {
	op->content = ASM_OP_REFERENCE | 
	  ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
	// op->content = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
	op->len = 2;
	op->ptr = opcode;
      
	op->regset = ASM_REGSET_R32;
	op->base_reg = sidbyte->base;
	op->scale = asm_int_pow2(sidbyte->sid);
	op->index_reg = sidbyte->index;
      }
    } else { 
      if (modrm->m == ASM_REG_EBP) {
	op->content = ASM_OP_REFERENCE | ASM_OP_VALUE;
	op->ptr = opcode;
	op->len = 5;
	memcpy(&op->imm, opcode + 1, 4);
      } else {
	
	op->ptr = opcode;
	op->len = 1;
	op->content = ASM_OP_REFERENCE | ASM_OP_BASE;
	
	op->base_reg = modrm->m;
      }
    }
    break;
    
  case 1:
    if (modrm->m == ASM_REG_ESP) {
      if (sidbyte->base == ASM_REG_ESP)
	op->content = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_SCALE;
      else
	op->content = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
      op->ptr = opcode;
      op->len = 3;
      op->regset = ASM_REGSET_R32;
      op->base_reg = sidbyte->base;
      op->index_reg = sidbyte->index;
      op->scale = asm_int_pow2(sidbyte->sid);
      if (*(opcode + 2) >= 0x80u)
	memcpy((char *) &op->imm + 1, "\xff\xff\xff", 3);
      else
	op->imm = 0;
      memcpy(&op->imm, opcode + 2, 1);
    } else {
      op->content = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_VALUE;
      op->len = 2;
      op->regset = ASM_REGSET_R32;
      op->base_reg = modrm->m;
      
      if (*(opcode + 1) >= 0x80u)
	memcpy((char *) &op->imm + 1, "\xff\xff\xff", 3);
      else
	op->imm = 0;
      memcpy(&op->imm, opcode + 1, 1);
    }
    break;
  case 2:
    if (modrm->m == ASM_REG_ESP) {
      if (sidbyte->base == ASM_REG_ESP)
	op->content = ASM_OP_REFERENCE | ASM_OP_VALUE |
	  ASM_OP_BASE | ASM_OP_SCALE;
      else
	op->content = ASM_OP_REFERENCE | ASM_OP_VALUE |
	  ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
	
      op->len = 6;
      op->ptr = opcode;
      
      op->base_reg = sidbyte->base;
      op->regset = ASM_REGSET_R32;
      op->index_reg = sidbyte->index;
      op->scale = asm_int_pow2(sidbyte->sid);
      memcpy(&op->imm, opcode + 2, 4);
	
    } else {
      
      
      op->content = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_VALUE;
      op->len = 5;
      op->ptr = opcode;
      op->regset = ASM_REGSET_R32;
      
      op->base_reg = modrm->m;
      memcpy(&op->imm, opcode + 1, 4);
    }
    break;
  case 3:
    op->content = ASM_OP_BASE;
    op->len = 1;
    op->ptr = opcode;
    op->regset = asm_proc_oplen(proc) ?
      ASM_REGSET_R16 : ASM_REGSET_R32;
    op->base_reg = modrm->m;
    break;
  }
  return (op->len);
}






