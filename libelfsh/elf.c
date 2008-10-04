/**
 * @file elf.c
 * @ingroup libelfsh
** elf.c for libelfsh
** 
** Started on  Mon Feb 26 04:09:38 2001 jfv
** Last update Mon Apr 21 20:58:41 2003 jfv
**
** $Id: elf.c,v 1.19 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libelfsh.h"

/**
 * Change the ELF header flags field 
 * @param hdr
 * @param flags
 * @return
 */
int		elfsh_set_flags(elfsh_Ehdr *hdr, eresi_Addr flags)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!hdr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", -1);
  hdr->e_flags = (elfsh_Word) flags;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Return the ELF header flags field 
 * @param hdr
 * @return
 */
elfsh_Word	elfsh_get_flags(elfsh_Ehdr *hdr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!hdr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", (elfsh_Word) -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (hdr->e_flags));
}

/**
 * Change the section header table file offset 
 * @param hdr
 * @param off
 * @return
 */
int		elfsh_set_shtoff(elfsh_Ehdr *hdr, eresi_Addr off)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!hdr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", -1);
  hdr->e_shoff = (eresi_Off) off;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Return the section header table file offset 
 * @param hdr
 * @return
 */
eresi_Off	elfsh_get_shtoff(elfsh_Ehdr *hdr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!hdr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", (eresi_Off) -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (hdr->e_shoff));
}


/**
 * Change the section header table entry count 
 * @param hdr
 * @param num
 * @return
 */
int		elfsh_set_shtnbr(elfsh_Ehdr *hdr, eresi_Addr num)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!hdr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", -1);
  hdr->e_shnum = (elfsh_Half) num;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Return the section header table entry count 
 * @param hdr
 * @return
 */
elfsh_Half	elfsh_get_shtnbr(elfsh_Ehdr *hdr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!hdr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", (elfsh_Half) -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (hdr->e_shnum));
}


/**
 * Change the program header table file offset 
 * @param hdr
 * @param num
 * @return
 */
int		elfsh_set_phtoff(elfsh_Ehdr *hdr, eresi_Addr num)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!hdr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", -1);
  hdr->e_phoff = (eresi_Off) num;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Return the program header table file offset 
 * @param hdr
 * @return
 */
eresi_Off	 elfsh_get_phtoff(elfsh_Ehdr *hdr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!hdr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", (eresi_Off) -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (hdr->e_phoff));
}

/**
 * Change the program header table file offset 
 * @param hdr
 * @param num
 * @return 
 */
int		elfsh_set_rphtoff(elfsh_Ehdr *hdr, eresi_Addr num)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!hdr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", -1);
  *((eresi_Off *) hdr->e_ident + EI_RPHT) =  (eresi_Off) num;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Return the program header table file offset 
 * @param hdr
 * @return 
 */
eresi_Off	 elfsh_get_rphtoff(elfsh_Ehdr *hdr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!hdr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", (eresi_Off) -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		(*(eresi_Off *) hdr->e_ident + EI_RPHT));
}

/**
 * Change the program header table entry count 
 * @param hdr
 * @param num
 * @return 
 */
int	elfsh_set_phtnbr(elfsh_Ehdr *hdr, eresi_Addr num)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!hdr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", -1);
  hdr->e_phnum = (elfsh_Half) num;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Return the program header table entry count 
 * @param hdr
 * @return 
 */
elfsh_Half	elfsh_get_phtnbr(elfsh_Ehdr *hdr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!hdr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", (elfsh_Half) -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (hdr->e_phnum));
}

/** 
 * Change the program entry point 
 * @param hdr
 * @param addr
 * @return 
 */
int		elfsh_set_entrypoint(elfsh_Ehdr *hdr, eresi_Addr addr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!hdr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", -1);
  hdr->e_entry = addr;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** 
 * Return the program entry point 
 * @param hdr
 * @return 
 */
eresi_Addr	elfsh_get_entrypoint(elfsh_Ehdr *hdr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!hdr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", (eresi_Addr) -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (hdr->e_entry));
}

/**
 * Return the machine type giving its e_arch field 
 * @param hdr
 * @param num
 * @return 
 */
elfsh_Half	elfsh_get_arch(elfsh_Ehdr *hdr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!hdr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", (elfsh_Half) -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (hdr->e_machine));
}

/**
 * Change the machine type e_arch field 
 * @param hdr
 * @param val
 * @return 
 */
unsigned int	elfsh_set_arch(elfsh_Ehdr *hdr, eresi_Addr val)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!hdr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", (unsigned int) -1);
  hdr->e_machine = (elfsh_Half) val;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Return the ELF object type giving its e_type field 
 * @param hdr
 * @return 
 */
elfsh_Half	elfsh_get_objtype(elfsh_Ehdr *hdr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!hdr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", (elfsh_Half) -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (hdr->e_type));
}

/**
 * Return the ELF object type giving its e_type field 
 * @param e
 * @param val
 * @return 
 */
unsigned int	elfsh_set_objtype(elfsh_Ehdr *e, eresi_Addr val)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!e)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", (unsigned int) -1);
  e->e_type = (elfsh_Half) val;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Return the ELF section string table index field 
 * @param e
 * @return 
 */
