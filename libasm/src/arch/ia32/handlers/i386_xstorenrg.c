/*
** $Id: i386_xstorenrg.c,v 1.4 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_shld_rmv_rv_cl" opcode="0xa7"/>
*/

int     i386_xstorenrg(asm_instr *new, unsigned char *opcode, unsigned int len,
		       asm_processor *proc)
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
  return (new->len);
}

