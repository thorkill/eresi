/*
** 
** sparc_decode.c in 
** 
** Author  : <sk at devhell dot org>
** Started : Tue Dec  2 20:59:30 2003
** Updated : Thu Dec  4 03:14:05 2003
*/

#include <libasm.h>

/**************************************************************************
 * op is 00
 *************************************************************************/
int	sparc_decode_branches(asm_instr *ins, u_char *buf, u_int len, 
			 					asm_processor *proc) {
  /* 
   * We'll use the "branch" format generically and use other formats whenever
   * it's necessary (ie. whenever the "branch" format doesn't fit)
   */ 
  struct s_decode_branch opcode;
  
  struct s_decode_pbranch opcodep;
  struct s_decode_rbranch opcoder;
  struct s_asm_proc_sparc *inter;
  
  sparc_convert_branch(&opcode, buf, proc);
  sparc_convert_pbranch(&opcodep, buf, proc);
  sparc_convert_rbranch(&opcoder, buf, proc);
  
  #if DEBUG_SPARC
    printf("[DEBUG sparc_decode_branches op = %i a = %i\n", opcode.op, opcode.a);
  #endif
  
  inter = proc->internals;
  switch(opcode.op2) {
    case 0: /* ILLTRAP */
      ins->instr = ASM_SP_ILLTRAP;
      ins->nb_op = 1;
      ins->op1.type = ASM_SP_OTYPE_DISPLACEMENT;
      ins->op1.imm = opcode.immediate;
      break;
      
    case 1: /* BPcc */
      ins->instr = inter->bcc_table[opcodep.cond];
      ins->type = ASM_TYPE_CONDBRANCH;
  	  ins->nb_op = 2;
  	  ins->op1.type = ASM_SP_OTYPE_DISPLACEMENT;
  	  ins->op1.imm = opcodep.imm;
  	  ins->op2.type = ASM_SP_OTYPE_CC;
  	  ins->op2.base_reg = opcodep.cc+4;
  	  ins->annul = opcodep.a;
  	  ins->prediction = opcodep.p;
      break;
      
    case 2: /* Bicc */
      ins->instr = inter->bcc_table[opcode.cond];
  	  ins->type = ASM_TYPE_CONDBRANCH;
  	  ins->nb_op = 1;
  	  ins->op1.type = ASM_SP_OTYPE_DISPLACEMENT;
  	  ins->op1.imm = opcode.imm;
  	  ins->annul = opcode.a;
  	  /* This will avoid having ',pn' on non-prediction branches outputs */
  	  ins->prediction = 1;    	
      break;
      
    case 3: /* BPr */
      ins->instr = inter->brcc_table[opcoder.rcond];
      ins->type = ASM_TYPE_CONDBRANCH;
  	  ins->nb_op = 2;
  	  ins->op1.type = ASM_SP_OTYPE_DISPLACEMENT;
  	  ins->op1.imm = opcoder.d16;
  	  ins->op2.type = ASM_SP_OTYPE_REGISTER;
  	  ins->op2.base_reg = opcoder.rs1;
  	  ins->annul = opcoder.a;
  	  ins->prediction = opcoder.p;
      break;
      
    case 4: /* SETHI */            
      if (!opcode.rd && !opcode.imm) {
      	ins->instr = ASM_SP_NOP;
      	ins->nb_op = 0;
      }
      else {
	    ins->instr = ASM_SP_SETHI;
	  	ins->nb_op = 2;
	  	ins->op1.type = ASM_SP_OTYPE_REGISTER;
	  	ins->op1.base_reg = opcode.rd;
	  	ins->op2.type = ASM_SP_OTYPE_SETHI;
	  	ins->op2.imm = opcode.imm;
      }  	  
      break;
      
    case 5: /* FBPfcc */
      ins->instr = inter->fbcc_table[opcodep.cond];
  	  ins->type = ASM_TYPE_CONDBRANCH;
  	  ins->nb_op = 2;
  	  ins->op1.type = ASM_SP_OTYPE_DISPLACEMENT;
  	  ins->op1.imm = opcodep.imm;
  	  ins->op2.type = ASM_SP_OTYPE_CC;
  	  ins->op2.base_reg = opcodep.cc;
  	  ins->annul = opcodep.a;
  	  ins->prediction = opcodep.p;    	      
      break;
      
    case 6: /* FBfcc */
      ins->instr = inter->fbcc_table[opcode.cond];
  	  ins->type = ASM_TYPE_CONDBRANCH;
  	  ins->nb_op = 1;
  	  ins->op1.type = ASM_SP_OTYPE_DISPLACEMENT;
  	  ins->op1.imm = opcode.imm;
  	  ins->annul = opcode.a;
  	  /* This will avoid having ',pn' on non-prediction branches outputs */
  	  ins->prediction = 1;    	
      break;
      
    case 7: /* illegal */
      break;    
  }
        
  return (4);
}

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
  ins->instr = ASM_SP_CALL;
  ins->type = ASM_TYPE_CALLPROC;
  ins->nb_op = 1;
  ins->op1.type = ASM_SP_OTYPE_DISP30;
  ins->op1.imm = opcode.displacement;  
  
  return (4);
}

