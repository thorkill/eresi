/*
** elf.c for elfsh
**
** Started on  Fri Nov  2 15:21:14 2001 mayhem
** Last update Thu Aug 21 03:29:09 2003 mayhem
*/
#include "elfsh.h"



/* Change ELFCLASS and ELFMAGIC if necessary */
static void	*vm_hdrfixup(Elf32_Ehdr *header)
{
  char		c;
  char		*h;

  h = (char *) header;
  if (world.state.vm_mode != ELFSH_VMSTATE_SCRIPT)
    {
      fprintf(stderr,
	      " [!] ELFsh has detected a bogus or unsupported "
	      "header in this object\n"
	      " [!] Next input may be erroneous data\n");
    again:
      fprintf(stderr, " [!] Fixup header and continue ? (y/n) ");
      c = getc(stdin);
      if (c != 'y' && c != 'n')
	goto again;
      else if (c == 'n')
	return (NULL);
    }
  elfsh_set_magic(header, *(u_int *) ELFMAG);
  h[EI_CLASS] = ELFCLASS32;
  return (h);
}


/* Display the elf header */
int		cmd_elf()
{
  Elf32_Ehdr	*header;
  u_int		archnum;
  u_int		typenum;
  u_int		enconum;
  char		*type;
  char		*arch;
  char		*enco;
  char		*name;
  u_int		offset;
  char		enco_unk[ELFSH_MEANING + 1];
  char		type_unk[ELFSH_MEANING + 1];
  char		arch_unk[ELFSH_MEANING + 1];
  u_int		magic;
  char		buf[BUFSIZ];

  /* Fetch ELF header */
  header = elfsh_get_hdr(world.current);
  if (header == NULL)
    RET(-1);

  /* Fixup the header and print it again if requested */
  if (elfsh_get_magic(header) != *(u_int *) ELFMAG ||
      (elfsh_get_class(header) != ELFCLASS32 &&
       elfsh_get_class(header) != ELFCLASS64))
    {
      header = vm_hdrfixup(header);
      if (header == NULL)
	RET(-1);
    }

  /* Resolve entry point */
  magic = elfsh_get_magic(header);

  name = elfsh_reverse_metasym(world.current, header->e_entry, &offset);
  if (name != NULL)
    {
      if (offset != 0)
	snprintf(buf, sizeof(buf), "%s + %u", name, offset);
      else
	snprintf(buf, sizeof(buf), "%s", name);
    }


  /* Retreive names from the ELF header */
  archnum = elfsh_get_arch(header);
  typenum = elfsh_get_objtype(header);
  enconum = elfsh_get_encoding(header);
  arch = (char *) (archnum >= ELFSH_ARCHTYPE_MAX ?
		   vm_build_unknown(arch_unk, "type", archnum) :
		   elfsh_arch_type[archnum]);
  type = (char *) (typenum >= ELFSH_OBJTYPE_MAX  ?
		   vm_build_unknown(type_unk, "type", typenum) :
		   elfsh_obj_type[typenum].desc);
  enco = (char *) (enconum >= ELFSH_ENCODING_MAX ?
		   vm_build_unknown(enco_unk, "type", enconum) :
		   elfsh_encoding[enconum].desc);

  printf("\n [ELF HEADER]\n [Object %s, MAGIC 0x%08X]\n\n",
	 world.current->name, magic);

  printf(" Architecture       : %18s   ELF Version        : %14u   \n"
	 " Object type        : %18s   SHT strtab index   : %14u   \n"
	 " Data encoding      : %18s   SHT foffset        : %14u   \n"
	 " PHT foffset        : %18u   SHT entries number : %14u   \n"
	 " PHT entries number : %18u   SHT entry size     : %14u   \n"
	 " PHT entry size     : %18u   ELF header size    : %14u   \n"
	 " Entry point        : %18p   [%s]   		           \n"
	 " {PAX FLAGS = 0x%X}			                   \n"
	 " PAX_PAGEEXEC       : %18s   PAX_EMULTRAMP      : %14s   \n"
	 " PAX_MPROTECT       : %18s   PAX_RANDMMAP       : %14s   \n"
	 " PAX_RANDEXEC       : %18s   PAX_SEGMEXEC       : %14s   \n\n",
	 arch, (u_int) header->e_version, type, header->e_shstrndx,
	 enco, (u_int) header->e_shoff, (u_int) header->e_phoff, header->e_shnum,
	 header->e_phnum, header->e_shentsize, header->e_phentsize,
	 header->e_ehsize, (void *) header->e_entry,
	 (name != NULL ? buf : "?"), (u_int) header->e_flags,
	 (elfsh_get_pax_pageexec(header)  ? "Enabled"    : "Disabled"),
	 (elfsh_get_pax_emultramp(header) ? "Emulated"   : "Not emulated"),
	 (elfsh_get_pax_mprotect(header)  ? "Restricted" : "Not restricted"),
	 (elfsh_get_pax_randmmap(header)  ? "Randomized" : "Not randomized"),
	 (elfsh_get_pax_randexec(header)  ? "Randomized" : "Not randomized"),
	 (elfsh_get_pax_segmexec(header)  ? "Enabled"    : "Disabled"));

  return (0);
}
