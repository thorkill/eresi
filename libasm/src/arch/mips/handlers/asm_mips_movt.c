/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include "libasm.h"

int asm_mips_movt(asm_instr *ins, u_char *buf, u_int len,
                  asm_processor *proc)
{
   ins->instr = ASM_MIPS_MOVT;
   asm_mips_operand_mov(&ins->op[0], buf, ASM_MIPS_OTYPE_MOV, ins);

   return 777;
}
