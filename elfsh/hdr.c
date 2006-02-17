/*
** elf.c for elfsh
**
** Started on  Fri Nov  2 15:21:14 2001 mayhem
** Last update Thu Mar 11 15:02:24 2004 mayhem
*/
#include "elfsh.h"



/* Change ELFCLASS and ELFMAGIC if necessary */
static void	*vm_hdrfixup(elfsh_Ehdr *header)
{
  char		c;
  char		*h;
  char		logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  h = (char *) header;
  if (world.state.vm_mode != ELFSH_VMSTATE_SCRIPT)
    {
     snprintf(logbuf, BUFSIZ - 1, " [!] ELFsh has detected a bogus or unsupported "
	      "header in this object\n"
	      " [!] Next input may be erroneous data\n");
     vm_output(logbuf);
    again:
     snprintf(logbuf, BUFSIZ - 1, " [!] Fixup header and continue ? (y/n) ");
     vm_output(logbuf);
     c = getc(stdin);
      if (c != 'y' && c != 'n')
	goto again;
      else if (c == 'n')
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "ELF header remained unfixed", (NULL));
    }
  elfsh_set_magic(header, *(u_int *) ELFMAG);
  h[EI_CLASS] = ELFCLASS32;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (h));
}


static char	*vm_printostype(elfshobj_t *file)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (elfsh_get_ostype(file))
    {
    case ELFSH_OS_LINUX:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ("Linux"));
    case ELFSH_OS_NETBSD:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ("NetBSD"));
    case ELFSH_OS_FREEBSD:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ("FreeBSD"));
    case ELFSH_OS_OPENBSD:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ("OpenBSD"));
    case ELFSH_OS_SOLARIS:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ("Solaris"));
    case ELFSH_OS_BEOS:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ("Beos"));
    case ELFSH_OS_ERROR:
    default:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ("Unknown"));
    }
}


/* Display the elf header */
int		cmd_elf()
{
  elfsh_Ehdr	*header;
  u_int		archnum;
  u_int		typenum;
  u_int		enconum;
  char		*type;
  char		*arch;
  char		*enco;
  char		*name;
  elfsh_SAddr   offset;
  char		enco_unk[ELFSH_MEANING + 1];
  char		type_unk[ELFSH_MEANING + 1];
  char		arch_unk[ELFSH_MEANING + 1];
  u_int		magic;
  char		buf[BUFSIZ];
  char		logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Fetch ELF header */
  header = elfsh_get_hdr(world.curjob->current);
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

  //name = elfsh_reverse_metasym(world.curjob->current, header->e_entry, &offset);
  name = NULL;
  if (name != NULL)
    {
      if (offset != 0)
	snprintf(buf, sizeof(buf), "%s + %u", name, (u_int) offset);
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

  snprintf(logbuf, BUFSIZ - 1,
	   "\n [ELF HEADER]\n [Object %s, MAGIC 0x%08X]\n\n",
	   world.curjob->current->name, magic);
  vm_output(logbuf);
  
  snprintf(logbuf, BUFSIZ, 
	   " %s : %s   %s : %s\n"
	   " %s : %s   %s : %s\n"
	   " %s : %s   %s : %s\n"
	   " %s : %s   %s : %s\n"
	   " %s : %s   %s : %s\n"
	   " %s : %s   %s : %s\n"
	   " %s : %s   %s : %s\n"
	   " %s : %s   %s\n"
	   " {%s = %s}\n"
	   " %s : %s   %s : %s\n"
	   " %s : %s   %s : %s\n"
	   " %s : %s   %s : %s\n\n",
	   vm_colorfieldstr_fmt("%-20s", "Architecture"),
	   vm_colorstr_fmt("%18s", arch), 
	   vm_colorfieldstr_fmt("%-20s", "ELF Version"),
	   vm_colornumber("%14u", (u_int) header->e_version),
	   vm_colorfieldstr_fmt("%-20s", "Object type"),
	   vm_colorstr_fmt("%18s", type), 
	   vm_colorfieldstr_fmt("%-20s", "SHT strtab index"),
	   vm_colornumber("%14u", header->e_shstrndx),
	   vm_colorfieldstr_fmt("%-20s", "Data encoding"),
	   vm_colorstr_fmt("%18s", enco), 
	   vm_colorfieldstr_fmt("%-20s", "SHT foffset"),
	   vm_colornumber(UFMT14, header->e_shoff), 
	   vm_colorfieldstr_fmt("%-20s", "PHT foffset"),
	   vm_colornumber(UFMT18, header->e_phoff), 
	   vm_colorfieldstr_fmt("%-20s", "SHT entries number"),
	   vm_colornumber("%14u", header->e_shnum),
	   vm_colorfieldstr_fmt("%-20s", "PHT entries number"),
	   vm_colornumber("%18u", header->e_phnum), 
	   vm_colorfieldstr_fmt("%-20s", "SHT entry size"),
	   vm_colornumber("%14u", header->e_shentsize), 
	   vm_colorfieldstr_fmt("%-20s", "PHT entry size"),
	   vm_colornumber("%18u", header->e_phentsize),
	   vm_colorfieldstr_fmt("%-20s", "ELF header size"),
	   vm_colornumber("%14u", header->e_ehsize), 
	   vm_colorfieldstr_fmt("%-20s", "Runtime PHT offset"),
	   vm_colornumber("%18u", elfsh_get_rphtoff(header)),
	   vm_colorfieldstr_fmt("%-20s", "Fingerprinted OS"),
	   vm_colorstr_fmt("%14s", vm_printostype(world.curjob->current)),
	   vm_colorfieldstr_fmt("%-20s", "Entry point"),
	   vm_coloraddress(XFMT18, header->e_entry), 
	   (name != NULL ? vm_colorstr_fmt("[%s]", buf) : vm_colorwarn("[?]")), 
	   vm_colorstr("OLD PAX FLAGS"),
	   vm_coloraddress("0x%X", header->e_flags),
	   vm_colorfieldstr_fmt("%-20s", "PAX_PAGEEXEC"),
	   vm_colorstr_fmt("%18s", (elfsh_get_pax_pageexec(header)  ? "Enabled"    : "Disabled")),
	   vm_colorfieldstr_fmt("%-20s", "PAX_EMULTRAMP"),
	   vm_colorstr_fmt("%14s", (elfsh_get_pax_emultramp(header) ? "Emulated"   : "Not emulated")),
	   vm_colorfieldstr_fmt("%-20s", "PAX_MPROTECT"),
	   vm_colorstr_fmt("%18s", (elfsh_get_pax_mprotect(header)  ? "Restricted" : "Not restricted")),
	   vm_colorfieldstr_fmt("%-20s", "PAX_RANDMAP"),
	   vm_colorstr_fmt("%14s", (elfsh_get_pax_randmmap(header)  ? "Randomized" : "Not randomized")),
	   vm_colorfieldstr_fmt("%-20s", "PAX_RANDEXEC"),
	   vm_colorstr_fmt("%18s", (elfsh_get_pax_randexec(header)  ? "Randomized" : "Not randomized")),
	   vm_colorfieldstr_fmt("%-20s", "PAX_SEGMEXEC"),
	   vm_colorstr_fmt("%14s", (elfsh_get_pax_segmexec(header)  ? "Enabled"    : "Disabled")));
  vm_output(logbuf);
  vm_endline();
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
