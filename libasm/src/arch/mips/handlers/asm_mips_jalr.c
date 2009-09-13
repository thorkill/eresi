/**
* @file libasm/src/arch/mips/handlers/asm_mips_jalr.c
** @ingroup MIPS_instrs
*/
/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_jalr(asm_instr *ins, u_char *buf, u_int len,
                  asm_processor *proc)
{
   struct s_mips_decode_reg temp;

   ins->instr = ASM_MIPS_JALR;
   ins->type = ASM_TYPE_CALLPROC;
   mips_convert_format_r(&temp, buf);
   ins->op[0].baser = temp.rs;
   asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_REGISTER, ins);

   /* Exceptions: None */

   return 4;
}
