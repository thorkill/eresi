/**
** @file asm_sparc_op_name.c
** @ingroup s-operands
*/
#include <libasm.h>
#include <libasm-sparc.h>

#define ASM_POOL_SIZE 100
#define ASM_OP_NAME_LEN 64

/* FIFO sort-of memory manager */
char *asm_name_pool_alloc() {
  static char* asm_name_pool[ASM_POOL_SIZE] = {};
  static int index = 0;

  char *ret;

  if (asm_name_pool[index] != NULL)
    free(asm_name_pool[index]);

  ret = asm_name_pool[index] = (char *) malloc(ASM_OP_NAME_LEN);
  
  index = (index+1)%ASM_POOL_SIZE;
  
  return ret;
}

char *asm_sparc_get_op_name(asm_operand *op) {
  char *buf;

  switch (op->content) {

    case ASM_SP_OTYPE_REGISTER:
      return get_sparc_register(op->baser);
    case ASM_SP_OTYPE_SREGISTER:
      return get_sparc_sregister(op->baser);
    case ASM_SP_OTYPE_PREGISTER:
      return get_sparc_pregister(op->baser);
    case ASM_SP_OTYPE_FREGISTER:
      return get_sparc_fregister(op->baser);
    case ASM_SP_OTYPE_CC:
      return get_sparc_cc(op->baser);

    case ASM_SP_OTYPE_IMM_ADDRESS:
      buf = asm_name_pool_alloc();
      sprintf(buf, "[ %s", get_sparc_register(op->baser));
      if (op->imm)
        sprintf(buf+strlen(buf), " + 0x%x", op->imm);
        
      sprintf(buf+strlen(buf), " ]");
      
      if (op->address_space != ASM_SP_ASI_P)
        sprintf(buf+strlen(buf), " %%asi");

      return buf;
    case ASM_SP_OTYPE_REG_ADDRESS:
      buf = asm_name_pool_alloc();
      if (op->indexr > 0)
        sprintf(buf, "[ %s + %s ]", get_sparc_register(op->baser),
      								get_sparc_register(op->indexr));
      								
  	  else
  	    sprintf(buf, "[ %s ]", get_sparc_register(op->baser));
      								
      if (op->address_space != ASM_SP_ASI_P)
        sprintf(buf+strlen(buf), " 0x%x", op->address_space);

      return buf;
      
    default:
      return NULL;

  }
}

