/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include "libasm.h"

int asm_mips_jal(asm_instr *ins, u_char *buf, u_int len,
                  asm_processor *proc)
{
   ins->instr = ASM_MIPS_JAL;
   asm_mips_operand_j(&ins->op[0], buf, ASM_MIPS_OTYPE_JUMP, ins);

   return 777;
}
