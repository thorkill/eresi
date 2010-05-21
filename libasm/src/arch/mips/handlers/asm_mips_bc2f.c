/**
* @file libasm/src/arch/mips/handlers/asm_mips_bc2f.c
** @ingroup MIPS_instrs
*/
/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_bc2f(asm_instr *ins, u_char *buf, u_int len,
                  asm_processor *proc)
{
   struct s_mips_decode_imm temp;

   ins->instr = ASM_MIPS_BC2F;
   ins->type =  ASM_TYPE_ARCH | ASM_TYPE_BRANCH | ASM_TYPE_CONDCONTROL;
   mips_convert_format_i(&temp, buf);
   ins->op[0].imm = temp.im;
   asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_BRANCH, ins);

   /* Exceptions: Coprocessor Unusable, Reserved instruction */

   return 4;
}
