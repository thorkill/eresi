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

/* Store every traces, this table store another hash table for each key */
hash_t traces_table;

/* Whole active elements */
int trace_enabled_count = 0;

#define ELFSH_TRACES_TABLE_NAME "elfsh_traces_table"
#define ELFSH_TRACES_PATTERN "traces_%s"

/* Create a new trace */
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

/* Queue pointers */
elfshtraces_t **trace_queue = NULL;
int queue_count = 0;
#define ELFSH_TRACES_DEFAULT_STEP 20
int queue_step = 0;

/* The queue is used to store function that will be traced.
   The aglorithm is two differents function then I don't wanna
   recheck which function must be traced.
 */
static int		elfsh_trace_queue_add(elfshtraces_t *elm)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (trace_queue == NULL)
    {
      queue_step = ELFSH_TRACES_DEFAULT_STEP;
      XALLOC(__FILE__, __FUNCTION__, __LINE__, trace_queue, 
	     sizeof(elfshtraces_t)*queue_step, -1);
    }
  else
    {
      queue_step += ELFSH_TRACES_DEFAULT_STEP;
      XREALLOC(__FILE__, __FUNCTION__, __LINE__, trace_queue, trace_queue,
	     sizeof(elfshtraces_t)*queue_step, -1);
    }

  trace_queue[queue_count++] = elm;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Clean the queue if we setup one */
static int		elfsh_trace_queue_clean()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (trace_queue)
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__, trace_queue);
      trace_queue = NULL;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* This buffers are used to generate the file */
char		buf[BUFSIZ];
char		bufex[BUFSIZ];
char		args[BUFSIZ];
char		argsproto[BUFSIZ];
char		argshexa[BUFSIZ];

