/*
** elf.c for libelfsh
** 
** Started on  Mon Feb 26 04:09:38 2001 mayhem
** Last update Mon Apr 21 20:58:41 2003 mayhem
*/
#include "libelfsh.h"

/* Change the ELF header flags field */
int		elfsh_set_flags(Elf32_Ehdr *hdr, Elf32_Word flags)
{
  if (!hdr)
    ELFSH_SETERROR("[libelfsh:set_flags] Invalid NULL parameter\n", -1);
  hdr->e_flags = flags;
  return (0);
}

/* Return the ELF header flags field */
Elf32_Word	elfsh_get_flags(Elf32_Ehdr *hdr)
{
  if (!hdr)
    ELFSH_SETERROR("[libelfsh:get_flags] Invalid NULL parameter\n", -1);
  return (hdr->e_flags);
}

/* Change the section header table file offset */
int		elfsh_set_shtoff(Elf32_Ehdr *hdr, Elf32_Off off)
{
  if (!hdr)
    ELFSH_SETERROR("[libelfsh:set_shtoff] Invalid NULL parameter\n", -1);
  hdr->e_shoff = off;
  return (0);
}

/* Return the section header table file offset */
Elf32_Off	elfsh_get_shtoff(Elf32_Ehdr *hdr)
{
  if (!hdr)
    ELFSH_SETERROR("[libelfsh:get_shtoff] Invalid NULL parameter\n", -1);
  return (hdr->e_shoff);
}


/* Change the section header table entry count */
int		elfsh_set_shtnbr(Elf32_Ehdr *hdr, Elf32_Half num)
{
  if (!hdr)
    ELFSH_SETERROR("[libelfsh:set_shtnbr] Invalid NULL parameter\n", -1);
  hdr->e_shnum = num;
  return (0);
}

/* Return the section header table entry count */
Elf32_Half	elfsh_get_shtnbr(Elf32_Ehdr *hdr)
{
  if (!hdr)
    ELFSH_SETERROR("[libelfsh:get_shtnbr] Invalid NULL parameter\n", -1);
  return (hdr->e_shnum);
}


/* Change the program header table file offset */
int		elfsh_set_phtoff(Elf32_Ehdr *hdr, Elf32_Off num)
{
  if (!hdr)
    ELFSH_SETERROR("[libelfsh:set_phtoff] Invalid NULL parameter\n", -1);
  hdr->e_phoff = num;
  return (0);
}

/* Return the program header table file offset */
Elf32_Off	 elfsh_get_phtoff(Elf32_Ehdr *hdr)
{
  if (!hdr)
    ELFSH_SETERROR("[libelfsh:get_phtoff] Invalid NULL parameter\n", -1);
  return (hdr->e_phoff);
}

/* Change the program header table entry count */
int	elfsh_set_phtnbr(Elf32_Ehdr *hdr, Elf32_Half num)
{
  if (!hdr)
    ELFSH_SETERROR("[libelfsh:set_phtnbr] Invalid NULL parameter\n", -1);
  hdr->e_phnum = num;
  return (0);
}

/* Return the program header table entry count */
Elf32_Half	elfsh_get_phtnbr(Elf32_Ehdr *hdr)
{
  if (!hdr)
    ELFSH_SETERROR("[libelfsh:get_phtnbr] Invalid NULL parameter\n", -1);
  return (hdr->e_phnum);
}

/* Change the program entry point */
int		elfsh_set_entrypoint(Elf32_Ehdr *hdr, Elf32_Addr addr)
{
  if (!hdr)
    ELFSH_SETERROR("[libelfsh:set_entrypoint] Invalid NULL parameter\n", -1);
  hdr->e_entry = addr;
  return (0);
}

/* Return the program entry point */
Elf32_Addr	elfsh_get_entrypoint(Elf32_Ehdr *hdr)
{
  if (!hdr)
    ELFSH_SETERROR("[libelfsh:get_entrypoint] Invalid NULL parameter\n", -1);
  return (hdr->e_entry);
}