/**************************************************************************
 * op is 10
 *************************************************************************/
int	sparc_decode_arithmetic(asm_instr *ins, u_char *buf, u_int len, 
			      			asm_processor *proc) {
  /* 
   * We'll use the "format3" format generically and use other formats whenever
   * it's necessary (ie. whenever the "format3" format doesn't fit)
   */
  struct s_decode_format3 opcode;
  struct s_decode_format4 opcode4;  
  
  struct s_asm_proc_sparc *inter;

  sparc_convert_format3(&opcode, buf, proc);
  sparc_convert_format4(&opcode4, buf, proc);
  
  #ifdef DEBUG_SPARC
    printf("[DEBUG sparc_decode_format3_rs op = %i rd = %i op3 = %i"
	 		"rs1 = %i zero = %i none = %i rs2 = %i]\n",
	 		opcode.op, opcode.rd, opcode.op3, opcode.rs1, opcode.zero,
	 		opcode.none, opcode.rs2);
  #endif
  
  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];
  
  switch (opcode.op3) {
  	case 0x19: /* Free/reserved opcodes */
  	case 0x1d:
  	case 0x29:
  	case 0x33:
  	case 0x3f:
  	  ins->instr = ASM_SP_BAD;
  	  break;
  	
  	case 0x25: /* Shifts */
  	case 0x26:
  	case 0x27: 
  	  /* Find the right kind of shift - (SLL/SRL/SRA){X} */ 	  
  	  ins->instr = inter->shift_table[opcode.op3 - 0x25 + 
  	  									((opcode.imm & 0x1000)?1:0)]; 
  	  
  	  ins->nb_op = 3;
  	  ins->op1.type = ASM_SP_OTYPE_REGISTER;
  	  ins->op1.base_reg = opcode.rd;  
  	  ins->op3.type = ASM_SP_OTYPE_REGISTER;
  	  ins->op3.base_reg = opcode.rs1;
  
  	  if (opcode.i == 0) {
    	ins->op2.type = ASM_SP_OTYPE_REGISTER;
  		ins->op2.base_reg = opcode.rs2;
  	  }
  	  else {  		
  		ins->op2.type = ASM_SP_OTYPE_IMMEDIATE;
  	  	ins->op2.imm = opcode.shcnt;
  	  }
  	    	
  	  break;
  	  
  	case 0x28: 
  	  if (opcode.rs1 != 15) { /* RD*(-PR) */
  	    ins->nb_op = 2;
  	    ins->op1.type = ASM_SP_OTYPE_REGISTER;
  	    ins->op1.base_reg = opcode.rd;
  	    ins->op2.type = ASM_SP_OTYPE_SREGISTER;
  	    ins->op2.base_reg = opcode.rs1;
  	  }
  	  else {  	  	
  	  	if (opcode.rd != 0)
  	  	  ins->instr = ASM_SP_BAD;
  	  	else if (opcode.i == 0) /* STBAR */ {
  	  	  ins->instr = ASM_SP_STBAR;  	  	  
  	  	}
  	  	else if (opcode.i == 1) /* MEMBAR */ {
  	  	  ins->instr = ASM_SP_MEMBAR;
  	  	  ins->nb_op = 1;
  	  	  ins->op1.type = ASM_SP_OTYPE_IMMEDIATE;
  	  	  /* operand = cmask OR mmask */
  	  	  ins->op1.imm = ((opcode.imm & 0x70) >> 4) |
  	  	  					(opcode.imm & 0xf);
  	  	}  	  	
  	  }
  	  break;
  	
  	case 0x2a: /* RDPR */
  	  if (opcode.rs1 < ASM_PREG_BAD16 || opcode.rs1 > ASM_PREG_BAD30) {
  	    ins->nb_op = 2;
  	    ins->op1.type = ASM_SP_OTYPE_REGISTER;
  	    ins->op1.base_reg = opcode.rd;
  	    ins->op2.type = ASM_SP_OTYPE_PREGISTER;
  	    ins->op2.base_reg = opcode.rs1;
  	  }
  	  else
  	    ins->instr = ASM_SP_BAD;
  	    
  	  break;
  	  
  	case 0x2b: /* FLUSHW */
  	  break;
  	  
  	case 0x2c: /* MOVcc */  	  
  	  if (opcode4.cc2 == 0) { /* fcc{0,1,2,3,4} */
  	  	ins->instr = inter->movcc_table[opcode4.cond];  	  	
  	  }
  	  else if (opcode4.cc0 == 0) { /* icc or xcc */
  	  	ins->instr = inter->movfcc_table[opcode4.cond];
  	  }
  	  else {
  	    ins->instr = ASM_SP_BAD;
  	    break;
  	  }
  	  
  	  ins->nb_op = 3;
  	  ins->op1.type = ASM_SP_OTYPE_REGISTER;
  	  ins->op1.base_reg = opcode4.rd;
  	  ins->op3.type = ASM_SP_OTYPE_CC;
  	  ins->op3.base_reg = opcode4.cc;
  	  	
  	  if (opcode.i == 0) {
    	ins->op2.type = ASM_SP_OTYPE_REGISTER;
  		ins->op2.base_reg = opcode.rs2;
  	  }
  	  else {  		
  		ins->op2.type = ASM_SP_OTYPE_IMMEDIATE;
  	  	ins->op2.imm = opcode.imm;
  	  }
  	    
  	  break;
  	  
  	case 0x2e: /* POPC */
  	  if (opcode.rs1 == 0) {
  	  	ins->instr = ASM_SP_POPC;
  	  	ins->nb_op = 2;
  	  	
  	  	ins->op1.type = ASM_SP_OTYPE_REGISTER;
  	    ins->op1.base_reg = opcode.rd;
  
  	    if (opcode.i == 0) {
    	  ins->op2.type = ASM_SP_OTYPE_REGISTER;
  		  ins->op2.base_reg = opcode.rs2;
  	    }
  	    else {  		
  		  ins->op2.type = ASM_SP_OTYPE_IMMEDIATE;
  	  	  ins->op2.imm = opcode.imm;
  	    }  	  	
  	  }
  	  else
  	    ins->instr = ASM_SP_BAD;
  	    
  	  break;
  	    
  	case 0x2f: /* MOVr */
  	  ins->instr = inter->movr_table[opcode.rcond];
  	  ins->nb_op = 3;
  	  ins->op1.type = ASM_SP_OTYPE_REGISTER;
  	  ins->op1.base_reg = opcode.rd;  
  	  ins->op3.type = ASM_SP_OTYPE_REGISTER;
  	  ins->op3.base_reg = opcode.rs1;
  
  	  if (opcode.i == 0) {
    	ins->op2.type = ASM_SP_OTYPE_REGISTER;
  		ins->op2.base_reg = opcode.rs2;
  	  }
  	  else {  		
  		ins->op2.type = ASM_SP_OTYPE_IMMEDIATE;
  	  	ins->op2.imm = opcode.imm10;
  	  }
  	  
  	  break;
  	  
  	case 0x30: /* WR*(-PR), SIR */
  	  if (opcode.rd == 1)
  	  	ins->instr = ASM_SP_BAD;
  	  else if (opcode.rd == 15) { /* SIR */
  	  	ins->instr = ASM_SP_SIR;
  	  	ins->nb_op = 1;
  	  	ins->op1.type = ASM_SP_OTYPE_IMMEDIATE;
  	  	ins->op1.imm = opcode.imm;
  	  }
  	  else { /* WR */
  	  	ins->nb_op = 3;
  	  	
  	    ins->op1.type = ASM_SP_OTYPE_SREGISTER;
  	    if (opcode.rd == 4 || opcode.rd == 5) /* can't write PC or TICK */
  	      ins->op1.base_reg = ASM_SREG_BAD;
  	    else
  	      ins->op1.base_reg = opcode.rd;
  	        
  	    ins->op3.type = ASM_SP_OTYPE_REGISTER;
  	    ins->op3.base_reg = opcode.rs1;
  
  	    if (opcode.i == 0) {
    	  ins->op2.type = ASM_SP_OTYPE_REGISTER;
  		  ins->op2.base_reg = opcode.rs2;
  	    }
  	    else {  		
  		  ins->op2.type = ASM_SP_OTYPE_IMMEDIATE;
  	  	  ins->op2.imm = opcode.imm;
  	    }
  	  }
  	  
  	  break;
  	  
  	case 0x31: /* SAVED, RESTORED */
  	  if (opcode.rd == 0)
  	    ins->instr = ASM_SP_SAVED;
  	  else if (opcode.rd == 1)
  	    ins->instr = ASM_SP_RESTORED;
  	  else
  	    ins->instr = ASM_SP_BAD;
  	    
  	  break;
  	  
  	case 0x32: /* WRPR */
  	  ins->nb_op = 3;
      ins->op1.type = ASM_SP_OTYPE_PREGISTER;
      if (opcode.rd == 31) /* can't write VER */
  	    ins->op1.base_reg = ASM_PREG_BAD16;
  	  else
  	    ins->op1.base_reg = opcode.rd;
  	        
  	  ins->op3.type = ASM_SP_OTYPE_REGISTER;
  	  ins->op3.base_reg = opcode.rs1;
  
  	  if (opcode.i == 0) {
    	ins->op2.type = ASM_SP_OTYPE_REGISTER;
  		ins->op2.base_reg = opcode.rs2;
  	  }
  	  else {  		
  		ins->op2.type = ASM_SP_OTYPE_IMMEDIATE;
  	    ins->op2.imm = opcode.imm;
  	  }
  	    
  	  break;
  	  
  	case 0x34: /* FPop1 */
  	  ins->instr = inter->fpop1_table[opcode.opf];
  	  if (opcode.opf < 0x40 || opcode.opf >= 0x70) {
  	  	ins->nb_op = 2;
  	  }
  	  else { /* 0x40 < opf < 0x69 - add, sub, mul, div */
  	  	ins->nb_op = 3;
  	  	ins->op3.type = ASM_SP_OTYPE_FREGISTER;
  	  	ins->op3.base_reg = opcode.rs1;
  	  }
  	  
  	  ins->op1.type = ASM_SP_OTYPE_FREGISTER;
  	  ins->op1.base_reg = opcode.rd;
  	  ins->op2.type = ASM_SP_OTYPE_FREGISTER;
  	  ins->op2.base_reg = opcode.rs2;
  	  
  	  break;
  	  
  	case 0x35: /* FPop2 */
  	  /* At the time of this implementation, only the 5 least-significative
  	   * bits of 'opf' really select an FPop2 operation. As the instruction
  	   * set evolves, maybe we'll need to change this in the future and build
  	   * a real table to cover all 9 bits. 
  	   */ 
  	  switch (opcode.opf & 0x1f) {
  	  	case 1:
  	  	case 2:
  	  	case 3: /* FMOV(s,d,q)cc */
  	  	  if (opcode.opf_cc < 4)
  	  	    ins->instr = inter->fmovfcc_table[(((opcode.opf & 0x1f) - 1) * 8)
  	  	    									+ opcode.cond];
  	  	  else if (opcode.opf_cc == 4 || opcode.opf_cc == 6)
  	  	    ins->instr = inter->fmovcc_table[(((opcode.opf & 0x1f) - 1) * 8)
  	  	    									+ opcode.cond];
  	  	  else {
  	  	    ins->instr = ASM_SP_BAD;
  	  	    break;
  	  	  }
  	  	  
  	  	  ins->nb_op = 3;
  	  	  ins->op1.type = ASM_SP_OTYPE_FREGISTER;
  	  	  ins->op1.base_reg = opcode.rd;
  	  	  ins->op2.type = ASM_SP_OTYPE_FREGISTER;
  	  	  ins->op2.base_reg = opcode.rs2;
  	  	  ins->op3.type = ASM_SP_OTYPE_CC;
  	  	  ins->op3.base_reg = opcode.opf_cc	;
  	  	  
  	  	  break;
  	  	case 5:
  	  	case 6:
  	  	case 7: /* FMOV(s,d,q)r */
  	  	  ins->instr = inter->fmovr_table[(((opcode.opf & 0x1f) - 6) * 8)
  	  	  									+ opcode.rcond];
  	  	  ins->nb_op = 3;
  	  	  ins->op1.type = ASM_SP_OTYPE_FREGISTER;
  	  	  ins->op1.base_reg = opcode.rd;
  	  	  ins->op2.type = ASM_SP_OTYPE_FREGISTER;
  	  	  ins->op2.base_reg = opcode.rs2;
  	  	  ins->op3.type = ASM_SP_OTYPE_REGISTER;
  	  	  ins->op3.base_reg = opcode.rs1;
  	  	  break;
  	  	case 17:
  	  	case 18:
  	  	case 19:
  	  	case 21:
  	  	case 22:
  	  	case 23: /* FCMP(s,d,q), FCMPE(s,d,q) */
  	  	  ins->instr = inter->fcmp_table[(opcode.opf & 0x1f) - 16];
  	  	  ins->nb_op = 3;
  	  	  ins->op1.type = ASM_SP_OTYPE_FREGISTER;
  	  	  ins->op1.base_reg = opcode.rs2;
  	  	  ins->op2.type = ASM_SP_OTYPE_FREGISTER;
  	  	  ins->op2.base_reg = opcode.rs1;
  	  	  ins->op3.type = ASM_SP_OTYPE_CC;
  	  	  ins->op3.base_reg = opcode.cc;
  	  	  break;
  	  	default:
  	  	  ins->instr = ASM_SP_BAD;
  	  	  break;
  	  }
  	  break;
  	  
  	case 0x36: /* IMPDEP1 */
  	case 0x37: /* IMPDEP2 */
  	  break;
  	  
  	case 0x38: /* JMPL */
  	  ins->nb_op = 2;
  	  ins->op1.type = ASM_SP_OTYPE_REGISTER;
  	  ins->op1.base_reg = opcode.rd;
  	  if (opcode.i) {
  	    ins->op2.type = ASM_SP_OTYPE_IMM_ADDRESS;
  	    ins->op2.imm = opcode.imm;
  	  }
  	  else {
  	  	ins->op2.type = ASM_SP_OTYPE_REG_ADDRESS;
  	    ins->op2.index_reg = opcode.rs2;
  	  }
  	  ins->op2.base_reg = opcode.rs1;
  	  break;
  	  
  	case 0x39: /* RETURN */
  	  ins->nb_op = 1;
  	  if (opcode.i) {
  	    ins->op1.type = ASM_SP_OTYPE_IMM_ADDRESS;
  	    ins->op1.imm = opcode.imm;
  	  }
  	  else {
  	  	ins->op1.type = ASM_SP_OTYPE_REG_ADDRESS;
  	    ins->op1.index_reg = opcode.rs2;
  	  }
  	  ins->op1.base_reg = opcode.rs1;
  	  break;
  	  
  	case 0x3a: /* Tcc */
  	  ins->instr = inter->tcc_table[opcode4.cond];
  	  ins->nb_op = 2;
  	  if (opcode4.i) {
  	    ins->op1.type = ASM_SP_OTYPE_IMM_ADDRESS;
  	    ins->op1.imm = opcode4.sw_trap;
  	  }
  	  else {
  	  	ins->op1.type = ASM_SP_OTYPE_REG_ADDRESS;
  	    ins->op1.index_reg = opcode4.rs2;
  	  }
  	  ins->op1.base_reg = opcode4.rs1;
  	  
  	  ins->op2.type = ASM_SP_OTYPE_CC;
  	  ins->op2.base_reg = (opcode4.cc & 0x3) + 4;
  	  
  	  break;
  	  
  	case 0x3b: /* FLUSH */
  	  ins->nb_op = 1;
  	  if (opcode.i) {
  	    ins->op1.type = ASM_SP_OTYPE_IMM_ADDRESS;
  	    ins->op1.imm = opcode.imm;
  	  }
  	  else {
  	  	ins->op1.type = ASM_SP_OTYPE_REG_ADDRESS;
  	    ins->op1.index_reg = opcode.rs2;
  	  }
  	  ins->op1.base_reg = opcode.rs1;
  	  break;
  	    	
  	case 0x3e: /* DONE, RETRY */
  	  if (opcode.rd == 0) /* DONE */
  	    ins->instr = ASM_SP_DONE;
  	  else if (opcode.rd == 1)
  	    ins->instr = ASM_SP_RETRY; /* RETRY */
  	  else
  	    ins->instr = ASM_SP_BAD;
  	    
  	  break;
  	  
  	default: /* Arithmetic and logical operations */
  	  ins->nb_op = 3;
  	  ins->op1.type = ASM_SP_OTYPE_REGISTER;
  	  ins->op1.base_reg = opcode.rd;  
  	  ins->op3.type = ASM_SP_OTYPE_REGISTER;
  	  ins->op3.base_reg = opcode.rs1;
  
  	  if (opcode.i == 0) {
    	ins->op2.type = ASM_SP_OTYPE_REGISTER;
  		ins->op2.base_reg = opcode.rs2;
  	  }
  	  else {  		
  		ins->op2.type = ASM_SP_OTYPE_IMMEDIATE;
  	  	ins->op2.imm = opcode.imm;
  	  }
  	    	
  	  break;
  }
      
  return (4);
}