elfsh_Half	elfsh_get_shstrtab_index(elfsh_Ehdr *e)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!e)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", (elfsh_Half) -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (e->e_shstrndx));
}

/**
 * Set the ELF section string table index field 
 * @param e
 * @param val
 * @return 
 */
unsigned int	elfsh_set_shstrtab_index(elfsh_Ehdr *e, eresi_Addr val)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!e)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter ", (unsigned int) -1);
  e->e_shstrndx = (elfsh_Half) val;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Change the e_version flag 
 * @param e
 * @param val
 * @return 
 */
unsigned int	elfsh_set_version(elfsh_Ehdr *e, eresi_Addr val)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!e)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", (unsigned int) -1);
  e->e_version = (elfsh_Word) val;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Read the version flags 
 * @param e
 * @return 
 */
elfsh_Word	elfsh_get_version(elfsh_Ehdr *e)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!e)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", (elfsh_Word) -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (e->e_version));
}

/**
 * Change the ELF header size field 
 * @param e
 * @param val
 * @return 
 */
unsigned int	elfsh_set_ehsize(elfsh_Ehdr *e, eresi_Addr val)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!e)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", (unsigned int) -1);
  e->e_ehsize = (elfsh_Half) val;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Return the ELF header size field 
 * @param e
 * @return 
 */
elfsh_Half	elfsh_get_ehsize(elfsh_Ehdr *e)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!e)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", (elfsh_Half) -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (e->e_ehsize));
}

/**
 * Change the Program Header Entry size field 
 * @param e
 * @param val
 * @return 
 */
unsigned int	elfsh_set_phentsize(elfsh_Ehdr *e, eresi_Addr val)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!e)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", (unsigned int) -1);
  e->e_phentsize = (elfsh_Half) val;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Return the Program Header Entry size field 
 * @param e
 * @return 
 */
elfsh_Half	elfsh_get_phentsize(elfsh_Ehdr *e)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!e)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", (elfsh_Half) -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (e->e_phentsize));
}

/**
 * Change the Section Header Entry size field 
 * @param e
 * @param val
 * @return 
 */
unsigned int	elfsh_set_shentsize(elfsh_Ehdr *e, eresi_Addr val)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!e)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", (unsigned int) -1);
  e->e_shentsize = (elfsh_Half) val;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Return the Section Header Entry size field 
 * @param e
 * @return 
 */
elfsh_Half	elfsh_get_shentsize(elfsh_Ehdr *e)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!e)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", (elfsh_Half) -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (e->e_shentsize));
}


/**
 * Return the encoding type 
 * @param hdr
 * @return 
 */
int		elfsh_get_encoding(elfsh_Ehdr *hdr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!hdr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (hdr->e_ident[EI_DATA]));
}

/**
 * Change the encoding (little endian, big endian, or else) 
 * @param hdr
 * @param type
 * @return 
 */
int		elfsh_set_encoding(elfsh_Ehdr *hdr, eresi_Addr type)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!hdr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", -1);
  hdr->e_ident[EI_DATA] = (unsigned char) type;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Return the magic 
 * @param hdr
 * @return 
 */
int		elfsh_get_magic(elfsh_Ehdr *hdr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (hdr == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (*(unsigned int *) hdr->e_ident));
}

/**
 * Change the magic 
 * @param hdr
 * @param mag
 * @return 
 */
int		elfsh_set_magic(elfsh_Ehdr *hdr, eresi_Addr mag)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (hdr == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", -1);
  *(unsigned int *) hdr->e_ident = (unsigned int) mag;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Return the class 
 * @param hdr
 * @return 
 */
int		elfsh_get_class(elfsh_Ehdr *hdr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!hdr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (hdr->e_ident[EI_CLASS]));
}

/**
 * Change the class 
 * @param hdr
 * @param eclass
 * @return 
 */
int		elfsh_set_class(elfsh_Ehdr *hdr, eresi_Addr eclass)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!hdr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", -1);
  hdr->e_ident[EI_CLASS] = (unsigned char) eclass;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Endianize ELF header 
 * @param e
 * @param byteorder
 * @return 
 */
