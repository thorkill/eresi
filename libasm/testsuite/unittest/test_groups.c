#include <libasm.h>
#include <stdio.h>
#include <libaspect.h>


#define OPCODE_TO_TEST "\xE8\x00\x00\x5b"
#define OPCODE_VADDR   0x0000EDA7

typedef struct s_instr2test
{
  char	*opcodes;
  int	len;
  int	vaddr;
  char	*disasm32;
  char	*disasm16;
} instr2test_t;



instr2test_t g_instr2test_array[] =
  {
    { "\xA1\x10\x00\x00\x00", 2, 0x000FF5CC, "mov           0x00000010,%eax", "mov           0x00000010,%ax"},
    { "\xFE\x06", 2, 0x000FF316, "inc           (%esi)", "inc           (%si)"},
    { "\x66\x2E\x8B\x0B\x66", 5, 0x000FF5D1, "mov           (%ebx),%cs:%ecx", "mov           (%bx),%cs:%cx"},
    { "\xEB\x00", 2, 0x000FF2E4, "jmp           0x000FF2E6", ""},

    { "\xE8\x09\x47\x00\x00", 5, 0x000FF2E6, "", ""},

    //    { "\x70\x00", 0x000FF318 , 2, "", ""},
    //    { "\x70\x00", 0x000FF318 , 2, "", ""},
    //{ "\x70\x00", 0x000FF318 , 2, "", ""},
    { NULL, 0, 0, "", ""},
  };

/**
 *
 */

char	*dump_bytes(unsigned char *ptr, int len)
{
  int			i;
  static char	buffer[513];
  
  for(i = 0; i < len; i++)
    sprintf(buffer + (i * 2), "%02X", *(ptr + i));
  return (buffer);
}

int compare_disassembly(char *orig, char *dumped)
{
    return (strcmp(orig, dumped));  
}

/**
 * @return 1 on success, 0 on failure
 */
int test32(instr2test_t *instr2test)
{
  asm_processor proc;
  asm_instr     ins;
  int		ret;
  char		*dump;
  int		vaddr;

  ret = asm_init_arch(&proc, ASM_PROC_IA32);
  if (!ret)
    {
      printf("Initialization ret = %i\n", ret);
      return (0);
    }
  ret = asm_read_instr(&ins, instr2test->opcodes, 12, &proc);

  vaddr = instr2test->vaddr;
  dump = asm_display_instr_att(&ins, vaddr);
  ret = compare_disassembly(instr2test->disasm32, dump);
  if (ret != 0)
    {
      printf("test32 failed | bytes : %s | waited : '%s' | got '%s'\n", 
	     dump_bytes(instr2test->opcodes, instr2test->len),
	     instr2test->disasm32, dump);
      return (0);
    }
}

/**
 * @return 1 on success, 0 on failure
 */
int test16(instr2test_t *instr2test)
{
  asm_processor proc;
  asm_instr     ins;
  int		ret;
  char		*dump;
  int		vaddr;

  ret = asm_init_arch(&proc, ASM_PROC_IA32);
  if (!ret)
    {
      printf("Initialization ret = %i\n", ret);
      return (0);
    }
  asm_ia32_switch_mode(&proc, INTEL_REAL);
  ret = asm_read_instr(&ins, instr2test->opcodes, 12, &proc);
  
  vaddr = instr2test->vaddr;
  dump = asm_display_instr_att(&ins, vaddr);
  ret = compare_disassembly(instr2test->disasm16, dump);
  if (ret != 0)
    {
      printf("test16 failed | bytes : %s | waited : '%s' | got '%s'\n", 
	     dump_bytes(instr2test->opcodes, instr2test->len),
	     instr2test->disasm16, dump);
      return (0);
    }
  
  return (1);
}

int	run_test(instr2test_t *instr2test)
{	  
  int failed = 0;
  
  if ((test32(instr2test)) != 1)
    {
      failed = 1;
    }
	  
  if ((test16(instr2test)) != 1)
    {
      failed = 1;
    }
  if (failed == 1)
    return (0);
  return (1);
}
/**
 * Call both test for protected and real mode
 */

int main(int ac, char **av)
{
  int	ret;
  int   failed;
  int	global_failed;
  instr2test_t	*instr2test;
  int	number;
  

  if (ac < 2)
    {      
      for (instr2test = g_instr2test_array; instr2test->opcodes != NULL; instr2test++)
	{
	  run_test(instr2test);
	}
    }
  else
    {
      number = atoi(av[1]);
      if ((number >= 0) && (number < 6))
	run_test(&g_instr2test_array[number]);
      else
	printf("test %i out of range\n", number);
    }
}
