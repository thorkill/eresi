/*
** relinject.c for libelfsh
**
** Original SPARC relocation function by thegrugq
**
** Started on  Fri Mar 28 14:55:37 2003 mayhem
** Last update Thu Aug 21 03:33:53 2003 mayhem
*/
#include "libelfsh.h"



/* Perform relocation on entry for INTEL architecture */
static int      elfsh_relocate_i386(elfshsect_t		*new,
				    Elf32_Rel		*cur,
				    u_long		*dword,
				    u_long		addr)
{
  switch (elfsh_get_reltype(cur))
    {

      /* Absolute reference */
    case R_386_32:
      *dword = addr;
      break;

      /* Relative reference */
    case R_386_PC32:
      *dword = addr - (new->shdr->sh_addr + cur->r_offset + sizeof(u_long));
      break;

    default:
      ELFSH_SETERROR("[libelfsh:relocate_etrel_section] "
		     "Unsupported relocation type\n", -1);
    }
  return (0);
}



/* Perform relocation on entry for SPARC architecture */
static int       elfsh_relocate_sparc(elfshsect_t       * new,
                                      Elf32_Rela        * cur,
                                      u_long            * dword,
                                      u_long              addr)
{
  int		retval;
  u_long	result;

#define ADD     (cur->r_addend)
#define BAS     (elfsh_get_object_baseaddr(new->parent))
#define GOT     0 /* GOT isn't available ... */
#define PLT     0 /* PLT isn't available ... */
#define PLA     (new->shdr->sh_addr + cur->r_offset)
#define SYM     (addr)

#define VERIFY8(x)      (((x) & (0xFFFFFFFF <<  8)) ? 1 : 0)
#define VERIFY13(x)     (((x) & (0xFFFFFFFF << 13)) ? 1 : 0)
#define VERIFY16(x)     (((x) & (0xFFFFFFFF << 16)) ? 1 : 0)
#define VERIFY22(x)     (((x) & (0xFFFFFFFF << 22)) ? 1 : 0)
#define VERIFY30(x)     (((x) & (0xFFFFFFFF << 30)) ? 1 : 0)
#define VERIFY32(x)     (((x) & (0xFFFFFFFF <<  0)) ? 1 : 0)

#define TRUNCATE8(x)    ((x) & ~(0xFFFFFFFF <<  8))
#define TRUNCATE13(x)   ((x) & ~(0xFFFFFFFF << 13))
#define TRUNCATE16(x)   ((x) & ~(0xFFFFFFFF << 16))
#define TRUNCATE22(x)   ((x) & ~(0xFFFFFFFF << 22))
#define TRUNCATE30(x)   ((x) & ~(0xFFFFFFFF << 30))
#define TRUNCATE32(x)   (x) /* XXX just assume its 32bit */

  retval = result = 0;
  switch (elfsh_get_reltype((Elf32_Rel *)cur))
    {
    case R_SPARC_NONE:
      break;
    case R_SPARC_8:
      result = SYM + ADD;
      if (VERIFY8(result))
	; // XXX error condition.
      break;
    case R_SPARC_16:
      result = SYM + ADD;
      if (VERIFY16(result))
	; // XXX error condition
      break;
    case R_SPARC_32:
      result = SYM + ADD;
      if (VERIFY32(result))
	; // XXX error condition
      break;
    case R_SPARC_DISP8:
      result = SYM + ADD - PLA;
      if (VERIFY8(result))
	; // XXX error condition
      break;
    case R_SPARC_DISP16:
      result = SYM + ADD - PLA;
      if (VERIFY16(result))
	; // XXX error condition
      break;
    case R_SPARC_DISP32:
      result = SYM + ADD - PLA;
      if (VERIFY32(result))
	; // XXX error condition
      break;
    case R_SPARC_WDISP30:
      result = (SYM + ADD - PLA) >> 2;
      if (VERIFY30(result))
	; // XXX error condition
      break;
    case R_SPARC_WDISP22:
      result = (SYM + ADD - PLA) >> 10;
      if (VERIFY22(result))
	; // XXX error condition
      break;
    case R_SPARC_HI22:
      result = TRUNCATE22(((SYM + ADD) >> 10));
      break;
    case R_SPARC_22:
      result = SYM + ADD;
      if (VERIFY22(result))
	; // XXX error condition
      break;
    case R_SPARC_13:
      result = SYM + ADD;
      if (VERIFY13(result))
	; // XXX error condition
      break;
    case R_SPARC_LO10:
      result = TRUNCATE13(((SYM + ADD) & 0x3FF));
      break;
    case R_SPARC_GOT10:
    case R_SPARC_GOT13:
    case R_SPARC_GOT22:
      // unsupported
      retval = -1;
      break;
    case R_SPARC_PC10:
      result = TRUNCATE13(((SYM + ADD - PLA) & 0x3FF));
      break;
    case R_SPARC_PC22:
      result = (SYM + ADD - PLA) >> 10;
      if (VERIFY22(result))
	; // XXX error condition
      break;
    case R_SPARC_WPLT30:
      // unsupported
      retval = -1;
      break;
    case R_SPARC_COPY:
      break;
    case R_SPARC_GLOB_DAT:
      result = SYM + ADD;
      if (VERIFY32(result))
	; // XXX error condition
      break;
    case R_SPARC_JMP_SLOT:
      retval = -1;
      break;
    case R_SPARC_RELATIVE:
      result = BAS + ADD;
      if (VERIFY32(result))
	; // XXX error condition
      break;
    case R_SPARC_UA32:
      result = SYM + ADD;
      if (VERIFY32(result))
	; // XXX error condition
      break;
    default:
      ELFSH_SETERROR("[libelfsh:relocate_sparc] Unsupported architecture\n",
		     -1);
      break;
    }

#undef ADD
#undef BAS
#undef GOT
#undef PLT
#undef PLA
#undef SYM
  *dword += result;
  return (retval);
}