/**************************************************************************
 * op is 11
 *************************************************************************/
int	sparc_decode_memory(asm_instr *ins, u_char *buf, u_int len, 
			      		asm_processor *proc) {

  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;

  sparc_convert_format3(&opcode, buf, proc);
  
  #ifdef DEBUG_SPARC
  printf("[DEBUG sparc_ins_decode_imm op = %i rd = %i op3 = %i rs1 = %i sign = %i"
	  " immediate = %i]\n",
	  opcode.op, opcode.rd, opcode.op3, opcode.rs1,
	  opcode.sign, opcode.immediate);
  #endif
  
  inter = proc->internals;
  ins->instr = inter->op3_table[opcode.op3];
  
  switch(opcode.op3) {
  	case 0x0: /* Load Integer */
  	case 0x1:
  	case 0x2:
  	case 0x3:
  	case 0x8:
  	case 0x9:
  	case 0xa:
  	case 0xb:
  	case 0xd: /* LDSTUB */
  	case 0xf: /* SWAP */
  	  ins->nb_op = 2;
      ins->op1.type = ASM_SP_OTYPE_REGISTER;
      ins->op1.base_reg = opcode.rd;
      
      if (opcode.i) {
  	    ins->op2.type = ASM_SP_OTYPE_IMM_ADDRESS;
  	    ins->op2.imm = opcode.imm;
  	  }
  	  else {
  	  	ins->op2.type = ASM_SP_OTYPE_REG_ADDRESS;
  	    ins->op2.index_reg = opcode.rs2;
  	  }
  	  ins->op2.base_reg = opcode.rs1;
  	  
  	  break;
  	  
  	case 0x4: /* Store Integer */
  	case 0x5:
  	case 0x6:
  	case 0x7:
  	case 0xe:
  	  ins->nb_op = 2;      
      if (opcode.i) {
  	    ins->op1.type = ASM_SP_OTYPE_IMM_ADDRESS;
  	    ins->op1.imm = opcode.imm;
  	  }
  	  else {
  	  	ins->op1.type = ASM_SP_OTYPE_REG_ADDRESS;
  	    ins->op1.index_reg = opcode.rs2;
  	  }
  	  ins->op1.base_reg = opcode.rs1;
  	  
  	  ins->op2.type = ASM_SP_OTYPE_REGISTER;
      ins->op2.base_reg = opcode.rd;
  	  
  	  break;
  	  
  	case 0x10: /* Load Integer from alternate space */
  	case 0x11:
  	case 0x12:
  	case 0x13:
  	case 0x18:
  	case 0x19:
  	case 0x1a:
  	case 0x1b:
  	case 0x1d: /* LDSTUBA */
  	case 0x1f: /* SWAPA */
  	  ins->nb_op = 2;
      ins->op1.type = ASM_SP_OTYPE_REGISTER;
      ins->op1.base_reg = opcode.rd;
      
      if (opcode.i) {
  	    ins->op2.type = ASM_SP_OTYPE_IMM_ADDRESS;
  	    ins->op2.imm = opcode.imm;
  	    /* This value doesn't matter. Just has to be != 0x80 */
  	    ins->op2.address_space = -1;
  	  }
  	  else {
  	  	ins->op2.type = ASM_SP_OTYPE_REG_ADDRESS;
  	    ins->op2.index_reg = opcode.rs2;
  	    ins->op2.address_space = opcode.none;
  	  }
  	  ins->op2.base_reg = opcode.rs1;
  	  
  	  break;
  	
  	case 0x14: /* Store Integer into alternate space */
  	case 0x15:
  	case 0x16:
  	case 0x17:
  	case 0x1e:
  	  ins->nb_op = 2;      
      if (opcode.i) {
  	    ins->op1.type = ASM_SP_OTYPE_IMM_ADDRESS;
  	    ins->op1.imm = opcode.imm;
  	    /* This value doesn't matter. Just has to be != 0x80 */
  	    ins->op1.address_space = -1;
  	  }
  	  else {
  	  	ins->op1.type = ASM_SP_OTYPE_REG_ADDRESS;
  	    ins->op1.index_reg = opcode.rs2;
  	    ins->op1.address_space = opcode.none;
  	  }
  	  ins->op1.base_reg = opcode.rs1;
  	  
  	  ins->op2.type = ASM_SP_OTYPE_REGISTER;
      ins->op2.base_reg = opcode.rd;
  	  
  	  break;
  	  
  	case 0x20: /* Load floating-point */
  	case 0x22:
  	case 0x23:
  	  ins->nb_op = 2;
      ins->op1.type = ASM_SP_OTYPE_FREGISTER;
      ins->op1.base_reg = opcode.rd;
      
      if (opcode.i) {
  	    ins->op2.type = ASM_SP_OTYPE_IMM_ADDRESS;
  	    ins->op2.imm = opcode.imm;
  	  }
  	  else {
  	  	ins->op2.type = ASM_SP_OTYPE_REG_ADDRESS;
  	    ins->op2.index_reg = opcode.rs2;
  	  }
  	  ins->op2.base_reg = opcode.rs1;
  	  
  	  break;
  	  
  	case 0x21: /* LDFSR, LDXFSR */
  	  if (opcode.rd == 0)
  	    ins->instr = ASM_SP_LDFSR;
  	  else if (opcode.rd == 1)
  	    ins->instr = ASM_SP_LDXFSR;
  	  else {
  	  	ins->instr = ASM_SP_BAD;
  	  	break;
  	  }
  	  
  	  ins->nb_op = 2;
      ins->op1.type = ASM_SP_OTYPE_FREGISTER;
      ins->op1.base_reg = ASM_FREG_FSR;
      
      if (opcode.i) {
  	    ins->op2.type = ASM_SP_OTYPE_IMM_ADDRESS;
  	    ins->op2.imm = opcode.imm;
  	  }
  	  else {
  	  	ins->op2.type = ASM_SP_OTYPE_REG_ADDRESS;
  	    ins->op2.index_reg = opcode.rs2;
  	  }
  	  ins->op2.base_reg = opcode.rs1;
  	  
  	  break;
  	  
  	case 0x24: /* Store floating-point */
  	case 0x26:
  	case 0x27:
  	  ins->nb_op = 2;      
      if (opcode.i) {
  	    ins->op1.type = ASM_SP_OTYPE_IMM_ADDRESS;
  	    ins->op1.imm = opcode.imm;
  	  }
  	  else {
  	  	ins->op1.type = ASM_SP_OTYPE_REG_ADDRESS;
  	    ins->op1.index_reg = opcode.rs2;
  	  }
  	  ins->op1.base_reg = opcode.rs1;
  	  
  	  ins->op2.type = ASM_SP_OTYPE_FREGISTER;
      ins->op2.base_reg = opcode.rd;
  	  
  	  break;
  	  
  	case 0x25: /* STFSR, STXFSR */
  	  if (opcode.rd == 0)
  	    ins->instr = ASM_SP_LDFSR;
  	  else if (opcode.rd == 1)
  	    ins->instr = ASM_SP_LDXFSR;
  	  else {
  	  	ins->instr = ASM_SP_BAD;
  	  	break;
  	  }
  	  
  	  ins->nb_op = 2;      
      if (opcode.i) {
  	    ins->op1.type = ASM_SP_OTYPE_IMM_ADDRESS;
  	    ins->op1.imm = opcode.imm;
  	  }
  	  else {
  	  	ins->op1.type = ASM_SP_OTYPE_REG_ADDRESS;
  	    ins->op1.index_reg = opcode.rs2;
  	  }
  	  ins->op1.base_reg = opcode.rs1;
  	  
  	  ins->op2.type = ASM_SP_OTYPE_FREGISTER;
      ins->op2.base_reg = ASM_FREG_FSR;
  	  
  	  break;
  	  
  	case 0x2d: /* PREFETCH */
  	  ins->nb_op = 2;
  	  ins->op1.type = ASM_SP_OTYPE_IMMEDIATE;
  	  ins->op1.imm = opcode.rd;
  	  if (opcode.i) {
  	    ins->op2.type = ASM_SP_OTYPE_IMM_ADDRESS;
  	    ins->op2.imm = opcode.imm;
  	  }
  	  else {
  	  	ins->op2.type = ASM_SP_OTYPE_REG_ADDRESS;
  	    ins->op2.index_reg = opcode.rs2;
  	  }
  	  ins->op2.base_reg = opcode.rs1;
  	  break;
  	  
  	case 0x30: /* Load floating-point from Alternate Space */
  	case 0x32:
  	case 0x33:
  	  ins->nb_op = 2;
      ins->op1.type = ASM_SP_OTYPE_FREGISTER;
      ins->op1.base_reg = opcode.rd;
      
      if (opcode.i) {
  	    ins->op2.type = ASM_SP_OTYPE_IMM_ADDRESS;
  	    ins->op2.imm = opcode.imm;
  	    /* This value doesn't matter. Just has to be != 0x80 */
  	    ins->op2.address_space = -1;
  	  }
  	  else {
  	  	ins->op2.type = ASM_SP_OTYPE_REG_ADDRESS;
  	    ins->op2.index_reg = opcode.rs2;
  	    ins->op2.address_space = opcode.none;
  	  }
  	  ins->op2.base_reg = opcode.rs1;
  	  
  	  break;
  	  
  	case 0x34: /* Store floating-point into Alternate Space */
  	case 0x36:
  	case 0x37:
  	  ins->nb_op = 2;      
      if (opcode.i) {
  	    ins->op1.type = ASM_SP_OTYPE_IMM_ADDRESS;
  	    ins->op1.imm = opcode.imm;
  	    /* This value doesn't matter. Just has to be != 0x80 */
  	    ins->op1.address_space = -1;
  	  }
  	  else {
  	  	ins->op1.type = ASM_SP_OTYPE_REG_ADDRESS;
  	    ins->op1.index_reg = opcode.rs2;
  	    ins->op1.address_space = opcode.none;
  	  }
  	  ins->op1.base_reg = opcode.rs1;
  	  
  	  ins->op2.type = ASM_SP_OTYPE_FREGISTER;
      ins->op2.base_reg = opcode.rd;
  	  
  	  break;
  	  
  	case 0x3d: /* PREFETCHA */
  	  ins->nb_op = 2;
  	  ins->op1.type = ASM_SP_OTYPE_IMMEDIATE;
  	  ins->op1.imm = opcode.rd;
  	  if (opcode.i) {
  	    ins->op2.type = ASM_SP_OTYPE_IMM_ADDRESS;
  	    ins->op2.imm = opcode.imm;
  	    /* This value doesn't matter. Just has to be != 0x80 */
  	    ins->op2.address_space = -1;
  	  }
  	  else {
  	  	ins->op2.type = ASM_SP_OTYPE_REG_ADDRESS;
  	    ins->op2.index_reg = opcode.rs2;
  	    ins->op2.address_space = opcode.none;
  	  }
  	  ins->op2.base_reg = opcode.rs1;
  	  break;
  	  
  	case 0x3c: /* CASA */
  	case 0x3e: /* CASXA */
  	  ins->nb_op = 3;
  	  ins->op1.type = ASM_SP_OTYPE_REGISTER;
      ins->op1.base_reg = opcode.rd;
  	  ins->op2.type = ASM_SP_OTYPE_REGISTER;
      ins->op2.base_reg = opcode.rs2;
      if (opcode.i) {
  	    ins->op3.type = ASM_SP_OTYPE_IMM_ADDRESS;
  	    /* Special case */
  	    ins->op3.imm = 0;
  	    /* This value doesn't matter. Just has to be != 0x80 */
  	    ins->op3.address_space = -1;
  	  }
  	  else {
  	  	ins->op3.type = ASM_SP_OTYPE_REG_ADDRESS;
  	  	/* Special case */
  	    ins->op3.index_reg = -1;
  	    ins->op3.address_space = opcode.none;
  	  }
  	  ins->op3.base_reg = opcode.rs1;
  	  
  	  break;
  	  
  	default: ins->instr = ASM_SP_BAD;  	  
  	  
  }
  
  return (4);
}
