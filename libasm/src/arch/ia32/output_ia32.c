/*
** private - do not distribute
** 
** Author  : <sk at devhell dot org>
** Started : Xxx Xxx xx xx:xx:xx 2002
** Updated : Thu Mar 11 00:40:31 2004
*/

/*
  Sat Jul 13 00:15:51 2002
  removed a bug in scalar output.

*/

#include <libasm.h>
#include <libasm-int.h>

void	asm_resolve_ia32(void *d, u_int val, char *buf, u_int len)
{
  sprintf(buf, "0x%x", len);
}

char	*asm_get_instr_name(asm_instr *i) {
  return (i->proc->instr_table[i->instr]);
}

void	output_instr(asm_instr *instr) {
  
  printf("%10s  ", instr->proc->instr_table[instr->instr]);
  if (instr->op1.type) {
    switch(instr->op1.content) {
      
    }
  } /* !instr->op1 */ else {
    
  }
  /*
  printf("\t;; len: %5u   ", instr->len);
  if (instr->type & IS_MEM_WRITE)
    printf("MW : Y ");
  else
    printf("MW : N ");

  if (instr->type & IS_MEM_READ)
    printf("MR : Y   ");
  else
    printf("MR : N   ");
  if (instr->type & IS_CALL)
    printf("CALL : Y    ");
  else 
    printf("CALL : N    ");
  if (instr->type & IS_JMP)
    printf("JMP : Y  ");
  else
    printf("JMP : N  ");
  if (instr->type & IS_COND_BRANCH)
    printf("CONDBR : Y  "); 
  else
    printf("CONDBR : N   ");
  */
  puts("");
    
}
  

char *get_reg_intel(u_int r, int regset) {
  char	*rsub[8] ={ "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh" };
  char	*r16[8] = { "ax", "cx", "dx", "bx", "sp", "bp", "si", "di" };
  char	*r32[8] = { "eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi" };
  char	*rseg[8] = { "es", "cs", "ss", "ds", "fs", "gs", "#@!", "#@!" };
  char	*rmm[8] = { "mm0", "mm1", "mm2", "mm3", "mm4", "mm5", "mm6", "mm7"};
  char	*rxmm[8] = {"xmm0", "xmm1", "xmm2","xmm3", "xmm4", "xmm5","xmm6", "xmm7"};
  char	*rcr[8] = { "cr0", "cr1", "cr2", "cr3", "cr4", "cr5", "cr6", "cr7"};
  char	*rdr[8] = { "db0", "db1", "db2", "db3", "db4", "db5", "db6", "db7"};
  char	*rdefault[8] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
  
  if (r >= 0 && r < 8)
    switch(regset) {
    case ASM_REGSET_R8:
      return (rsub[r]);
    case ASM_REGSET_R16:
      return (r16[r]);
    case ASM_REGSET_R32:
      return (r32[r]);
    case ASM_REGSET_XMM:
      return (rxmm[r]);
    case ASM_REGSET_CREG:
      return (rcr[r]);
    case ASM_REGSET_DREG:
      return (rdr[r]);
    case ASM_REGSET_MM:
      return (rmm[r]);
    case ASM_REGSET_SREG:
      return (rseg[r]);
    default:
      return (rdefault[r]);
    }
  return ("\?\?\?");
}


/*********
 *
 *
 *
 */

