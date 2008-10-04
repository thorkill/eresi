/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_jr(asm_instr *ins, unsigned char *buf, unsigned int len,
                  asm_processor *proc)
{
   struct s_mips_decode_reg temp;

   ins->instr = ASM_MIPS_JR;
   mips_convert_format_r(&temp, buf);
   ins->op[0].baser = temp.rs;
   asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_REGISTER, ins);

   if (temp.rs == ASM_MIPS_REG_RA)
      ins->type = ASM_TYPE_RETPROC;
   else
      ins->type = ASM_TYPE_IMPBRANCH;

   /* Exceptions: None */

   return 4;
}
