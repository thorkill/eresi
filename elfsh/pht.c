/*
** pht.c for elfsh
** 
** Started on  Fri Nov  2 15:18:35 2001 mayhem
** Last update Mon Mar 17 04:44:58 2003 mayhem
*/
#include "elfsh.h"




/* Display the program header table (pht) */
int		cmd_pht()
{
  Elf32_Phdr	*phdr;
  Elf32_Shdr	*shdr;
  int		num;
  int		shtnum;
  int		index;
  int		index2;
  char		*type;
  u_int		typenum;
  elfshsect_t	*list;
  regex_t	*tmp;
  char		buff[256];
  char		type_unk[ELFSH_MEANING + 1];

  /* Fetch PHT */
  if ((phdr = elfsh_get_pht(world.current, &num)) == 0)
    RET(-1);
  CHOOSE_REGX(tmp);
  printf(" [Program header table .::. PHT]\n [Object %s]\n\n", 
	 world.current->name);

  /* Primary view (2 modes, depending on the quiet flag) */
  for (index = 0; index < num; index++)
    {
      
      typenum = elfsh_get_segment_type(phdr + index);
      type    = (char *) (typenum >= ELFSH_SEGTYPE_MAX ? 
			  vm_build_unknown(type_unk, "type", typenum) : 
			  elfsh_seg_type[typenum].desc);

      if (!world.state.vm_quiet)
	snprintf(buff, sizeof(buff), 
		 " [%02u] 0x%08X -> 0x%08X %c%c%c memsz(%010u) "
		 "foffset(%010u) filesz(%010u) align(%04u) => %s",
		 index, phdr[index].p_vaddr,
		 phdr[index].p_vaddr + phdr[index].p_memsz,
		 (elfsh_segment_is_readable(&phdr[index])   ? 'r' : '-'),
		 (elfsh_segment_is_writable(&phdr[index])   ? 'w' : '-'),
		 (elfsh_segment_is_executable(&phdr[index]) ? 'x' : '-'),
		 phdr[index].p_memsz,
		 phdr[index].p_offset,
		 phdr[index].p_filesz,
		 phdr[index].p_align,
		 type);
      else
	snprintf(buff, sizeof(buff), 
		 " [%02u] 0x%08X -> 0x%08X %c%c%c memsz(%010u) "
		 "foff(%010u) filesz(%010u)",
		 index, phdr[index].p_vaddr,
		 phdr[index].p_vaddr + phdr[index].p_memsz,
		 (elfsh_segment_is_readable(&phdr[index])   ? 'r' : '-'),
		 (elfsh_segment_is_writable(&phdr[index])   ? 'w' : '-'),
		 (elfsh_segment_is_executable(&phdr[index]) ? 'x' : '-'),
		 phdr[index].p_memsz,
		 phdr[index].p_offset,
		 phdr[index].p_filesz);

      if (!tmp || (tmp && !regexec(tmp, buff, 0, 0, 0)))
	puts(buff);

    }

  printf("\n [Program header table .::. SHT correlation]"
	 "\n [Object %s]\n\n", world.current->name);

  /* Retreive the sht */
  if ((shdr = elfsh_get_sht(world.current, &shtnum)) == 0)
    RET(-1);

   printf(" [*] SHT %s \n", 
	  (world.current->shtrb ? 
	   "has been rebuilt \n" :
	   "is not stripped \n"));
   
  /* Alternate View */
  for (index = 0; index < num; index++, index2 = 0)
    {
      typenum = elfsh_get_segment_type(phdr + index);
      type    = (char *) (typenum >= ELFSH_SEGTYPE_MAX ? "Unknown" : 
			  elfsh_seg_type[typenum].name);
      printf(" [%02u] %-10s \t", index, type);
      fflush(stdout);
      
      for (index2 = 0, list = world.current->sectlist; list; list = list->next)
	if (elfsh_segment_is_parent(list, phdr + index))
	  {
	    index2++;
	    printf("%s%s ", 
		   (list->shdr->sh_offset + list->shdr->sh_size > 
		    phdr[index].p_offset + phdr[index].p_filesz ? "|" : ""),
		   elfsh_get_section_name(world.current, list));
	    fflush(stdout);
	  }
      puts("");
    }


  puts("");
  return (0);
}
