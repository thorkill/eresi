/*
** sht_rebuild.c for elfsh
** 
** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 
** This code is functional as it, but it is WORK IN PROGRESS 
** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 
**
** Started on  Tue Mar 26 19:07:23 2002 mayhem
** Last update Sat May 10 04:18:26 2003 mayhem
*/
#include "libelfsh.h"




/* INTERNAL FUNCTION : ELFsh SHT entry insertion algorithm */
static int		add_sht_entry(elfshobj_t *file, int phdr_index, Elf32_Shdr shdr, char *name)
{
  elfshsect_t		*sect;
  int			index;
  int			namelen;
  int			len;
  int			off;
  char			buff[BUFSIZ];

  namelen = strlen(name);

  printf("[libelfsh debug] Insertion section header *%s* \n", name);

  for (index = 0; index < file->hdr->e_shnum; index++)
    {
      
      /* Used to allow a 3 bytes padding for the current section */
      len = file->sht[index].sh_size - shdr.sh_size;

      printf("[libelfsh debug] SHT rebuild: len = *%d* \n", len);

      if (len < 0)
	len = -len;

      /* The new section perfectly match an existing one (with an allowed padding of 3 bytes) */
      if (file->sht[index].sh_offset == shdr.sh_offset && len < 4)
	{

	  /* In the rare case where both names have equal lenght, we can optimize a bit */
	  len = (file->sht[index].sh_name ? 
		 strlen(file->secthash[ELFSH_SECTION_SHSTRTAB]->data + file->sht[index].sh_name) : 0);
	  if (len == namelen)
	    {
	      printf("[libelfsh debug] SHT rebuild case 1.1 \n");
	      memcpy(file->secthash[ELFSH_SECTION_SHSTRTAB]->data + file->sht[index].sh_name, name, len);
	    }

	  /* The off variable is used just to clean the code . This case is the common one . */
	  else
	    {
	      printf("[libelfsh debug] SHT rebuild case 1.2 \n");
	      off = file->sht[index].sh_name + len + 1;
	      memmove(file->secthash[ELFSH_SECTION_SHSTRTAB]->data + file->sht[index].sh_name, 
		      file->secthash[ELFSH_SECTION_SHSTRTAB]->data + off, 
		      file->secthash[ELFSH_SECTION_SHSTRTAB]->shdr->sh_size - off);
	      file->sht[index].sh_size -= len + 1;
	      elfsh_update_nameidx(file, file->sht[index].sh_name, len + 1);
	      file->sht[index].sh_name = elfsh_insert_in_shstrtab(file, name);
	    }
	  break;
	}
      

      /* The new header defines a section matching the beginning area of an existing section */
      else if (file->sht[index].sh_offset == shdr.sh_offset)
	{
	  printf("[libelfsh debug] SHT rebuild case 2 \n");
	  XREALLOC(file->sht, file->sht, (file->hdr->e_shnum + 1) * file->hdr->e_shentsize, -1);
	  memmove(file->sht + index + 1, 
		  file->sht + index, 
		  file->hdr->e_shentsize * (file->hdr->e_shnum - index));
	  
	  file->sht[index] = shdr;
	  if (strcmp(ELFSH_SECTION_NAME_BSS, name))
	    {
	      file->sht[index + 1].sh_size -= (shdr.sh_size);
	      file->sht[index + 1].sh_offset += (shdr.sh_size + 1);
	    }
	  file->sht[index + 1].sh_addr += (file->sht[index + 1].sh_addr ? shdr.sh_size : 0);

	  if (file->hdr->e_shstrndx >= index)
	    file->hdr->e_shstrndx++;
	  file->secthash[ELFSH_SECTION_SHSTRTAB]->shdr = file->sht + file->hdr->e_shstrndx;
	  file->sht[index].sh_name = elfsh_insert_in_shstrtab(file, name);

	  XALLOC(sect, sizeof(*sect), -1);
	  if (elfsh_add_section(file, sect, index, NULL, ELFSH_SHIFTING_ABSENT) < 0)
	    return (-1);
	  
	  file->hdr->e_shnum++;
	  elfsh_update_linkidx(file, index, 1);
	  break;
	}
      

      /* The new header defines a section matching the end area of an existing section */
      else if (file->sht[index].sh_offset + file->sht[index].sh_size == shdr.sh_offset + shdr.sh_size)
	{
	  printf("[libelfsh debug] SHT rebuild case 3 \n");
	  XREALLOC(file->sht, file->sht, (file->hdr->e_shnum + 1) * file->hdr->e_shentsize, -1);
	  memmove(file->sht + index + 2, 
		  file->sht + index + 1, 
		  file->hdr->e_shentsize * (file->hdr->e_shnum - index - 1));

	  file->sht[index + 1] = shdr;
	  file->sht[index].sh_size -= shdr.sh_size;

	  file->hdr->e_shstrndx++;
	  file->secthash[ELFSH_SECTION_SHSTRTAB]->shdr = file->sht + file->hdr->e_shstrndx;
	  file->sht[index + 1].sh_name = elfsh_insert_in_shstrtab(file, name);
	  
	  XALLOC(sect, sizeof(*sect), -1);
	  if (elfsh_add_section(file, sect, index, NULL, ELFSH_SHIFTING_ABSENT) < 0)
	    return (-1);

	  file->hdr->e_shnum++;
	  elfsh_update_linkidx(file, index + 1, 1);
	  break;
	}
      

      /* The new header defines a section strictly included in an already existing section */
      else if (file->sht[index].sh_offset < shdr.sh_offset && 
	       file->sht[index].sh_offset + file->sht[index].sh_size > shdr.sh_offset + shdr.sh_size)
	{
	  printf("[libelfsh debug] SHT rebuild case 4 \n");

	  XREALLOC(file->sht, file->sht, (file->hdr->e_shnum + 2) * file->hdr->e_shentsize, -1);
	  memmove(file->sht + index + 3, 
		  file->sht + index + 1, 
		  file->hdr->e_shentsize * (file->hdr->e_shnum - index - 1));

	  file->sht[index + 1] = shdr;
	  file->sht[index + 2] = elfsh_create_shdr(0, file->sht[index].sh_type,
						   file->sht[index].sh_flags,
						   shdr.sh_addr + shdr.sh_size,
						   shdr.sh_offset + shdr.sh_size,
						   file->sht[index].sh_size - 
						   (shdr.sh_offset - file->sht[index].sh_offset) - 
						   shdr.sh_size,
						   0, 0, 0, 0);
	  file->sht[index].sh_size = shdr.sh_offset - file->sht[index].sh_offset;

	  file->hdr->e_shstrndx += 2;
	  file->secthash[ELFSH_SECTION_SHSTRTAB]->shdr = file->sht + file->hdr->e_shstrndx;

	  file->sht[index + 1].sh_name = elfsh_insert_in_shstrtab(file, name);
	  snprintf(buff, sizeof(buff), "%s_%u", 
		   (file->sht[index + 2].sh_addr ? ELFSH_SECTION_NAME_MAPPED : ELFSH_SECTION_NAME_UNMAPPED),
		   file->sht[index + 2].sh_offset);
	  file->sht[index + 2].sh_name = elfsh_insert_in_shstrtab(file, buff);

	  XALLOC(sect, sizeof(*sect), -1);
	  if (elfsh_add_section(file, sect, index, NULL, ELFSH_SHIFTING_ABSENT) < 0)
	    return (-1);
	  XALLOC(sect, sizeof(*sect), -1);
	  if (elfsh_add_section(file, sect, index + 2, NULL, ELFSH_SHIFTING_ABSENT) < 0)
	    return (-1);

	  file->hdr->e_shnum += 2;
	  elfsh_update_linkidx(file, index, 1);
	  break;
	}
    }       

  return (0);
}




