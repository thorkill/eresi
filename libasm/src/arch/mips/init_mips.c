/**
 * @file init_mips.c
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
  int i = 0, converted = 0;
  u_int dim[2];
  LIBASM_HANDLER_FETCH(fetch);
  //int (*fetch)(asm_instr *,u_char *,u_int,asm_processor *) = 0;
  
  /* we have to convert to the endiannes of the architecture
   * were libasm is running keeping in mind that the target
   * object can be either in big or little endian. */
#if __BYTE_ORDER == __LITTLE_ENDIAN
  if(asm_config_get_endian() == ASM_CONFIG_LITTLE_ENDIAN) {
    memcpy((char *)&converted,buf,sizeof(converted));
  } else {
    for(i=0;i<4;i++)
      ((char *)(&converted))[3-i] = *(buf + i);
  }
#else
  if(asm_config_get_endian() == ASM_CONFIG_BIG_ENDIAN) {
    memcpy((char *)&converted,buf,sizeof(converted));
  } else {
      for(i=0;i<4;i++)
	((char *)(&converted))[i] = *(buf + 3 - i);
  }
#endif
  
   ins->proc = proc;
   ins->len = 4;
   ins->ptr_instr = buf;
   ins->nb_op = 0;
   ins->type = ASM_TYPE_NONE;
   
   vec = aspect_vector_get("disasm-mips");

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

   dim[0] = converted >> 24;

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
         }
         break;
      }
      case MIPS_OPCODE_REGIMM:
         dim[1] = (converted & 0x3f) + 
                  MIPS_SPECIAL_FUNCTION_NUM; 
         break;
      case MIPS_OPCODE_SPECIAL2:
         dim[1] = (converted & 0x3f) + 
                  MIPS_SPECIAL_FUNCTION_NUM +
                  MIPS_REGIMM_FUNCTION_NUM;
         break;
      case MIPS_OPCODE_SPECIAL3:
         dim[1] = (converted & 0x3f) + 
                  MIPS_SPECIAL_FUNCTION_NUM +
                  MIPS_REGIMM_FUNCTION_NUM +
                  MIPS_SPECIAL2_FUNCTION_NUM;
         switch(dim[1])
         {
            case MIPS_OPCODE_BSHFL:
	      dim[2] = ((converted & 0x7c0) >> 6) + 6;
         }
         break;
   }
   
   fetch = aspect_vectors_select(vec,dim);
   return (fetch(ins,(u_char *)&converted,len,proc));
}

/**
 * XXX: Move this to a place more appropriate.
 */
void	asm_register_mips(asm_processor *proc, int opt);

/**
 * Mips initialization function. 
 * @param proc
 * @return Always 1
 */
int	asm_init_mips(asm_processor *proc)
{
  proc->fetch = fetch_mips;
  proc->display_handle = asm_mips_display_instr;
  proc->internals = 0;
  proc->type = ASM_PROC_MIPS;
  proc->instr_table = (char **)e_mips_instrs;

  asm_init_vectors(proc);
  asm_register_mips(proc,0);
  return (1);
}



