/*
** 
** 
** 
** 
**
** $Id: init_mips.c,v 1.4 2007-05-11 10:48:29 may Exp $
**
*/

#include <libasm.h>



int	fetch_mips(asm_instr *ins, u_char *buf, u_int len, asm_processor *proc)
{
  int	op6;
  int	converted;

#if __BYTE_ORDER == __LITTLE_ENDIAN
  u_char	*ptr;
  int		i;
  
  ptr = (u_char *) &converted;
  for (i = 0; i < 4; i++)
    *(ptr + i) = *(buf + 3 - i);  
#else
  memcpy((char *) &converted, buf, sizeof(converted));
#endif

  printf("opcode = [%i%i%i%i%i%i] function = [%i%i%i%i%i%i]",
	 MGETBIT(converted, 31),MGETBIT(converted, 30),MGETBIT(converted, 29),
	 MGETBIT(converted, 28),MGETBIT(converted, 27),MGETBIT(converted, 26),
	 
	 MGETBIT(converted, 5),MGETBIT(converted, 4),MGETBIT(converted, 3),
	 MGETBIT(converted, 2),MGETBIT(converted, 1),MGETBIT(converted, 0));
  op6 = mips_extract_opcode(converted);
  
  ins->len = 4;
  if (op6 == 0)
    return (mips_decode_rtype(ins, (u_char *) &converted, len, proc));
  else if (op6 & 0x2)
    return (mips_decode_jtype(ins, (u_char *) &converted, len, proc));
  else if (op6 & 0x10)
    return (mips_decode_ctype(ins, (u_char *) &converted, len, proc));
  else
    return (mips_decode_itype(ins, (u_char *) &converted, len, proc));
}



void	asm_init_mips(asm_processor *proc)
{
  
  proc->fetch = fetch_mips;
  proc->display_handle = asm_mips_display_instr;
  proc->internals = 0;
  proc->instr_table = malloc(sizeof (char*) * (ASM_MIPS_BAD + 1));
  proc->instr_table[ASM_MIPS_] = "unimp";
  proc->instr_table[ASM_MIPS_ADD] = "add";
  proc->instr_table[ASM_MIPS_ADDI] = "addi";
  proc->instr_table[ASM_MIPS_ADDIU] = "addiu";
  proc->instr_table[ASM_MIPS_ADDU] = "addu";
  proc->instr_table[ASM_MIPS_AND] = "and";
  proc->instr_table[ASM_MIPS_ANDI] = "andi";
  proc->instr_table[ASM_MIPS_BEQ] = "beq";
  proc->instr_table[ASM_MIPS_BGTZ] = "bgtz";
  proc->instr_table[ASM_MIPS_BLEZ] = "blez";
  proc->instr_table[ASM_MIPS_BNE] = "bne";
  proc->instr_table[ASM_MIPS_DIV] = "duv";
  proc->instr_table[ASM_MIPS_DIVU] = "divu";
  proc->instr_table[ASM_MIPS_J] = "j";
  proc->instr_table[ASM_MIPS_JAL] = "jal";
  proc->instr_table[ASM_MIPS_JALR] = "jalr";
  proc->instr_table[ASM_MIPS_JR] = "jr";
  proc->instr_table[ASM_MIPS_LB] = "lb";
  proc->instr_table[ASM_MIPS_LBU] = "lbu";
  proc->instr_table[ASM_MIPS_LH] = "lh";
  proc->instr_table[ASM_MIPS_LHI] = "lhi";
  proc->instr_table[ASM_MIPS_LHU] = "lhu";
  proc->instr_table[ASM_MIPS_LO] = "lo";
  proc->instr_table[ASM_MIPS_LW] = "lw";
  proc->instr_table[ASM_MIPS_MFHI] = "mfhi";
  proc->instr_table[ASM_MIPS_MFLO] = "mflo";
  proc->instr_table[ASM_MIPS_MTHI] = "mthi";
  proc->instr_table[ASM_MIPS_MTLO] = "mtlo";
  proc->instr_table[ASM_MIPS_MULT] = "mult";
  proc->instr_table[ASM_MIPS_MULTU] = "multu";
  proc->instr_table[ASM_MIPS_NOR] = "nor";
  proc->instr_table[ASM_MIPS_OR] = "or";
  proc->instr_table[ASM_MIPS_ORI] = "ori";
  proc->instr_table[ASM_MIPS_REG] = "ERR";
  proc->instr_table[ASM_MIPS_SB] = "sb";
  proc->instr_table[ASM_MIPS_SH] = "sh";
  proc->instr_table[ASM_MIPS_SLL] = "sll";
  proc->instr_table[ASM_MIPS_SLLV] = "sllv";
  proc->instr_table[ASM_MIPS_SLT] = "slt";
  proc->instr_table[ASM_MIPS_SLTI] = "slti";
  proc->instr_table[ASM_MIPS_SLTIU] = "sltiu";
  proc->instr_table[ASM_MIPS_SLTU] = "sltu";
  proc->instr_table[ASM_MIPS_SRA] = "sra";
  proc->instr_table[ASM_MIPS_SRAV] = "srav";
  proc->instr_table[ASM_MIPS_SRL] = "srl";
  proc->instr_table[ASM_MIPS_SRLV] = "srlv";
  proc->instr_table[ASM_MIPS_SUB] = "sub";
  proc->instr_table[ASM_MIPS_SUBU] = "subu";
  proc->instr_table[ASM_MIPS_SW] = "sw";
  proc->instr_table[ASM_MIPS_TRAP] = "trap";
  proc->instr_table[ASM_MIPS_XOR] = "xor";
  proc->instr_table[ASM_MIPS_XORI] = "xori";
  proc->instr_table[ASM_MIPS_BAD] = "bad";
}
