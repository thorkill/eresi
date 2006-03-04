/*
** pht.c for elfsh
** 
** Started on  Fri Nov  2 15:18:35 2001 mayhem
**
*/
#include "elfsh.h"



/* Display description for extended PHT types */
char		*vm_display_pdesc(u_int type)
{
  u_int		idx;
  char		type_unk[ELFSH_MEANING + 1];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  for (idx = 0; idx < ELFSH_EXTSEG_MAX; idx++)
    if (elfsh_extseg_type[idx].val == type)
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			 ((char *) elfsh_extseg_type[idx].desc));
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     vm_build_unknown(type_unk, "type", type));
}


/* Display name for extended PHT types */
char		*vm_display_pname(u_int type)
{
  u_int		idx;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  for (idx = 0; idx < ELFSH_EXTSEG_MAX; idx++)
    if (elfsh_extseg_type[idx].val == type)
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			 ((char *) elfsh_extseg_type[idx].name));

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__,
		     "Unknown");
}


/* Display a PHT */
void	        vm_print_pht(elfsh_Phdr *phdr, uint16_t num, elfsh_Addr base)
{
  elfsh_Shdr	*shdr;
  int		shtnum;
  int		index;
  int		index2;
  char		*type;
  u_int		typenum;
  elfshsect_t	*list;
  regex_t	*tmp;
  char		buff[512];
  char		logbuf[BUFSIZ];
  
  elfsh_Addr	addr;
  elfsh_Addr	addr_end;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  FIRSTREGX(tmp);
  
  /* Primary view (2 modes, depending on the quiet flag) */
  for (index = 0; index < num; index++)
    {
      
      typenum = elfsh_get_segment_type(phdr + index);
      type    = (char *) (typenum >= ELFSH_SEGTYPE_MAX ? 
			  vm_display_pdesc(typenum) : 
			  elfsh_seg_type[typenum].desc);

      addr = phdr[index].p_vaddr;
      addr_end = phdr[index].p_vaddr + phdr[index].p_memsz;
      if (elfsh_is_debug_mode())
	{
	  addr_end += base;
	  addr += base;
	}

      if (!world.state.vm_quiet)
	snprintf(buff, sizeof(buff), 
		 " %s %s -> %s %c%c%c %s%s%s "
		 "%s%s%s %s%s%s %s%s%s => %s\n",
		 vm_colornumber("[%02u]", index), 
		 vm_coloraddress(XFMT, addr), 
		 vm_coloraddress(XFMT, addr_end),
		 (elfsh_segment_is_readable(&phdr[index])   ? 'r' : '-'),
		 (elfsh_segment_is_writable(&phdr[index])   ? 'w' : '-'),
		 (elfsh_segment_is_executable(&phdr[index]) ? 'x' : '-'),
		 vm_colorfieldstr("memsz("),
		 vm_colornumber(UFMT, phdr[index].p_memsz),
		 vm_colorfieldstr(")"),
		 vm_colorfieldstr("foffset("),
		 vm_colornumber(UFMT, phdr[index].p_offset),
		 vm_colorfieldstr(")"),
		 vm_colorfieldstr("filesz("),
		 vm_colornumber(UFMT, phdr[index].p_filesz),
		 vm_colorfieldstr(")"),
		 vm_colorfieldstr("align("),
		 vm_colornumber(UFMT, phdr[index].p_align),
		 vm_colorfieldstr(")"),
		 vm_colortypestr(type));

      else
	snprintf(buff, sizeof(buff), 
		 " %s %s -> %s %c%c%c %s%s%s "
		 "%s%s%s %s%s%s\n",
		 vm_colornumber("[%02u]", index), 
		 vm_coloraddress(XFMT, addr), 
		 vm_coloraddress(XFMT, addr_end),
		 (elfsh_segment_is_readable(&phdr[index])   ? 'r' : '-'),
		 (elfsh_segment_is_writable(&phdr[index])   ? 'w' : '-'),
		 (elfsh_segment_is_executable(&phdr[index]) ? 'x' : '-'),
		 vm_colorfieldstr("memsz("),
		 vm_colornumber(UFMT, phdr[index].p_memsz),
		 vm_colorfieldstr(")"),
		 vm_colorfieldstr("foffset("),
		 vm_colornumber(UFMT, phdr[index].p_offset),
		 vm_colorfieldstr(")"),
		 vm_colorfieldstr("filesz("),
		 vm_colornumber(UFMT, phdr[index].p_filesz),
		 vm_colorfieldstr(")"));

      if (!tmp || (tmp && !regexec(tmp, buff, 0, 0, 0)))
	vm_output(buff);

      vm_endline();

    }

  snprintf(logbuf, BUFSIZ - 1, 
	   "\n [SHT correlation]"
	   "\n [Object %s]\n\n", world.curjob->current->name);
  vm_output(logbuf);

  /* Retreive the sht */
  if ((shdr = elfsh_get_sht(world.curjob->current, &shtnum)) == 0)
    ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);

  snprintf(logbuf, BUFSIZ - 1, " [*] SHT %s \n", 
	   (world.curjob->current->shtrb ? 
	    "has been rebuilt \n" :
	    "is not stripped \n"));
  vm_output(logbuf);
   
  /* Alternate View */
  for (index = 0; index < num; index++, index2 = 0)
    {
      typenum = elfsh_get_segment_type(phdr + index);
      type    = (char *) (typenum >= ELFSH_SEGTYPE_MAX ? 
			  vm_display_pname(typenum)    : 
			  elfsh_seg_type[typenum].name);

      snprintf(logbuf, BUFSIZ - 1, " %s %s \t", 
	       vm_colornumber("[%02u]", index), 
	       vm_colortypestr_fmt("%-10s", type));
      vm_output(logbuf);

      vm_endline();
      
      /* In SHT */
      for (index2 = 0, list = world.curjob->current->sectlist; 
	   list; list = list->next)
	if (elfsh_segment_is_parent(list, phdr + index))
	  {
	    index2++;
	    snprintf(logbuf, BUFSIZ - 1, "%s%s ", 
		     (list->shdr->sh_offset + list->shdr->sh_size > 
		      phdr[index].p_offset + phdr[index].p_filesz ? "|" : ""),
		     vm_colorstr(elfsh_get_section_name(world.curjob->current, list)));
	    vm_output(logbuf);

	    vm_endline();
	  }

      /* In RSHT */
      for (index2 = 0, list = world.curjob->current->rsectlist; 
	   list; list = list->next)
	if (elfsh_segment_is_parent(list, phdr + index))
	  {
	    index2++;
	    snprintf(logbuf, BUFSIZ - 1, "%s%s ", 
		     (list->shdr->sh_addr + list->shdr->sh_size > 
		      phdr[index].p_vaddr + phdr[index].p_memsz ? "|" : ""),
		     vm_colorstr(elfsh_get_section_name(world.curjob->current, list)));
	    vm_output(logbuf);

	    vm_endline();
	  }

      vm_output("\n");
    }
  
  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}



