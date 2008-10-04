/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_bc1tl(asm_instr *ins, unsigned char *buf, unsigned int len,
                  asm_processor *proc)
{
   struct s_mips_decode_imm temp;

   ins->instr = ASM_MIPS_BC1TL;
   ins->type =  ASM_TYPE_ARCH | ASM_TYPE_CONDBRANCH;
   mips_convert_format_i(&temp, buf);
   ins->op[0].imm = temp.im;
   asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_BRANCH, ins);

   /* Exceptions: Coprocessor Unusable, Reserved instruction */
   /* FPU Exceptions: Unimplemented Operation */

   return 4;
}