void		elfsh_endianize_elfhdr(elfsh_Ehdr *e, char byteorder)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __BYTE_ORDER == __LITTLE_ENDIAN
  if (byteorder == ELFDATA2MSB) {
#elif __BYTE_ORDER == __BIG_ENDIAN
    if (byteorder == ELFDATA2LSB) {
#else
#error Unexpected __BYTE_ORDER !
#endif
      e->e_type      = swap16(e->e_type);
      e->e_machine   = swap16(e->e_machine);
      e->e_version   = swap32(e->e_version);
      e->e_entry     = swaplong(e->e_entry);
      e->e_phoff     = swaplong(e->e_phoff);
      e->e_shoff     = swaplong(e->e_shoff);
      e->e_flags     = swap32(e->e_flags);
      e->e_ehsize    = swap16(e->e_ehsize);
      e->e_phentsize = swap16(e->e_phentsize);
      e->e_phnum     = swap16(e->e_phnum);
      e->e_shentsize = swap16(e->e_shentsize);
      e->e_shnum     = swap16(e->e_shnum); 
      e->e_shstrndx  = swap16(e->e_shstrndx);
    }

    PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 * Sanity check of header type
 * @param file
 * @return 
 */
int elfsh_check_hdr_type(elfshobj_t *file) 
{  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (file->hdr->e_type) 
    {
    case ET_NONE: break;
    case ET_REL:  break;
    case ET_EXEC: break;
    case ET_DYN:  break;
    case ET_CORE: break;
    default:
      file->hdr->e_type = ET_NONE;
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
	           "file->hdr->e_type is not valid", NULL);
      break;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Sanity check of machin type
 * @param file
 * @return 
 */
int elfsh_check_hdr_machine(elfshobj_t *file) 
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /** /usr/include/elf.h e_machine may be > 18. 
   *  disabling this check for the moment.
   if (file->hdr->e_machine > 16) {
   file->hdr->e_machine = ET_NONE;
   PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
   "file->hdr->e_machine is not valid", NULL);
   }
   */

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Sanity check of version value
 * @param file
 * @return 
 */
int elfsh_check_hdr_version(elfshobj_t *file) 
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->hdr->e_version != EV_CURRENT) 
    {
    file->hdr->e_version = EV_CURRENT;
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "file->hdr->e_version is not valid", NULL);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Sanity check of phoff value
 * @param file
 * @return 
 */
int elfsh_check_hdr_phoff(elfshobj_t *file) 
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->hdr->e_phoff > (eresi_Off) file->fstat.st_size) 
    {
    file->hdr->e_phoff = ET_NONE;
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "file->hdr->e_phoff > file length", NULL);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Sanity check of shoff value
 * @param file
 * @return 
 */
int elfsh_check_hdr_shoff(elfshobj_t *file) 
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->hdr->e_shoff > (eresi_Off) file->fstat.st_size) 
    {
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "file->hdr->e_shoff > file length", NULL);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * perform some sanity checks on ELF header 
 * @param file
 * @return 
 */
int		elfsh_check_hdr(elfshobj_t *file) 
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  elfsh_check_hdr_type(file);
  elfsh_check_hdr_machine(file);
  elfsh_check_hdr_version(file);
  elfsh_check_hdr_phoff(file);
  elfsh_check_hdr_shoff(file);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Load the ELF header 
 * @param file
 * @return 
 */
int	elfsh_load_hdr(elfshobj_t *file)
{
  int	len;
  void	*data;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->hdr != NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sizeof(elfsh_Ehdr)));

  XALLOC(__FILE__, __FUNCTION__, __LINE__,data, sizeof(elfsh_Ehdr), -1);
  file->hdr = (elfsh_Ehdr *) data;
  if ((len = read(file->fd, file->hdr, sizeof(elfsh_Ehdr))) <= 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, (char *)PROFILER_ERRORS_ARRAY, 
		 len);

  if (config_safemode() && (!file->hdr->e_shnum))
   PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
   "file->hdr->e_shnum is not valid", -1); 
  
  if (config_safemode() && (!file->hdr->e_shentsize && (file->hdr->e_type != ET_CORE)))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "file->hdr->e_shentsize is not valid", -1); 

  elfsh_endianize_elfhdr(file->hdr, file->hdr->e_ident[EI_DATA]);

  /* If we are in safe mode, we check the ELF header */
  if (config_safemode())
    elfsh_check_hdr(file);

#if defined(ERESI32)
  if (file->hdr->e_ident[EI_CLASS] != ELFCLASS32)
#elif defined(ERESI64)
    if (file->hdr->e_ident[EI_CLASS] != ELFCLASS64)    
#endif
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Wrong ELFsh configuration for this ELF class",
		   -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (len));
}

/**
 * Return a ptr on the ELF header (load it before if not done yet) 
 * @param file
 * @return 
 */