void	att_dump_operand(asm_instr *ins, int num, unsigned int addr,
			 void *bufptr) {
  char	resolved[256];
  int	base_reg;
  int	index_reg;
  int	scale;
  int	imm;
  asm_operand *op;
  char	*buffer;

  buffer = bufptr;
    
  base_reg = index_reg = scale = imm = 0;

  switch(num)
    {
    case 1:
      op = &ins->op1;
      break;
    case 2:
      op = &ins->op2;
      break;
    case 3:
      op = &ins->op3;
      break;
    }


  // if (op->content & ASM_OP_VALUE)
    asm_operand_get_immediate(ins, num, addr, &imm);
    // if (op->content & ASM_OP_BASE)
    asm_operand_get_basereg(ins, num, addr, &base_reg);
    // if (op->content & ASM_OP_INDEX)
    asm_operand_get_indexreg(ins, num, addr, &index_reg);
    // if (op->content & ASM_OP_SCALE)
    asm_operand_get_scale(ins, num, addr, &scale);
  
  if (ASM_OP_ADDRESS & op->content) {
    if (ASM_OP_REFERENCE & op->content) 
      op->proc->resolve_immediate(op->proc->resolve_data, 
					     imm, resolved, 256);
    else if (ASM_OP_VALUE & op->content)
      op->proc->resolve_immediate(op->proc->resolve_data, 
					     imm + addr + ins->len, resolved, 256);
    else
      op->proc->resolve_immediate(op->proc->resolve_data, 
					     imm + addr + ins->len, resolved, 256);
  } else
    op->proc->resolve_immediate(op->proc->resolve_data,
					   imm, resolved, 256);
  
  switch(op->content & ~ASM_OP_FIXED) {
  case ASM_OP_BASE|ASM_OP_ADDRESS:
    sprintf(buffer, "*%%%s", 
	    get_reg_intel(base_reg, op->regset));
    break;
  case ASM_OP_BASE:
    sprintf(buffer, "%%%s", 
	    get_reg_intel(base_reg, op->regset));
    break;
    /*
      case OP_SUBREG: 
      sprintf(buffer, "%%%s",  
      get_subreg_intel(base_reg)); 
      break;
      case OP_SEGREG:
      break;
    */
  case ASM_OP_VALUE:
    sprintf(buffer, "$%s", 
	    resolved);
    break;
  case ASM_OP_VALUE | ASM_OP_ADDRESS:
    sprintf(buffer, "%s", 
	    resolved);
    break;
  case ASM_OP_REFERENCE | ASM_OP_VALUE:
    sprintf(buffer, "%s", 
	    resolved);
    break;
  case ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_ADDRESS:
    sprintf(buffer, "*%s", 
	    resolved);
    break;
  case ASM_OP_REFERENCE | ASM_OP_BASE:
    sprintf(buffer, "(%%%s)", 
	    get_reg_intel(base_reg, op->regset));
    break;
  case ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_ADDRESS:
    sprintf(buffer, "*(%%%s)", 
	    get_reg_intel(base_reg, op->regset));
    break;
  case ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE:
    sprintf(buffer, "%s(%%%s)", 
	    resolved
	    , get_reg_intel(base_reg, op->regset));
    break;
  case ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_ADDRESS:
    sprintf(buffer, "*%s(%%%s)", 
	    resolved
	    , get_reg_intel(base_reg, op->regset));
    break;
  case 
    ASM_OP_REFERENCE | ASM_OP_ADDRESS | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE:
      sprintf(buffer, "*(%%%s,%%%s,%d)",
	      get_reg_intel(base_reg, op->regset),
	      get_reg_intel(index_reg, op->regset), 
	      scale);
  break;
  case ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_SCALE:
    sprintf(buffer, "(%%%s,%d)", 
	    get_reg_intel(base_reg, op->regset),
	    scale);
    break;
  case ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE:
    sprintf(buffer, "(%%%s,%%%s,%d)",
	    get_reg_intel(base_reg, op->regset),
	    get_reg_intel(index_reg, op->regset), 
	    scale);
    break;
  case ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_SCALE:
    sprintf(buffer, "%s(%%%s,%d)",
	    resolved,
	    get_reg_intel(base_reg, op->regset),
	    scale);
    break;
  case ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE | ASM_OP_VALUE:
    sprintf(buffer, "%s(%%%s,%%%s,%d)", 
	    resolved, 
	    get_reg_intel(base_reg, op->regset), 
	    get_reg_intel(index_reg, op->regset), 
	    scale);
    break;
    case ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE | 
      ASM_OP_VALUE | ASM_OP_ADDRESS:
	sprintf(buffer, "*%s(%%%s,%%%s,%d)", 
		resolved, 
		get_reg_intel(base_reg, op->regset), 
		get_reg_intel(index_reg, op->regset), 
		scale);
    break;
  case ASM_OP_REFERENCE | ASM_OP_INDEX | ASM_OP_VALUE | ASM_OP_SCALE |ASM_OP_ADDRESS:
    sprintf(buffer, "*%s(,%%%s,%d)",
	    resolved,
	    get_reg_intel(index_reg, op->regset),
	    scale);
    break;
  case ASM_OP_REFERENCE | ASM_OP_VALUE  | ASM_OP_INDEX | ASM_OP_SCALE:
    sprintf(buffer, "%s(,%%%s,%d)",
	    resolved,
	    get_reg_intel(index_reg, op->regset),
	    scale);
    break;
  case ASM_OP_REFERENCE | ASM_OP_INDEX | ASM_OP_SCALE:
    sprintf(buffer, "(,%%%s,%d)",
	    get_reg_intel(index_reg, op->regset),
	    scale);
    break;
  case ASM_OP_FPU | ASM_OP_BASE:
    strcat(buffer, "%st");
    break;
  case ASM_OP_FPU | ASM_OP_BASE | ASM_OP_SCALE:
    sprintf(buffer, "%%st(%d)", scale);
    break;
    
  case 0:
    break;
  default:
    sprintf(buffer, "(...)");
  }
}

