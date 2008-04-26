#include <stdio.h>
#include "libasm.h"

int main()
{
  asm_processor	proc;
  if (!asm_init_arch(&proc, ASM_PROC_IA32))
    printf("[x] IA32 supported\n");
  else
    printf("[ ] IA32 unsupported\n");
  if (!asm_init_arch(&proc, ASM_PROC_SPARC))
    printf("[x] SPARC supported\n");
  else
    printf("[ ] SPARC unsupported\n");
  if (!asm_init_arch(&proc, ASM_PROC_MIPS))
    printf("[x] MIPS supported\n");
  else
    printf("[ ] MIPS unsupported\n");
  return (0);
}