void	*elfsh_get_hdr(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->hdr != NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (file->hdr));
  else if (elfsh_load_hdr(file) <= 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to load ELF header", NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (file->hdr));
}


/**
 * 
 * @param file
 * @return 1 if the file is static or dynamic 
 */
int		elfsh_static_file(elfshobj_t *file)
{
  elfsh_Phdr	*p;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  p = elfsh_get_segment_by_type(file, PT_DYNAMIC, 0);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (p ? 0 : 1));
}

/**
 * @param file
 * @return 1 if the file is dynamic
 */
int		elfsh_dynamic_file(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,  
		(!elfsh_static_file(file)));
}

/**
 * Get notes section from core
 * @param file
 * @return
 */
int elfsh_get_core_notes(elfshobj_t *file)
{
  elfsh_Nhdr  nhdr;

  int         i, offset, filesz, 
	      j, namesz, descsz;
  int         pos;
  char        name[18];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->pht == NULL)
    elfsh_load_pht(file);

  for (i = 0; i != file->hdr->e_phnum; i++) 
    {
    if (elfsh_get_segment_type(&file->pht[i]) == PT_NOTE) 
      {
	offset = elfsh_get_segment_offset(&file->pht[i]);
	filesz = elfsh_get_segment_filesz(&file->pht[i]);

	if(lseek(file->fd, offset, SEEK_SET) < 0)
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);

	pos = lseek(file->fd, 0L, SEEK_CUR);

	for(j = 0; pos < offset + filesz; j++) 
	  {
	    if(read(file->fd, &nhdr, sizeof(nhdr)) != sizeof(nhdr))
	      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);

	    namesz = elfsh_roundup(nhdr.n_namesz, 4);
	    descsz = elfsh_roundup(nhdr.n_descsz, 4);

	    if(namesz > 16) continue;

	    if(read(file->fd, name, namesz) != namesz) 
	      break;
	    name[namesz] = 0;

	    switch(elfsh_get_ostype(file)) 
	      {
		case ELFSH_OS_LINUX:
		  file->coretype = ELFSH_CORE_LINUX;
		  file->core.filesz = filesz;
		  break;
		case ELFSH_OS_FREEBSD:
		  file->coretype = ELFSH_CORE_FREEBSD;
		  file->bsdcore.filesz = filesz;
		  break;
	      }

	    switch(nhdr.n_type) 
	      {
		case NT_PRPSINFO:
		  if(file->coretype == ELFSH_CORE_FREEBSD) 
		    {
		      if(read(file->fd, &file->bsdcore.prpsinfo, descsz) != descsz)
			PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);
		      file->bsdcore.flags |= ELFSH_CORE_HAS_PRPSINFO;
		    } 
		  else 
		    {
		      if(read(file->fd, &file->core.prpsinfo, descsz) != descsz)
			PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);
		      file->core.flags |= ELFSH_CORE_HAS_PRPSINFO;
		    }
		  break;
		case NT_PRSTATUS:
		  if(file->coretype == ELFSH_CORE_FREEBSD) 
		    {
		      if(read(file->fd, &file->bsdcore.prstatus, descsz) != descsz)
			PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);
		      file->bsdcore.flags |= ELFSH_CORE_HAS_PRSTATUS;
		    } 
		  else 
		    {
		      if(read(file->fd, &file->core.prstatus, descsz) != descsz)
			PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);
		      file->core.flags |= ELFSH_CORE_HAS_PRSTATUS;
		    }
		    break;
	      case NT_FPREGSET:
		if(file->coretype == ELFSH_CORE_FREEBSD) 
		  {
		    if(read(file->fd, &file->bsdcore.fpregs, descsz) != descsz)
		      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);
		    file->bsdcore.flags |= ELFSH_CORE_HAS_FPREGSET;
		  } 
		else 
		  {
		    if(read(file->fd, &file->core.fpregs, descsz) != descsz)
		      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);
		    file->core.flags |= ELFSH_CORE_HAS_FPREGSET;
		  }
		break;
	      case NT_AUXV: /* TODO: figure out how use this data in a auxv vector */
		// if (read(file->fd, &file->core.auxv, descsz) != descsz)
		//  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);
	      case NT_TASKSTRUCT:
	      case NT_PLATFORM:
	      case NT_GWINDOWS:
	      case NT_ASRS:
	      case NT_PSTATUS:
	      case NT_PRCRED:
	      case NT_UTSNAME:
	      case NT_LWPSTATUS:
	      case NT_LWPSINFO:
	      case NT_PRFPXREG:
	      default: break;
	    }
	  pos = lseek(file->fd, 0L, SEEK_CUR);
	}
      }
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
}
