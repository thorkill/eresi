/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_bal(asm_instr *ins, unsigned char *buf, unsigned int len,
                  asm_processor *proc)
{
   struct s_mips_decode_imm temp;

   ins->instr = ASM_MIPS_BAL;
   ins->type =  /* ASM_TYPE_IMPBRANCH */ ASM_TYPE_CALLPROC;
   mips_convert_format_i(&temp, buf);
   ins->op[0].imm = temp.im;
   asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_BRANCH, ins);

   /* Exceptions: None */

   return 4;
}
