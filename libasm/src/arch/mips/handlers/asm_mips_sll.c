/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_sll(asm_instr *ins, unsigned char *buf, unsigned int len,
                  asm_processor *proc)
{
   struct s_mips_decode_reg temp;
   unsigned int converted = 0;

   memcpy((char *)&converted,buf,sizeof(converted));

   switch ((converted >> 6) & 0xFFFFF) {

      case MIPS_OPCODE_NOP:
         asm_mips_nop(ins,buf,len,proc);
         break;
      case MIPS_OPCODE_SSNOP:
         asm_mips_ssnop(ins,buf,len,proc);
         break;
      default:
         ins->instr = ASM_MIPS_SLL;
         ins->type = ASM_TYPE_ARITH;
         mips_convert_format_r(&temp, buf);
         ins->op[0].baser = temp.rd;
         asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_REGISTER, ins);
         ins->op[1].baser = temp.rt;
         asm_mips_operand_fetch(&ins->op[1], buf, ASM_MIPS_OTYPE_REGISTER, ins);
         ins->op[2].imm = temp.sa;
         asm_mips_operand_fetch(&ins->op[2], buf, ASM_MIPS_OTYPE_IMMEDIATE, ins);

         /* Exceptions: None */

         break;
   }
   return 4;
}