/* Perform relocation on entry */
static int      elfsh_relocate_entry(elfshsect_t        *new,
                                     void               *reloc,
                                     u_long             *dword,
                                     u_long             addr)
{
  int retval;

  switch (elfsh_get_arch(new->parent->hdr))
    {
    case EM_386:
    /* XXX: case EM_486: */
       retval = elfsh_relocate_i386(new, (Elf32_Rel *) reloc, dword, addr);
       break;
    case EM_SPARC:
    case EM_SPARC32PLUS:
      retval = elfsh_relocate_sparc(new, (Elf32_Rela *) reloc, dword, addr);
      break;
    case EM_SPARCV9: /* can't support just yet.. */
    default:
      ELFSH_SETERROR("[libelfsh:relocate_entry] Unsupported architecture\n",
		     -1);
      break;
    }
  return retval;
}



/* Relocate the just injected section */
static int	elfsh_relocate_etrel_section(elfshsect_t *new,
					     elfshsect_t *reltab)
{
  Elf32_Rel	*cur;
  u_int		index;
  Elf32_Sym	*sym;
  u_int		size;
  long		*dword;
  long		addr;
  char		*name;
  char		tmpname[BUFSIZ];
  elfshsect_t	*sect;



#if __DEBUG_RELADD__
  printf("[DEBUG_RELADD] Using reloc table from %s [%s] data at %p \n",
	 reltab->parent->name, 	 reltab->name, reltab->data);
#endif

  /* Loop on the relocation table entries */
  size = (reltab->shdr->sh_type == SHT_RELA ?
	  sizeof(Elf32_Rela) : sizeof(Elf32_Rel));
  size = reltab->shdr->sh_size / size;

  for (index = 0; index < size; index++)
    {

      /* Get symbol value in ET_REL */
      cur = (reltab->shdr->sh_type == SHT_RELA ?
	     (void *) (((Elf32_Rela *) reltab->data) + index):
	     (void *) (((Elf32_Rel  *) reltab->data) + index));
      sym  = elfsh_get_symbol_from_reloc(reltab->parent, cur);
      name = elfsh_get_symname_from_reloc(reltab->parent, cur);
      if (sym == NULL || name == NULL)
	return (-1);

      /* Grab a pointer on the dword that need to be relocated */
      dword = (long *) ((char *) new->data + cur->r_offset);

      /*
      ** If symbol type is NOTYPE, we use ET_EXEC symtab, else if
      ** symbol link is COMMON, we use ET_REL symbol inserted in ET_EXEC
      ** during BSS sizescan in bss.c:elfsh_find_bsslen()
      */
      if (elfsh_get_symbol_type(sym) == STT_NOTYPE ||
	  elfsh_get_symbol_link(sym) == SHN_COMMON)
	{

	  sym = elfsh_get_metasym_by_name(new->parent, name);
	  if (sym == NULL)
	    ELFSH_SETERROR("[libelfsh:relocate_etrel_section] "
			   "Cant find requested symbol in ET_EXEC\n", -1);
	  addr = sym->st_value;

#if __DEBUG_RELADD__
	  printf("[DEBUG_RELADD] Relocate using existing symbol %-20s [%08X]\n",
		 name, (u_int) addr);
#endif

	}


      /* Compute addr giving the injected section's vaddr in ET_EXEC */
      else
	{

	  /* Find target section in ET_REL */
	  sect = elfsh_get_section_by_index(reltab->parent, sym->st_shndx,
					    NULL, NULL);
	  if (sect == NULL)
	    ELFSH_SETERROR("[libelfsh:relocate_etrel_section] "
			   "Cant find extracted section in ET_REL\n", -1);

	  /* Find corresponding inserted section in ET_EXEC */
	  snprintf(tmpname, sizeof(tmpname), "%s%s", reltab->parent->name, sect->name);
	  sect = elfsh_get_section_by_name(new->parent, tmpname, NULL, NULL, NULL);
	  if (sect == NULL)
	    ELFSH_SETERROR("[libelfsh:relocate_etrel_section] "
			   "Cant find inserted section in ET_EXEC\n", -1);

	  /* Compute pointer value */
	  addr = sect->shdr->sh_addr;
	  addr += (elfsh_get_symbol_type(sym) == STT_SECTION &&
		   !FILE_IS_SPARC(sect->parent) ? *dword : sym->st_value);


#if __DEBUG_RELADD__
	  printf("[DEBUG_RELADD] Relocate using section %-20s base [-> %08X] \n",
		 sect->name, (unsigned int) addr);
#endif


	}


      /* Perform relocation */
      if (elfsh_relocate_entry(new, cur, dword, addr) < 0)
	return (-1);

    }
  return (0);
}



