/*
** $Id: i386_group15.c,v 1.2 2007-01-26 17:48:31 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="i386_group15" opcode="0xae"/>
 */

int i386_group15(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm        *modrm;  
  modrm = (struct s_modrm *) opcode + 1;
  
  new->len += 1;
  
  switch(modrm->r) {
        case 2:
          new->instr = ASM_LDMXCSR;
          new->op1.type = ASM_OTYPE_ENCODED;
          operand_rmv(&new->op1, opcode + 1, len - 1, proc);
          new->len += new->op1.len;
        break;
        
        case 3:
          new->instr = ASM_STMXCSR;
          new->op1.type = ASM_OTYPE_ENCODED;
          operand_rmv(&new->op1, opcode + 1, len - 1, proc);
          new->len += new->op1.len;
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