/* Generate each function on the file */
static int		elfsh_trace_save_table(FILE *fp, elfshobj_t *file, hash_t *table)
{
  int			z = 0;
  u_int			index;
  int			keynbr;
  char			**keys;
  elfshtraces_t		*ret_trace;
  char			*start;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  keys = hash_get_keys(table, &keynbr);

  if (keys)
    {
      for (index = 0; index < keynbr; index++)
	{
	  ret_trace = hash_get(table, keys[index]);

	  if (ret_trace && ret_trace->enable && ret_trace->file->id == file->id)
	    {
	      /* Add in the queue */
	      elfsh_trace_queue_add(ret_trace);

	      argshexa[0] = 0;
	      argsproto[0] = 0;
	      args[0] = 0;

	      // Do we have arguments in this function ? 
	      if (ret_trace->argc > 0)
		{	
		  // Arguments by arguments 
		  for (z = 0; z < ret_trace->argc && z < ELFSH_TRACES_MAX_ARGS; z++)
		    {
		      start = z == 0 ? "" : ", ";

		      // char type is just what we need to setup a good size 
		      snprintf(buf, BUFSIZ - 1, "%schar a%d[%d]", 
			       start, z, 
			       ret_trace->arguments[z].size);

		      if (strlen(argsproto) + strlen(buf) + 1 > BUFSIZ)
			break;

		      strncat(argsproto, buf, strlen(buf));

		      snprintf(buf, BUFSIZ - 1, "%sa%d", start, z);

		      if (strlen(args) + strlen(buf) + 1 > BUFSIZ)
			break;

		      strncat(args, buf, strlen(buf));

		      if (ret_trace->type == ELFSH_ARG_TYPE_BASED)
			snprintf(buf, BUFSIZ - 1, "%s%s %s: 0x%%%%%s", 
				 start, ret_trace->arguments[z].typename,
				 ret_trace->arguments[z].name, "x");
		      else
			snprintf(buf, BUFSIZ - 1, "%s0x%%%%%s", start, "x");

		      if (strlen(argshexa) + strlen(buf) + 1 > BUFSIZ)
			break;

		      strncat(argshexa, buf, strlen(buf));
		    }
		}

	      // Tracing function with a human readable form
	      snprintf(bufex, BUFSIZ - 1, "int %%1$s_trace(%%2$s)\n{\n"
		       "\tint ret;\n"
		       "\tprintf(\"%%%%s + %%1$s(%s)\\n\", pad_print(1)%s%s);\n"
		       "\tret = old_%%1$s(%%3$s);\n"
		       "\tprintf(\"%%%%s - %%1$s = %%%%x\\n\", pad_print(0), ret);\n"
		       "\treturn ret;\n}\n",
		       argshexa, (strlen(args) > 0 ? "," : ""), args);

	      snprintf(buf, BUFSIZ - 1, bufex, 
		       ret_trace->funcname, argsproto, args);

	      fwrite(buf, strlen(buf), sizeof(char), fp);	      
	    }
	}
      
      hash_free_keys(keys);
    }  

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Proceed tracing elements on the file during save cmd */
int			elfsh_traces_save(elfshobj_t *file)
{
  u_int			index;
  int			keynbr;
  char			**keys;
  hash_t		*table;
  FILE			*fp;
  char 			tfname[] = "/tmp/tracingXXXXXX";
  int			osize = strlen(tfname) + 3;
  char			rsofname[50];
  char			rtfname[50];
  elfshobj_t		*tobj;
  int			idx;
  int			rs = 0, fd;
  struct stat		s;
  elfsh_Addr		addr;
  elfsh_Sym		*dst;
  int			err;
  char			*system[6];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Do we will have something to trace ? */
  if (trace_enabled_count <= 0)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  keys = hash_get_keys(&traces_table, &keynbr);

  if (keys == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Can't get keys and I should", -1);

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

  /* Iterate */
  for (index = 0; index < keynbr; index++)
    {
      table = hash_get(&traces_table, keys[index]);
      
      if (table)
	elfsh_trace_save_table(fp, file, table);
    }

  hash_free_keys(keys);
  fclose(fp);

  if (queue_count == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "No functions found", (-1));

  // Compile the tmp c file to create a relocatable file to inject
  if (rename(tfname, rtfname) < 0)
    {
      elfsh_trace_queue_clean();
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Rename failed", (-1));
    }

  /* Simple array for execvp */
  system[0] = "gcc";
  system[1] = "-c";
  system[2] = rtfname;
  system[3] = "-o";
  system[4] = rsofname;
  system[5] = NULL;

  err = 0;
  if (!fork())
    err = execvp(system[0], system);
  else
    wait(NULL); /* Wait gcc */

  if (err < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Compilation failed", -1);

  // Load the new relocatable file for ET_REL injection
  tobj = elfsh_map_obj(rsofname);
  if (!tobj)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Failed to load relocatable file", -1);

  /* Inject the loaded file */
  idx = elfsh_inject_etrel(file, tobj);
  if (idx < 0)
    {
      elfsh_trace_queue_clean();
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Failed to inject ET_REL with workspace", -1);
    }

  // Hijack functions with the new functions injected 
  for (index = 0; index < queue_count && trace_queue[index]; index++)
  {
    snprintf(buf, BUFSIZ, "%s_trace", trace_queue[index]->funcname);	  

    // Retrieve symbol
    dst = elfsh_get_symbol_by_name(file, buf);
    if (dst == NULL)
      {
	elfsh_trace_queue_clean();
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Failed to find trace function", -1);
      }

    addr = dst->st_value;

#if __DEBUG_TRACES__
    printf("[DEBUG TRACE] (%03u) HIJACK: %s / %s\n", index, trace_queue[index]->funcname, buf);
#endif

    // Start to hijack the function 
    err = elfsh_hijack_function_by_name(file, 
					ELFSH_HIJACK_TYPE_FLOW,
					trace_queue[index]->funcname, 
					addr, NULL);
    if (err < 0)
      {
	elfsh_trace_queue_clean();
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Failed to hijack a function", -1);
      }
  }

  elfsh_trace_queue_clean();

  /* Save procedure already relocate but we made some modifications too
     then we restart this procedure.
   */
  if (elfsh_save_relocate(file) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Failed to relocate traced binary", -1);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Init the table and add the default trace (global) */
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

/* Retreive a trace from its name */
hash_t			*elfsh_traces_gettrace(char *trace)
{
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

  /* Default trace by default */
  if (!trace)
    trace = ELFSH_TRACES_TYPE_DEFAULT;

  table = hash_get(&traces_table, trace);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, table);
}

/* Search a symbol on dependences file */
elfshobj_t   		*elfsh_traces_search_sym(elfshobj_t *file, char *name)
{
  char			**keys;
  int			keynbr;
  u_int			index;
  elfshobj_t		*child, *find = NULL;
  elfsh_Sym		*sym;
  elfshsect_t		*sect;
  char			*sect_name;
  u_int			bindnum;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 
  
  keys = hash_get_keys(&(file->child_hash), &keynbr);

  if (keys)
    {
      for (index = 0; index < keynbr; index++)
	{
	  child = (elfshobj_t *) hash_get(&(file->child_hash), keys[index]);

	  if (child)
	    {
	      sym = elfsh_get_dynsymbol_by_name(child, name);

	      if (!sym)
		continue;

	      bindnum = elfsh_get_symbol_bind(sym);
	      
	      /* We need a global or weak symbol */
	      if (bindnum == STB_GLOBAL || bindnum == STB_WEAK)
		{
		  sect = elfsh_get_section_by_index(child, sym->st_shndx, NULL, NULL);

		  /* The symbol must be local and pointing on sect_name */
		  if (sect && (sect_name = elfsh_get_section_name(child, sect)) != NULL
		      && !strncmp(sect_name, ".text", 5))
		    {
		      find = child;
		      break;
		    }
		}
	    }
	  
	  /* Search deeper on this child */
	  find = elfsh_traces_search_sym(child, name);
	  
	  if (find)
	    break;
	}
    }
  
  hash_free_keys(keys);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, find);
}

/* Can we trace this symbol ? */
static int		elfsh_traces_tracable_sym(elfshobj_t *file, char *name, elfsh_Sym *symtab,
						  int num, u_char dynsym, elfsh_Addr *vaddr)
{
  u_int			index;
  elfshsect_t		*sect;
  char			*sect_name;
  char			*func_name;

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
      if (elfsh_get_symbol_bind(symtab + index) != STB_GLOBAL
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

      // FIXME: A function name containing "." will not be compiled right with gcc 
      if (strstr(func_name, "."))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "We didn't support function name with a '.'", -1);

      if (vaddr)
	*vaddr = symtab[index].st_value;

      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Function not found", -1);
}

/* Check if a fonction can be traced or not */
int 			elfsh_traces_tracable(elfshobj_t *file, char *name,
					      elfsh_Addr *vaddr, u_char *external)
{
  elfsh_Sym		*symtab, *dynsym;
  int			symnum = 0, dynsymnum = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  symtab = elfsh_get_symtab(file, &symnum);
  dynsym = elfsh_get_dynsymtab(file, &dynsymnum);

  if (symnum + dynsymnum <= 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "No symbols found", -1);

  /* sym */
  if (elfsh_traces_tracable_sym(file, name, symtab, symnum, 0, vaddr) == 0)
    {
      if (external)
	*external = 0;

      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* dynsym */
  if (elfsh_traces_tracable_sym(file, name, dynsym, dynsymnum, 1, vaddr) == 0)
    {
      if (external)
	*external = 1;

      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Not tracable", -1);
}

/* Add the function on the trace table */
elfshtraces_t 		*elfsh_traces_funcadd(char *trace, 
					      char *name, elfshtraces_t *newtrace)
{
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  table = elfsh_traces_gettrace(trace);

  if (!table)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Trace table not found", NULL);

  if (hash_get(table, name) != NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Function already exist", NULL);

  trace_enabled_count++;
  hash_add(table, newtrace->funcname, (void *) newtrace);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newtrace);
}

/* Delete the function from the trace table */
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
      trace_enabled_count--;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Enable the function from the trace table */
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

  if (ret_trace->enable != 1)
    trace_enabled_count++;

  ret_trace->enable = 1;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Change the status of a whole trace */
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
	    {
	      if (ret_trace->enable != status)
		{
		  if (ret_trace->enable == 1)
		    trace_enabled_count--;
		  else
		    trace_enabled_count++;
		}
	      ret_trace->enable = status;
	    }
	}

      hash_free_keys(keys);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Enable all functions of a trace */
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

/* Disable all functions of a trace */
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

/* Disable a functions of a trace */
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

  if (ret_trace->enable != 0)
    trace_enabled_count--;

  ret_trace->enable = 0;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Delete all functions of a trace */
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

/* Delete a trace */
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
