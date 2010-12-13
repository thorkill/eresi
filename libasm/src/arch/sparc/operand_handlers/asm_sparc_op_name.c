/**
 * @defgroup SPARC_operands SPARC operands API.
 * @ingroup sparc
 */
/**
* @file libasm/src/arch/sparc/operand_handlers/asm_sparc_op_name.c
** @ingroup SPARC_operands
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
  if (ret != NULL)
    index = (index + 1) % ASM_POOL_SIZE;
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
      op->sbaser = get_sparc_register(op->baser);
      sprintf(buf, "[ %s", op->sbaser);
      if (op->imm)
        sprintf(buf+strlen(buf), " + 0x%x", op->imm);
        
      sprintf(buf+strlen(buf), " ]");
      
      if (op->address_space != ASM_SP_ASI_P)
        sprintf(buf+strlen(buf), " %%asi");

      return buf;
    case ASM_SP_OTYPE_REG_ADDRESS:
      buf = asm_name_pool_alloc();
      op->sbaser = get_sparc_register(op->baser);

      if (op->indexr > 0)
	{
	  op->sindex = get_sparc_register(op->indexr);
	  sprintf(buf, "[ %s + %s ]", op->sbaser, op->sindex);
	}
      else
	sprintf(buf, "[ %s ]", op->sbaser);
      
      if (op->address_space != ASM_SP_ASI_P)
        sprintf(buf+strlen(buf), " 0x%x", op->address_space);

      return buf;
      
    default:
      return NULL;

  }
}