/* Return the machine type giving its e_arch field */
Elf32_Half	elfsh_get_arch(Elf32_Ehdr *e)
{
  if (!e)
    ELFSH_SETERROR("[libelfsh:get_arch] Invalid NULL parameter\n", -1);
  return (e->e_machine);
}


/* Change the machine type e_arch field */
u_int	elfsh_set_arch(Elf32_Ehdr *e, Elf32_Half val)
{
  if (!e)
    ELFSH_SETERROR("[libelfsh:set_arch] Invalid NULL parameter\n", -1);
  e->e_machine = val;
  return (0);
}


/* Return the ELF object type giving its e_type field */
Elf32_Half	elfsh_get_objtype(Elf32_Ehdr *e)
{
  if (!e)
    ELFSH_SETERROR("[libelfsh:get_objtype] Invalid NULL parameter\n", -1);
  return (e->e_type);
}

/* Return the ELF object type giving its e_type field */
u_int	elfsh_set_objtype(Elf32_Ehdr *e, Elf32_Half val)
{
  if (!e)
    ELFSH_SETERROR("[libelfsh:set_objtype] Invalid NULL parameter\n", -1);
  e->e_type = val;
  return (0);
}

/* Return the ELF section string table index field */
Elf32_Half	elfsh_get_shstrtab_index(Elf32_Ehdr *e)
{
  if (!e)
    ELFSH_SETERROR("[libelfsh:get_shstrtab_index] Invalid NULL parameter\n", 
		   -1);
  return (e->e_shstrndx);
}

/* Set the ELF section string table index field */
u_int	elfsh_set_shstrtab_index(Elf32_Ehdr *e, Elf32_Half val)
{
  if (!e)
    ELFSH_SETERROR("[libelfsh:set_shstrtab_index] Invalid NULL parameter \n", 
		   -1);
  e->e_shstrndx = val;
  return (0);
}


/* Change the e_version flag */
u_int	elfsh_set_version(Elf32_Ehdr *e, Elf32_Word val)
{
  if (!e)
    ELFSH_SETERROR("[libelfsh:set_version] Invalid NULL parameter\n", -1);
  e->e_version = val;
  return (0);
}

/* Read the version flags */
Elf32_Word	elfsh_get_version(Elf32_Ehdr *e)
{
  if (!e)
    ELFSH_SETERROR("[libelfsh:get_version] Invalid NULL parameter\n", 
		   -1);
  return (e->e_version);
}

/* Change the ELF header size field */
u_int	elfsh_set_ehsize(Elf32_Ehdr *e, Elf32_Half val)
{
  if (!e)
    ELFSH_SETERROR("[libelfsh:set_ehsize] Invalid NULL parameter\n", -1);
  e->e_ehsize = val;
  return (0);
}

/* Return the ELF header size field */
Elf32_Half	elfsh_get_ehsize(Elf32_Ehdr *e)
{
  if (!e)
    ELFSH_SETERROR("[libelfsh:get_ehsize] Invalid NULL parameter\n", -1);
  return (e->e_ehsize);
}

/* Change the Program Header Entry size field */
u_int	elfsh_set_phentsize(Elf32_Ehdr *e, Elf32_Half val)
{
  if (!e)
    ELFSH_SETERROR("[libelfsh:set_phentsize] Invalid NULL parameter\n", -1);
  e->e_phentsize = val;
  return (0);
}

/* Return the Program Header Entry size field */
Elf32_Half	elfsh_get_phentsize(Elf32_Ehdr *e)
{
  if (!e)
    ELFSH_SETERROR("[libelfsh:get_phentsize] Invalid NULL parameter\n", 
		   -1);
  return (e->e_phentsize);
}

/* Change the Section Header Entry size field */
u_int	elfsh_set_shentsize(Elf32_Ehdr *e, Elf32_Half val)
{
  if (!e)
    ELFSH_SETERROR("[libelfsh:set_shentsize] Invalid NULL parameter\n", -1);
  e->e_shentsize = val;
  return (0);
}

