/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_movf(asm_instr *ins, u_char *buf, u_int len,
                  asm_processor *proc)
{
   ins->instr = ASM_MIPS_MOVF;
   ins->type = ASM_TYPE_ARCH;
   asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_MOV, ins);

   /* Exceptions: Reserved Instruction, Coprocessor Unusable */

   return 777;
}
