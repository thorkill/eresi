/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_cvt_d_l(asm_instr *ins, unsigned char *buf, unsigned int len,
                  asm_processor *proc)
{
   struct s_mips_decode_reg temp;

   ins->instr = ASM_MIPS_CVT_D_L;
   ins->type = ASM_TYPE_ARITH | ASM_TYPE_ARCH;
   mips_convert_format_r(&temp, buf);
   ins->op[0].regset = ASM_MIPS_REG_FPU;
   ins->op[0].baser = temp.sa;
   asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_REGISTER, ins);
   ins->op[1].regset = ASM_MIPS_REG_FPU;
   ins->op[1].baser = temp.rd;
   asm_mips_operand_fetch(&ins->op[1], buf, ASM_MIPS_OTYPE_REGISTER, ins);

   /* Exception: Reserved Instruction, Coprocessor Unusable */
   /* FPU Exceptions: Inexact, Unimplemented Operation, Invalid Operation */

   return 4;
}