/**********88
 *
 *
 *
 */


int	asm_operand_get_att(asm_instr *ins, int num, int opt, void *valptr)
{
  att_dump_operand(ins, num, opt, valptr);
  return (1);
}




/*

 */

char	*asm_ia32_display_instr_att(asm_instr *instr, 
			       int addr) {
  static char	buffer[1024];

  if (!instr)
    return (0);
  memset(buffer, 0, 1024);
  
  if (instr->prefix & ASM_PREFIX_REP)
    strcat(buffer, "repz ");
  if (instr->prefix & ASM_PREFIX_REPNE)
    strcat(buffer, "repnz ");
  
  if (instr->instr >= 0 && instr->instr <= ASM_BAD && instr->proc->instr_table[instr->instr] != NULL) 
    sprintf(buffer + strlen(buffer), "%s", instr->proc->instr_table[instr->instr]);
  else 
    {
      sprintf(buffer + strlen(buffer), "int_err");
      return (buffer);
    }
  
  
  if (instr->op1.type) {
    instr->op1.proc = instr->proc;
    while(strlen(buffer) < 14)
      strcat(buffer, " ");
    if (instr->op3.type) {
      instr->op3.proc = instr->proc;
      asm_operand_get_att(instr, 3, addr, buffer + strlen(buffer));
      /*
      att_dump_operand(buffer + strlen(buffer), &instr->op3,
		       addr + instr->len);
      */    
      strcat(buffer, ",");
    }
 
    if (instr->op2.type) {
      instr->op2.proc = instr->proc;
      switch(instr->op2.prefix & ASM_PREFIX_SEG) {
      case ASM_PREFIX_ES:
	strcat(buffer, "%es:");
	break;
      case ASM_PREFIX_DS:
	strcat(buffer, "%ds:");
	break;
      }
      asm_operand_get_att(instr, 2, addr, buffer + strlen(buffer));
      /*
      att_dump_operand(buffer + strlen(buffer), &instr->op2, 
		       addr + instr->len);
      */
      strcat(buffer, ",");
    }
      switch(instr->op1.prefix & ASM_PREFIX_SEG) {
      case ASM_PREFIX_ES:
	strcat(buffer, "%es:");
	break;
      case ASM_PREFIX_DS:
	strcat(buffer, "%ds:");
	break;
      }
      
      switch (instr->prefix & ASM_PREFIX_MASK) {
      case ASM_PREFIX_CS:
	strcat(buffer, "%cs:");
	break;
      case ASM_PREFIX_ES:
	strcat(buffer, "%es:");
	break;
      case ASM_PREFIX_DS:
	strcat(buffer, "%ds:");
	break;
      case ASM_PREFIX_GS:
	strcat(buffer, "%gs:");
	break;
      case ASM_PREFIX_FS:
	strcat(buffer, "%fs:");
	break;
      case ASM_PREFIX_SS:
	strcat(buffer, "%ss:");
	break;
      }
      asm_operand_get_att(instr, 1, addr, buffer + strlen(buffer));
      /*
      att_dump_operand(buffer + strlen(buffer), &instr->op1, 
		       addr + instr->len);
      */
  }
  
  return(buffer);
}

