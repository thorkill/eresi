/**
 * @file check.c
** @ingroup libetrace
** 
** @brief check functions, valid faddr or is tracable.
** 
** Started Jul 2 2005 00:03:44 mxatone
** 
**
** $Id: check.c,v 1.2 2007-11-29 10:25:02 rival Exp $
**
*/
#include "libelfsh.h"
#include "libasm.h"
#include "libetrace.h"

/**
 * Can we trace this symbol ? 
 * @param file target file
 * @param name function name
 * @param symtab symbol table (symtab or dynsym)
 * @param num number of symbol on the table
 * @param dynsym 0 = symtab 1 = dynsym (internal/external)
 * @param vaddr fill symbol virtual address
 * @see etrace_tracable
 */
static int		etrace_tracable_sym(elfshobj_t *file, char *name, elfsh_Sym *symtab,
						  int num, unsigned char dynsym, eresi_Addr *vaddr)
{
  unsigned int			index;
  elfshsect_t		*sect;
  char			*sect_name;
  char			*func_name;
  unsigned char		bind;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (index = 0; index < num; index++)
    {
      // Only function symbols
      if (elfsh_get_symbol_type(symtab + index) != STT_FUNC)
	continue;

      // Retrieve symbol section store into st_value
      sect = elfsh_get_parent_section(file, symtab[index].st_value, NULL);
      
      // Try to get the section using index 
      if (sect == NULL && symtab[index].st_shndx)
	sect = elfsh_get_section_by_index(file, symtab[index].st_shndx, NULL, NULL);

      if (sect == NULL)
	continue;

      sect_name = elfsh_get_section_name(file, sect);
      
      // Only global, plt & text 
      // Make sure we look at the beginning of the name, including the .
      bind = elfsh_get_symbol_bind(symtab + index);
      if ((bind != STB_GLOBAL && bind != STB_LOCAL)
	  || (strncmp(sect_name, ".plt", 4) && strncmp(sect_name, ".text", 5)))
	continue;
     
      // Switch between symbol table and dynamic symbol table
      if (!dynsym)
	func_name = elfsh_get_symbol_name(file, symtab + index);
      else
	func_name = elfsh_get_dynsymbol_name(file, symtab + index);

      /* Compare names */
      if (strcmp(name, func_name) != 0)
	continue;

      /* Unusable symbol */
      if (symtab[index].st_value == 0)
	continue;

      if (vaddr)
	*vaddr = symtab[index].st_value;

      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Function not found", -1);
}

/**
 * Check if a function address is valid or not
 * @param file target file
 * @param addr function address
 * @param vaddr returned virtual address
 */
int			etrace_valid_faddr(elfshobj_t *file, eresi_Addr addr,
					   eresi_Addr *vaddr, unsigned char *dynsym)
{
  int			retvalue;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!file || !addr || !vaddr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  /* Our addr must be called */
  retvalue = elfsh_addr_is_called(file, addr);

  if (retvalue >= 0)
    *vaddr = addr;


  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, retvalue);
}


/**
 * @brief Check if a function can be traced or not.
 * @param file Target file.
 * @param name Function name.
 * @param vaddr Pointer where to save the function virtual address.
 * @param external Pointer where to save function status (internal / external).
 * @return Tracable (0), Untracable (-2) or Error (-1).
 * @see etrace_tracable_sym
 */
int 			etrace_tracable(elfshobj_t *file, char *name,
					eresi_Addr *vaddr, unsigned char *external)
{
  elfsh_Sym		*symtab, *dynsym;
  int			symnum = 0, dynsymnum = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!file || !name || !vaddr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  /* Some function can't be traced */
  if (etrace_untracable(file, name) == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Untracable function", -2);    

  /* Retrieve symbol tables pointer / number */
  symtab = elfsh_get_symtab(file, &symnum);
  dynsym = elfsh_get_dynsymtab(file, &dynsymnum);

  if (symnum + dynsymnum <= 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "No symbols found", -1);

  /* sym */
  if (etrace_tracable_sym(file, name, symtab, symnum, 0, vaddr) == 0)
    {
      if (external)
	*external = 0;

      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* dynsym */
  if (etrace_tracable_sym(file, name, dynsym, dynsymnum, 1, vaddr) == 0)
    {
      if (external)
	*external = 1;

      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Not tracable", -1);
}