/* Inject a section from ET_REL object into ET_EXEC */
static int	elfsh_inject_etrel_section(elfshobj_t *file, elfshsect_t *sect)
{
  Elf32_Shdr	hdr;
  elfshsect_t	*new;
  char		*newname;
  char		writable;
  int		mode;
  char		*data;

  /* else create a new section */
  hdr = elfsh_create_shdr(0, sect->shdr->sh_type, sect->shdr->sh_flags,
			  0, 0, sect->shdr->sh_size, 0, 0, 0, 0);
  XALLOC(newname, strlen(sect->parent->name) + strlen(sect->name) + 2, -1);
  sprintf(newname, "%s%s", sect->parent->name, sect->name);
  new = elfsh_create_section(newname);

  /* Copy the data */
  XALLOC(data, sect->shdr->sh_size, -1);
  memcpy(data, sect->data, sect->shdr->sh_size);

  /* Inject new section by top or after bss depending on its type */
  writable = elfsh_get_section_writableflag(sect->shdr);

  /* FreeBSD is incompatible with pre-interp injection */
#ifdef __FreeBSD__
  mode     = ELFSH_DATA_INJECTION;
#else
  mode     = (writable ? ELFSH_DATA_INJECTION : ELFSH_CODE_INJECTION);
#endif

  if (elfsh_insert_mapped_section(file, new, hdr, data, mode) < 0)
    goto bad;
  new = elfsh_get_section_by_name(file, newname, NULL, NULL, NULL);
  if (new == NULL)
    goto bad;

  return (0);
 bad:
  free(newname);
  return (-1);
}



