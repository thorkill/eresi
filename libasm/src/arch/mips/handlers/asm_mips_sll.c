/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include "libasm.h"

int asm_mips_sll(asm_instr *ins, u_char *buf, u_int len,
                  asm_processor *proc)
{
   u_int converted = 0;

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
         break;
   }
   return 777;
}
