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
int	sparc_decode_branch(asm_instr *ins, u_char *buf, u_int len, 
			 asm_processor *proc) {
  struct s_decode_branch	opcode;
  struct s_asm_proc_sparc	*inter;


  sparc_convert_branch(&opcode, buf, proc);
  #if DEBUG_SPARC
  printf("[DEBUG sparc_decode_branch op = %i a = %i\n", opcode.op, opcode.a);
#endif
  inter = proc->internals;
  switch(opcode.op2)
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
  ins->instr = inter->format0_branch_table[opcode.cond];
  ins->type = ASM_TYPE_CONDBRANCH;
  ins->nb_op = 1;
  ins->op1.type = ASM_SP_OTYPE_DISPLACEMENT;
  if (opcode.immediate & 0x200000)
    ins->op1.imm = opcode.immediate | 0xFFE00000;
    else
      ins->op1.imm = opcode.immediate;
  ins->len = 4;
  sparc_instruction2synthetic(ins);
  return (4);
}

/**
 * op is 01
 */
/**************************************************************************
 * op is 01
 *************************************************************************/
int	sparc_decode_call(asm_instr *ins, u_char *buf, u_int len, 
		       asm_processor *proc) {
  struct s_decode_call	opcode;
  
  
  sparc_convert_call(&opcode, buf, proc);
  #ifdef DEBUG_SPARC
  printf("[DEBUG_SPARC sparc_decode_call]\n");
  #endif
  ins->ptr_instr = buf;
  if (opcode.displacement != 0)
    {
      ins->instr = ASM_SP_CALL;
      ins->type = ASM_TYPE_CALLPROC;
      ins->nb_op = 1;
      ins->op1.imm = opcode.displacement;
      ins->op1.type = ASM_SP_OTYPE_DISP30;
    }
  else
    {
      ins->instr = ASM_SP_NOP;
    }
  ins->len = 4;
  /*
  ins->op1.type = ASM_OTYPE_JUMP;
  ins->op1.size = ASM_SIZE_30BITS;
  ins->op1.content = ASM_OP_VALUE;
  */  
  sparc_instruction2synthetic(ins);
  return (4);
}


int	sparc_decode_sethi(asm_instr *ins, u_char *buf, u_int len, 
			asm_processor *proc) {
  struct s_decode_sethi	opcode;
  

#ifdef DEBUG_SPARC
  printf("[DEBUG_SPARC sparc_decode_sethi]\n");
  #endif
  sparc_convert_sethi(&opcode, buf, proc);
  
  ins->instr = ASM_SP_SETHI;
  ins->len = 4;
  ins->nb_op = 2;
  ins->op1.type = ASM_SP_OTYPE_REGISTER;
  ins->op1.base_reg = opcode.rd;
  ins->op2.type = ASM_SP_OTYPE_SETHI;
  ins->op2.imm = opcode.immediate;
  sparc_instruction2synthetic(ins);
  return (4);
}


/**************************************************************************
 * op is 10
 *************************************************************************/
int	sparc_decode_format3_rs(asm_instr *ins, u_char *buf, u_int len, 
			      asm_processor *proc) {

  struct s_decode_f3rs		opcode;
  struct s_asm_proc_sparc	*inter;

  sparc_convert_format3_rs(&opcode, buf, proc);
  #ifdef DEBUG_SPARC
  printf("[DEBUG sparc_decode_format3_rs op = %i rd = %i op3 = %i"
	 "rs1 = %i zero = %i none = %i rs2 = %i]\n",
	 opcode.op, opcode.rd, opcode.op3, opcode.rs1, opcode.zero,
	 opcode.none, opcode.rs2);
#endif
  
  inter = proc->internals;
  
  if (opcode.op == 2)
    ins->instr = inter->format3_op2_table[opcode.op3];
  else
    {
      ins->instr = inter->format3_op3_table[opcode.op3];
      ins->type = sparc_type_instr(ins->instr);
    }
  ins->len = 4;
  ins->nb_op = 3;
  ins->op1.type = ASM_SP_OTYPE_REGISTER;
  ins->op1.base_reg = opcode.rd;
  ins->op2.type = ASM_SP_OTYPE_REGISTER;
  ins->op2.base_reg = opcode.rs1;
  ins->op3.type = ASM_SP_OTYPE_REGISTER;
  ins->op3.base_reg = opcode.rs2;
    
  sparc_instruction2synthetic(ins);
  return (4);
}

/**************************************************************************
 * op is 11
 *************************************************************************/
int	sparc_decode_format3_imm(asm_instr *ins, u_char *buf, u_int len, 
			      asm_processor *proc) {

  struct s_decode_f3imm		opcode;
  struct s_asm_proc_sparc	*inter;

  sparc_convert_format3_imm(&opcode, buf, proc);
  #ifdef DEBUG_SPARC
  printf("[DEBUG sparc_ins_decode_imm op = %i rd = %i op3 = %i rs1 = %i sign = %i"
	  " immediate = %i]\n",
	  opcode.op, opcode.rd, opcode.op3, opcode.rs1,
	  opcode.sign, opcode.immediate);
  #endif
  inter = proc->internals;
  if (opcode.op == 2)
    ins->instr = inter->format3_op2_table[opcode.op3];
  else
    {
      ins->instr = inter->format3_op3_table[opcode.op3];
      ins->type = sparc_type_instr(ins->instr);
    }
  ins->len = 4;
  ins->nb_op = 3;
  ins->op1.type = ASM_SP_OTYPE_REGISTER;
  ins->op1.base_reg = opcode.rd;
  ins->op2.type = ASM_SP_OTYPE_REGISTER;
  ins->op2.base_reg = opcode.rs1;
  if (opcode.immediate & 0x1000)
    ins->op3.imm = 0xFFFFF000 | opcode.immediate;
    else
      ins->op3.imm = opcode.immediate;
  ins->op3.type = ASM_SP_OTYPE_IMMEDIATE;
  
  sparc_instruction2synthetic(ins);
  return (4);
}

int	sparc_decode_other(asm_instr *ins, u_char *buf, u_int len,
			   asm_processor *proc)
{
  
  ins->len = 4;
  ins->instr = ASM_SP_BAD;
  
  sparc_instruction2synthetic(ins);
  return (4);
}




