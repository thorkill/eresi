/**
* @file libasm/src/arch/mips/handlers/asm_mips_jal.c
** @ingroup MIPS_instrs
*/
/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_jal(asm_instr *ins, u_char *buf, u_int len,
                  asm_processor *proc)
{
   struct s_mips_decode_jump temp;

   ins->instr = ASM_MIPS_JAL;
   ins->type = ASM_TYPE_CALLPROC;
   mips_convert_format_j(&temp,buf);
   ins->op[0].imm = temp.ta;
   asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_JUMP, ins);

   /* Exceptions: None */

   return 4;
}