/* Return the Section Header Entry size field */
Elf32_Half	elfsh_get_shentsize(Elf32_Ehdr *e)
{
  if (!e)
    ELFSH_SETERROR("[libelfsh:get_shentsize] Invalid NULL parameter\n", -1);
  return (e->e_shentsize);
}


/* Return the encoding type */
int		elfsh_get_encoding(Elf32_Ehdr *hdr)
{
  if (!hdr)
    ELFSH_SETERROR("[libelfsh:get_encoding] Invalid NULL parameter\n", -1);
  return (hdr->e_ident[EI_DATA]);
}

/* Change the encoding (little endian, big endian, or else) */
int		elfsh_set_encoding(Elf32_Ehdr *hdr, u_char type)
{
  if (!hdr)
    ELFSH_SETERROR("[libelfsh:set_encoding] Invalid NULL parameter\n", -1);
  hdr->e_ident[EI_DATA] = type;
  return (0);
}


/* Return the magic */
int		elfsh_get_magic(Elf32_Ehdr *hdr)
{
  if (hdr == NULL)
    ELFSH_SETERROR("[libelfsh:get_magic] Invalid NULL parameter\n", -1);
  return (*(u_int *) hdr->e_ident);
}


/* Change the magic */
int		elfsh_set_magic(Elf32_Ehdr *hdr, u_int mag)
{
  if (hdr == NULL)
    ELFSH_SETERROR("[libelfsh:set_magic] Invalid NULL parameter\n", -1);
  *(u_int *) hdr->e_ident = mag;
  return (0);
}

/* Return the class */
int		elfsh_get_class(Elf32_Ehdr *hdr)
{
  if (!hdr)
    ELFSH_SETERROR("[libelfsh:get_class] Invalid NULL parameter\n", -1);
  return (hdr->e_ident[EI_CLASS]);
}


/* Change the class */
int		elfsh_set_class(Elf32_Ehdr *hdr, u_char class)
{
  if (!hdr)
    ELFSH_SETERROR("[libelfsh:set_class] Invalid NULL parameter\n", -1);
  hdr->e_ident[EI_CLASS] = class;
  return (0);
}





/* Load the ELF header */
int		elfsh_load_hdr(elfshobj_t *file)
{
  int	len;
  Elf32_Ehdr *e;
  
  if (file->hdr != NULL)
    return (sizeof(Elf32_Ehdr));
  XALLOC(file->hdr, sizeof(Elf32_Ehdr), -1);
  if ((len = read(file->fd, file->hdr, sizeof(Elf32_Ehdr))) <= 0)
    ELFSH_SETERROR(ELFSH_ERR_ARRAY, len);
#if __BYTE_ORDER == __LITTLE_ENDIAN
  if (file->hdr->e_ident[EI_DATA] == ELFDATA2MSB) 
    {
#elif __BYTE_ORDER == __BIG_ENDIAN
  if (file->hdr->e_ident[EI_DATA] == ELFDATA2LSB) 
    {
#else
#error Unexpected __BYTE_ORDER !
#endif
	  e = file->hdr;
	  e->e_type = swap16(e->e_type);
	  e->e_machine = swap16(e->e_machine);
	  e->e_version = swap32(e->e_version);
	  e->e_entry = swap32(e->e_entry);
	  e->e_phoff = swap32(e->e_phoff);
	  e->e_shoff = swap32(e->e_shoff);
	  e->e_flags = swap32(e->e_flags);
	  e->e_ehsize= swap16(e->e_ehsize);
	  e->e_phentsize = swap16(e->e_phentsize);
	  e->e_phnum = swap16(e->e_phnum);
	  e->e_shentsize = swap16(e->e_shentsize);
	  e->e_shnum = swap16(e->e_shnum); 
	  e->e_shstrndx = swap16(e->e_shstrndx);
     }
  return (len);
}


/* Return a ptr on the ELF header (load it before if not done yet) */
void	*elfsh_get_hdr(elfshobj_t *file)
{
  if (file->hdr != NULL)
    return (file->hdr);
  else if (elfsh_load_hdr(file) <= 0)
    return (NULL);
  return (file->hdr);
}