/* Display the program header table (PHT) */
int		cmd_pht()
{
  elfsh_Phdr	*phdr;
  char		logbuf[BUFSIZ];
  int		num;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Fetch and Print PHT */
  if ((phdr = elfsh_get_pht(world.curjob->current, &num)) == 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get PHT", -1);
  
  snprintf(logbuf, BUFSIZ - 1, 
	   " [Program Header Table .::. PHT]\n [Object %s]\n\n", 
	   world.curjob->current->name);
  vm_output(logbuf);
  vm_print_pht(phdr, num, world.curjob->current->rhdr.base);

  /* End */
  vm_output("\n");
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Print the runtime PHT */
int             cmd_rpht()
{
  elfsh_Phdr    *phdr;
  char          logbuf[BUFSIZ];
  int           num;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Fetch and Print Runtime (alternative) PHT */
  if ((phdr = elfsh_get_rpht(world.curjob->current, &num)) == 0)
    RET(-1);
  snprintf(logbuf, BUFSIZ, 
	   "\n [Runtime Program Header Table .::. RPHT]\n [Object %s]\n\n", 
	   world.curjob->current->name);
  vm_output(logbuf);
  vm_print_pht(phdr, num, world.curjob->current->rhdr.base);
  
  /* End */
  vm_output("\n");
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

