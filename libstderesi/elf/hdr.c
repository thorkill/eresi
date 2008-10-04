/**
 * @file elf.c for elfsh
 *
 * Started on  Fri Nov  2 15:21:14 2001 jfv
 * Last update Thu Mar 11 15:02:24 2004 jfv
 *
 * $Id: hdr.c,v 1.2 2008-02-16 12:32:27 thor Exp $
 *
 */
#include "libstderesi.h"



/** 
 * Change ELFCLASS and ELFMAGIC if necessary 
 * @param header
 */
static void	*revm_hdrfixup(elfsh_Ehdr *header)
{
  char		c;
  char		*h;
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  h = (char *) header;
  if (world.state.revm_mode != REVM_STATE_SCRIPT)
    {
     snprintf(logbuf, BUFSIZ - 1, " [!] ELFsh has detected a bogus or unsupported "
	      "header in this object\n"
	      " [!] Next input may be erroneous data\n");
     revm_output(logbuf);
    again:
     snprintf(logbuf, BUFSIZ - 1, " [!] Fixup header and continue ? (y/n) ");
     revm_output(logbuf);
     c = getc(stdin);
      if (c != 'y' && c != 'n')
	goto again;
      else if (c == 'n')
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "ELF header remained unfixed", (NULL));
    }
  elfsh_set_magic(header, *(unsigned int *) ELFMAG);
  h[EI_CLASS] = ELFCLASS32;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (h));
}

/**
 * Need doxygen comment
 * @param file
 * @return
 */

static char	*revm_printostype(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (elfsh_get_ostype(file))
    {
    case ELFSH_OS_LINUX:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ("Linux"));
    case ELFSH_OS_NETBSD:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ("NetBSD"));
    case ELFSH_OS_FREEBSD:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ("FreeBSD"));
    case ELFSH_OS_OPENBSD:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ("OpenBSD"));
    case ELFSH_OS_SOLARIS:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ("Solaris"));
    case ELFSH_OS_BEOS:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ("Beos"));
    case ELFSH_OS_ERROR:
    default:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ("Unknown"));
    }
}


/** 
 * Display the elf header 
 */
