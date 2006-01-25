/*
** private - do not distribute
** 
** Author  : <sk at devhell dot org>
** Started : Xxx Xxx xx xx:xx:xx 2002
** Updated : Thu Mar 11 00:40:31 2004
*/

/*
  Sat Jul 13 00:15:51 2002
  removed a bug in scalar output.

*/

#include <libasm.h>
#include <libasm-int.h>


/*
  to add symbol support, it seems to be the good place.
*/

void	asm_resolve_immediate(asm_processor *proc, u_int val, char *buf, u_int len) {
  
  proc->resolve_immediate(proc->resolve_data, val, buf, len);  
}



/*

 */

char	*asm_display_instr_att(asm_instr *instr, 
			       int addr) {
  return (instr->proc->display_handle(instr, addr));
}
















