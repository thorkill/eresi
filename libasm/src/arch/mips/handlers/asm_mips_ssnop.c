/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include "libasm.h"

int asm_mips_ssnop(asm_instr *ins, u_char *buf, u_int len,
                  asm_processor *proc)
{
   ins->instr = ASM_MIPS_SSNOP;
   asm_mips_operand_noop(&ins->op[0], buf, ASM_MIPS_OTYPE_NOOP, ins);

   return 777;
}