/* INTERNAL FUNCTION : Rebuild an initial SHT and create .mapped, .unmapped, and .shstrtab */
static int	init_sht(elfshobj_t *file, u_int num)
{
  Elf32_Phdr	*low;
  Elf32_Phdr	*high;
  Elf32_Shdr	shdr;
  elfshsect_t	*sect;
  struct stat	st;
  char		buff[256];
  u_int		index;

  /* Get the file size on disk */
  if (fstat(file->fd, &st) != 0)
    return (-1);
  file->hdr->e_shentsize = sizeof(Elf32_Shdr);

  /* Do the first pass : guess the borders for .mapped and .unmapped */
  for (low = high = NULL, index = 0; index < num; index++)
    {
      if (!high || (file->pht[index].p_offset + file->pht[index].p_filesz) > (high->p_offset + high->p_filesz))
	high = file->pht + index;
      if (!low || file->pht[index].p_offset < low->p_offset)
	low = file->pht + index;
    }

  /* Create the initial SHT */
  XALLOC(file->sht, file->hdr->e_shentsize * 3, -1);
  file->hdr->e_shoff = st.st_size;
  file->hdr->e_shnum = 3;
  file->hdr->e_shstrndx = 2;

  /* Insert the .mapped section */
  shdr = elfsh_create_shdr(0, SHT_PROGBITS, SHF_ALLOC, low->p_vaddr, low->p_offset, 
			   high->p_offset + high->p_filesz, 0, 0, 0, 0);
  file->sht[0] = shdr;
  XALLOC(sect, sizeof(elfshsect_t), -1);
  if (elfsh_add_section(file, sect, 0, NULL, ELFSH_SHIFTING_NONE) < 0)
    return (-1);

  /* Insert the .unmapped section */
  shdr = elfsh_create_shdr(0, SHT_PROGBITS, 0, 0, high->p_offset + high->p_filesz, 
			   st.st_size - (high->p_offset + high->p_filesz), 0, 0, 0, 0);
  file->sht[1] = shdr;
  XALLOC(sect, sizeof(elfshsect_t), -1);
  if (elfsh_add_section(file, sect, 1, NULL, ELFSH_SHIFTING_NONE) < 0)
    return (-1);

  /* Insert the section header string table (.sh_strtab) */
  shdr = elfsh_create_shdr(0, SHT_STRTAB, 0, 0, st.st_size, 0, 0, 0, 0, 0);
  file->sht[2] = shdr;
  XALLOC(sect, sizeof(elfshsect_t), -1);
  if (elfsh_add_section(file, sect, 2, NULL, ELFSH_SHIFTING_NONE) < 0)
    return (-1);
  file->secthash[ELFSH_SECTION_SHSTRTAB] = sect;

  /* Insert names in .shstrtab */
  snprintf(buff, sizeof(buff), "%s_%u", ELFSH_SECTION_NAME_MAPPED, file->sht[0].sh_offset); 
  file->sht[0].sh_name = elfsh_insert_in_shstrtab(file, buff);
  snprintf(buff, sizeof(buff), "%s_%u", ELFSH_SECTION_NAME_UNMAPPED, file->sht[1].sh_offset); 
  file->sht[1].sh_name = elfsh_insert_in_shstrtab(file, buff);
  file->sht[2].sh_name = elfsh_insert_in_shstrtab(file, ELFSH_SECTION_NAME_SHSTRTAB);

  return (0);
}



