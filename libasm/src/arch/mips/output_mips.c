/**
 * @file output_mips.c
 * @brief This file respond for MIPS ASCII output engine...
 *
 * fix and fill
 *          - Adam 'pi3' Zabrocki
 *
 */
/* Manuel Martin - 2007 */

#include <libasm.h>

/**
 * @fn char *asm_mips_display_operand(asm_instr *ins, int num, unsigned int addr)
 * @brief Return ASCII representation of a mips operand
 *
 * @param ins Pointer to asm_instr structure.
 * @param num Now it unused.
 * @parram addr Virtual Address of instruction.
 * @return A pointer to a static buffer or NULL on error.
 */
char *asm_mips_display_operand(asm_instr *ins, int num, unsigned int addr)
{
  unsigned int i;
  static char bufer[40];
  char temp[3][12];

  asm_operand *op;
  memset(bufer,0x0,sizeof(bufer));
  for (i=0;i<3;i++) {
     op = &ins->op[i];
     memset(&temp[i][0],0x0,sizeof(temp[i]));
     switch(op->type) {

        case ASM_MIPS_OTYPE_REGISTER:

           snprintf(temp[i],sizeof(temp[i]),(i) ? ",%s" : "%s",(op->regset) ? e_mips_registers[op->baser].fpu_mnemonic : e_mips_registers[op->baser].ext_mnemonic);
           break;

        case ASM_MIPS_OTYPE_IMMEDIATE:

           snprintf(temp[i],sizeof(temp[i]),(i) ? ",%d" : "%d",(short)op->imm);
	   break;

        case ASM_MIPS_OTYPE_JUMP:

           snprintf(temp[i],sizeof(temp[i]),(i) ? ",0x%x" : "0x%x",(op->imm << 2) | ((((addr + 8) >> 28) & 0xF) << 28));
	   break;

        case ASM_MIPS_OTYPE_NONE:
        case ASM_MIPS_OTYPE_NOOP:

//        snprintf(bufer,sizeof(bufer)," ");

	   break;

        case ASM_MIPS_OTYPE_BRANCH:

           snprintf(temp[i],sizeof(temp[i]),(i) ? ",0x%x" : "0x%x",(addr+(((short)op->imm+1)*4)));
	   break;

        case ASM_MIPS_OTYPE_REGBASE:

           snprintf(temp[i],sizeof(temp[i]),"(%s)",(op->regset) ? e_mips_registers[op->baser].fpu_mnemonic : e_mips_registers[op->baser].ext_mnemonic);
	   break;

     }
  }

  for (i=0;i<3;i++)
     if (temp[i][0])
        strcat(bufer,temp[i]);

  return (bufer[0]) ? bufer : NULL;

}

/**
 * @fn char *asm_mips_display_instr(asm_instr *ins,int addr)
 * @brief Return ASCII representation of a mips instruction with operand.
 *
 * @param ins Pointer to instruction structure.
 * @param addr Virtual Address of instruction.
 * @return Pointer to a static buffer or NULL on error.
 */
char *asm_mips_display_instr(asm_instr *ins,int addr)
{
   static char buf[32+40];
   char *tmp = asm_mips_display_operand(ins,0x0,addr);

   bzero(buf,sizeof(buf));

   if (tmp)
      snprintf(buf,sizeof(buf),"%s %s",e_mips_instrs[ins->instr].mnemonic,asm_mips_display_operand(ins,0x0,addr));
   else
      snprintf(buf,sizeof(buf),"%s",e_mips_instrs[ins->instr].mnemonic);

   return buf;
}
