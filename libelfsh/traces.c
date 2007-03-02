/*
** traces.c for libelfsh
**
** All functions that help to trace content
**
** Started Jul 2 2005 00:03:44 mxatone
**
*/
#include "libelfsh.h"

#define TRACES_CFLOW 	1
#define TRACES_PLT 	2
#define TRACES_GOT 	3

char buf[BUFSIZ];

hash_t traces_table;

#define ELFSH_TRACES_TABLE_NAME "elfsh_traces_table"
#define ELFSH_TRACES_PATTERN "traces_%s"

typedef struct 	s_eetrace
{
  char 		*funcname;
  int		redirtype;
  elfsh_Addr	addr;
  
} e2dbgtraced_t;

/* XXX: Need to use libgcc and not gcc directly */
/*
FILE		*elfsh_traces_init(char *tfname, char *rsofname, char *rtfname)
{
  int		rs = 0, fd;
  FILE 		*fp;
  char		buf[BUFSIZ + 1];
  int		osize = strlen(tfname)+3;
  struct stat	s;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
    
 fretry:
  if (rs > 30)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot create temporary file (retry more than 30 times)", 
		      NULL);
  
  rs++;

  // Generate a random temporary name file 
  fd = mkstemp(tfname);

  if (fd == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot create temporary file", NULL);

  fp = fdopen(fd, "w");

  if (fp == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot read temporary file", NULL);

  // gcc needs rights extentions
  snprintf(rtfname, osize, "%s.c", tfname);
  snprintf(rsofname, osize, "%s.o", tfname);

  // If one of this two files exists, we retry
  if (lstat(rtfname, &s) == 0 
      || lstat(rsofname, &s) == 0)
    goto fretry;

#if __DEBUG_TRACES__
  printf("[DEBUG TRACE] Open trace temporary filename: %s\n", tfname);
  printf("[DEBUG TRACE] Futur trace filename: %s\n", rtfname);
  printf("[DEBUG TRACE] Relocatable filename: %s\n", rsofname);
#endif

// Write basic stuff, include headers and
//   pad functions (to have a tree like form)

  snprintf(buf, BUFSIZ, 
	   "#include <stdio.h>\n\n"
	   "char pad_str[64];\n"
	   "int pad_count = 0;\n\n"
	   "char *pad_print(int inc)\n{\n"
	   "\tint i;\n"
	   "\tif (inc == 0)\n\t\tpad_count -= 2;\n"
	   "\tfor (i = 0; i < pad_count; i++)\n"
	   "\t\tpad_str[i] = ' ';\n"
	   "\t\tpad_str[i] = 0;\n"
	   "\tif (inc == 1)\n\t\tpad_count += 2;\n"
	   "\treturn pad_str;\n}\n");

  fwrite(buf, strlen(buf), sizeof(char), fp);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, fp);
}
*/


/* Add a function to the tracing file */
/*
int		elfsh_traces_add(FILE *fp, int *argcount, char *func_name)
{
  int		z = 0;
  char		buf[BUFSIZ + 1];
  char		bufex[BUFSIZ + 1];
  char		args[BUFSIZ + 1];
  char		argsproto[BUFSIZ + 1];
  char		argshexa[BUFSIZ + 1];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  argshexa[0] = 0;
  argsproto[0] = 0;
  args[0] = 0;

  // Do we have arguments in this function ? 
  if (argcount[0] != 0)
  {
    snprintf(argshexa, BUFSIZ, "%%%%%s", "08x");
    snprintf(argsproto, BUFSIZ, "char a%d[%d]", 0, argcount[0]);
    snprintf(args, BUFSIZ, "a%d", 0);
	
    // Arguments by arguments 
    for (z = 1; z < ELFSH_TRACES_MAX_ARGS && argcount != NULL; z++)
    {
      if (argcount[z] == 0)
	break;

	// char type is just what we need to setup a good size 
      snprintf(buf, BUFSIZ, ", char a%d[%d]", z, argcount[z]);

      if (strlen(argsproto) + strlen(buf) + 1 > BUFSIZ)
	break;

      strncat(argsproto, buf, strlen(buf));

      snprintf(buf, BUFSIZ, ", a%d", z);

      if (strlen(args) + strlen(buf) + 1 > BUFSIZ)
	break;

      strncat(args, buf, strlen(buf));

      snprintf(buf, BUFSIZ, ", %%%%%s", "08x");

      if (strlen(argshexa) + strlen(buf) + 1 > BUFSIZ)
	break;

      strncat(argshexa, buf, strlen(buf));
    }
  }

  // Tracing function with a human readable form
  snprintf(bufex, BUFSIZ, "int %%1$s_trace(%%2$s)\n{\n"
	   "\tint ret;\n"
	   "\tprintf(\"%%%%s + %%1$s(%s)\\n\", pad_print(1)%s%s);\n"
	   "\tret = old_%%1$s(%%3$s);\n"
	   "\tprintf(\"%%%%s - %%1$s\\n\", pad_print(0));\n"
	   "\treturn ret;\n}\n",
	   argshexa, (strlen(args) > 0 ? "," : ""), args);

  snprintf(buf, BUFSIZ, bufex, 
	   func_name, argsproto, args);

  fwrite(buf, strlen(buf), sizeof(char), fp);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, z);
}
*/



