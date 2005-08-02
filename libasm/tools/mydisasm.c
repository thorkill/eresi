/*
** Author  : <sk at devhell dot org>
** Started : Mon Jun 10 01:49:20 2002
** Updated : Thu Dec  4 02:46:23 2003
*/

/* 
 * this tool is designed to disassemble binary with the same output as
 * objdump (except symbols.)
 * 
 * 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <libelfsh.h>
#include <libasm.h>


int	usage(char *p) {
  printf("Usage: %s <binary> <sym/vaddr> <[len]>\n", 
	  p);
  return (-1);
}

int	main(int ac, char **av) {
  u_char	*ptr;
  unsigned long	start;
  unsigned long	end;
  unsigned long	len;
  u_int		curr;
  unsigned long	vaddr;
  char		*att_dump;
  int		i;
  Elf32_Sym	*sym;
  elfshobj_t	*obj;
  asm_instr	instr;
  asm_processor	proc;
  u_int		arch;

  if (ac < 3)
    return (usage(av[0]));
  
  obj = elfsh_load_obj(av[1]);
  if (!obj)
    {
      printf("failed opening %s\n", av[1]);
      return (-1);
    }


  /* fetching 2nd argument <start> : symbol or vaddr */

  arch = elfsh_get_arch(obj->hdr);

  vaddr = 0;
  if (!(sym = elfsh_get_metasym_by_name(obj, av[2])))
    vaddr = strtoul(av[2], 0, 16);
  else
    vaddr = sym->st_value;
  
  if (!vaddr)
    {
      printf("invalid start %s: null or cannot find symbol\n", av[2]);
      elfsh_unload_obj(obj);
      return (-1);
    }
  
  /* fetching third argument if present L: <len> */
  
  if (av[3])
    len = atol(av[3]);
  else
    {
      if (!sym)
	{
	  printf("len required with no symbol information\n");
	  elfsh_unload_obj(obj);
	  return (-1);
	} 
      else
	len = sym->st_size;
    }
  
  
  end = vaddr + len;
  
  /* select arch */
  
  switch(arch)
    {
    case EM_SPARC:
      asm_init_sparc(&proc);
      break;
    case EM_386:
      asm_init_i386(&proc);
      break;
    default:
      printf("unsupported architecture\n");
      elfsh_unload_obj(obj);
      return (-1);
    }
  
  /*
    filling buffer
  */
  
  ptr = malloc(len + 1);
  memset(ptr, 0, len + 1);
  start = elfsh_get_foffset_from_vaddr(obj, vaddr);

  
  
  curr = elfsh_raw_read(obj, start, ptr, len);
  if (curr != len)
    {
      printf("error reading %li bytes at %li -> read %i bytes\n", len, start, curr);
      elfsh_unload_obj(obj);
      return (-1);
    }
 
  /* disassembling loop */

  curr = 0;
  
  while(curr < len) {
    if (asm_read_instr(&instr, ptr + curr, len - curr, &proc) > 0) {
      att_dump = asm_display_instr_att(&instr, (int) vaddr + curr);
      if (att_dump) {
	printf("0x%08x:\t", (int) vaddr + curr);
	printf("%30s\t", att_dump);
	for (i = 0; i < instr.len; i++)
	  printf("%02x ", *(ptr + curr + i));
	puts("");
	curr += asm_instr_len(&instr);
      } else
	curr++;
    } else {
      printf("0x%08x: .byte 0x%02x\n", (int) vaddr + curr, *(ptr + curr));
      printf(";; error reading instruction at %p\n", ptr + curr);
      printf(";; dumping opcodes: %02x %02x %02x %02x\n", 
	     *(ptr + curr), *(ptr + curr + 1), 
	     *(ptr + curr + 2), *(ptr + curr + 3));
      curr++;
    }
  }
  free(ptr);
  elfsh_unload_obj(obj);
  return (0);
}
  




