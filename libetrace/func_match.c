/*
** @file func_match.c
**
** Libetrace implementation in the ERESI project.
**
** Started on  Sun Jun 22 12:10:12 2007 jfv
** $Id: libetrace.h,v 1.2 2007-11-29 10:25:02 rival Exp $
**
*/
#include "libetrace.h"

/**
 * @brief Match a function in a symbol table.
 * @param sect Section pointer for symbol table.
 * @param num Element number.
 * @param preg Reguler expression to use.
 * @param func_list Function table.
 * @param count Counter pointer (from the table).
 * @param get_symname Function ptr to get symbol name.
 * @return Success (0) or Error (-1).
 */
static int		trace_match_symtab(elfshsect_t	*sect, 
					   int		num, 
					   regex_t	*preg,
					   char		***func_list, 
					   unsigned int	*count,
					   char		*(*get_symname)(elfshobj_t *f, 
									elfsh_Sym *s))
{
  unsigned int			index;
  elfsh_Sym		*table;
  char			*name;
  char			**f_list;
  unsigned int			cnum;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Check argument before anything */
  if (!sect || !preg || !func_list || !count || !get_symname)
     PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  f_list = *func_list;
  cnum = *count;
  table = (elfsh_Sym *) (sect->shdr->sh_addr ? elfsh_get_raw(sect) : sect->data);

  /* Parse every function */
  for (index = 0; index < num; index++)
    {
      /* Only functions */
      if (elfsh_get_symbol_type(table + index) != STT_FUNC
	  || table[index].st_value == 0)
	continue;

      name = get_symname(sect->parent, table + index);

      /* Check if this name is valid */
      if (name == NULL || *name == 0)
	continue;

      /* We match a function */
      if (regexec(preg, name, 0, 0, 0) == 0)
	{
	  /* Do we need to realloc ? */
	  if (((cnum+1) % TRACE_MATCH_ALLOCSTEP) == 0)
	    {
	      XREALLOC(__FILE__, __FUNCTION__, __LINE__, f_list, f_list, 
		       sizeof(char*) * (cnum + 1 + TRACE_MATCH_ALLOCSTEP), -1);

	      /* Blank new elements */
	      memset(&f_list[cnum], 0x00, TRACE_MATCH_ALLOCSTEP*sizeof(char*));

	      /* Update the pointer, data can move during a reallocation */
	      *func_list = f_list;
	    }

	  /* Add the function in the table */
	  f_list[cnum] = name;

	  *count = ++cnum;
	}
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0); 
}

/**
 * Match functions from an addrtable
 * @param func_list function list pointer
 * @param count position on the list
 */
int			trace_match_addrtable(elfshobj_t *curfile, char ***func_list, unsigned int *count)
{
  eresi_Addr		*alist = NULL;
  unsigned int			index;
  char			**f_list;
  unsigned int			cnum;
  char			tmpstr[256];
  elfsh_Sym		*sym;
  char			*toadd;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!func_list || !count)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  f_list = *func_list;
  cnum = *count;
  
  /* Retrieve all called address in this binary */
  if (elfsh_addr_get_func_list(curfile, &alist) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Can't get call function list", -1);

  for (index = 0; alist[index]; index++)
    {
      sym = elfsh_get_symbol_by_value(curfile, alist[index], 
				      0, ELFSH_EXACTSYM);
      
      /* Find a symbol for this address */
      if (sym)
	{
	  toadd = elfsh_get_symbol_name(curfile, sym);
	}
      else
	{
	  snprintf(tmpstr, 255, TRACE_PRE_FUNC "" AFMT, 
		   alist[index]);
	  toadd = tmpstr;
	}

      /* Do we need to realloc ? */
      if (((cnum+1) % TRACE_MATCH_ALLOCSTEP) == 0)
	{
	  XREALLOC(__FILE__, __FUNCTION__, __LINE__, f_list, f_list, 
		   sizeof(char*) * (cnum + 1 + TRACE_MATCH_ALLOCSTEP), -1);

	  /* Blank new elements */
	  memset(&f_list[cnum], 0x00, TRACE_MATCH_ALLOCSTEP*sizeof(char*));
	  
	  /* Update the pointer, data can move during a reallocation */
	  *func_list = f_list;
	}

      /* If its temp string, strdup it */
      if (toadd == tmpstr)
	toadd = strdup(tmpstr);

      f_list[cnum] = toadd;

      *count = ++cnum;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0); 
}