/* Prepare to trace functions */
/*
int			elfsh_traces_functions(FILE *fp, elfsh_Sym *symtab, 
					   unsigned int symnum, char **selfuncs)
{
  static unsigned int	called = 0;
  unsigned int		index;
  unsigned int		count;
  unsigned int		z;
  unsigned int		off;
  elfshsect_t		*sect;
  char			*sect_name;
  char			*func_name;
  int			*argcount;
  unsigned int		numargs;
#if 0
  char			buf[BUFSIZ];
#endif

  // Some preliminaries checks
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  called++;
  if (symtab == NULL || symnum == 0)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  for (index = 0, count = 1; index < symnum; index++)
    {
      // Only function symbols
      if (elfsh_get_symbol_type(symtab + index) != STT_FUNC)
	continue;
      
      sect = elfsh_get_parent_section(world.curjob->current, 
				      symtab[index].st_value, NULL);
      
      // Try to get the section using index 
      if (sect == NULL && symtab[index].st_shndx)
	sect = elfsh_get_section_by_index(world.curjob->current,
					  symtab[index].st_shndx, NULL, NULL);
      if (sect == NULL)
	continue;
      sect_name = elfsh_get_section_name(world.curjob->current, sect);
      
      // Only global, plt & text 
      // Make sure we look at the beginning of the name, including the .
      if (elfsh_get_symbol_bind(symtab + index) != STB_GLOBAL
	  || (strncmp(sect_name, ".plt", 4) && strncmp(sect_name, ".text", 5)))
	continue;
     
      // Switch between symbol table and dynamic symbol table
      switch (called)
	{
	case 0:
	  func_name = elfsh_get_symbol_name(world.curjob->current, symtab + index);
	  break;
	default:
	  func_name = elfsh_get_dynsymbol_name(world.curjob->current, symtab + index);
	  break;
	}

      // FIXME: A function name containing "." will not be compiled right with gcc 
      if (strstr(func_name, "."))
	continue;
      
      // have we chosen which function will be traced ?
      for (z = 0; world.curjob->curcmd->param[z]; z++)
	if (!strcmp(world.curjob->curcmd->param[z], func_name))
	  break;
      
      // This function should not be traced
      if (world.curjob->curcmd->param[z])
	continue;
      
      off = (int) elfsh_get_raw(sect) + symtab[index].st_value;
      off -= (sect->parent->rhdr.base + sect->shdr->sh_addr);
      
      // Get args number table
      argcount = elfsh_args_count(world.curjob->current, off, symtab[index].st_value);
      if (argcount == NULL)
	continue;
      selfuncs[count - 1] = strdup(func_name);
 
      // Add function handle into tmp file 
      numargs = elfsh_traces_add(fp, argcount, func_name);
      
      // Print some informations
#if 0
      snprintf(buf, BUFSIZ, "[%03d] %-24s => %-20s argnum = %d \n", 
	       count, func_name, sect_name, numargs);      
      vm_output(buf);      
#endif
      count++;
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, count - 1);
}
*/

