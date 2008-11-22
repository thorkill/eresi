/**
 * @file obj.c
 * @ingroup libelfsh
** obj.c for elfsh
** 
** Started on  Wed Feb 12 00:07:06 2003 jfv
** Last update Sun Mar  4 00:46:13 2007 thorkill
**
** $Id: obj.c,v 1.11 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libelfsh.h"


/**
 * Create an object map from scratch. Generally used to represent memory without true ELF structure.
 * @param name Name to be given to the map.
 * @return The elfshobj_t describing the new map.
 */
elfshobj_t		*elfsh_create_obj(char *name, eresi_Addr start, u_int size, 
					  elfsh_Half arch, elfsh_Half type, u_char enc, u_char clas)
{
  elfshobj_t		*file;
  u_int			strsize;
  elfsh_Shdr		maphdr;
  elfsh_Shdr	        symhdr;
  elfsh_Shdr		strhdr;
  elfsh_Shdr		shstrhdr;
  elfsh_Ehdr		*hdr;
  elfsh_Shdr		*sht;
  elfsh_Phdr		*pht;
  u_int			mapsz;
  u_int			symsz;
  u_int			strsz;
  u_int			shstrsz;
  u_int			symtabsz;
  elfshsect_t		*mapped;
  elfshsect_t		*symtab;
  elfshsect_t		*strtab;
  elfshsect_t		*shstrtab;
  elfsh_Sym		mappedsym;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Allocate the map structure */
  XALLOC(__FILE__, __FUNCTION__, __LINE__, file, sizeof(elfshobj_t), NULL);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, hdr, sizeof(elfsh_Ehdr), NULL);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, sht, sizeof(elfsh_Shdr) * 4, NULL);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, pht, sizeof(elfsh_Phdr), NULL);
  file->name = strdup(name);
  file->fd = (-1);

  /* Compute some needed sizes */
  mapsz = strlen(ELFSH_SECTION_NAME_MAPPED);
  symsz = strlen(ELFSH_SECTION_NAME_SYMTAB);
  strsz = strlen(ELFSH_SECTION_NAME_STRTAB);
  shstrsz = strlen(ELFSH_SECTION_NAME_SHSTRTAB);
  strsize = mapsz + symsz + strsz + shstrsz + 4;
  symtabsz = sizeof(elfsh_Sym);

  /* Setup header */
  elfsh_set_shentsize(hdr, sizeof(elfsh_Shdr));
  elfsh_set_phentsize(hdr, sizeof(elfsh_Phdr));
  elfsh_set_ehsize(hdr, sizeof(elfsh_Ehdr));
  elfsh_set_version(hdr, EV_CURRENT);
  elfsh_set_shstrtab_index(hdr, 3);
  elfsh_set_objtype(hdr, ET_EXEC);
  elfsh_set_shtnbr(hdr, 4);
  elfsh_set_phtnbr(hdr, 1);
  elfsh_set_shtoff(hdr, sizeof(elfsh_Ehdr) + sizeof(elfsh_Phdr) + size + symtabsz + (strsize * 2));
  elfsh_set_phtoff(hdr, sizeof(elfsh_Ehdr));
  elfsh_set_arch(hdr, arch);
  elfsh_set_objtype(hdr, type);
  elfsh_set_entrypoint(hdr, start);
  elfsh_set_encoding(hdr, enc);
  elfsh_set_class(hdr, clas);
  memcpy(hdr->e_ident, ELFMAG, strlen(ELFMAG));
  file->hdr = hdr;

  /* Create sections table */
  maphdr = elfsh_create_shdr(0, SHT_PROGBITS, SHF_ALLOC, 0, 
			     sizeof(elfsh_Ehdr) + sizeof(elfsh_Phdr), 
			     size, 0, 0, sizeof(eresi_Addr), 0);

  symhdr = elfsh_create_shdr(mapsz + 1, SHT_SYMTAB, SHF_WRITE, 0, 
			     sizeof(elfsh_Ehdr) + sizeof(elfsh_Phdr) + size, 
			     symtabsz, 2, 0, 0, sizeof(elfsh_Sym));

  strhdr = elfsh_create_shdr(mapsz + symsz + 2, SHT_STRTAB, SHF_WRITE, 0, 
			     sizeof(elfsh_Ehdr) + sizeof(elfsh_Phdr) + size + symtabsz, 
			     strsize, 1, 0, 0, 0);

  shstrhdr = elfsh_create_shdr(mapsz + symsz + strsz + 3, SHT_STRTAB, SHF_WRITE, 0, 
			       sizeof(elfsh_Ehdr) + sizeof(elfsh_Phdr) + size + symtabsz + strsize, 
			       strsize, 0, 0, 0, 0);

  memcpy(sht, &maphdr, sizeof(elfsh_Shdr));
  memcpy((char *) sht + sizeof(elfsh_Shdr), &symhdr, sizeof(elfsh_Shdr));
  memcpy((char *) sht + sizeof(elfsh_Shdr) * 2, &strhdr, sizeof(elfsh_Shdr));
  memcpy((char *) sht + sizeof(elfsh_Shdr) * 3, &shstrhdr, sizeof(elfsh_Shdr));
  file->sht = sht;

  /* Create segments table */
  elfsh_set_segment_type(pht, PT_LOAD);
  elfsh_set_segment_flags(pht, PF_X | PF_W | PF_R);
  elfsh_set_segment_memsz(pht, size);
  file->pht = pht;

  /* Allocate and fill section descriptors */
  XALLOC(__FILE__, __FUNCTION__, __LINE__, mapped, sizeof(elfshsect_t), NULL);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, symtab, sizeof(elfshsect_t), NULL);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, strtab, sizeof(elfshsect_t), NULL);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, shstrtab, sizeof(elfshsect_t), NULL);
  
  mapped->name = strdup(ELFSH_SECTION_NAME_MAPPED);
  mapped->parent = file;
  mapped->phdr = pht;
  mapped->shdr = sht;
  mapped->next = symtab;
  XALLOC(__FILE__, __FUNCTION__, __LINE__, mapped->data, size, NULL);

  symtab->name = strdup(ELFSH_SECTION_NAME_SYMTAB);
  symtab->parent = file;
  symtab->shdr = (elfsh_Shdr *) ((char *) sht + sizeof(elfsh_Shdr));
  symtab->prev = mapped;
  symtab->next = strtab;
  symtab->index = 1;
  symtab->curend = sizeof(elfsh_Sym);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, symtab->data, sizeof(elfsh_Sym), NULL);
  mappedsym = elfsh_create_symbol(0, size, STT_SECTION, 0, 0, 0);
  memcpy(symtab->data, &mappedsym, sizeof(elfsh_Sym));

  strtab->name = strdup(ELFSH_SECTION_NAME_STRTAB);
  strtab->parent = file;
  strtab->shdr = (elfsh_Shdr *) ((char *) sht + sizeof(elfsh_Shdr) * 2);
  strtab->prev = symtab;
  strtab->next = shstrtab;
  strtab->index = 2;
  XALLOC(__FILE__, __FUNCTION__, __LINE__, strtab->data, mapsz + 1, NULL);
  strcpy(strtab->data, ELFSH_SECTION_NAME_MAPPED);

  shstrtab->name = strdup(ELFSH_SECTION_NAME_SHSTRTAB);
  shstrtab->parent = file;
  shstrtab->shdr = (elfsh_Shdr *) ((char *) sht + sizeof(elfsh_Shdr) * 3);
  shstrtab->prev = strtab;
  shstrtab->index = 3;
  XALLOC(__FILE__, __FUNCTION__, __LINE__, shstrtab->data, strsize, NULL);
  strcpy(shstrtab->data, ELFSH_SECTION_NAME_MAPPED);
  strcpy(shstrtab->data + mapsz + 1, ELFSH_SECTION_NAME_SYMTAB);
  strcpy(shstrtab->data + mapsz + symsz + 2, ELFSH_SECTION_NAME_STRTAB);
  strcpy(shstrtab->data + mapsz + symsz + strsz + 3, ELFSH_SECTION_NAME_SHSTRTAB);

  /* The final touch */
  file->sectlist = mapped;
  file->secthash[ELFSH_SECTION_SYMTAB] = symtab;
  file->secthash[ELFSH_SECTION_STRTAB] = strtab;
  file->secthash[ELFSH_SECTION_SHSTRTAB] = shstrtab;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, file);
}


