/*
** sht.c for elfsh
** 
** Started on  Fri Nov  2 15:17:58 2001 mayhem

*/
#include "elfsh.h"


/* Display The Section header table (SHT) */
int		cmd_sht()
{
  elfshsect_t	*cur;
  elfsh_Shdr	*shdr;
  regex_t	*tmp;
  char		*name;
  char		*type;
  char		buff[256];
  u_int		num;
  u_int		index;
  u_int		typenum;
  char		type_unk[ELFSH_MEANING + 1];
  char		logbuf[BUFSIZ];

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Fetch SHT and choose regex */
  shdr = elfsh_get_sht(world.curjob->current, &num);
  if (shdr == NULL)
    QUIT_ERROR(-1);
  FIRSTREGX(tmp);
  
  snprintf(logbuf, BUFSIZ - 1, " [SECTION HEADER TABLE .::. SHT %s]\n"
	   " [Object %s]\n\n", 
	   (world.curjob->current->shtrb ? "has been rebuilt" : "is not stripped"),
	   world.curjob->current->name);
  vm_output(logbuf);
  
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
      cur = elfsh_get_section_by_index(world.curjob->current, index, NULL, NULL);
      name = elfsh_get_section_name(world.curjob->current, cur);
      if (name == NULL)
	name = ELFSH_NULL_STRING;
      
      /* Output is different depending on the quiet flag */
      if (!world.state.vm_quiet)
	snprintf(buff, sizeof(buff), 
		 " [%03u] " XFMT " %c%c%c%c%c%c%c %-30s "
		 "foffset:" UFMT " size:%08u link:%02u info:%04u "
		 "entsize:%04u align:%04u => %s\n",
		 index, 
		 (elfsh_is_debug_mode() ? 
		 world.curjob->current->base + elfsh_get_section_addr(shdr + index) :
		 elfsh_get_section_addr(shdr + index)),
		 (elfsh_get_section_allocflag(shdr + index)     ? 'a' : '-'),
		  (elfsh_get_section_writableflag(shdr + index)  ? 'w' : '-'),
		 (elfsh_get_section_execflag(shdr + index)      ? 'x' : '-'),
		 (elfsh_get_section_mergeableflag(shdr + index) ? 'm' : '-'),
		 (elfsh_get_section_strflag(shdr + index)       ? 's' : '-'),
		  (elfsh_get_section_linkflag(shdr + index)      ? 'l' : '-'),
		 (elfsh_get_section_orderflag(shdr + index)     ? 'o' : '-'),
		 (name != NULL ? name : ELFSH_NULL_STRING),
		 elfsh_get_section_foffset(shdr + index),
		 elfsh_get_section_size(shdr + index),
		 elfsh_get_section_link(shdr + index),
		 elfsh_get_section_info(shdr + index),
		 elfsh_get_section_entsize(shdr + index),
		 elfsh_get_section_align(shdr + index),
		 type);
       else
	 snprintf(buff, sizeof(buff), 
		  " [%03u] " XFMT " %c%c%c%c%c%c%c %-15s foff:" UFMT " sz:%08u link:%02u\n",
		  index, elfsh_get_section_addr(shdr + index),
		  (elfsh_get_section_allocflag(shdr + index)    ? 'a' : '-'),
		  (elfsh_get_section_writableflag(shdr + index) ? 'w' : '-'),
		  (elfsh_get_section_execflag(shdr + index)     ? 'x' : '-'),
		  (elfsh_get_section_mergeableflag(shdr + index) ? 'm' : '-'),
		  (elfsh_get_section_strflag(shdr + index)       ? 's' : '-'),
		  (elfsh_get_section_linkflag(shdr + index)      ? 'l' : '-'),
		  (elfsh_get_section_orderflag(shdr + index)     ? 'o' : '-'),
		  (name != NULL ? name : ELFSH_NULL_STRING),
		  elfsh_get_section_foffset(shdr + index),
		  elfsh_get_section_size(shdr + index),
		  elfsh_get_section_link(shdr + index));
       
	 if (!tmp || (tmp && !regexec(tmp, buff, 0, 0, 0)))
	   vm_output(buff);
	 
     }

   vm_output("\n");
   return (0);
}
