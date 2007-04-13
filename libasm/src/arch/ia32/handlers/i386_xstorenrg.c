/*
** $Id: i386_xstorenrg.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_shld_rmv_rv_cl" opcode="0xa7"/>
*/

int     i386_xstorenrg(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{ 
  switch(*(opcode + 1))
    {
    case 0xc0:  new->instr = ASM_XSTORERNG; break;
    case 0xd0:  new->instr = ASM_XCRYPTCBC; break;
    case 0xe0:  new->instr = ASM_XCRYPTCFB; break;
    case 0xe8:  new->instr = ASM_XCRYPTOFB; break;
    default: new->instr = ASM_NONE;
    }
  new->len += 2;
  #if LIBASM_USE_OPERAND_VECTOR
#endif
  return (new->len);
}

