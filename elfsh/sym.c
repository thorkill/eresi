/*
** sym.c for elfsh
** 
** Started on  Fri Nov  2 15:19:38 2001 mayhem
** Last update Mon Jun 23 04:46:40 2003 mayhem
*/
#include "elfsh.h"




/* Print the chosen symbol table */
int		ds(elfshsect_t	*sect,
		   Elf32_Sym	*tab,
		   u_int        num,
		   regex_t	*regx,
		   char		*(*get_symname)(elfshobj_t *f, Elf32_Sym *s))
{
  Elf32_Sym	*table;
  char		*name;
  char		*type;
  char		*bind;
  u_int		typenum;
  u_int		bindnum;
  u_int		foff;
  int		index;
  char		*sect_name;
  char		buff[256];
  char		off[20];
  char		type_unk[ELFSH_MEANING + 1];
  char		bind_unk[ELFSH_MEANING + 1];

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
	ELFSH_SETERROR("[elfsh:ds] Unknown sort mode\n", -1);
      }
  else
    table = sect->data;
  
  /* Browse symtab */
  for (index = 0; index < num; index++)
    {

      /* Retreive names */
      typenum = elfsh_get_symbol_type(table + index);
      bindnum = elfsh_get_symbol_bind(table + index);
      type = (char *) (typenum >= ELFSH_SYMTYPE_MAX ? 
		       vm_build_unknown(type_unk, "type", typenum) : 
		       elfsh_sym_type[typenum].desc);
      bind = (char *) (bindnum >= ELFSH_SYMBIND_MAX ?
		       vm_build_unknown(bind_unk, "type", bindnum) : 
		       elfsh_sym_bind[bindnum].desc);
      name = get_symname(world.current, table + index);
      sect_name = NULL;
      sect = elfsh_get_parent_section(world.current, 
				      table[index].st_value, 
				      NULL);
      if (sect == NULL && table[index].st_shndx)
	sect = elfsh_get_section_by_index(world.current, table[index].st_shndx,
					  NULL, NULL);
      if (sect != NULL)
	sect_name = elfsh_get_section_name(world.current, sect);

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
	      elfsh_get_foffset_from_vaddr(world.current, table[index].st_value));
					
      if (sect && sect->shdr->sh_addr != table[index].st_value)
	sprintf(off, " + %u", (u_int) (table[index].st_value - sect->shdr->sh_addr));
      else
	*off = '\0';

      /* Different output depending on the quiet flag */
      if (!world.state.vm_quiet)
	{
	  snprintf(buff, sizeof(buff), 
		   " [%03u] %-10p %-8s %-40s size:%010u "
		   "foffset:%06u scope:%-6s sctndx:%02u => %s%s",
		   index, (void *) elfsh_get_symbol_value(table + index),
		   type, name, elfsh_get_symbol_size(table + index), 
		   foff, bind, 
		   elfsh_get_symbol_link(table + index),
		   sect_name, off);
	}

      else
	{
	  snprintf(buff, sizeof(buff), 
		   " [%03u] %-10p %-8s %-15s sz:%06u foff:%06u scop:%-6s",
		   index, (void *) elfsh_get_symbol_value(table + index),
		   type, name, elfsh_get_symbol_size(table + index), 
		   foff, bind);
	}
      
      if (regx == NULL || 
	  (regx != NULL && regexec(regx, buff, 0, 0, 0) == NULL))
	puts(buff);
	  
    }
  
  puts("");
  return (0);
}





/* Display the symbol table */
int		cmd_sym()
{
  elfshsect_t	*sct;
  Elf32_Sym	*symtab;
  regex_t	*tmp;
  u_int		num;

  symtab = elfsh_get_symtab(world.current, &num);
  if (symtab == NULL)
    RET(-1);
  sct = elfsh_get_section_by_type(world.current, SHT_SYMTAB, NULL, NULL, NULL, 0);
  printf(" [SYMBOL TABLE]\n [Object %s]\n\n", world.current->name);
  CHOOSE_REGX(tmp);
  return (ds(sct, symtab, num, tmp, elfsh_get_symbol_name));
}




/* Display the dynamic symbol table */
int		cmd_dynsym()
{
  elfshsect_t	*sct;
  Elf32_Sym	*dynsym;
  regex_t	*tmp;
  u_int		num;

  dynsym = elfsh_get_dynsymtab(world.current, &num);
  if (dynsym == NULL)
    RET(-1);
  sct = elfsh_get_section_by_type(world.current, SHT_DYNSYM, NULL, NULL, NULL, 0);
  printf(" [DYNAMIC SYMBOL TABLE]\n [Object %s]\n\n", world.current->name);
  CHOOSE_REGX(tmp);
  return (ds(sct, dynsym, num, tmp, elfsh_get_dynsymbol_name));
}












