/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_j(asm_instr *ins, unsigned char *buf, unsigned int len,
                  asm_processor *proc)
{
   struct s_mips_decode_jump temp;

   ins->instr = ASM_MIPS_J;
   ins->type = ASM_TYPE_IMPBRANCH;
   mips_convert_format_j(&temp,buf);
   ins->op[0].imm = temp.ta;
   asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_JUMP, ins);

   /* Exceptions: None */

   return 4;
}
