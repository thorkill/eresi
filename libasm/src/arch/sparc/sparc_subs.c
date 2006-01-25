/*
** 
** sparc_subs.c in 
** 
** Author  : <sk at devhell dot org>
** Started : Tue Dec  2 20:59:30 2003
** Updated : Thu Dec  4 03:14:05 2003
*/
#include <libasm.h>

/**************************************************************************
 * op is 00
 *************************************************************************/
int	sparc_ins_branch(asm_instr *ins, u_char *buf, u_int len, 
			 asm_processor *proc) {
  struct s_opcode0_branch	*opcode;
  struct s_asm_proc_sparc	*inter;


  opcode = sparc_convert_branch(buf, proc);
  #if DEBUG_SPARC
  printf("[DEBUG_SPARC sparc_ins_branch]\n");
  printf("op = %i a = %i\n", opcode->op, opcode->a);
#endif
  inter = proc->internals;
  switch(opcode->op2)
    {
    case 0: /* unimplemented */
      break;
    case 1: /* illegal */
      break;
    case 2: /* 010 -> conditionnal branch - integer unit */
      
      
      break;
    case 3: /* illegal */
      break;
    case 4: /* 100 -> sethi */
      break;
    case 5: /* illegal */
      break;
    case 6: /* conditionnal branch - floating point unit */
      break;
    case 7: /* conditionnal branch - coprocessor */
      break;
      }
  ins->instr = inter->format0_branch_table[opcode->cond];
  ins->type = ASM_TYPE_CONDBRANCH;
  ins->op1.type = ASM_SP_OTYPE_DISPLACEMENT;
  ins->op1.type = opcode->immediate;
  ins->len = 4;
  return (4);
}

/**
 * op is 01
 */
/**************************************************************************
 * op is 01
 *************************************************************************/
int	sparc_ins_call(asm_instr *ins, u_char *buf, u_int len, 
		       asm_processor *proc) {
  struct s_format1_call	*opcode;
  
  
  opcode = sparc_convert_opcode_call(buf, proc);
  #ifdef DEBUG_SPARC
  printf("[DEBUG_SPARC sparc_ins_call]\n");
  #endif
  ins->ptr_instr = buf;
  ins->instr = ASM_SP_CALL;
  ins->type = ASM_TYPE_CALLPROC;
  ins->len = 4;
  /*
  ins->op1.type = ASM_OTYPE_JUMP;
  ins->op1.size = ASM_SIZE_30BITS;
  ins->op1.content = ASM_OP_VALUE;
  */  
  return (4);
}


int	sparc_ins_sethi(asm_instr *ins, u_char *buf, u_int len, 
			asm_processor *proc) {
  struct s_opcode0_sethi	*opcode;
  

#ifdef DEBUG_SPARC
  printf("[DEBUG_SPARC sparc_ins_sethi]\n");
  #endif
  opcode = sparc_convert_sethi(buf, proc);
  
  ins->instr = ASM_SP_SETHI;
  ins->len = 4;
  ins->nb_op = 2;
  ins->op1.type = ASM_SP_OTYPE_REGISTER;
  ins->op1.base_reg = opcode->rd;
  ins->op2.type = ASM_SP_OTYPE_IMMEDIATE;
  ins->op2.imm = opcode->immediate;
  return (4);
}


/**************************************************************************
 * op is 10
 *************************************************************************/
int	sparc_ins_format3_op2(asm_instr *ins, u_char *buf, u_int len, 
			      asm_processor *proc) {

  struct s_format3_rs		*opcode;
  struct s_asm_proc_sparc	*inter;

  if (MGETBIT(13))
    opcode = sparc_convert_format3_rs(buf, proc);
  else
    opcode = 
  
  #ifdef DEBUG_SPARC
  printf("[DEBUG_SPARC sparc_ins_format3_op2]\n");
  printf("[DEBUG_SPARC op = %i rd = %i op3 = %i"
	  "rs1 = %i zero = %i none = %i rs2 = %i]\n",
	  opcode->op, opcode->rd, opcode->op3, opcode->rs1, opcode->zero,
	  opcode->none, opcode->rs2);
#endif
  
  inter = proc->internals;
  ins->instr = inter->format3_op2_table[opcode->rd];
  ins->len = 4;
  ins->nb_op = 2;
  // ins->op1.type = 

  return (4);
}

/**************************************************************************
 * op is 11
 *************************************************************************/
int	sparc_ins_format3_op3(asm_instr *ins, u_char *buf, u_int len, 
			      asm_processor *proc) {

  struct s_format3_imm		*opcode;
  struct s_asm_proc_sparc	*inter;

  opcode = sparc_convert_format3_imm(buf, proc);
  opcode = (struct s_format3_imm *) buf;
  #ifdef DEBUG_SPARC
  printf("[DEBUG_SPARC sparc_ins_format3_op3]\n");
  printf("[DEBUG_SPARC op = %i rd = %i op3 = %i rs1 = %i sign = %i"
	  " immediate = %i]\n",
	  opcode->op, opcode->rd, opcode->op3, opcode->rs1,
	  opcode->sign, opcode->immediate);
  #endif
  inter = proc->internals;
  ins->instr = inter->format3_op3_table[opcode->rd];
  ins->len = 4;
  ins->op1.type = ASM_OTYPE_REGISTER;
  ins->op1.base_reg = opcode->rd;
  ins->op2.type = ASM_OTYPE_OFFSET;
  ins->op2.base_reg = opcode->rs1;
  // ins->op2.imm = opcode->immediate;
  
  return (4);
}







