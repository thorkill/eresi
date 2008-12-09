#include <libasm.h>
#include <stdio.h>
#include <libaspect.h>

#define OPCODE_TO_TEST "\xE8\x00\x01\x00\x00"
#define OPCODE_VADDR   0x0804000

/**
 * @return 1 on success, 0 on failure
 */
int test32(void)
{
  asm_processor proc;
  asm_instr     ins;
  int		ret;
  char		*dump;
  int		vaddr;

  ret = asm_init_arch(&proc, ASM_PROC_IA32);
  printf("Initialization ret = %i\n", ret);
  ret = asm_read_instr(&ins, OPCODE_TO_TEST, 5, &proc);
  printf("asm_read_instr returned %i\n", ret);
  vaddr = OPCODE_VADDR;
  dump = asm_display_instr_att(&ins, vaddr);
  printf("Disasm -> %s\n", dump);
  return (0);
}

/**
 * @return 1 on success, 0 on failure
 */
int test16(void)
{
  asm_processor proc;
  asm_instr     ins;
  int		ret;
  char		*dump;
  int		vaddr;

  ret = asm_init_arch(&proc, ASM_PROC_IA32);
  printf("Initialization ret = %i\n", ret);
  asm_ia32_switch_mode(&proc, INTEL_REAL);
  ret = asm_read_instr(&ins, OPCODE_TO_TEST, 5, &proc);
  printf("asm_read_instr returned %i\n", ret);
  vaddr = OPCODE_VADDR;
  dump = asm_display_instr_att(&ins, vaddr);
  printf("Disasm -> %s\n", dump);
  return (0);
}

/**
 * Call both test for protected and real mode
 */



int main(int ac, char **av)
{
  int	ret;
  int   failed;
  
  failed = 0;
  
  if ((test32()) != 1)
    {
      printf("test32 failed\n");
      failed = 1;
    }

  if ((test16()) != 1)
    {
      printf("test16 failed\n");
      failed = 1;
    }

  return (failed);
}
