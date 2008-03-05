/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_mfc2(asm_instr *ins, u_char *buf, u_int len,
                  asm_processor *proc)
{
   ins->instr = ASM_MIPS_MFC2;
   ins->type = ASM_TYPE_ARCH;
   asm_mips_operand_cop2(&ins->op[0], buf, ASM_MIPS_OTYPE_COP2, ins);

   /* Exceptions: Coprocesor Unusable */

   return 777;
}
