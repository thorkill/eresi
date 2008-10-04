/**
 * @file sht.c
 * 
 * Started on  Fri Nov  2 15:17:58 2001 jfv
 * 
 *
 * $Id: sht.c,v 1.2 2008-02-16 12:32:27 thor Exp $
 *
 */
#include "libstderesi.h"

/** 
 * Find the right sht type name 
 * @param typenum
 */
char		*revm_fetch_sht_typedesc(elfsh_Word typenum)
{
  char		*ret = NULL;
  unsigned int		index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (index = 0; index < ELFSH_SHTYPE_MAX; index++)
    {
      if (elfsh_sh_type[index].val == (eresi_Addr) typenum)
	{
	  ret = (char *) elfsh_sh_type[index].desc;
	  break;
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/** 
 * Display The Section header table (SHT) 
 * @param shdr
 * @param num
 * @param rtflag
 * @return
 */
int		revm_sht_print(elfsh_Shdr *shdr, unsigned int num, char rtflag)
{
  elfshsect_t	*cur;
  regex_t	*tmp;
  char		*name;
  char		*type;
  char		buff[BUFSIZ];
  unsigned int		index;
  elfsh_Word   	typenum;
  char		type_unk[ERESI_MEANING + 1];
  eresi_Addr	addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Fetch SHT and choose regex */
  FIRSTREGX(tmp);
  
  /* Iterate as much as there is entries in SHT */
  for (index = 0; index < num; index++)
    {

      /* Get section type */
      typenum = elfsh_get_section_type(shdr + index);
      type = revm_fetch_sht_typedesc(typenum);

      if (!type)
	type = revm_build_unknown(type_unk, "type", (unsigned long) typenum);

      if (!type)
	type = ELFSH_NULL_STRING;
      
      /* Get section name */
      if (!rtflag)
	cur = elfsh_get_section_by_index(world.curjob->curfile, index, NULL, NULL);
      else
	cur = elfsh_get_rsection_by_index(world.curjob->curfile, index, NULL, NULL);
      name = elfsh_get_section_name(world.curjob->curfile, cur);
      if (name == NULL)
	name = ELFSH_NULL_STRING;

      /* Compute section address */
      /* Base for ET_EXEC is always 0 */
      addr = (elfsh_is_debug_mode() && !elfsh_section_is_runtime(cur) ?
	      world.curjob->curfile->rhdr.base + elfsh_get_section_addr(shdr + index) :
	      elfsh_get_section_addr(shdr + index));

      /* Output is different depending on the quiet flag */
      if (!world.state.revm_quiet)
	snprintf(buff, sizeof(buff), 
		 " %s %s %c%c%c%c%c%c%c %s "
		 "%s%s %s%s %s%s %s%s "
		 "%s%s %s%s => %s\n",
		 revm_colornumber("[%03u]", index), revm_coloraddress(XFMT, addr),
		 (elfsh_get_section_allocflag(shdr + index)     ? 'a' : '-'),
		 (elfsh_get_section_writableflag(shdr + index)  ? 'w' : '-'),
		 (elfsh_get_section_execflag(shdr + index)      ? 'x' : '-'),
		 (elfsh_get_section_mergeableflag(shdr + index) ? 'm' : '-'),
		 (elfsh_get_section_strflag(shdr + index)       ? 's' : '-'),
		  (elfsh_get_section_linkflag(shdr + index)      ? 'l' : '-'),
		 (elfsh_get_section_orderflag(shdr + index)     ? 'o' : '-'),
		 (name != NULL ? revm_colorstr_fmt("%-30s", name) : ELFSH_NULL_STRING),
		 revm_colorfieldstr("foffset:"),
		 revm_colornumber(UFMT, elfsh_get_section_foffset(shdr + index)),
		 revm_colorfieldstr("size:"),
		 revm_colornumber("%08u", elfsh_get_section_size(shdr + index)),
		 revm_colorfieldstr("link:"),
		 revm_colornumber("%02u", elfsh_get_section_link(shdr + index)),
		 revm_colorfieldstr("info:"),
		 revm_colornumber("%04u", elfsh_get_section_info(shdr + index)),
		 revm_colorfieldstr("entsize:"),
		 revm_colornumber("%04u", elfsh_get_section_entsize(shdr + index)),
		 revm_colorfieldstr("align:"),
		 revm_colornumber("%04u", elfsh_get_section_align(shdr + index)),
		 revm_colortypestr(type));
       else
	 snprintf(buff, sizeof(buff), 
		  " %s %s %c%c%c%c%c%c%c %s %s%s %s%s %s%s\n",
		  revm_colornumber("[%03u]", index), revm_coloraddress(XFMT, addr),
		  (elfsh_get_section_allocflag(shdr + index)    ? 'a' : '-'),
		  (elfsh_get_section_writableflag(shdr + index) ? 'w' : '-'),
		  (elfsh_get_section_execflag(shdr + index)     ? 'x' : '-'),
		  (elfsh_get_section_mergeableflag(shdr + index) ? 'm' : '-'),
		  (elfsh_get_section_strflag(shdr + index)       ? 's' : '-'),
		  (elfsh_get_section_linkflag(shdr + index)      ? 'l' : '-'),
		  (elfsh_get_section_orderflag(shdr + index)     ? 'o' : '-'),
		  (name != NULL ? revm_colorstr_fmt("%-15s", name) : ELFSH_NULL_STRING),
		  revm_colorfieldstr("foff:"),
		  revm_colornumber(UFMT, elfsh_get_section_foffset(shdr + index)),
		  revm_colorfieldstr("sz:"),
		  revm_colornumber("%08u", elfsh_get_section_size(shdr + index)),
		  revm_colorfieldstr("link:"),
		  revm_colornumber("%02u", elfsh_get_section_link(shdr + index)));
     
      if (!tmp || (tmp && !regexec(tmp, buff, 0, 0, 0)))
	revm_output(buff);
      revm_endline();
    }
  
   revm_output("\n");

   PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/** 
 * Print section header table 
 */
int		cmd_sht()
{
  elfsh_Shdr	*shdr;
  int		num;
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Fetch SHT and choose regex */
  shdr = elfsh_get_sht(world.curjob->curfile, &num);
  if (shdr == NULL)
    QUIT_ERROR(-1);
  snprintf(logbuf, BUFSIZ - 1, " [SECTION HEADER TABLE .::. SHT %s]\n"
	   " [Object %s]\n\n", 
	   (world.curjob->curfile->shtrb ? "has been rebuilt" : "is not stripped"),
	   world.curjob->curfile->name);
  revm_output(logbuf);
  revm_sht_print(shdr, num, 0);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * Print runtime section header table 
 */
int		cmd_rsht()
{
  elfsh_Shdr	*shdr;
  int		num;
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Fetch RSHT */
  shdr = elfsh_get_runtime_sht(world.curjob->curfile, &num);
  if (shdr == NULL)
    QUIT_ERROR(-1);
  
  snprintf(logbuf, BUFSIZ - 1, 
	   " [RUNTIME SECTION HEADER TABLE .::. RSHT resides in memory only]\n"
	   " [Object %s]\n\n", world.curjob->curfile->name);
  revm_output(logbuf);
  revm_sht_print(shdr, num, 1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
    
