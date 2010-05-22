/**
* @file libasm/src/arch/arm/output_arm.c
** @ingroup arm
*/
#include <libasm.h>

void asm_resolve_arm(void *d, eresi_Addr val, char *buf, u_int len)
{
  uint32_t addr;
  
  addr = (uint32_t) val;
  snprintf(buf, len, "0x%X", addr);
}

char *asm_arm_get_register(int reg)
{
  switch (reg)
    {
    case ASM_ARM_REG_R0: return "r0";
    case ASM_ARM_REG_R1: return "r1";
    case ASM_ARM_REG_R2: return "r2";
    case ASM_ARM_REG_R3: return "r3";
    case ASM_ARM_REG_R4: return "r4";
    case ASM_ARM_REG_R5: return "r5";
    case ASM_ARM_REG_R6: return "r6";
    case ASM_ARM_REG_R7: return "r7";
    case ASM_ARM_REG_R8: return "r8";
    case ASM_ARM_REG_R9: return "r9";
    case ASM_ARM_REG_R10: return "r10";
    case ASM_ARM_REG_R11: return "r11";
    case ASM_ARM_REG_R12: return "r12";
    case ASM_ARM_REG_R13: return "sp";
    case ASM_ARM_REG_R14: return "lr";
    case ASM_ARM_REG_PC: return "pc";
    case ASM_ARM_REG_CPSR: return "cpsr";
    case ASM_ARM_REG_SPSR: return "spsr";
    default: return "bad";
    }
}

char *asm_arm_get_shift_type(u_int shift_type)
{
  switch (shift_type)
    {
    case ASM_ARM_SHIFT_LSL: return "lsl";
    case ASM_ARM_SHIFT_LSR: return "lsr";
    case ASM_ARM_SHIFT_ASR: return "asr";
    case ASM_ARM_SHIFT_ROR: return "ror";
    case ASM_ARM_SHIFT_RRX: return "rrx";
    default: return "bad";
    }
}

char *asm_arm_get_coprocessor(int coproc)
{
  switch (coproc)
    {
    case ASM_ARM_COPROC_P0: return "p0";
    case ASM_ARM_COPROC_P1: return "p1";
    case ASM_ARM_COPROC_P2: return "p2";
    case ASM_ARM_COPROC_P3: return "p3";
    case ASM_ARM_COPROC_P4: return "p4";
    case ASM_ARM_COPROC_P5: return "p5";
    case ASM_ARM_COPROC_P6: return "p6";
    case ASM_ARM_COPROC_P7: return "p7";
    case ASM_ARM_COPROC_P8: return "p8";
    case ASM_ARM_COPROC_P9: return "p9";
    case ASM_ARM_COPROC_P10: return "p10";
    case ASM_ARM_COPROC_P11: return "p11";
    case ASM_ARM_COPROC_P12: return "p12";
    case ASM_ARM_COPROC_P13: return "p13";
    case ASM_ARM_COPROC_P14: return "p14";
    case ASM_ARM_COPROC_P15: return "p15";
    default: return "bad";
    }
}

char *asm_arm_get_coprocessor_register(int reg)
{
  switch (reg)
    {
    case ASM_ARM_COPROC_REG_CR0: return "cr0";
    case ASM_ARM_COPROC_REG_CR1: return "cr1";
    case ASM_ARM_COPROC_REG_CR2: return "cr2";
    case ASM_ARM_COPROC_REG_CR3: return "cr3";
    case ASM_ARM_COPROC_REG_CR4: return "cr4";
    case ASM_ARM_COPROC_REG_CR5: return "cr5";
    case ASM_ARM_COPROC_REG_CR6: return "cr6";
    case ASM_ARM_COPROC_REG_CR7: return "cr7";
    case ASM_ARM_COPROC_REG_CR8: return "cr8";
    case ASM_ARM_COPROC_REG_CR9: return "cr9";
    case ASM_ARM_COPROC_REG_CR10: return "cr10";
    case ASM_ARM_COPROC_REG_CR11: return "cr11";
    case ASM_ARM_COPROC_REG_CR12: return "cr12";
    case ASM_ARM_COPROC_REG_CR13: return "cr13";
    case ASM_ARM_COPROC_REG_CR14: return "cr14";
    case ASM_ARM_COPROC_REG_CR15: return "cr15";
    default: return "bad";
    }
}

/**
 * Dump a ARM operand to a buffer.
 * @param ins Pointer to instruction structure.
 * @param num Number of the operand to dump
 * @param addr Address of the instruction
 * @param buf Buffer to store operand ascii representation.
 */