/*
** TODO: 
**  - Module more
**  - Support arguments
**  - use libgcc and not gcc with file
*/
/*
int		traces_add(char *name, char **optarg)
{
  int	       	index;
  int		symnum = 0;
  int		dynsymnum = 0;
  int		count = 0;
  elfsh_Sym	*symtab;
  elfsh_Sym	*dynsym;
  FILE		*fp;
  elfshobj_t	*tobj;
  int		idx;
  int		err;
  elfsh_Addr	addr;
  elfsh_Sym	*dst;
  char		buf[BUFSIZ + 1];
  char 		tfname[] = "/tmp/tracingXXXXXX";
  int		osize = strlen(tfname) + 3;
  char		rsofname[osize];
  char		rtfname[osize];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  elfsh_setup_hooks();

  vm_output(" .: Tracing Functions :.\n\n");

  // Do we have symbols ?
  symtab = elfsh_get_symtab(world.curjob->current, &symnum);
  dynsym = elfsh_get_dynsymtab(world.curjob->current, &dynsymnum);

  if (symnum + dynsymnum == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No symbols found", (-1));

  // Initialize the tracing
  fp = elfsh_traces_init(tfname, rsofname, rtfname);
  if (fp == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot init tmp files", (-1));

  // Create temporary storage
  char 		*selfuncs[symnum + dynsymnum + 1];

  for (index = 0; index < symnum + dynsymnum + 1; index++)
    selfuncs[index] = NULL;

  // Call on both symbol tables
  count += elfsh_traces_functions(fp, symtab, symnum, selfuncs);
  count += elfsh_traces_functions(fp, dynsym, dynsymnum, selfuncs + count);
  fclose(fp);

#if __DEBUG_TRACES__
  printf("[DEBUG TRACE] TRACED FUNCTIONS COUNT = %d\n", count);
#endif

  // If we didn't trace a function, we don't need to continue
  if (!count)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No function found", (-1));
  
#if __DEBUG_TRACES__
  printf("[DEBUG TRACE] RENAME %s to %s\n", tfname, rtfname);
#endif

  // Compile the tmp c file to create a relocatable file to inject
  if (rename(tfname, rtfname) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Rename failed", (-1));

  snprintf(buf, BUFSIZ, "gcc -c %s -o %s", rtfname, rsofname);
  vm_system(buf);

  // Load the new relocatable file for ET_REL injection
  tobj = elfsh_map_obj(rsofname);
  if (!tobj)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Failed to load relocatable file", (-1));

  idx = elfsh_inject_etrel(world.curjob->current, tobj);	  
  if (idx < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Failed to inject ET_REL with workspace", (-1));

  // Hijack functions with the new functions injected 
  for (index = 0; index < count; index++)
  {
    snprintf(buf, BUFSIZ, "%s_trace", selfuncs[index]);	  

    // Retrieve symbol
    dst = elfsh_get_symbol_by_name(world.curjob->current, buf);
    if (dst == NULL)
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Failed to find trace function", (-1));
    addr = dst->st_value;

#if __DEBUG_TRACES__
    printf("[DEBUG TRACE] (%03u) HIJACK: %s / %s\n", index, selfuncs[index], buf);
#endif

    // Start to hijack the function 
    err = elfsh_hijack_function_by_name(world.curjob->current, 
					ELFSH_HIJACK_TYPE_FLOW,
					selfuncs[index], 
					addr, NULL);
    if (err < 0)
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Failed to hijack a function", (-1));
  }
  
  vm_output("\n [*] Binary ready to be traced. You can now save and run it.\n\n");

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
*/

hash_t			*elfsh_traces_createtrace(char *trace)
{
  hash_t		*newhash;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

  if (!trace)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", NULL);    

  /* Create a new trace tracename */
  snprintf(buf, BUFSIZ - 1, ELFSH_TRACES_PATTERN, trace);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, newhash, sizeof(hash_t), NULL);
  hash_init(newhash, strdup(buf), 30, ASPECT_TYPE_UNKNOW);

  hash_add(&traces_table, strdup(trace), (void *) newhash);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newhash);
}

int			*elfsh_traces_inittrace()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

  /* Do we already set default table */
  if (traces_table.ent == NULL)
    {
      hash_init(&traces_table, ELFSH_TRACES_TABLE_NAME, 30, ASPECT_TYPE_UNKNOW);
      elfsh_traces_createtrace(ELFSH_TRACES_TYPE_DEFAULT);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

hash_t			*elfsh_traces_gettrace(char *trace)
{
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

  if (!trace)
    trace = ELFSH_TRACES_TYPE_DEFAULT;

  table = hash_get(&traces_table, trace);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, table);
}

