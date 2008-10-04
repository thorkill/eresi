/**
 * @file sym.c
 * 
 * Started on  Fri Nov  2 15:19:38 2001 jfv
 *
 *
 * $Id: sym.c,v 1.2 2008-02-16 12:32:27 thor Exp $
 *
 */
#include "libstderesi.h"


/** 
 * Print the chosen symbol table 
 * @param file
 * @param sect
 * @param tab
 * @param num
 * @param regx
 * @param get_symname
 * @return
 */
int		ds(elfshobj_t	*file,
		   elfshsect_t	*sect,
		   elfsh_Sym	*tab,
		   unsigned int        num,
		   regex_t	*regx,
		   char		*(*get_symname)(elfshobj_t *f, elfsh_Sym *s))
{
  elfsh_Sym	*table;
  char		*name;
  char		*type;
  char		*bind;
  unsigned int		typenum;
  unsigned int		bindnum;
  unsigned int		foff;
  int		index;
  char		*sect_name;
  char		buff[512];
  char		off[20];
  char		type_unk[ERESI_MEANING + 1];
  char		bind_unk[ERESI_MEANING + 1];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sort the table if necessary */
  if (world.state.sort != NULL)
    switch (*world.state.sort)
      {
      case ELFSH_SORT_BY_ADDR:
	table = sect->altdata;
	break;
      case ELFSH_SORT_BY_SIZE:
	table = sect->terdata;
	break;
      default:
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unknown sort mode", -1);
      }

  /* Avoid reading inexistant memory in the process for .symtab */
  else
    table = (elfsh_Sym *) (sect->shdr->sh_addr ? elfsh_get_raw(sect) : sect->data);

  /* Browse symtab */
  for (index = 0; index < num; index++)
    {

      /* Retreive names */
      typenum = elfsh_get_symbol_type(table + index);
      bindnum = elfsh_get_symbol_bind(table + index);
      type = (char *) (typenum > ELFSH_SYMTYPE_MAX ? 
		       revm_build_unknown(type_unk, "type", typenum) : 
		       elfsh_sym_type[typenum].desc);
      bind = (char *) (bindnum >= ELFSH_SYMBIND_MAX ?
		       revm_build_unknown(bind_unk, "type", bindnum) : 
		       elfsh_sym_bind[bindnum].desc);
      name = get_symname(world.curjob->curfile, table + index);
      sect_name = NULL;
      sect = elfsh_get_parent_section(world.curjob->curfile, 
				      table[index].st_value, 
				      NULL);
      if (sect == NULL && table[index].st_shndx)
	sect = elfsh_get_section_by_index(world.curjob->curfile, 
					  table[index].st_shndx,
					  NULL, NULL);
      if (sect != NULL)
	sect_name = elfsh_get_section_name(world.curjob->curfile, sect);

      /* Fixup names */
      if (name == NULL || *name == 0)
	name = ELFSH_NULL_STRING;
      if (type == NULL || *type == 0)
	type = ELFSH_NULL_STRING;
      if (bind == NULL || *bind == 0)
	bind = ELFSH_NULL_STRING;
      if (sect_name == NULL)
	sect_name = ELFSH_NULL_STRING;
      foff = (!table[index].st_value ? 0 : 
	      elfsh_get_foffset_from_vaddr(world.curjob->curfile, 
					   table[index].st_value));
					
      if (sect && sect->shdr->sh_addr != table[index].st_value)
	sprintf(off, " + %s", revm_colornumber("%u", 
					     (unsigned int) (table[index].st_value - sect->shdr->sh_addr)));
      else
	*off = '\0';

      /* Different output depending on the quiet flag */
      if (!world.state.revm_quiet)
	{
	  snprintf(buff, sizeof(buff), 
		   " %s %s %s %s %s%s "
		   "%s%s %s%s %s%s => %s%s\n",
		   revm_colornumber("[%03u]", index), 
		   revm_coloraddress(XFMT, (eresi_Addr) elfsh_get_symbol_value(table + index) + file->rhdr.base), 
		   revm_colortypestr_fmt("%-8s", type), 
		   revm_colorstr_fmt("%-40s", name),
		   revm_colorfieldstr("size:"),
		   revm_colornumber("%010u", elfsh_get_symbol_size(table + index)), 				  
		   revm_colorfieldstr("foffset:"),
		   revm_colornumber("%06u", foff),
		   revm_colorfieldstr("scope:"),
		   revm_colortypestr_fmt("%-6s", bind), 
		   revm_colorfieldstr("sctndx:"),
		   revm_colornumber("%02u", elfsh_get_symbol_link(table + index)),
		   revm_colorstr(sect_name), off);
	}

      else
	{
	  snprintf(buff, sizeof(buff), 
		   " %s %s %s %s %s%s %s%s %s%-6s\n",
		   revm_colornumber("[%03u]", index), 
		   revm_coloraddress(XFMT, (eresi_Addr) elfsh_get_symbol_value(table + index) + file->rhdr.base),
		   revm_colortypestr_fmt("%-8s", type), revm_colorstr_fmt("%-15s", name), 
		   revm_colorfieldstr("sz:"),
		   revm_colornumber("%06u", elfsh_get_symbol_size(table + index)),
		   revm_colorfieldstr("foff:"),
		   revm_colornumber("%06u", foff),
		   revm_colorfieldstr("scop:"),
		   revm_colortypestr_fmt("%-6s", bind));
	}
      
      if (regx == NULL || 
	  (regx != NULL && regexec(regx, buff, 0, 0, 0) == 0))
	{
	  /* If the user ask quit, we just break */
	  if (revm_output(buff) == -1)
	    break;
	}

      revm_endline();
	  
    }

  revm_endline();
  
  revm_output("\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}





/** 
 * Display the symbol table 
 */
int		cmd_sym()
{
  elfshsect_t	*sct;
  elfsh_Sym	*symtab;
  regex_t	*tmp;
  int		num;
  char		logbuf[BUFSIZ];
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  symtab = elfsh_get_symtab(world.curjob->curfile, &num);
  if (symtab == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Can't retrieve symtab", -1);

  sct = elfsh_get_section_by_type(world.curjob->curfile, 
				  SHT_SYMTAB, 0, NULL, NULL, 0);
  snprintf(logbuf, BUFSIZ, " [SYMBOL TABLE]\n [Object %s]\n\n",
	   world.curjob->curfile->name);
  revm_output(logbuf);
  FIRSTREGX(tmp);
  
  ret = ds(world.curjob->curfile, sct, symtab, 
	   num, tmp, elfsh_get_symbol_name);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}




/** 
 * Display the dynamic symbol table 
 */
int		cmd_dynsym()
{
  elfshsect_t	*sct;
  elfsh_Sym	*dynsym;
  regex_t	*tmp;
  int		num;
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  dynsym = elfsh_get_dynsymtab(world.curjob->curfile, &num);
  if (dynsym == NULL)
    RET(-1);
  else
    {      
      sct = elfsh_get_section_by_name(world.curjob->curfile, 
				      ELFSH_SECTION_NAME_ALTDYNSYM, 
				      NULL, NULL, &num);
      if (!sct)
	sct = elfsh_get_section_by_type(world.curjob->curfile, SHT_DYNSYM, 0, 
					NULL, NULL, &num);
      
      if (!sct)
	RET(-1);
      num = num / sizeof(elfsh_Sym);
    }

  snprintf(logbuf, BUFSIZ - 1,
	   " [DYNAMIC SYMBOL TABLE]\n [Object %s]\n [Section %s]\n", 
	   world.curjob->curfile->name, sct->name);
  
  revm_output(logbuf);
  FIRSTREGX(tmp);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		ds(world.curjob->curfile, sct, dynsym, 
		   num, tmp, elfsh_get_dynsymbol_name));
}












