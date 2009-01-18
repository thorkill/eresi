/**
 * @file output_ia32.c
 * @ingroup libasm_ia32
** $Id$
** 
** Author  : <sk at devhell dot org>
** Started : Xxx Xxx xx xx:xx:xx 2002
** Updated : Thu Mar 11 00:40:31 2004
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 * Default resolving handler for virtual address
 * 
 */

void	asm_resolve_ia32(void *d, eresi_Addr val, char *buf, u_int len)
{
  snprintf(buf, len, XFMT,  val);
}

/**
 * Instruction label handler.
 * Return ia32 instruction label
 *
 *
 */

char	*asm_get_instr_name(asm_instr *i) 
{
  return (i->proc->instr_table[i->instr]);
}

void	output_instr(asm_instr *instr) 
{  
  printf("%10s\n", instr->proc->instr_table[instr->instr]);
}


/**
 * Return register ascii string
 * @param r Register
 * @param regset Register Set (32 bits registers, 16 bits, control ...)
 */  

char *get_reg_intel(int r, int regset) 
{
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


/**
 * Dump an operand output in att syntax to a buffer.
 * @param instr Pointer to instruction structure
 * @param num Number of operand to dump
 * @param addr Virtual address of instruction
 * @param bufptr Buffer to dump operand to
 * @bug 2007-06-16 : Raise a segfault when used lonely
 */

void		att_dump_operand(asm_instr *ins, int num, eresi_Addr addr, void *bufptr)
{
  char		resolved[256];
  eresi_Addr	addr_mask;
  int		baser;
  int		indexr;
  int		scale;
  int		imm;
  asm_operand	*op;
  char		*buffer;

  /* Fetch some necessary info */
  addr_mask = asm_proc_opsize(ins->proc) ? 0x000fffff : 0xffffffff;
  op = 0;
  buffer = bufptr;
  baser = indexr = scale = imm = 0;
  switch (num)
    {
    case 1:
      op = &ins->op[0];
      break;
    case 2:
      op = &ins->op[1];
      break;
    case 3:
      op = &ins->op[2];
      break;
    }
  asm_operand_get_immediate(ins, num, addr, &imm);
  asm_operand_get_basereg(ins, num, addr, &baser);
  asm_operand_get_indexreg(ins, num, addr, &indexr);
  asm_operand_get_scale(ins, num, addr, &scale);
    
  /* Resolve target addresses if any, dealing with real/protected mode addressing */
  if (op->type & ASM_OP_ADDRESS) 
    {
      if (op->type & ASM_OP_REFERENCE) 
	ins->proc->resolve_immediate(ins->proc->resolve_data, imm & addr_mask, resolved, 256);
      else
	{
	  addr = asm_dest_resolve(ins->proc, addr, imm + ins->len);
	  ins->proc->resolve_immediate(ins->proc->resolve_data, addr, resolved, 256);
	}
    } 
  else if (op->len == 1)
    snprintf(resolved, sizeof(resolved), "0x%02X", (u_char) imm);
  else
    ins->proc->resolve_immediate(ins->proc->resolve_data, imm, resolved, 256);

  /* Resolve any potential encoded information */
  switch (op->type) 
    {
    case ASM_OP_BASE|ASM_OP_ADDRESS:
      sprintf(buffer, "*%%%s", 
	      get_reg_intel(baser, op->regset));
      break;
    case ASM_OP_BASE:
      sprintf(buffer, "%%%s", 
	      get_reg_intel(baser, op->regset));
      break;
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
	      get_reg_intel(baser, op->regset));
      break;
    case ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_ADDRESS:
      sprintf(buffer, "*(%%%s)", 
	      get_reg_intel(baser, op->regset));
      break;
    case ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE:
      sprintf(buffer, "%s(%%%s)", resolved,
	      get_reg_intel(baser, op->regset));
      break;
    case ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_ADDRESS:
      sprintf(buffer, "*%s(%%%s)", resolved,
	      get_reg_intel(baser, op->regset));
      break;
    case 
      ASM_OP_REFERENCE | ASM_OP_ADDRESS | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE:
      sprintf(buffer, "*(%%%s,%%%s,%d)",
	      get_reg_intel(baser, op->regset),
	      get_reg_intel(indexr, op->regset), 
	      scale);
      break;
    case ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_SCALE:
      sprintf(buffer, "(%%%s,%d)", 
	      get_reg_intel(baser, op->regset),
	      scale);
      break;
    case ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE:
      sprintf(buffer, "(%%%s,%%%s,%d)",
	      get_reg_intel(baser, op->regset),
	    get_reg_intel(indexr, op->regset), 
	      scale);
      break;
    case ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_SCALE:
      sprintf(buffer, "%s(%%%s,%d)",
	      resolved,
	      get_reg_intel(baser, op->regset),
	      scale);
      break;
    case ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE | ASM_OP_VALUE:
      sprintf(buffer, "%s(%%%s,%%%s,%d)", 
	      resolved, 
	      get_reg_intel(baser, op->regset), 
	      get_reg_intel(indexr, op->regset), 
	    scale);
      break;
    case ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE | 
      ASM_OP_VALUE | ASM_OP_ADDRESS:
      sprintf(buffer, "*%s(%%%s,%%%s,%d)", 
	      resolved, 
	      get_reg_intel(baser, op->regset), 
	      get_reg_intel(indexr, op->regset), 
	      scale);
      break;
    case ASM_OP_REFERENCE | ASM_OP_INDEX | ASM_OP_VALUE | ASM_OP_SCALE |ASM_OP_ADDRESS:
      sprintf(buffer, "*%s(,%%%s,%d)",
	      resolved,
	      get_reg_intel(indexr, op->regset),
	      scale);
      break;
    case ASM_OP_REFERENCE | ASM_OP_VALUE  | ASM_OP_INDEX | ASM_OP_SCALE:
      sprintf(buffer, "%s(,%%%s,%d)",
	      resolved,
	      get_reg_intel(indexr, op->regset),
	      scale);
      break;
    case ASM_OP_REFERENCE | ASM_OP_INDEX | ASM_OP_SCALE:
      sprintf(buffer, "(,%%%s,%d)",
	      get_reg_intel(indexr, op->regset),
	      scale);
      break;
    case ASM_OP_FPU | ASM_OP_BASE:
      sprintf(buffer, "%%st");
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

/**
 *
 *
 *
 */

int	asm_operand_get_att(asm_instr *ins, int num, int opt, void *valptr)
{
  att_dump_operand(ins, num, opt, valptr);
  return (1);
}




/**
 * @brief Return at&t ascii representation of an instruction
 * @param instr Pointer to an instruction structure.
 * @param addr Address of the instruction. May be used to compute offset of branch.
 * @return A Pointer to a static buffer containing ascii instruction
 */

char	*asm_ia32_display_instr_att(asm_instr *instr, 
				    eresi_Addr addr) 
{
  static char	buffer[1024];
  int		len;
  int		margin;

  if (!instr)
    return (0);

  memset(buffer, 0, 1024);
  
  if (instr->prefix & ASM_PREFIX_LOCK)
    strcat(buffer, "lock ");
  if (instr->prefix & ASM_PREFIX_REP)
    strcat(buffer, "repz ");
  if (instr->prefix & ASM_PREFIX_REPNE)
    strcat(buffer, "repnz ");

  if (instr->instr >= 0 && instr->instr <= ASM_BAD)
    { 
      if (instr->proc->instr_table[instr->instr] != NULL)
	sprintf(buffer + strlen(buffer), "%s", instr->proc->instr_table[instr->instr]);
      else
	sprintf(buffer + strlen(buffer), "missing");
    }  
  else 
    {
      sprintf(buffer + strlen(buffer), "out_of_range");
      return (buffer);
    }

  /* Add spaces */
  if (instr->op[0].content) 
    {
      len = strlen(buffer);
      margin = (int) config_get_data(CONFIG_ASM_ATT_MARGIN_FLAG);
      while (len++ < margin)
	strcat(buffer, " ");
      
      if (instr->op[2].content) 
	{
	  asm_operand_get_att(instr, 3, addr, buffer + strlen(buffer));
	  strcat(buffer, ",");
	}
 
    if (instr->op[1].content) 
      {
	switch(instr->op[1].prefix & ASM_PREFIX_SEG) 
	  {
	  case ASM_PREFIX_ES:
	    strcat(buffer, "%es:");
	    break;
	  case ASM_PREFIX_DS:
	    strcat(buffer, "%ds:");
	    break;
	  }
	asm_operand_get_att(instr, 2, addr, buffer + strlen(buffer));
	strcat(buffer, ",");
    }
    
    switch (instr->op[0].prefix & ASM_PREFIX_SEG) 
      {
      case ASM_PREFIX_ES:
	strcat(buffer, "%es:");
	break;
      case ASM_PREFIX_DS:
	strcat(buffer, "%ds:");
	break;
      }
    
    switch (instr->prefix & ASM_PREFIX_MASK) 
      {
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
  }
  
  return(buffer);
}