elfshtraces_t 		*elfsh_traces_funcadd(char *trace, char *name, elfshobj_t *file)
{
  hash_t		*table;
  elfshtraces_t 		*newtrace;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  table = elfsh_traces_gettrace(trace);

  if (!table)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		  "Trace table not found", NULL);

  if (hash_get(table, name) != NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		  "Function already exist", NULL);

  XALLOC(__FILE__, __FUNCTION__, __LINE__, newtrace, sizeof(elfshtraces_t), NULL);
  newtrace->funcname = strdup(name);
  newtrace->file = file;
  newtrace->enable = 1;

  hash_add(table, newtrace->funcname, (void *) newtrace);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newtrace);
}

int			elfsh_traces_funcrm(char *trace, char *name)
{
  elfshtraces_t		*ret_trace;
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  table = elfsh_traces_gettrace(trace);

  if (!table)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Trace table not found", -1);

  ret_trace = hash_get(table, name);

  if (ret_trace)
    {
      hash_del(table, name);
      XFREE(__FILE__, __FUNCTION__, __LINE__, ret_trace);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int			elfsh_traces_funcenable(char *trace, char *name)
{
  elfshtraces_t		*ret_trace;
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  table = elfsh_traces_gettrace(trace);

  if (!table)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		  "Trace table not found", -1);

  ret_trace = hash_get(table, name);

  if (!ret_trace)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Function not found", -1);

  ret_trace->enable = 1;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int			elfsh_traces_funcsetstatus(hash_t *table, int status)
{
  u_int			index;
  int			keynbr;
  char			**keys;
  elfshtraces_t		*ret_trace;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!table)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  keys = hash_get_keys(table, &keynbr);

  if (keys)
    {
      for (index = 0; index < keynbr; index++)
	{
	  ret_trace = (elfshtraces_t *) hash_get(table, keys[index]);
	  
	  if (ret_trace)
	    ret_trace->enable = status;
	}

      hash_free_keys(keys);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int			elfsh_traces_funcenableall(char *trace)
{
  u_int			index;
  int			keynbr;
  char			**keys;
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!trace)
    trace = ELFSH_TRACES_TYPE_DEFAULT;

  keys = hash_get_keys(&traces_table, &keynbr);

  if (keys)
    {
      for (index = 0; index < keynbr; index++)
	{
	  table = (hash_t *) hash_get(&traces_table, keys[index]);
	  
	  if (table)
	    elfsh_traces_funcsetstatus(table, 1);
	}

      hash_free_keys(keys);
    }  

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int			elfsh_traces_funcdisableall(char *trace)
{
  u_int			index;
  int			keynbr;
  char			**keys;
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!trace)
    trace = ELFSH_TRACES_TYPE_DEFAULT;

  keys = hash_get_keys(&traces_table, &keynbr);

  if (keys)
    {
      for (index = 0; index < keynbr; index++)
	{
	  table = (hash_t *) hash_get(&traces_table, keys[index]);
	  
	  if (table)
	    elfsh_traces_funcsetstatus(table, 0);
	}

      hash_free_keys(keys);
    }  

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int			elfsh_traces_funcdisable(char *trace, char *name)
{
  elfshtraces_t		*ret_trace;
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  table = elfsh_traces_gettrace(trace);

  if (!table)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		  "Trace table not found", -1);

  ret_trace = hash_get(table, name);

  if (!ret_trace)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Function not found", -1);

  ret_trace->enable = 0;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int			elfsh_traces_funcrmall(char *trace)
{
  char			**keys;
  u_int			index;
  int			keynbr;
  elfshtraces_t		*ret_trace;
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  table = elfsh_traces_gettrace(trace);

  if (!table)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Trace table not found", -1);

  keys = hash_get_keys(table, &keynbr);

  if (keys)
    {
      for (index = 0; index < keynbr; index++)
	{
	  ret_trace = (elfshtraces_t *) hash_get(table, keys[index]);
	  hash_del(table, keys[index]);

	  if (ret_trace)
	    XFREE(__FILE__, __FUNCTION__, __LINE__, ret_trace);
	}

      hash_free_keys(keys);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int			elfsh_traces_deletetrace(char *trace)
{
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  elfsh_traces_funcrmall(trace);

  table = elfsh_traces_gettrace(trace);
  hash_destroy(table);

  hash_del(&traces_table, trace);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