/**
 * Match a list of function from symbol tables
 * @param funcreg Function regex (or not ?)
 * @param func_list the final function list
 */
int			trace_match_funcname(elfshobj_t *curfile, char *funcname, char ***func_list)
{
  regex_t		preg;
  char			**f_list;
  unsigned int			count = 0;
  elfshsect_t		*sect;
  int			num;
  elfsh_Sym		*symtab;
  elfsh_Sym		*sym;
  char			funcreg[256];
  char			addrname[256];
  size_t		len;
  eresi_Addr		addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!funcname || !func_list)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  len = strlen(funcname);

  /* We don't want to strip some part of the submited function
   but if you find a function/regex of this size (for this purpose) ... */
  if (len > 255)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Function name is too long", -1);

  /* An address ? */
  if (IS_VADDR(funcname))
    {
      /* Retrieve the address */
      sscanf(funcname + 2, AFMT, &addr);

      /* Prealloc the list */
      XALLOC(__FILE__, __FUNCTION__, __LINE__, f_list, sizeof(char*)*2, -1);

      sym = elfsh_get_symbol_by_value(curfile, addr, 0, ELFSH_EXACTSYM);
      
      /* We have a symbol for this address */
      if (sym)
	{
	  f_list[0] = elfsh_get_symbol_name(curfile, sym);
	  f_list[1] = NULL;
	}
      else
	{
	  sym = elfsh_get_dynsymbol_by_value(curfile, addr, 0, ELFSH_EXACTSYM);

	  /* We have a dynamic symbol for this address */
	  if (sym)
	    {
	      f_list[0] = elfsh_get_dynsymbol_name(curfile, sym);
	      f_list[1] = NULL;
	    }
	  else
	    {
	      TRACE_GET_FUNC_NAME(addrname, 255, funcname);
	      
	      f_list[0] = strdup(addrname);
	      f_list[1] = NULL;
	    }
	}

      goto end;
    }

  /* Add ^ and $ if needed, else we will check too many things
     For example, someone wanna add "main" function, if we don't
     add those symbols, it will match __libc_start_main which is very
     special function and that can create problems and make the tracer
     useless */
  snprintf(funcreg, 255, "%s%s%s", 
	   funcname[0] != '^' ? "^" : "",
	   funcname,
	   funcname[len-1] != '$' ? "$" : "");

  /* Do we have a regex ? */
  if (regcomp(&preg, funcreg, 0) != 0)
    {
      XALLOC(__FILE__, __FUNCTION__, __LINE__, f_list, sizeof(char*)*2, -1);
      f_list[0] = funcname;
      f_list[1] = NULL;

      goto end;
    }

  /* Preallocation */
  XALLOC(__FILE__, __FUNCTION__, __LINE__, f_list, 
	 sizeof(char*) * TRACE_MATCH_ALLOCSTEP, -1);


  /* Total match case */
  if (TRACE_MATCH_ALL(funcname))
    {
      /* Match everything we can, symbol or not ! */
      trace_match_addrtable(curfile, &f_list, &count);
    }

  /**
   * Match on symbol table
   */
  symtab = elfsh_get_symtab(curfile, &num);
  
  if (symtab != NULL)
    { 
      sect = elfsh_get_section_by_type(curfile, 
				       SHT_SYMTAB, 0, NULL, NULL, 0);
      
      /* Match function regex in the symbol table */
      trace_match_symtab(sect, num, &preg, &f_list, &count, elfsh_get_symbol_name);
    }
      
  /**
   * Match on dynamic symbol table
   */
  symtab = elfsh_get_dynsymtab(curfile, &num);

  if (symtab != NULL)
    { 

      sect = elfsh_get_section_by_name(curfile, 
				      ELFSH_SECTION_NAME_ALTDYNSYM, 
				      NULL, NULL, &num);
      if (!sect)
	sect = elfsh_get_section_by_type(curfile, SHT_DYNSYM, 0, 
					NULL, NULL, &num);

      num /= sizeof(elfsh_Sym);

      /* Match function regex in the dynamic symbol table */
      trace_match_symtab(sect, num, &preg, &f_list, &count, elfsh_get_dynsymbol_name);
    }

  /* Do we get something ? */
  if (count == 0)
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__, f_list);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Can't match a single function", -1);
    }

 end:

  /* Set final pointer */
  *func_list = f_list;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
