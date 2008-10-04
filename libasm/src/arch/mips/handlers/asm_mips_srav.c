/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

/* SRAV rd,rt,rs */

int asm_mips_srav(asm_instr *ins, unsigned char *buf, unsigned int len,
                  asm_processor *proc)
{
   struct s_mips_decode_reg temp;

   ins->instr = ASM_MIPS_SRAV;
   ins->type = ASM_TYPE_ARITH;
   mips_convert_format_r(&temp, buf);
   ins->op[0].baser = temp.rd;
   asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_REGISTER, ins);
   ins->op[1].baser = temp.rt;
   asm_mips_operand_fetch(&ins->op[1], buf, ASM_MIPS_OTYPE_REGISTER, ins);
   ins->op[2].baser = temp.rs;
   asm_mips_operand_fetch(&ins->op[2], buf, ASM_MIPS_OTYPE_REGISTER, ins);

   /* Exceptions: None */

   return 4;
}