void	asm_arm_dump_operand(asm_instr *ins, int num, 
                             eresi_Addr addr, char *buf, u_int len)
{
  asm_operand *op;
  eresi_Addr address;
  u_int i, temp;
  u_char first;

  if (num > ins->nb_op)
    return;

  op = &ins->op[num-1];
  
  switch (op->content) 
    {
    case ASM_ARM_OTYPE_REGISTER:
      sprintf(buf, "%s", asm_arm_get_register(op->baser));
      if (op->indexing == ASM_ARM_ADDRESSING_PREINDEXED
          || op->indexing == ASM_ARM_ADDRESSING_POSTINDEXED)
        strcat(buf, "!");
      break;
    case ASM_ARM_OTYPE_IMMEDIATE:
      sprintf(buf, "#%i", op->imm);
      break;
    case ASM_ARM_OTYPE_REG_SCALED:
      sprintf(buf, "%s, %s",
              asm_arm_get_register(op->baser),
              asm_arm_get_shift_type(op->shift_type));
      if (op->indexr != ASM_ARM_REG_NUM)
        /* Scaled by register */
        sprintf(buf + strlen(buf), " %s",
                asm_arm_get_register(op->indexr));
      else if (op->shift_type !=  ASM_ARM_SHIFT_RRX)
        /* Scaled by immediate */
        sprintf(buf + strlen(buf), " #%i", op->imm);
      break;
    case ASM_ARM_OTYPE_REG_OFFSET:
      sprintf(buf, "[%s", asm_arm_get_register(op->baser));

      if (op->indexing == ASM_ARM_ADDRESSING_POSTINDEXED
          || op->indexing == ASM_ARM_ADDRESSING_UNINDEXED)
        strcat(buf, "]");

      if (op->indexr == ASM_ARM_REG_NUM)
        {
          /* Immediate offset */
          if (op->imm != 0)
            sprintf(buf + strlen(buf), ", #%s%i",
                    (op->offset_added ? "" : "-"),
                    op->imm);
        }
      else
        {
          sprintf(buf + strlen(buf), ", %s%s",
                  (op->offset_added ? "" : "-"),
                  asm_arm_get_register(op->indexr));
          if (op->shift_type != ASM_ARM_SHIFT_NUM)
            /* Scaled register offset */
            {
              sprintf(buf + strlen(buf), ", %s",
                      asm_arm_get_shift_type(op->shift_type));
              if (op->shift_type != ASM_ARM_SHIFT_RRX)
                sprintf(buf + strlen(buf), " #%i",
                        op->imm);
            }
        }

      if (op->indexing == ASM_ARM_ADDRESSING_PREINDEXED
          || op->indexing == ASM_ARM_ADDRESSING_OFFSET)
        strcat(buf, "]");

      if (op->indexing == ASM_ARM_ADDRESSING_PREINDEXED)
        strcat(buf, "!");

      break;
    case ASM_ARM_OTYPE_REG_LIST:
      strcat(buf, "{");
      temp = op->imm;
      first = 1;
      for (i = 0; i < 16; i++)
        {
          if (temp & 0x01)
            {
              if (!first)
                strcat(buf, ",");
              sprintf(buf + strlen(buf), "%s",
                      asm_arm_get_register(i));
              first = 0;
            }
          temp = temp >> 1;
        }
      strcat(buf, "}");
      if (op->regset == ASM_ARM_REGSET_USR)
        strcat(buf, "^");
      break;
    case ASM_ARM_OTYPE_DISP:
      address = asm_dest_resolve_arm(addr, op->imm, 0);
      ins->proc->resolve_immediate(ins->proc->resolve_data, address, buf, len);
      break;
    case ASM_ARM_OTYPE_DISP_HALF:
      address = asm_dest_resolve_arm(addr, op->imm, 1);
      ins->proc->resolve_immediate(ins->proc->resolve_data, address, buf, len);
      break;
    case ASM_ARM_OTYPE_COPROC:
      sprintf(buf, "%s", asm_arm_get_coprocessor(op->imm));
      break;
    case ASM_ARM_OTYPE_COPROC_REGISTER:
      sprintf(buf, "%s", asm_arm_get_coprocessor_register(op->baser));
      break;
    default:
      sprintf(buf, "err");
      break;
  }
}

char		*asm_arm_display_instr(asm_instr *instr, eresi_Addr addr)
{
  u_int32_t	i;
  static char 	buffer[1024];

  memset(buffer,0,sizeof(buffer));
  sprintf(buffer,"%s",instr->name);

  strcat(buffer, " ");
  for (i = 0; i < instr->nb_op; i++)
    {
      asm_arm_dump_operand(instr, i+1, addr, buffer + strlen(buffer), 1024 - strlen(buffer));
      if (i < instr->nb_op -1)
        strcat(buffer, ", ");
    }

  return buffer;
}
