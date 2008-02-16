/**
* @file trace.c
*
* @brief All functions that help to trace content
*
* Started Jul 2 2005 00:03:44 mxatone
*
*
* $Id: trace.c,v 1.2 2008-02-16 12:32:27 thor Exp $
*
*/
#include "libstderesi.h"
#include "libetrace.h"

char buf[BUFSIZ];

/* XXX: The good syntax should be : 
** 
** traces <add>     funcname     [optional_traces_name]  : add a function to a given trace
** traces <rm>      funcname     [optional_traces_name]  : remove a function from a given trace
** traces <enable>  funcname|all [optional_traces_name]  : enable tracing for a function in a given trace (enabled by default after a add)
** traces <disable> funcname|all [optional_traces_name]  : disable tracing for a function in a given trace
** traces <create>  tracename    <optionals funcnames>   : create a new trace with a pool of traced functions by default
** traces <delete>  tracename                            : delete a trace
** traces <flush>   tracename                            : remove all functions from a given trace
** traces [optional_traces_name]                         : list all available traces
**
*/

char **last_arguments = NULL;

#define TRACE_ADDR_TABLE "addrtable"

#define TRACE_PRE_FUNC "func_"
#define TRACE_GET_FUNC_NAME(_buffer, _size, _addr) \
snprintf(_buffer, _size, TRACE_PRE_FUNC "%s", _addr+2)

#define TRACE_MATCH_ALL(_funcname) \
!strcmp(_funcname, ".*")

/**
 * Add arguments for a futur traces_run
 * @param argc arguments number 
 * @param argv arguments
 * @see traces_run
 */
int			revm_traces_add_arguments(int argc, char **argv)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (argc < 1 || argv == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1); 

  /* Copy arguments */
  XALLOC(__FILE__, __FUNCTION__, __LINE__, last_arguments, sizeof(char *)*(argc+1), -1);
  memcpy(last_arguments, argv, sizeof(char*)*argc);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0); 
}

const int match_alloc_step = 10;

/**
 * Match a function in a symbol table
 * @param sect section pointer
 * @param num element number
 * @param preg reg structure
 * @param func_list function table
 * @param count counter pointer (from the table)
 * @param get_symname function ptr to get symbol name
 * @return 0 on success.
 */
