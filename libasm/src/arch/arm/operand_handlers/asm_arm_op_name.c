/**
 * @defgroup a-operands ARM operands API.
 * @ingroup arm
 */
/**
** @file asm_arm_op_name.c
** @ingroup a-operands
*/
#include <libasm.h>

/*
#define ASM_POOL_SIZE 100
#define ASM_OP_NAME_LEN 64
*/
/* FIFO sort-of memory manager */
/*
char 		*asm_name_pool_alloc()
{
  static char	*asm_name_pool[ASM_POOL_SIZE] = {};
  static int	index = 0;

  char *ret;

  if (asm_name_pool[index] != NULL)
    free(asm_name_pool[index]);

  ret = asm_name_pool[index] = (char *) malloc(ASM_OP_NAME_LEN);
  
  index = (index+1)%ASM_POOL_SIZE;
  
  return ret;
}
*/
char 	*asm_arm_get_op_name(asm_operand *op)
{
  switch (op->content)
    {
    case ASM_ARM_OTYPE_REGISTER:
      return asm_arm_get_register(op->baser);
      // TODO:
    default:
      return NULL;
    }
}
