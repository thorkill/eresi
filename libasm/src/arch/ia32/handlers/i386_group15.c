/*
** $Id: i386_group15.c,v 1.3 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="i386_group15" opcode="0xae"/>
 */

int i386_group15(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  struct s_modrm        *modrm;  
  modrm = (struct s_modrm *) opcode + 1;
  
  new->len += 1;
  
  switch(modrm->r) 
    {
    case 2:
      new->instr = ASM_LDMXCSR;
      new->op1.type = ASM_OTYPE_ENCODED;
#if LIBASM_USE_OPERAND_VECTOR
      new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODED, proc);
#else
      operand_rmv(&new->op1, opcode + 1, len - 1, proc);
      new->len += new->op1.len;
#endif
      break;
        
    case 3:
      new->instr = ASM_STMXCSR;
#if LIBASM_USE_OPERAND_VECTOR
      new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODED, proc);
#else
      new->op1.type = ASM_OTYPE_ENCODED;
      operand_rmv(&new->op1, opcode + 1, len - 1, proc);
      new->len += new->op1.len;
#endif
      break;
        
    case 7:
      new->instr = ASM_CLFLUSH;
      new->op1.type = ASM_OTYPE_GENERAL;
      break;
        
    default:
      new->len = 0;
      break;
    }
  
  return (new->len);
}
