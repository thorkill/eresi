/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include "libasm.h"

int asm_mips_dsubu(asm_instr *ins, u_char *buf, u_int len,
                  asm_processor *proc)
{
   ins->instr = ASM_MIPS_DSUBU;
   asm_mips_operand_r(&ins->op[0], buf, ASM_MIPS_OTYPE_REGISTER, ins);

   return 777;
}
