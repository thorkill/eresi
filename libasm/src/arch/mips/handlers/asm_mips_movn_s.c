/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_movn_s(asm_instr *ins, unsigned char *buf, unsigned int len,
                  asm_processor *proc)
{
   struct s_mips_decode_reg temp;

   ins->instr = ASM_MIPS_MOVN_S;
   ins->type = ASM_TYPE_ARITH | ASM_TYPE_ARCH | ASM_TYPE_ASSIGN;
   mips_convert_format_r(&temp, buf);
   ins->op[0].regset = ASM_MIPS_REG_FPU;
   ins->op[0].baser = temp.sa;
   asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_REGISTER, ins);
   ins->op[1].regset = ASM_MIPS_REG_FPU;
   ins->op[1].baser = temp.rd;
   asm_mips_operand_fetch(&ins->op[1], buf, ASM_MIPS_OTYPE_REGISTER, ins);
   ins->op[2].baser = temp.rt;
   asm_mips_operand_fetch(&ins->op[2], buf, ASM_MIPS_OTYPE_REGISTER, ins);

   /* Exception: Reserved Instruction, Coprocessor Unusable */
   /* FPU Exceptions: Unimplemented Operation */

   return 4;
}