/* First round : Read PHT and guess .bss, .interp, .dynamic and .notes bounds */
static void	sht_first_round(elfshobj_t *file, u_int num)
{
  /*
    Elf32_Phdr	*dyn;
    Elf32_Shdr	shdr;
  */
  u_int		index;
  int		flags;

  for (index = 0; index < num; index++)
    {
      flags = SHF_ALLOC;
      flags |= (elfsh_segment_is_writable(file->pht + index)    ? SHF_WRITE     : 0);
      flags |= (elfsh_segment_is_executable(file->pht + index)  ? SHF_EXECINSTR : 0);
      switch (file->pht[index].p_type)
	{

	  /* The dynamic segment */
	  /*
	    case PT_DYNAMIC:
	    dyn = file->pht + index;
	    shdr = elfsh_create_section_header(0, SHT_DYNAMIC, flags , file->pht[index].p_vaddr,
	    file->pht[index].p_offset, file->pht[index].p_filesz, 
	    0, 0, 0, 0);
	    add_sht_entry(file, index, shdr, ELFSH_SECTION_NAME_DYNAMIC);
	    break;
	  */

	  /* The interpretor segment */
	  /*
	    case PT_INTERP:
	    shdr = elfsh_create_section_header(0, SHT_PROGBITS, flags, file->pht[index].p_vaddr,
	    file->pht[index].p_offset, file->pht[index].p_filesz, 
	    0, 0, 0, 0);
	    add_sht_entry(file, index, shdr, ELFSH_SECTION_NAME_INTERP);
	    break;
	  */

	  /* This is the auxiliary information */
	  /*
	case PT_NOTE:
	  shdr = elfsh_create_section_header(0, SHT_NOTE, flags, file->pht[index].p_vaddr,
					     file->pht[index].p_offset, file->pht[index].p_filesz, 
					     0, 0, 0, 0);
	  add_sht_entry(file, index, shdr, ELFSH_SECTION_NAME_NOTES_ABI);
	  break;
	  */

	  /* Rebuild the .bss */
	  /*
	case PT_LOAD:
	  if (file->pht[index].p_memsz != file->pht[index].p_filesz)
	    {
	      shdr = elfsh_create_section_header(0, SHT_NOBITS, flags, 
						 file->pht[index].p_vaddr + file->pht[index].p_filesz, 
						 file->pht[index].p_offset + file->pht[index].p_filesz,
						 file->pht[index].p_memsz - file->pht[index].p_filesz,
						 0, 0, 0, 0);
	      add_sht_entry(file, index, shdr, ELFSH_SECTION_NAME_BSS);
	    }
	  break;
	  */

	default:
	  break;
	}
    }    
}


