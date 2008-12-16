#include <libasm.h>
#include <libasm-arm.h>

char		*asm_arm_display_instr(asm_instr *instr, eresi_Addr addr)
{
  static char 	buffer[1024];

  memset(buffer,0,sizeof(buffer));
  sprintf(buffer,"%s",instr->proc->instr_table[instr->instr]);

  // TODO: this is just a stub

  return buffer;
}