static int		revm_traces_match_in_symtable(elfshsect_t	*sect, 
						    int num, 
						    regex_t *preg,
						    char ***func_list, 
						    u_int *count,
						    char *(*get_symname)(elfshobj_t *f, 
									 elfsh_Sym *s))
{
  u_int			index;
  elfsh_Sym		*table;
  char			*name;
  char			**f_list;
  u_int			cnum;

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

      name = get_symname(world.curjob->curfile, table + index);

      /* Check if this name is valid */
      if (name == NULL || *name == 0)
	continue;

      /* We match a function */
      if (regexec(preg, name, 0, 0, 0) == 0)
	{
	  /* Do we need to realloc ? */
	  if (((cnum+1) % match_alloc_step) == 0)
	    {
	      XREALLOC(__FILE__, __FUNCTION__, __LINE__, f_list, f_list, 
		       sizeof(char*) * (cnum + 1 + match_alloc_step), -1);

	      /* Blank new elements */
	      memset(&f_list[cnum], 0x00, match_alloc_step*sizeof(char*));

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
static int		revm_traces_match_in_addrtable(char ***func_list, u_int *count)
{
  elfsh_Addr		*alist = NULL;
  u_int			index;
  char			**f_list;
  u_int			cnum;
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
  if (elfsh_addr_get_func_list(world.curjob->curfile, &alist) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Can't get call function list", -1);

  for (index = 0; alist[index]; index++)
    {
      sym = elfsh_get_symbol_by_value(world.curjob->curfile, alist[index], 
				      0, ELFSH_EXACTSYM);
      
      /* Find a symbol for this address */
      if (sym)
	{
	  toadd = elfsh_get_symbol_name(world.curjob->curfile, sym);
	}
      else
	{
	  snprintf(tmpstr, 255, TRACE_PRE_FUNC "" AFMT, 
		   alist[index]);
	  toadd = tmpstr;
	}

      /* Do we need to realloc ? */
      if (((cnum+1) % match_alloc_step) == 0)
	{
	  XREALLOC(__FILE__, __FUNCTION__, __LINE__, f_list, f_list, 
		   sizeof(char*) * (cnum + 1 + match_alloc_step), -1);

	  /* Blank new elements */
	  memset(&f_list[cnum], 0x00, match_alloc_step*sizeof(char*));
	  
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
static int		revm_traces_match_funcname(char *funcname, char ***func_list)
{
  regex_t		preg;
  char			**f_list;
  u_int			count = 0;
  elfshsect_t		*sect;
  int			num;
  elfsh_Sym		*symtab;
  elfsh_Sym		*sym;
  char			funcreg[256];
  char			addrname[256];
  size_t		len;
  elfsh_Addr		addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!funcreg || !func_list)
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

      sym = elfsh_get_symbol_by_value(world.curjob->curfile, addr, 0, ELFSH_EXACTSYM);
      
      /* We have a symbol for this address */
      if (sym)
	{
	  f_list[0] = elfsh_get_symbol_name(world.curjob->curfile, sym);
	  f_list[1] = NULL;
	}
      else
	{
	  sym = elfsh_get_dynsymbol_by_value(world.curjob->curfile, addr, 0, ELFSH_EXACTSYM);

	  /* We have a dynamic symbol for this address */
	  if (sym)
	    {
	      f_list[0] = elfsh_get_dynsymbol_name(world.curjob->curfile, sym);
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
	 sizeof(char*) * match_alloc_step, -1);


  /* Total match case */
  if (TRACE_MATCH_ALL(funcname))
    {
      /* Match everything we can, symbol or not ! */
      revm_traces_match_in_addrtable(&f_list, &count);
    }

  /**
   * Match on symbol table
   */
  symtab = elfsh_get_symtab(world.curjob->curfile, &num);
  
  if (symtab != NULL)
    { 
      sect = elfsh_get_section_by_type(world.curjob->curfile, 
				       SHT_SYMTAB, 0, NULL, NULL, 0);
      
      /* Match function regex in the symbol table */
      revm_traces_match_in_symtable(sect, num, &preg, &f_list, &count, elfsh_get_symbol_name);
    }
      
  /**
   * Match on dynamic symbol table
   */
  symtab = elfsh_get_dynsymtab(world.curjob->curfile, &num);

  if (symtab != NULL)
    { 

      sect = elfsh_get_section_by_name(world.curjob->curfile, 
				      ELFSH_SECTION_NAME_ALTDYNSYM, 
				      NULL, NULL, &num);
      if (!sect)
	sect = elfsh_get_section_by_type(world.curjob->curfile, SHT_DYNSYM, 0, 
					NULL, NULL, &num);

      num /= sizeof(elfsh_Sym);

      /* Match function regex in the dynamic symbol table */
      revm_traces_match_in_symtable(sect, num, &preg, &f_list, &count, elfsh_get_dynsymbol_name);
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

/**
 * Research the uniform debugging format function entry on a list of files 
 * @param files first element of a edfmtfile_t structure
 * @param name function name
 * @return pointer on a edfmtfunc_t (represent function debugging information)
 */
static edfmtfunc_t	*revm_traces_search_uni_file(edfmtfile_t *files, char *name)
{
  edfmtfunc_t		*func;
  edfmtfile_t		*file;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (file = files ; file != NULL; file = file->next)
    {
      func = hash_get(&(file->hfunc), name);

      if (func != NULL)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, func); 

      if (file->child)
	{
	  func = revm_traces_search_uni_file(file->child, name);
	  if (func != NULL)
	    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, func); 
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL); 
}

/**
 * Research the uniform debugging format function entry 
 * @param file first element of a edfmtfile_t structure
 * @param name function name
 * @return pointer on a edfmtfunc_t (represent function debugging information)
 */
static edfmtfunc_t    	*revm_traces_search_uni(elfshobj_t *file, char *name)
{
  edfmtfunc_t		*func;
  edfmtinfo_t		*uni;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  uni = edfmt_get_uniinfo(file);

  if (!uni)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "No debugging informations available", NULL); 

  /* Search on global scope */
  func = hash_get(&(uni->hfunc), name);
  if (func == NULL)
    {
      /* Search on local stop if needed */
      func = revm_traces_search_uni_file(uni->files, name);
      if (func == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Function not found", NULL); 
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, func); 
}

/**
 * Search the function entry on the uniform debugging format, and return it 
 * @param file first element of a edfmtfile_t structure
 * @param func_name function name
 * @param external indicate if we need internal or external research
 * @return pointer on a edfmtfunc_t (represent function debugging information)
 */
edfmtfunc_t 		*revm_traces_tracable_with_type(elfshobj_t *file, char *func_name,
							 u_char external)
{
  elfshobj_t		*sym_file;
  edfmtfunc_t		*func = NULL;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* We have an extern symbol */
  if (external)
    {
      sym_file = etrace_search_sym(file, func_name);
      
      if (sym_file)
	func = revm_traces_search_uni(sym_file, func_name);
    }
  else
    {
      /* Search in this file */
      func = revm_traces_search_uni(file, func_name);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, func);
}

/**
 * Construct the argument structure depending of the option we have 
 * @param file target file
 * @param ent pointer to set
 * @param func uniform debugging inforation if we got one
 * @param vaddr virtual address of the function if we don't have debugging information
 * @param external describe scope
 * @see revm_traces_createargs
 */
static int		revm_traces_build_args(elfshobj_t *file, elfshtraces_t *ent, 
					     edfmtfunc_t *func, elfsh_Addr vaddr, 
					     u_char external)
{
  u_int			index;
  int			*argcount;
  edfmtfuncarg_t	*arg;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

  if (!ent)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  elfsh_setup_hooks();

  ent->scope = external ? ELFSH_ARG_EXTERN : ELFSH_ARG_INTERN;

  /* Do we have uniform debugging information for this structure ? */
  if (func == NULL)
    {
      argcount = elfsh_args_count(file, 0, vaddr);
      
      ent->type = ELFSH_ARG_SIZE_BASED;
      ent->argc = 0;

      /* If we found some argument, else its 0 */
      if (argcount)
	{
	  /* Fill arguments */
	  for (index = 0; argcount[index] > 0; index++)
	    ent->arguments[index].size = argcount[index];
	  
	  ent->argc = index;
	  XFREE(__FILE__, __FUNCTION__, __LINE__, argcount);
	}
    }
  else
    {
      ent->type = ELFSH_ARG_TYPE_BASED;
      for (arg = func->arguments, index = 0; arg != NULL; arg = arg->next, index++)
	{
	  /* We setup argument name and typename */
	  ent->arguments[index].name = arg->name;
	  ent->arguments[index].typename = arg->type ? arg->type->name : NULL;

	  /* Other information are quite the same as arg counting method */
	  ent->arguments[index].size = 0;
	  if (arg->type->size > 0)
	    ent->arguments[index].size = arg->type->size; 
	  
	  if (ent->arguments[index].size < sizeof(elfsh_Addr))
	    ent->arguments[index].size = sizeof(elfsh_Addr); 
	}

      ent->argc = func->argc;
    }
      
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Allocate the traces  structure and call previous
 * @param file target file
 * @param func uniform debugging inforation if we got one
 * @param vaddr virtual address of the function if we don't have debugging information
 * @param external describe scope
 * @return generate trace structure
 * @see revm_traces_build_args
 */
elfshtraces_t		*revm_traces_createargs(elfshobj_t *file, char *name,
					      edfmtfunc_t *func, elfsh_Addr vaddr,
					      u_char external)
{
  elfshtraces_t 	*newtrace;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  // Alloc the structure
  XALLOC(__FILE__, __FUNCTION__, __LINE__, newtrace, sizeof(elfshtraces_t), NULL);
  snprintf(newtrace->funcname, ELFSH_TRACES_FUNC_SIZE - 1, "%s", name);
  newtrace->file = file;
  newtrace->enable = 1;
  newtrace->vaddr = vaddr;

#if __DEBUG_ARG_COUNT__
  printf("[DEBUG_ARG_COUNT] Builds args for %s\n", name);
#endif

  // We fill arguments part and check if we found the function
  if (revm_traces_build_args(file, newtrace, func, vaddr, external) < 0)
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__, newtrace);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Symbol not found or impossible to trace", NULL);
     } 

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newtrace);
}

/**
 * Add a function on a trace (from commande line)
 * @param file target
 * @param name function name
 * @param optarg trace name (optional)
 */
int		traces_add(elfshobj_t *file, char *name, char **optarg)
{
  elfshtraces_t 	*traces;
  edfmtfunc_t		*func = NULL;
  elfsh_Addr		vaddr = 0;
  u_char		external = 0;
  char			**func_list;
  u_int			index, traced = 0;
  char			*trace_name;
  int			ret;
  char			*current_name;
  elfsh_Addr		addr = 0;
  u_char		cont = 0;
  u_char		multiaddr = 0;
  size_t		prelen;
  elfshsect_t		*newsymsect;
  elfsh_Sym		*symbol;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name || !name[0])
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  /* Try to match functions with a regex */
  if (revm_traces_match_funcname(name, &func_list) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Can't match a single function", -1);

  /* Address case 
     if the function name (always the first) will have the address in its name
     else we found a symbol which correspond to the address */
  if (IS_VADDR(name) && strstr(func_list[0], name + 2) != NULL)
    sscanf(name + 2, AFMT, &addr);

  trace_name = optarg ? *optarg : NULL;

  /* When we try to match everything */
  multiaddr = TRACE_MATCH_ALL(name);

  if (multiaddr)
    {
      prelen = strlen(TRACE_PRE_FUNC);

      snprintf(buf, BUFSIZ - 1,
	       "\n\t[*] Start hooking all internal/external functions ...\n\n");
      revm_output(buf);
    }

  /* Try to add every function */
  for (index = 0; func_list[index] != NULL; index++)
    {
      external = 0;
      vaddr = 0;
      cont = 0;
      current_name = func_list[index];

      /* FIXME: A function name containing "." will not be compiled right with gcc */
      if (strstr(current_name, ".") || !PRINTABLE(current_name[0]))
	continue;

      /* Can we trace this function */
      if (addr > 0)
	ret = etrace_valid_faddr(file, addr, &vaddr, &external);
      else
	{
	  /* Multiple address tracing */
	  if (multiaddr && !strncmp(TRACE_PRE_FUNC, current_name, prelen))
	    {
	      /* Prepare for a new address function */
	      sscanf(current_name + prelen, AFMT, &vaddr);
	      ret = 0;
	    }
	  else
	    ret = etrace_tracable(file, current_name, &vaddr, &external);
	}

      if (ret < 0)
	{
	  buf[0] = 0x00;
	  switch(ret)
	    {
	    case -2: /* Untracable function */
	      snprintf(buf, BUFSIZ - 1, 
		       "\t[!] %s is an untracable function. Some dedicated OS functions are "
		       "context depend and can't be traced.\n\n",
		       current_name);
	      cont = 1;
	      break;

	    case -3: /* Not called address */
	      snprintf(buf, BUFSIZ - 1, 
		       "\t[!] %s (%s) address will be trace but we won't be able to find at least "
		       "one call for this function.\n\n",
		       current_name, name);
	      break;

	    case -4: /* Not called address */
	      snprintf(buf, BUFSIZ - 1, 
		       "\t[!] %s (%s) address can't be found on entrypoint section.\n\n",
		       current_name, name);
	      cont = 1;
	      break;
	    }

	  if (buf[0] != 0x00)
	    revm_output(buf);

	  if (cont)
	    continue;
	}

      /* Can we trace with libedfmt informations ? */
      if (addr > 0 || vaddr > 0)
	{
	  func = NULL;

	  /* Do we need to add the symbol right before hijacking ? */
	  symbol = elfsh_get_symbol_by_name(file, current_name);
	  
	  if (!symbol)
	    {
	      newsymsect = elfsh_get_parent_section(file, addr > 0 ? addr : vaddr, NULL);
	  
	      if (!newsymsect)
		continue;
	  
	      /* Add a new function symbol */
	      if (elfsh_insert_funcsym(file, current_name, 
				       addr > 0 ? addr : vaddr,
				       0, newsymsect->index) < 0)
		continue;
	    }
	  else if (!symbol->st_value) /* Invalid symbol */
	    continue;
	}
      
      if (!addr)
	func = revm_traces_tracable_with_type(file, current_name, external);

      /* Create the structure */
      traces = revm_traces_createargs(file, current_name, func, vaddr, external);

      if (!traces)
	continue;

      /* Add the function */
      if (etrace_funcadd(trace_name, current_name, traces) == NULL)
	continue;

      /* To avoid flood we don't display every added function for multiaddr */
      if (!multiaddr)
	{
	  snprintf(buf, BUFSIZ - 1, "\t[*] Added function %s successfully into trace %s\n\n",
		   current_name, trace_name ? trace_name : ELFSH_TRACES_TYPE_DEFAULT);
	  revm_output(buf);
	}

      traced++;
    }

  /* Free allocated table */
  if (index > 0)
    XFREE(__FILE__, __FUNCTION__, __LINE__, func_list);

  /* Do we add at least something ? */
  if (traced == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Can't add any founded functions", -1);

  /* We print the sum only */
  if (multiaddr)
    {
      snprintf(buf, BUFSIZ - 1, "\t[*] Added %d functions successfully into trace %s\n\n",
	      traced, trace_name ? trace_name : ELFSH_TRACES_TYPE_DEFAULT);
      revm_output(buf);      
    }

  /* Free function list */
  XFREE(__FILE__, __FUNCTION__, __LINE__, func_list);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
 
/** 
 * Delete a function from a trace 
 * @param file target
 * @param name function name
 * @param optarg trace name (optional)
 */
int		traces_rm(elfshobj_t *file, char *name, char **optarg)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name || !name[0])
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  if (etrace_funcrm(optarg ? *optarg : NULL, name) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Delete function failed", -1);

  snprintf(buf, BUFSIZ - 1, "\t[*] Deleted function %s successfully from trace %s\n\n",
	   name, optarg && *optarg ? *optarg : ELFSH_TRACES_TYPE_DEFAULT);
  revm_output(buf);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Exclude functions by regexec during the last stage
 * @param file target
 * @param freg first reg
 * @param oreg others reg list
 */
int		traces_exclude(elfshobj_t *file, char *freg, char **oreg)
{
  u_int		index;
  const char pattern[] = "\n\t[*] Exclude function %s successfully\n\n";

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (!freg || !freg[0])
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  /* Exclude first argument */
  if (etrace_funcexclude(freg) < 0)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);

  snprintf(buf, BUFSIZ - 1, pattern, freg);
  revm_output(buf);

  if (oreg)
    {
      for (index = 0; oreg[index] != NULL; index++)
	{
	  if (etrace_funcexclude(oreg[index]) < 0)
	    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);

	    snprintf(buf, BUFSIZ - 1, pattern, oreg[index]);
	    revm_output(buf);
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Delete excluded functions by regexec during the last stage
 * @param file target
 * @param freg first reg
 * @param oreg others reg list
 */
int		traces_rmexclude(elfshobj_t *file, char *freg, char **oreg)
{
  u_int		index;
  const char pattern[] = "\n\t[*] Delete excluded function %s successfully\n\n";

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!freg || !freg[0])
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  etrace_funcrmexclude(freg);

  snprintf(buf, BUFSIZ - 1, pattern, freg);
  revm_output(buf);

  if (oreg)
    {
      for (index = 0; oreg[index] != NULL; index++)
	{
	  etrace_funcrmexclude(oreg[index]);

	  snprintf(buf, BUFSIZ - 1, pattern, oreg[index]);
	  revm_output(buf);
	}
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Enable a function from a trace 
 * @param file target
 * @param name function name
 * @param optarg trace name (optional)
 */
int		traces_enable(elfshobj_t *file, char *name, char **optarg)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!strcmp(name, "all"))
    {
      if (etrace_funcenableall(optarg ? *optarg : NULL) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Enable all functions failed", -1);      
    }
  else
    {
      if (etrace_funcenable(optarg ? *optarg : NULL, name) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Enable function failed", -1);
    }

  snprintf(buf, BUFSIZ - 1, "\t[*] Enabled function %s successfully from trace %s\n\n",
	   name, optarg && *optarg ? *optarg : ELFSH_TRACES_TYPE_DEFAULT);
  revm_output(buf);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Disable a function from a trace 
 * @param file target
 * @param name function name
 * @param optarg trace name (optional)
 */
int		traces_disable(elfshobj_t *file, char *name, char **optarg)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!strcmp(name, "all"))
    {
      if (etrace_funcdisableall(optarg ? *optarg : NULL) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Disable all functions failed", -1);      
    }
  else
    {
      if (etrace_funcdisable(optarg ? *optarg : NULL, name) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Disable function failed", -1);
    }

  snprintf(buf, BUFSIZ - 1, "\t[*] Disabled function %s successfully from trace %s\n\n",
	   name, optarg && *optarg ? *optarg : ELFSH_TRACES_TYPE_DEFAULT);
  revm_output(buf);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Create a trace 
 * @param file target
 * @param name trace name
 * @param optarg list of function to directly add
 */
int		traces_create(elfshobj_t *file, char *name, char **optarg)
{
  char		*strarr[2];
  int		index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name || !name[0])
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  if (etrace_createtrace(name) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Create trace failed", -1);

  snprintf(buf, BUFSIZ - 1, "\t[*] Created trace %s successfully\n\n", name);
  revm_output(buf);

  if (optarg && *optarg)
    {
      /* Add gived elements */
      for (index = 0; optarg[index]; index++)
	{
	  strarr[0] = name;
	  strarr[1] = NULL;
	  if (traces_add(file, optarg[index], strarr) < 0)
	    continue;
	}

      revm_output("\n");
    }
    
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Delete a trace 
 * @param file target
 * @param name trace name
 * @param optarg not use
 */
int		traces_delete(elfshobj_t *file, char *name, char **optarg)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name || !name[0])
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  if (etrace_deletetrace(name) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Delete trace failed", -1);

  snprintf(buf, BUFSIZ - 1, "\t[*] Deleted trace %s successfully\n\n", name);
  revm_output(buf);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Flush a whole trace 
 * @param name trace name
 * @param optarg not use
 */
int		traces_flush(elfshobj_t *file, char *name, char **optarg)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name || !name[0])
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  if (etrace_funcrmall(name) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Flush trace failed", -1);

  snprintf(buf, BUFSIZ - 1, "\t[*] Flushed trace %s successfully\n\n", name);
  revm_output(buf);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Detail listing of a trace 
 * @param table trace hash table
 * @param name trace name
 * @see traces_list
 */
static int	traces_list_detail(hash_t *table, char *name)
{
  char		**keys;
  u_int		index;
  int		keynbr;
  elfshtraces_t	*entrie;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(buf, BUFSIZ - 1, " ~> %s: %s\n", 
	   revm_colorfieldstr("Trace name"),
	   revm_colorstr(name));
  revm_output(buf);

  keys = hash_get_keys(table, &keynbr);

  if (keys && keynbr > 0)
    {
      revm_output("\n");

      for (index = 0; index < keynbr; index++)
	{
	  entrie = (elfshtraces_t *) hash_get(table, keys[index]);
	  
	  if (entrie)
	    {
	      snprintf(buf, BUFSIZ - 1, " %s: %s %s: %s\n",
		       revm_colorfieldstr("Function name"),
		       revm_colorstr_fmt("%-15s", entrie->funcname), 
		       revm_colorfieldstr("status"),
		       revm_colortypestr(entrie->enable ? "enabled" : "disabled"));
	      revm_output(buf);
	      revm_endline();
	    }
	}

      revm_output("\n");

      hash_free_keys(keys);
    }
  else
    {
      revm_output("\n No function in this trace\n\n");
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Enumerate each trace and detail every functions 
 * @param name not use
 * @param optarg not use
 * @see traces_list_detail
 */
int		traces_list(elfshobj_t *file, char *name, char **optarg)
{
  char		**keys = NULL;
  u_int		index;
  int		keynbr;
  hash_t	*subtable;
  char		funcreg[256];
  size_t	len;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_output(" .: Trace list :.\n");

  if (traces_table.ent)
    keys = hash_get_keys(&traces_table, &keynbr);

  if (keys)
    {
      if (keynbr > 0)
	revm_output("\n");

      for (index = 0; index < keynbr; index++)
	{
	  subtable = hash_get(&traces_table, keys[index]);

	  if (subtable)
	    traces_list_detail(subtable, keys[index]);
	}

      hash_free_keys(keys);
    }
  else
    {
      revm_output("\n No tracing table available\n\n");
    }

  revm_output(" .: Exclude list :.\n");

  /* Reset */
  keynbr = 0;
  keys = NULL;

  /* Print exclude list */
  if (exclude_table.ent)
    keys = hash_get_keys(&exclude_table, &keynbr);

  if (keys)
    {
      if (exclude_table.ent)
	revm_output("\n");

      for (index = 0; index < keynbr; index++)
	{
	  len = strlen(keys[index]);
	  snprintf(funcreg, 255, "%s%s%s", 
		   keys[index][0] != '^' ? "^" : "",
		   keys[index],
		   keys[index][len-1] != '$' ? "$" : "");

	  snprintf(buf, BUFSIZ - 1, " %s %s %s %s %s\n", 
		   revm_colornumber("[%02u]", index+1), 
		   revm_colorfieldstr("name:"),
		   revm_colorstr_fmt("%-15s", keys[index]),
		   revm_colorfieldstr("regex:"),
		   revm_colorstr(funcreg));
	  revm_output(buf);
	  revm_endline();
	}

      hash_free_keys(keys);
    }
  else
    {
      revm_output("\n No exclude table available\n");
    }

  revm_output("\n");

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Run a trace session (used in etrace)
 * @param file target
 * @param argv argument list
 * @param argc argument counter
 */
int		traces_run(elfshobj_t *file, char **argv, int argc)
{
  char		*home;
  char		*filename;
  size_t	len;
  u_int		index;
  char		*path;
  char		*av[argc+2];
  int		ret;
  struct timeval now, after;
  double	difftime;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* We use etrace */
  if (world.state.revm_mode == REVM_STATE_TRACER)
    {
      argc = 0;
      if (last_arguments)
	{
	  while (last_arguments[argc] != NULL)
	    argc++;
	  argv = last_arguments;
	}
      else
	{
	  XALLOC(__FILE__, __FUNCTION__, __LINE__, argv, sizeof(char*), -1);
	  argv[0] = NULL;
	}
    }

  if (argc < 0 || !argv)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid parameters", -1); 

  if (!(elfsh_get_objtype(elfsh_get_hdr(file)) == ET_EXEC))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "We need a ET_EXEC binary", -1);

  home = getenv("HOME");

  if (!home)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Cannot find home directory", -1);
  
  snprintf(buf, BUFSIZ - 1, "%s/%s", home, REVM_TRACE_REP);

  /* Create temp directory if needed */
  mkdir(buf, 0700);

  /* We search the filename (we have a path) */
  len = strlen(file->name);
  filename = file->name;

  for (index = len; index > 0; index--)
    if (file->name[index] == '/')
      break;
  
  if (INTERVAL(0, index+1, len))
    filename = file->name + index + 1;

  /* Full path */
  snprintf(buf, BUFSIZ - 1, "%s/%s/%s", home, REVM_TRACE_REP, filename);
  path = strdup(buf);

  /* Pre register working objects for ELFsh */
  elfsh_register_working_objects(&world.curjob->loaded,
				 &world.shared_hash);

  if (elfsh_save_obj(file, path) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Failed to save object", -1);

  av[0] = path;
  for (index = 0; index < argc; index++)
    av[index+1] = argv[index];
  av[index+1] = NULL;

  gettimeofday(&now, NULL);

  /* Print start */
  revm_output("Start execution of ");
  for (index = 0; index < argc+1; index++)
    {
      snprintf(buf, BUFSIZ - 1, "%s ", av[index]);
      revm_output(buf);
    }
  revm_output("\n");

  /* Start with arguments */
  if (!fork())
    ret = execv(av[0], av);
  else
    wait(NULL);

  gettimeofday(&after, NULL);

  difftime = (after.tv_sec - now.tv_sec) + ((double) (after.tv_usec - now.tv_usec)) * 0.000001;

  snprintf(buf, BUFSIZ - 1, "End execution (%.6f sec)\n\n", difftime);
  revm_output(buf);

  XFREE(__FILE__, __FUNCTION__, __LINE__, path);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Create the structure that store trace cmd informations 
 * @param exec function pointer
 * @param flagName
 * @param flagArg
 */
revmtraces_t   *traces_create_CMDENT(int (*exec)(elfshobj_t *file, char *name, char **optarg),
				   char flagName, char flagArg)
{
  revmtraces_t *new;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(__FILE__, __FUNCTION__, __LINE__, new, sizeof(revmtraces_t), NULL);
  new->exec	= exec;
  new->flagName	= flagName;
  new->flagArg	= flagArg;
 
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}

/**
 * Add a command in sub commande hash table
 * @param cmd cmd name
 * @param exec function pointer
 * @param flagName
 * @param flagArg
 * @see traces_cmd_hash
 * @see traces_create_CMDENT
 */
int 		traces_addcmd(char *cmd, void *exec, char flagName, char flagArg)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  hash_add(&traces_cmd_hash, cmd, (void *) traces_create_CMDENT(exec, flagName, flagArg));

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int 			cmd_traces()
{
  revmtraces_t		*cmd;
  char			*fArg;
  char			**sArg;
  int			ret = 0;
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Init tracing elements */
  etrace_inittrace();

  switch(world.curjob->curcmd->argc)
    {
    case 0:
      traces_list(world.curjob->curfile, NULL, NULL);
      break;
    case 1:
      table = etrace_gettrace(world.curjob->curcmd->param[0]);

      if (!table)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unknown trace", -1);

      traces_list_detail(table, world.curjob->curcmd->param[0]);
      break;
    default:
      cmd = hash_get(&traces_cmd_hash, world.curjob->curcmd->param[0]);
      
      if (!cmd)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Unknown command", -1);

      fArg = NULL;
      sArg = NULL;

      if (cmd->flagName > 0)
	{
	  /* First argument is needed and doesn't submited */
	  if (!world.curjob->curcmd->param[1] && cmd->flagName == 2)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			      "First argument is needed", -1);

	  fArg = world.curjob->curcmd->param[1];

	  if (cmd->flagArg > 0)
	    {
	      /* Second argument is needed and doesn't submited */
	      if (!world.curjob->curcmd->param[2] && cmd->flagArg == 2)
		PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
				  "Second argument is needed", -1);

	      sArg = world.curjob->curcmd->param + 2;
	    }
	}

      ret = cmd->exec(world.curjob->curfile, fArg, sArg);      
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}

int 			cmd_traceadd()
{
  u_int			index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Init tracing elements */
  etrace_inittrace();

  if (world.curjob->curcmd->argc < 1)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Need at least one function", -1);

  /* Loop on each passed functions */
  for (index = 0; index < world.curjob->curcmd->argc; index++) 
    {
      if (traces_add(world.curjob->curfile, world.curjob->curcmd->param[index], NULL) < 0)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int 			cmd_traceexclude()
{
  u_int			index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.curjob->curcmd->argc < 1)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Need at least one function", -1);

  /* Loop on each passed functions */
  for (index = 0; index < world.curjob->curcmd->argc; index++) 
    {
      if (traces_exclude(world.curjob->curfile, world.curjob->curcmd->param[index], NULL) < 0)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int			cmd_tracerun()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (traces_run(world.curjob->curfile, 
		 world.curjob->curcmd->param, 
		 world.curjob->curcmd->argc) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to run current trace", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
