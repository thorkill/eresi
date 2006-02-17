/*
** private
** 
** Author  : <sk at devhell dot org>
** Started : Tue Feb 18 12:09:31 2003
** Updated : Mon Feb 17 00:34:03 2003
*/

#include <libasm.h>
#include <libasm-int.h>

/*
<i386 func="" opcode="0xc1"/>
*/

/*
 <i386 func="i386_bswap" opcode="0xc8"/>
*/

int     i386_bswap(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
 new->instr = ASM_BSWAP;
 new->len += 1;
 new->op1.type = ASM_OTYPE_GENERAL;
 return (new->len);
}
