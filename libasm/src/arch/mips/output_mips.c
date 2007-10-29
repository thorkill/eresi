/**
 * @defgroup output_mips Mips Ascii output engine
 * @file output_mips.c
 * @ingroup output_mips
 *
 */
/* Manuel Martin - 2007 */

#include <libasm.h>

/**
 * Return ascii representation of a mips operand
 *
 * @param ins
 * @param num
 * @parram addr
 * @return A pointer to a static buffer or NULL on error.
 */
char *asm_mips_display_operand(asm_instr *ins,int num,unsigned int addr)
{
  /**
  static char buf[32];
   asm_operand *op;

   switch(num) {
      case 1: op = &ins->op[0]; break;
      case 2: op = &ins->op[1]; break;
      case 3: op = &ins->op[2]; break;
      default: return NULL;
   }

   switch(op->type)
   {
      case ASM_MIPS_OTYPE_REGISTER:
         if(op->imm < 10)
            sprintf(buf,"%i",op->imm);
         else
            sprintf(buf,"0x%x",op->imm);
         break;
      case ASM_MIPS_OTYPE_IMMEDIATE:
         sprintf(buf,"%s",
            config_get_data(ASM_CONFIG_MIPS_REGISTER_FLAVOUR) ==
		 ASM_CONFIG_MIPS_EXTENDED_REGISTERS) ? 
            e_mips_registers[reg].ext_mnemonic : 
            e_mips_registers[reg].mnemonic;
         break;
      default:
         return "err";
   }

   return buf;
  */
  return ("unimplemented");
}

/**
 * Return ascii representation of a mips instruction
 *
 * @param ins Pointer to instruction structure.
 * @param addr Virtual address of instruction.
 * @return Pointer to a static buffer or NULL on error.
 */
char *asm_mips_display_instr(asm_instr *ins,int addr)
{
  /*
    static char buf[32];

    switch(ins->nb_op)
    {
    case 1:
    sprintf(buf,"%s %s",
    e_mips_instrs[ins->instr],
    asm_mips_display_operand(ins,1,addr));
    break;
    case 2:
    sprintf(buf,"%s %s,%s",
    e_mips_instrs[ins->instr],
    asm_mips_display_operand(ins,1,addr),
    asm_mips_display_operand(ins,2,addr));
    break;
    case 3:
    sprintf(buf,"%s %s,%s,%s",
    e_mips_instrs[ins->instr],
    asm_mips_display_operand(ins,1,addr),
    asm_mips_display_operand(ins,2,addr),
    asm_mips_display_operand(ins,3,addr));
    break;
    }

    return buf;
  */
  return ("unimplemented");
}