/* Inject a ET_REL object into a ET_EXEC object */
int		elfsh_inject_etrel(elfshobj_t *file, elfshobj_t *rel)
{
  elfshsect_t	*sect;
  elfshsect_t	*reltab;
  char		sctname[BUFSIZ];
  Elf32_Sym	newsym;
  u_int		symnbr;
  u_int		index;
  static Elf32_Sym	*sym;
  u_char	type;
  elfshsect_t	*bss;
  elfshzone_t	*zone;
  int		bss_size;


  /* Sanity checks */
  if (file == NULL || file->hdr == NULL || rel == NULL || rel->hdr == NULL)
    ELFSH_SETERROR("[libelfsh:inject_etrel] Invalid NULL parameter\n", -1);
  if (file->hdr->e_type != ET_EXEC || rel->hdr->e_type != ET_REL)
    ELFSH_SETERROR("[libelfsh:inject_etrel] Bad object types\n", -1);

  /* First fuzion BSS */
  bss = elfsh_fixup_bss(file);
  if (bss == NULL)
    return (-1);
  bss_size = elfsh_find_bsslen(file, rel);
  if (bss_size < 0)
    return (-1);
  zone = elfsh_create_bsszone(rel->name, bss->shdr->sh_size, bss_size);
  if (zone == NULL)
    return (-1);
  if (elfsh_add_bsszone(bss, zone) < 0)
    return (-1);


  /* First pass : find and inject all allocatable sections */
  for (index = 0; index < rel->hdr->e_shnum; index++)
    {

      /* Get the current section */
      sect = elfsh_get_section_by_index(rel, index, NULL, NULL);
      if (sect == NULL)
	ELFSH_SETERROR("[libelfsh:inject_etrel] Cant read section in ET_REL\n", -1);

      /* Check if the current section need to be mapped */
      if (elfsh_get_section_allocflag(sect->shdr) &&
	  sect->shdr->sh_size &&
	  sect->shdr->sh_type == SHT_PROGBITS &&
	  elfsh_inject_etrel_section(file, sect) < 0)
      return (-1);
    }

  /* Do a copy of the procedure linkage table for eventual redirection */
  if (elfsh_copy_plt(file) < 0)
    return (-1);

#if __DEBUG_RELADD__
  printf("[DEBUG_RELADD] Entering intermediate symbol injection loop\n");
#endif


  /* Intermediate pass : Inject ET_REL symbol table into host file */
  sym = elfsh_get_symtab(rel, &symnbr);
  for (index = 0; index < symnbr; index++)
    {

      type = elfsh_get_symbol_type(sym + index);

      /* Avoid non-injectable symbols */
      if (type != STT_FUNC && type != STT_OBJECT)
	continue;
      if (sym[index].st_shndx >= rel->hdr->e_shnum)
	continue;

      /* Find target section in ET_REL */
      sect = elfsh_get_section_by_index(rel, sym[index].st_shndx, NULL, NULL);
      if (sect == NULL)
	ELFSH_SETERROR("[libelfsh:inject_etrel] "
		       "Cant find extracted section in ET_REL\n", -1);

      /* Filter symbols using source section */
      /* XXX: not sure it is necessary */
      if (sect->shdr->sh_type != SHT_PROGBITS || !sect->shdr->sh_size ||
	  !elfsh_get_section_allocflag(sect->shdr))
	continue;

      /* Find corresponding inserted section in ET_EXEC */
      snprintf(sctname, sizeof(sctname), "%s%s", rel->name, sect->name);
      sect = elfsh_get_section_by_name(file, sctname, NULL, NULL, NULL);
      if (sect == NULL)
	ELFSH_SETERROR("[libelfsh:inject_etrel] "
		       "Cant find inserted section in ET_EXEC\n", -1);

#if __DEBUG_RELADD__
      printf("[DEBUG_RELADD] Injected ET_REL symbol %-20s [%08X] \n",
	     elfsh_get_symbol_name(rel, sym + index),
	     (u_int) (sect->shdr->sh_addr + sym[index].st_value));
#endif

      /* Add symbol in host file */
      newsym = elfsh_create_symbol(sect->shdr->sh_addr + sym[index].st_value,
				   sym[index].st_size,
				   elfsh_get_symbol_type(sym + index),
				   elfsh_get_symbol_bind(sym + index),
				   0, sect->index);
      if (elfsh_insert_symbol(file->secthash[ELFSH_SECTION_SYMTAB], &newsym,
			      elfsh_get_symbol_name(rel, sym + index)) < 0)
	return (-1);
    }

  /* Resynchronize sorted instances of symbol table */
  if (elfsh_sync_sorted_symtab(file->secthash[ELFSH_SECTION_SYMTAB]) < 0)
    return (-1);


#if __DEBUG_RELADD__
  printf("[DEBUG_RELADD] Entering final relocation loop\n");
#endif


  /* Last pass : relocate each inserted section */
  for (index = 0; index < rel->hdr->e_shnum; index++)
    {
      sect = elfsh_get_section_by_index(rel, index, NULL, NULL);
      if (sect == NULL)
	ELFSH_SETERROR("[libelfsh:inject_etrel] Cant get section in ET_REL\n", -1);

      /* Check if the section is mapped */
      if (elfsh_get_section_allocflag(sect->shdr) && sect->shdr->sh_size &&
	  sect->shdr->sh_type == SHT_PROGBITS)
	{

	  /* Find the associate relocation section */
	  snprintf(sctname, sizeof(sctname), "%s%s",
		   (IS_REL(sect) ? ".rel" : ".rela"), sect->name);
	  reltab = elfsh_get_section_by_name(rel, sctname, NULL, NULL, NULL);
	  if (reltab == NULL)
	    continue;

	  /* Find the injected instance of this allocatable section in the ET_EXEC */
	  snprintf(sctname, sizeof(sctname), "%s%s", sect->parent->name, sect->name);
	  sect = elfsh_get_section_by_name(file, sctname, NULL, NULL, NULL);
	  if (sect == NULL)
	    ELFSH_SETERROR("[libelfsh:inject_etrel] Cant get section in ET_EXEC\n", -1);
	  if (elfsh_relocate_etrel_section(sect, reltab) < 0)
	    return (-1);
	}
    }

  /* Everything ran OK */
  return (0);
}

