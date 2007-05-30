/**
 * $Id: output_mips.c,v 1.2 2007-05-30 15:53:58 heroine Exp $
 *
 *
 *
 *
 */

#include <libasm.h>

void	complete_buffer(char *buf)
{
  while (strlen(buf) < 16)
    strcat(buf, " ");
}

void	asm_resolve_mips(void *d, u_int val, char *buf, u_int len)
{
  sprintf(buf, "0x%x", val);
}

char	*get_mips_register(int r)
{
  switch (r)
    {
    case ASM_MIPS_REG_ZERO: return ("zero");
    case ASM_MIPS_REG_RES1: return ("at");
    case ASM_MIPS_REG_V0: return ("v0");
    case ASM_MIPS_REG_V1: return ("v1");
    case ASM_MIPS_REG_A0: return ("a0");
    case ASM_MIPS_REG_A1: return ("a1");
    case ASM_MIPS_REG_A2: return ("a2");
    case ASM_MIPS_REG_A3: return ("a3");
    case ASM_MIPS_REG_T0: return ("t0");
    case ASM_MIPS_REG_T1: return ("t1");
    case ASM_MIPS_REG_T2: return ("t2");
    case ASM_MIPS_REG_T3: return ("t3");
    case ASM_MIPS_REG_T4: return ("t4");
    case ASM_MIPS_REG_T5: return ("t5");
    case ASM_MIPS_REG_T6: return ("t6");
    case ASM_MIPS_REG_T7: return ("t7");
    case ASM_MIPS_REG_S0: return ("s0");
    case ASM_MIPS_REG_S1: return ("s1");
    case ASM_MIPS_REG_S2: return ("s2");
    case ASM_MIPS_REG_S3: return ("s3");
    case ASM_MIPS_REG_S4: return ("s4");
    case ASM_MIPS_REG_S5: return ("s5");
    case ASM_MIPS_REG_S6: return ("s6");
    case ASM_MIPS_REG_S7: return ("s7");
    case ASM_MIPS_REG_T8: return ("t8");
    case ASM_MIPS_REG_T9: return ("t9");
    case ASM_MIPS_REG_RES26: return ("k0");
    case ASM_MIPS_REG_RES27: return ("k1");
    case ASM_MIPS_REG_GP: return ("gp");
    case ASM_MIPS_REG_SP: return ("sp");
    case ASM_MIPS_REG_FP: return ("fp");
    case ASM_MIPS_REG_RA: return ("ra");
    }
  return ("err");
}

void	asm_mips_dump_operand(asm_instr *ins, int num, unsigned int addr, char *buf)
{
  asm_operand	*op;
  
  switch(num)
    {
    case 1: op = &ins->op1; break;
    case 2: op = &ins->op2; break;
    case 3: op = &ins->op3; break;      
    default: return;
    }
  
  switch(op->type)
    {
    case ASM_MIPS_OTYPE_REGISTER:
      sprintf(buf, "%s", get_mips_register(op->base_reg));
      break;
    default:
      sprintf(buf, "opp[%i]err", num);
      break;
    }
}

char	*asm_mips_display_instr(asm_instr *ins, int addr)
{
  static char	buffer[1024];
  
  memset(buffer, 0, sizeof (buffer));
  strcpy(buffer, ins->proc->instr_table[ins->instr]);
  complete_buffer(buffer);
  
  if (ins->op1.type)
    {
      asm_mips_dump_operand(ins, 1, addr, buffer + strlen(buffer));
      strcat(buffer, ",");
    }
  if (ins->op2.type)
    {
      asm_mips_dump_operand(ins, 2, addr, buffer + strlen(buffer));
      strcat(buffer, ",");
    }
  if (ins->op3.type)
    {
      asm_mips_dump_operand(ins, 3, addr, buffer + strlen(buffer));
      //strcat(buffer, ",");
    }
  return (buffer);
}