/* INTERNAL FUNCTION: guess .text, .data, .init, .fini, and .bss bounds */
static void	sht_second_round(elfshobj_t *file, u_int num)
{
  /* Elf32_Phdr	*dyn; */
  Elf32_Shdr	shdr;
  u_int		index;
  int		flags;
  char		*name;
  char		i;

  /* Do not remove if first_round() is entirely working with section paddings */
  return ;

  for (index = 0; index < num; index++)
    {
      flags = SHF_ALLOC;
      flags |= (elfsh_segment_is_writable(file->pht + index)    ? SHF_WRITE     : 0);
      flags |= (elfsh_segment_is_executable(file->pht + index)  ? SHF_EXECINSTR : 0);
      switch (file->pht[index].p_type)
	{
	case PT_LOAD:
	  shdr = elfsh_create_shdr(0, SHT_PROGBITS, flags, 
				   file->pht[index].p_vaddr, 
				   file->pht[index].p_offset, 
				   file->pht[index].p_filesz, 
				   0, 0, 0, 0);
	  i = INTERVAL(file->pht[index].p_vaddr, file->hdr->e_entry, 
		       file->pht[index].p_vaddr + file->pht[index].p_filesz);
	  name = (i ? ELFSH_SECTION_NAME_TEXT : ELFSH_SECTION_NAME_DATA);
	  add_sht_entry(file, index, shdr, name);
	default:
	  break;
	}
    }
}



/* Recreate the section header table examining the program header table */
int		elfsh_rebuild_sht(elfshobj_t *file)
{
  u_int		num;

  /* Some preliminary stuffs */
  if (!elfsh_get_pht(file, &num) || init_sht(file, num) < 0)
    return (-1);

  sht_first_round(file, num);
  sht_second_round(file, num);
  file->shtrb = 1;
  return (0);
}





