/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

/* SDBBP code */

int asm_mips_sdbbp(asm_instr *ins, u_char *buf, u_int len,
                  asm_processor *proc)
{
   struct s_mips_decode_jump temp;

   ins->instr = ASM_MIPS_SDBBP;
   ins->type = ASM_TYPE_ARCH;
   mips_convert_format_j(&temp,buf);
   ins->op[0].imm = (temp.ta >> 6) & 0xFFFFF;
   asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_JUMP, ins);

   /* Exceptions: Debug Breakpoint Exception */

   return 777;
}
