/**
 * @file pht.c
 * 
 * Started on  Fri Nov  2 15:18:35 2001 jfv
 *
 *
 * $Id: pht.c,v 1.2 2008-02-16 12:32:27 thor Exp $
 *
 */
#include "libstderesi.h"



/** 
 * @brief Display description for extended PHT types 
 * @param type
 */
char		*revm_display_pdesc(unsigned int type)
{
  unsigned int		idx;
  char		type_unk[ERESI_MEANING + 1];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (idx = 0; idx < ELFSH_EXTSEG_MAX; idx++)
    if (elfsh_extseg_type[idx].val == type)
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			 ((char *) elfsh_extseg_type[idx].desc));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     revm_build_unknown(type_unk, "type", type));
}


/** 
 * @brief Display name for extended PHT types 
 * @param type
 */
char		*revm_display_pname(unsigned int type)
{
  unsigned int		idx;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (idx = 0; idx < ELFSH_EXTSEG_MAX; idx++)
    if (elfsh_extseg_type[idx].val == type)
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			 ((char *) elfsh_extseg_type[idx].name));

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		     "Unknown");
}


/** 
 * @brief Display a PHT 
 * @param phdr
 * @param num
 * @param base
 */
void	        revm_pht_print(elfsh_Phdr *phdr, uint16_t num, eresi_Addr base)
{
  elfsh_Shdr	*shdr;
  int		shtnum;
  int		index;
  int		index2;
  char		*type;
  unsigned int		typenum;
  elfshsect_t	*list;
  regex_t	*tmp;
  char		buff[512];
  char		warnmsg[256];
  char		logbuf[BUFSIZ];
  int		check;
  
  eresi_Addr	addr;
  eresi_Addr	addr_end;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  FIRSTREGX(tmp);
  
  /* Primary view (2 modes, depending on the quiet flag) */
  for (index = 0; index < num; index++)
    {
      
      typenum = elfsh_get_segment_type(phdr + index);
      type    = (char *) (typenum >= ELFSH_SEGTYPE_MAX ? 
			  revm_display_pdesc(typenum) : 
			  elfsh_seg_type[typenum].desc);

      addr = phdr[index].p_vaddr;
      addr_end = phdr[index].p_vaddr + phdr[index].p_memsz;
      if (elfsh_is_debug_mode())
	{
	  addr_end += base;
	  addr += base;
	}

      /* We check if we have a correct alignment */
      check = (addr - phdr[index].p_offset) & (phdr[index].p_align - 1);

      if (check != 0)
	snprintf(warnmsg, 255, "Wrong alignment (%d)", check);

      if (!world.state.revm_quiet)
	snprintf(buff, sizeof(buff), 
		 " %s %s -> %s %c%c%c %s%s%s "
		 "%s%s%s %s%s%s %s%s%s => %s %s\n",
		 revm_colornumber("[%02u]", index), 
		 revm_coloraddress(XFMT, addr), 
		 revm_coloraddress(XFMT, addr_end),
		 (elfsh_segment_is_readable(&phdr[index])   ? 'r' : '-'),
		 (elfsh_segment_is_writable(&phdr[index])   ? 'w' : '-'),
		 (elfsh_segment_is_executable(&phdr[index]) ? 'x' : '-'),
		 revm_colorfieldstr("memsz("),
		 revm_colornumber(UFMT, phdr[index].p_memsz),
		 revm_colorfieldstr(")"),
		 revm_colorfieldstr("foffset("),
		 revm_colornumber(UFMT, phdr[index].p_offset),
		 revm_colorfieldstr(")"),
		 revm_colorfieldstr("filesz("),
		 revm_colornumber(UFMT, phdr[index].p_filesz),
		 revm_colorfieldstr(")"),
		 revm_colorfieldstr("align("),
		 revm_colornumber(UFMT, phdr[index].p_align),
		 revm_colorfieldstr(")"),
		 revm_colortypestr(type),
		 check != 0 ? revm_colorwarn(warnmsg) : ""
		 );

      else
	snprintf(buff, sizeof(buff), 
		 " %s %s -> %s %c%c%c %s%s%s "
		 "%s%s%s %s%s%s\n",
		 revm_colornumber("[%02u]", index), 
		 revm_coloraddress(XFMT, addr), 
		 revm_coloraddress(XFMT, addr_end),
		 (elfsh_segment_is_readable(&phdr[index])   ? 'r' : '-'),
		 (elfsh_segment_is_writable(&phdr[index])   ? 'w' : '-'),
		 (elfsh_segment_is_executable(&phdr[index]) ? 'x' : '-'),
		 revm_colorfieldstr("memsz("),
		 revm_colornumber(UFMT, phdr[index].p_memsz),
		 revm_colorfieldstr(")"),
		 revm_colorfieldstr("foffset("),
		 revm_colornumber(UFMT, phdr[index].p_offset),
		 revm_colorfieldstr(")"),
		 revm_colorfieldstr("filesz("),
		 revm_colornumber(UFMT, phdr[index].p_filesz),
		 revm_colorfieldstr(")"));

      if (!tmp || (tmp && !regexec(tmp, buff, 0, 0, 0)))
	revm_output(buff);

      revm_endline();

    }

  snprintf(logbuf, BUFSIZ - 1, 
	   "\n [SHT correlation]"
	   "\n [Object %s]\n\n", world.curjob->curfile->name);
  revm_output(logbuf);

  /* Retreive the sht */
  if ((shdr = elfsh_get_sht(world.curjob->curfile, &shtnum)) == 0)
    PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);

  snprintf(logbuf, BUFSIZ - 1, " [*] SHT %s \n", 
	   (world.curjob->curfile->shtrb ? 
	    "has been rebuilt \n" :
	    "is not stripped \n"));
  revm_output(logbuf);
   
  /* Alternate View */
  for (index = 0; index < num; index++, index2 = 0)
    {
      typenum = elfsh_get_segment_type(phdr + index);
      type    = (char *) (typenum >= ELFSH_SEGTYPE_MAX ? 
			  revm_display_pname(typenum)    : 
			  elfsh_seg_type[typenum].name);

      snprintf(logbuf, BUFSIZ - 1, " %s %s \t", 
	       revm_colornumber("[%02u]", index), 
	       revm_colortypestr_fmt("%-10s", type));
      revm_output(logbuf);

      revm_endline();
      
      /* In SHT */
      for (index2 = 0, list = world.curjob->curfile->sectlist; 
	   list; list = list->next)
	if (elfsh_segment_is_parent(list, phdr + index))
	  {
	    index2++;
	    snprintf(logbuf, BUFSIZ - 1, "%s%s ", 
		     (list->shdr->sh_offset + list->shdr->sh_size > 
		      phdr[index].p_offset + phdr[index].p_filesz ? "|" : ""),
		     revm_colorstr(elfsh_get_section_name(world.curjob->curfile, list)));
	    revm_output(logbuf);

	    revm_endline();
	  }

      /* In RSHT */
      for (index2 = 0, list = world.curjob->curfile->rsectlist; 
	   list; list = list->next)
	if (elfsh_segment_is_parent(list, phdr + index))
	  {
	    index2++;
	    snprintf(logbuf, BUFSIZ - 1, "%s%s ", 
		     (list->shdr->sh_addr + list->shdr->sh_size > 
		      phdr[index].p_vaddr + phdr[index].p_memsz ? "|" : ""),
		     revm_colorstr(elfsh_get_section_name(world.curjob->curfile, list)));
	    revm_output(logbuf);

	    revm_endline();
	  }

      revm_output("\n");
    }
  
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}