int		cmd_elf()
{
  elfsh_Ehdr	*header;
  unsigned int		archnum;
  unsigned int		typenum;
  unsigned int		enconum;
  char		*type;
  char		*arch;
  char		*enco;
  char		*name;
  elfsh_SAddr   offset;
  char		enco_unk[ERESI_MEANING + 1];
  char		type_unk[ERESI_MEANING + 1];
  char		arch_unk[ERESI_MEANING + 1];
  unsigned int		magic;
  char		buf[BUFSIZ];
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Fetch ELF header */
  header = elfsh_get_hdr(world.curjob->curfile);
  if (header == NULL)
    RET(-1);
  
  /* Fixup the header and print it again if requested */
  if (elfsh_get_magic(header) != *(unsigned int *) ELFMAG ||
      (elfsh_get_class(header) != ELFCLASS32 &&
       elfsh_get_class(header) != ELFCLASS64))
    {
      header = revm_hdrfixup(header);
      if (header == NULL)
	RET(-1);
    }

  /* Resolve entry point */
  magic = elfsh_get_magic(header);

  //name = elfsh_reverse_metasym(world.curjob->curfile, header->e_entry, &offset);
  name = NULL;
  if (name != NULL)
    {
      if (offset != 0)
	snprintf(buf, sizeof(buf), "%s + %u", name, (unsigned int) offset);
      else
	snprintf(buf, sizeof(buf), "%s", name);
    }


  /* Retreive names from the ELF header */
  archnum = elfsh_get_arch(header);
  typenum = elfsh_get_objtype(header);
  enconum = elfsh_get_encoding(header);
  arch = (char *) (archnum >= ELFSH_ARCHTYPE_MAX ?
		   revm_build_unknown(arch_unk, "type", archnum) :
		   elfsh_arch_type[archnum].desc);
  type = (char *) (typenum >= ELFSH_OBJTYPE_MAX  ?
		   revm_build_unknown(type_unk, "type", typenum) :
		   elfsh_obj_type[typenum].desc);
  enco = (char *) (enconum >= ELFSH_ENCODING_MAX ?
		   revm_build_unknown(enco_unk, "type", enconum) :
		   elfsh_encoding[enconum].desc);

  snprintf(logbuf, BUFSIZ - 1,
	   "\n [ELF HEADER]\n [Object %s, MAGIC 0x%08X]\n\n",
	   world.curjob->curfile->name, magic);
  revm_output(logbuf);
  
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
	   revm_colorfieldstr_fmt("%-20s", "Architecture"),
	   revm_colorstr_fmt("%18s", arch), 
	   revm_colorfieldstr_fmt("%-20s", "ELF Version"),
	   revm_colornumber("%14u", (unsigned int) header->e_version),
	   revm_colorfieldstr_fmt("%-20s", "Object type"),
	   revm_colorstr_fmt("%18s", type), 
	   revm_colorfieldstr_fmt("%-20s", "SHT strtab index"),
	   revm_colornumber("%14u", header->e_shstrndx),
	   revm_colorfieldstr_fmt("%-20s", "Data encoding"),
	   revm_colorstr_fmt("%18s", enco), 
	   revm_colorfieldstr_fmt("%-20s", "SHT foffset"),
	   revm_colornumber(UFMT14, header->e_shoff), 
	   revm_colorfieldstr_fmt("%-20s", "PHT foffset"),
	   revm_colornumber(UFMT18, header->e_phoff), 
	   revm_colorfieldstr_fmt("%-20s", "SHT entries number"),
	   revm_colornumber("%14u", header->e_shnum),
	   revm_colorfieldstr_fmt("%-20s", "PHT entries number"),
	   revm_colornumber("%18u", header->e_phnum), 
	   revm_colorfieldstr_fmt("%-20s", "SHT entry size"),
	   revm_colornumber("%14u", header->e_shentsize), 
	   revm_colorfieldstr_fmt("%-20s", "PHT entry size"),
	   revm_colornumber("%18u", header->e_phentsize),
	   revm_colorfieldstr_fmt("%-20s", "ELF header size"),
	   revm_colornumber("%14u", header->e_ehsize), 
	   revm_colorfieldstr_fmt("%-20s", "Runtime PHT offset"),
	   revm_colornumber("%18u", elfsh_get_rphtoff(header)),
	   revm_colorfieldstr_fmt("%-20s", "Fingerprinted OS"),
	   revm_colorstr_fmt("%14s", revm_printostype(world.curjob->curfile)),
	   revm_colorfieldstr_fmt("%-20s", "Entry point"),
	   revm_coloraddress(XFMT18, header->e_entry), 
	   (name != NULL ? revm_colorstr_fmt("[%s]", buf) : revm_colorwarn("[?]")), 
	   revm_colorstr("OLD PAX FLAGS"),
	   revm_coloraddress("0x%X", header->e_flags),
	   revm_colorfieldstr_fmt("%-20s", "PAX_PAGEEXEC"),
	   revm_colorstr_fmt("%18s", (elfsh_get_pax_pageexec(header)  ? "Enabled"    : "Disabled")),
	   revm_colorfieldstr_fmt("%-20s", "PAX_EMULTRAMP"),
	   revm_colorstr_fmt("%14s", (elfsh_get_pax_emultramp(header) ? "Emulated"   : "Not emulated")),
	   revm_colorfieldstr_fmt("%-20s", "PAX_MPROTECT"),
	   revm_colorstr_fmt("%18s", (elfsh_get_pax_mprotect(header)  ? "Restricted" : "Not restricted")),
	   revm_colorfieldstr_fmt("%-20s", "PAX_RANDMAP"),
	   revm_colorstr_fmt("%14s", (elfsh_get_pax_randmmap(header)  ? "Randomized" : "Not randomized")),
	   revm_colorfieldstr_fmt("%-20s", "PAX_RANDEXEC"),
	   revm_colorstr_fmt("%18s", (elfsh_get_pax_randexec(header)  ? "Randomized" : "Not randomized")),
	   revm_colorfieldstr_fmt("%-20s", "PAX_SEGMEXEC"),
	   revm_colorstr_fmt("%14s", (elfsh_get_pax_segmexec(header)  ? "Enabled"    : "Disabled")));
  revm_output(logbuf);
  revm_endline();
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
