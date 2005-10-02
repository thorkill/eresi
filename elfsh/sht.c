/*
** sht.c for elfsh
** 
** Started on  Fri Nov  2 15:17:58 2001 mayhem

*/
#include "elfsh.h"


/* Display The Section header table (SHT) */
int		vm_print_sht(elfsh_Shdr *shdr, u_int num, char rtflag)
{
  elfshsect_t	*cur;
  regex_t	*tmp;
  char		*name;
  char		*type;
  char		buff[BUFSIZ];
  u_int		index;
  u_int		typenum;
  char		type_unk[ELFSH_MEANING + 1];
  elfsh_Addr	addr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Fetch SHT and choose regex */
  FIRSTREGX(tmp);
  
  /* Iterate as much as there is entries in SHT */
  for (index = 0; index < num; index++)
    {

      /* Get section type */
      typenum = elfsh_get_section_type(shdr + index);
      type    = (char *) (typenum >= ELFSH_SHTYPE_MAX ? 
			  vm_build_unknown(type_unk, "type", typenum) :
			  elfsh_sh_type[typenum].desc);
      if (!type)
	type = ELFSH_NULL_STRING;
      
      /* Get section name */
      if (!rtflag)
	cur = elfsh_get_section_by_index(world.curjob->current, index, NULL, NULL);
      else
	cur = elfsh_get_rsection_by_index(world.curjob->current, index, NULL, NULL);
      name = elfsh_get_section_name(world.curjob->current, cur);
      if (name == NULL)
	name = ELFSH_NULL_STRING;

      /* Compute section address */
      /* Base for ET_EXEC is always 0 */
      addr = (elfsh_is_debug_mode() && !elfsh_section_is_runtime(cur) ?
	      world.curjob->current->rhdr.base + elfsh_get_section_addr(shdr + index) :
	      elfsh_get_section_addr(shdr + index));

      /* Output is different depending on the quiet flag */

      if (!world.state.vm_quiet)
	snprintf(buff, sizeof(buff), 
		 " %s %s %c%c%c%c%c%c%c %s "
		 "%s%s %s%s %s%s %s%s "
		 "%s%s %s%s => %s\n",
		 vm_colornumber("[%03u]", index), vm_coloraddress(XFMT, addr),
		 (elfsh_get_section_allocflag(shdr + index)     ? 'a' : '-'),
		  (elfsh_get_section_writableflag(shdr + index)  ? 'w' : '-'),
		 (elfsh_get_section_execflag(shdr + index)      ? 'x' : '-'),
		 (elfsh_get_section_mergeableflag(shdr + index) ? 'm' : '-'),
		 (elfsh_get_section_strflag(shdr + index)       ? 's' : '-'),
		  (elfsh_get_section_linkflag(shdr + index)      ? 'l' : '-'),
		 (elfsh_get_section_orderflag(shdr + index)     ? 'o' : '-'),
		 (name != NULL ? vm_colorstr_fmt("%-30s", name) : ELFSH_NULL_STRING),
		 vm_colorfieldstr("foffset:"),
		 vm_colornumber(UFMT, elfsh_get_section_foffset(shdr + index)),
		 vm_colorfieldstr("size:"),
		 vm_colornumber("%08u", elfsh_get_section_size(shdr + index)),
		 vm_colorfieldstr("link:"),
		 vm_colornumber("%02u", elfsh_get_section_link(shdr + index)),
		 vm_colorfieldstr("info:"),
		 vm_colornumber("%04u", elfsh_get_section_info(shdr + index)),
		 vm_colorfieldstr("entsize:"),
		 vm_colornumber("%04u", elfsh_get_section_entsize(shdr + index)),
		 vm_colorfieldstr("align:"),
		 vm_colornumber("%04u", elfsh_get_section_align(shdr + index)),
		 vm_colortypestr(type));
       else
	 snprintf(buff, sizeof(buff), 
		  " %s %s %c%c%c%c%c%c%c %s %s%s %s%s %s%s\n",
		  vm_colornumber("[%03u]", index), vm_coloraddress(XFMT, addr),
		  (elfsh_get_section_allocflag(shdr + index)    ? 'a' : '-'),
		  (elfsh_get_section_writableflag(shdr + index) ? 'w' : '-'),
		  (elfsh_get_section_execflag(shdr + index)     ? 'x' : '-'),
		  (elfsh_get_section_mergeableflag(shdr + index) ? 'm' : '-'),
		  (elfsh_get_section_strflag(shdr + index)       ? 's' : '-'),
		  (elfsh_get_section_linkflag(shdr + index)      ? 'l' : '-'),
		  (elfsh_get_section_orderflag(shdr + index)     ? 'o' : '-'),
		  (name != NULL ? vm_colorstr_fmt("%-15s", name) : ELFSH_NULL_STRING),
		  vm_colorfieldstr("foff:"),
		  vm_colornumber(UFMT, elfsh_get_section_foffset(shdr + index)),
		  vm_colorfieldstr("sz:"),
		  vm_colornumber("%08u", elfsh_get_section_size(shdr + index)),
		  vm_colorfieldstr("link:"),
		  vm_colornumber("%02u", elfsh_get_section_link(shdr + index)));
      
      
	 if (!tmp || (tmp && !regexec(tmp, buff, 0, 0, 0)))
	   vm_output(buff);
	 
	 vm_endline();
    }
  
   vm_output("\n");

   ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Print section header table */
int		cmd_sht()
{
  elfsh_Shdr	*shdr;
  u_int		num;
  char		logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Fetch SHT and choose regex */
  shdr = elfsh_get_sht(world.curjob->current, &num);
  if (shdr == NULL)
    QUIT_ERROR(-1);
  snprintf(logbuf, BUFSIZ - 1, " [SECTION HEADER TABLE .::. SHT %s]\n"
	   " [Object %s]\n\n", 
	   (world.curjob->current->shtrb ? "has been rebuilt" : "is not stripped"),
	   world.curjob->current->name);
  vm_output(logbuf);
  vm_print_sht(shdr, num, 0);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Print runtime section header table */
int		cmd_rsht()
{
  elfsh_Shdr	*shdr;
  u_int		num;
  char		logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Fetch RSHT */
  shdr = elfsh_get_runtime_sht(world.curjob->current, &num);
  if (shdr == NULL)
    QUIT_ERROR(-1);
  
  snprintf(logbuf, BUFSIZ - 1, " [RUNTIME SECTION HEADER TABLE .::. RSHT resides in memory only]\n"
	   " [Object %s]\n\n", world.curjob->current->name);
  vm_output(logbuf);
  vm_print_sht(shdr, num, 1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
    