/** 
 * @brief Display the program header table (PHT) 
 */
int		cmd_pht()
{
  elfsh_Phdr	*phdr;
  char		logbuf[BUFSIZ];
  int		num;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Fetch and Print PHT */
  if ((phdr = elfsh_get_pht(world.curjob->curfile, &num)) == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get PHT", -1);
  
  snprintf(logbuf, BUFSIZ - 1, 
	   " [Program Header Table .::. PHT]\n [Object %s]\n\n", 
	   world.curjob->curfile->name);
  revm_output(logbuf);
  revm_pht_print(phdr, num, world.curjob->curfile->rhdr.base);

  /* End */
  revm_output("\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/** 
 * @brief Print the runtime PHT 
 */
int             cmd_rpht()
{
  elfsh_Phdr    *phdr;
  char          logbuf[BUFSIZ];
  int           num;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Fetch and Print Runtime (alternative) PHT */
  if ((phdr = elfsh_get_rpht(world.curjob->curfile, &num)) == 0)
    RET(-1);
  snprintf(logbuf, BUFSIZ, 
	   "\n [Runtime Program Header Table .::. RPHT]\n [Object %s]\n\n", 
	   world.curjob->curfile->name);
  revm_output(logbuf);
  revm_pht_print(phdr, num, world.curjob->curfile->rhdr.base);
  
  /* End */
  revm_output("\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

