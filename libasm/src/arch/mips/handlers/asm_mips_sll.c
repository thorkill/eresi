/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include "libasm.h"

int asm_mips_sll(asm_instr *ins, u_char *buf, u_int len,
                  asm_processor *proc)
{
   u_int i, converted = 0;
   u_char *for_help;

   if (asm_config_get_endian() == ASM_CONFIG_BIG_ENDIAN) {
      memcpy((char *)&converted,buf,sizeof(converted));
   } else if (asm_config_get_endian() == ASM_CONFIG_LITTLE_ENDIAN) {
     for_help = (u_char*)&converted;
     for(i=0;i<4;i++)
        *(for_help + i) = *(buf + 3 - i);
   } else {
     printf("Where am I ?!?!?!\n");
     exit(-1);
   }

   switch ((converted >> 6) & 0xFFFFF) {

      case MIPS_OPCODE_NOP:
         asm_mips_nop(ins,buf,len,proc);
         break;
      case MIPS_OPCODE_SSNOP:
         asm_mips_ssnop(ins,buf,len,proc);
         break;
      default:
         fprintf(stderr,"sll\n");
	 break;
   }
   return 777;
}