/**
 * Open the file 'name' and create a descriptor.
 * @param name File path to open.
 * @return The elfshobj_t describing the loaded file.
 */
elfshobj_t		*elfsh_load_obj(char *name)
{
  elfshobj_t	*file;
 
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(__FILE__, __FUNCTION__, __LINE__,file, sizeof(elfshobj_t), NULL);
  XOPEN(file->fd, name, O_RDONLY, 0, NULL);
  file->name = strdup(name);

  /* Get the file size on disk */
  if (0 != fstat(file->fd,&file->fstat))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to get fstat(2)", NULL);

  file->hdr = elfsh_get_hdr(file);
  file->rights = O_RDONLY;
  if (file->hdr == NULL || file->name == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to get ELF header", NULL);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (file));
}



/**
 * Free the ELF descriptor and its fields 
 * @param file The elfsh obj
 */
void		elfsh_unload_obj(elfshobj_t *file)
{
  elfshsect_t	*sect;
  elfshsect_t	*next;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  XFREE(__FILE__, __FUNCTION__, __LINE__, file->hdr);
  XFREE(__FILE__, __FUNCTION__, __LINE__, file->pht);
  XFREE(__FILE__, __FUNCTION__, __LINE__, file->sht);

  for (sect = file->sectlist; sect; sect = next)
    {
      /* FIXME: Prevent dobule free() */
      if (sect->data == sect->altdata)
	sect->altdata = NULL;

      if (sect->data == sect->terdata)
	sect->terdata = NULL;

      if (sect->data == sect->lastdata)
	sect->lastdata = NULL;

      XFREE(__FILE__, __FUNCTION__, __LINE__,sect->data);

      if (sect->altdata)
	XFREE(__FILE__, __FUNCTION__, __LINE__,sect->altdata);
      
      if (sect->terdata)
	XFREE(__FILE__, __FUNCTION__, __LINE__,sect->terdata);

      if (sect->lastdata)
	XFREE(__FILE__, __FUNCTION__, __LINE__,sect->lastdata);
      
      XFREE(__FILE__, __FUNCTION__, __LINE__,sect->name);
      next = sect->next;
      XFREE(__FILE__, __FUNCTION__, __LINE__,sect);
    }

  if (file->fd)
    close(file->fd);

  XFREE(__FILE__, __FUNCTION__, __LINE__,file->name);
  XFREE(__FILE__, __FUNCTION__, __LINE__,file);

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}
