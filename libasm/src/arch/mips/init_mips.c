/**
 * @file init_mips.c
 *
 * fix and fill
 *          - Adam 'pi3' Zabrocki
 *
 * Manuel Martin - 2007 
 */

#include <libasm.h>


/**
 * Mips main fetching handler.
 * This function is called by asm_read_instr
 * @param ins
 * @param buf
 * @param len
 * @param proc
 * @return Lengh of instruction or 0 on error.
 */
int fetch_mips(asm_instr *ins, u_char *buf, u_int len, asm_processor *proc)
{
  vector_t *vec = 0;
  u_int i = 0, converted = 0;
  u_int dim[3];
  u_char *for_help;
  LIBASM_HANDLER_FETCH(fetch);
  //int (*fetch)(asm_instr *,u_char *,u_int,asm_processor *) = 0;
  
  /* we have to convert to the endiannes of the architecture
   * were libasm is running keeping in mind that the target
   * object can be either in big or little endian. */
//#if __BYTE_ORDER == __LITTLE_ENDIAN
//  if(asm_config_get_endian() == ASM_CONFIG_LITTLE_ENDIAN) {
//    memcpy((char *)&converted,buf,sizeof(converted));
//  } else {
//    for(i=0;i<4;i++)
//      ((char *)(&converted))[3-i] = *(buf + i);
//  }
//#else
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
//#endif
  
   ins->proc = proc;
   ins->len = 4;
   ins->ptr_instr = buf;
   ins->nb_op = 0;
   ins->type = ASM_TYPE_NONE;
   
   vec = aspect_vector_get(LIBASM_VECTOR_OPCODE_MIPS);

/*
 * Instruction hierarchy, lacks COP0 COP1 COP2 and COP1X
 * subclasses wich are fpu/privileged/coprocessor 2 stuff
 *
 * dim[0] = 0-63 = base class : 64 combinations
 *    dim[1] = 0-63 = SPECIAL subclass : 64 combinations
 *       dim[2] = 0-1 = MOVCI subclass
 *       dim[2] = 2-3 = SRL subclass
 *       dim[2] = 4-5 = SRLV subclass 
 *    dim[1] = 64-95 = REGIMM subclass : 32 combinations
 *    dim[1] = 96-159 = SPECIAL2 subclass : 64 combinations
 *    dim[1] = 160-223 = SPECIAL3 subclass : 64 combinations
 *       dim[2] = 6-37 = BSHFL subclass : 32 combinations
 */

//   dim[0] = converted >> 24;
   dim[0] = converted >> 26;
   dim[1] = 0;
   dim[2] = 0;

   switch(dim[0])
   {
      case MIPS_OPCODE_SPECIAL:
      {
	dim[1] = converted & 0x3f;
	switch(dim[1])
         {
            case MIPS_OPCODE_MOVCI:
               dim[2] = (converted & 0x10000 ) >> 16;
               break;
            case MIPS_OPCODE_SRL:
               dim[2] = ((converted & 0x200000 ) >> 21) + 2;
               break;
            case MIPS_OPCODE_SRLV:
               dim[2] = ((converted & 0x40) >> 6) + 4;
               break;
//	    case MIPS_OPCODE_SLL:
//	       dim[2] = (converted >> 6) & 0xFFFFF;
//	       break;
         }
         break;
      }
      case MIPS_OPCODE_REGIMM:
/*
         dim[1] = (converted & 0x3f) + 
                  MIPS_SPECIAL_FUNCTION_NUM; 
*/
         dim[1] = ((converted >> 16) & 0x1F);
         break;
      case MIPS_OPCODE_SPECIAL2:
/*
         dim[1] = (converted & 0x3f) + 
                  MIPS_SPECIAL_FUNCTION_NUM +
                  MIPS_REGIMM_FUNCTION_NUM;
*/
         dim[1] = converted & 0x3F;
         break;
      case MIPS_OPCODE_SPECIAL3:
/*
         dim[1] = (converted & 0x3f) + 
                  MIPS_SPECIAL_FUNCTION_NUM +
                  MIPS_REGIMM_FUNCTION_NUM +
                  MIPS_SPECIAL2_FUNCTION_NUM;
*/
         dim[1] = converted & 0x3F;
         switch(dim[1])
         {
            case MIPS_OPCODE_BSHFL:
	      dim[2] = ((converted & 0x7c0) >> 6) + 6;
         }
         break;

      case MIPS_OPCODE_COP0:
         break;
      case MIPS_OPCODE_COP1:
         break;
      case MIPS_OPCODE_COP2:
         do {
            u_int tmp = (converted >> 25) & 0x1;
	 
	    if (tmp)
	       dim[1] = tmp;
	    else {
               dim[1] = (converted >> 21) & 0x1F;
	       if (dim[1] == MIPS_OPCODE_BC)
	          dim[2] = (converted >> 16) & 0x3;
	    }
	 } while(0);
         break;
      case MIPS_OPCODE_COP1X:
         break;

   }
   
   fetch = aspect_vectors_select(vec,dim);
   return (fetch(ins,(u_char *)&converted,len,proc));
}

/**
 * Mips initialization function. 
 * @param proc
 * @return Always 0
 */
int	asm_init_mips(asm_processor *proc)
{
  proc->fetch = fetch_mips;
  proc->display_handle = asm_mips_display_instr;
  proc->internals = 0;
  proc->type = ASM_PROC_MIPS;
  proc->instr_table = (char **)e_mips_instrs;

  asm_arch_register(proc, 0);

  return (0);
}



