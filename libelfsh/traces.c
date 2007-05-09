/*
** traces.c for libelfsh
** 
** All functions that help to trace content
** 
** Started Jul 2 2005 00:03:44 mxatone
** 
**
** $Id: traces.c,v 1.12 2007-05-09 21:40:42 mxatone Exp $
**
*/
#include "libelfsh.h"

#define TRACES_CFLOW 	1
#define TRACES_PLT 	2
#define TRACES_GOT 	3

char buf[BUFSIZ];

#define TRACE_USED_longjmp		0
#define TRACE_USED__setjmp		1
#define TRACE_USED_signal		2
#define TRACE_USED_snprintf		3
#define TRACE_USED_memset		4
#define TRACE_USED_gettimeofday		5
#define TRACE_USED_write		6
#define TRACE_USED_MAX			7

#define TRACE_UNTRACABLE_NAME "untracable"

typedef struct _trace_used
{
  char *name;
  u_char exist;
} trace_used, *ptrace_used;

/* Untracable table */
hash_t traces_untracable;
u_char untracable_ostype;

#define TRACE_FUNCTIONS_ADD(_name) \
{ #_name, 0 }

#define FUNC_BEGIN(_name) \
(trace_functions[TRACE_USED_##_name].exist ? "old_" : "")

trace_used trace_functions[] = 
  { 
    TRACE_FUNCTIONS_ADD(longjmp),
    TRACE_FUNCTIONS_ADD(_setjmp),
    TRACE_FUNCTIONS_ADD(signal),
    TRACE_FUNCTIONS_ADD(snprintf),
    TRACE_FUNCTIONS_ADD(memset),
    TRACE_FUNCTIONS_ADD(gettimeofday),
    TRACE_FUNCTIONS_ADD(write),
    { "", 0 }
  };

/**
 * Store every traces, this table store another hash table for each key 
 */
hash_t traces_table;

/**
 * Whole active elements 
 */
int trace_enabled_count = 0;

#define ELFSH_TRACES_TABLE_NAME "elfsh_traces_table"
#define ELFSH_TRACES_PATTERN "traces_%s"

/**
 * Check every function used on the new module if there already exist or not
 * @param file host file
 */
static int		elfsh_check_trace_functions(elfshobj_t *file)
{
  u_int			index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

  /* Each used functions */
  for (index = 0; index < TRACE_USED_MAX; index++)
    {
      /* Check on dynsym table */
      if (elfsh_get_dynsymbol_by_name(file, trace_functions[index].name))
	{
	  trace_functions[index].exist = 1;
	}
      else
	{
	  trace_functions[index].exist = 0;
	}
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Create a new trace 
 * @param trace trace name
 * @return trace hash table
 */
hash_t			*elfsh_traces_createtrace(char *trace)
{
  hash_t		*newhash;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

  if (!trace)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", NULL);

  elfsh_traces_inittrace();

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

/**
 * The queue is used to store function that will be traced.
 * The aglorithm is two differents function then I don't wanna
 * recheck which function must be traced.
 * @param elm function to queue
 * @see elfsh_traces_save_table
 * @see elfsh_traces_save
 */
static int		elfsh_traces_queue_add(elfshtraces_t *elm)
{
  u_int			index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (trace_queue == NULL)
    {
      queue_step = ELFSH_TRACES_DEFAULT_STEP;
      XALLOC(__FILE__, __FUNCTION__, __LINE__, trace_queue, 
	     sizeof(elfshtraces_t)*queue_step, -1);
    }
  else
    {
      /* Check if we already have it */
      for (index = 0; index < queue_step && trace_queue[index]; index++)
	{
	  if (!strcmp(elm->funcname, trace_queue[index]->funcname))
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			 "Already added", -1);
	}
      
      queue_step += ELFSH_TRACES_DEFAULT_STEP;
      XREALLOC(__FILE__, __FUNCTION__, __LINE__, trace_queue, trace_queue,
	     sizeof(elfshtraces_t)*queue_step, -1);
    }

  trace_queue[queue_count++] = elm;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Clean the queue if we setup one 
 */
static int		elfsh_traces_queue_clean()
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
char		bufex[BUFSIZ];

/**
 * Generate each function on the file 
 * @param fp file pointer
 * @param file object (target)
 * @param table a trace hash table
 */
static int		elfsh_traces_save_table(FILE *fp, elfshobj_t *file, hash_t *table)
{
  int			z = 0;
  u_int			index;
  int			keynbr;
  char			**keys;
  elfshtraces_t		*ret_trace;
  char			*start;
  int			ret;

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
	      ret = elfsh_traces_queue_add(ret_trace);

	      /* Allocation failed */
	      if (ret == -1)
		break;

	      /* Already handled */
	      if (ret == -2)
		continue;

	      snprintf(bufex, BUFSIZ - 1, "int %s_trace(", ret_trace->funcname);
	      fwrite(bufex, strlen(bufex), sizeof(char), fp);

	      /* Function arguments */
	      if (ret_trace->argc > 0)
		{
		  for (z = 0; z < ret_trace->argc && z < ELFSH_TRACES_MAX_ARGS; z++)
		    {
		      start = z == 0 ? "" : ", ";

		      snprintf(bufex, BUFSIZ - 1, "%sARG(%d) a%d", 
			       start, 
			       ret_trace->arguments[z].size, z);
		      fwrite(bufex, strlen(bufex), sizeof(char), fp);
		    }
		}

	      snprintf(bufex, BUFSIZ - 1, 
		       ")\n{\n"
		       "\tint ret;\n\tunsigned char readable, isstring;\n"
		       "\tstruct timeval before, after;\n"
		       "\tgettrace_time();\n"
		       "\tT_snprintf(logbuf, LOGBUFSIZ - 1, \"%%s + %s(\", pad_print(1));\n"
		       "\tWRITENOW();\n",
		       ret_trace->funcname);
	      fwrite(bufex, strlen(bufex), sizeof(char), fp);

	      /* Printf arguments */
	      if (ret_trace->argc > 0)
		{
		  for (z = 0; z < ret_trace->argc && z < ELFSH_TRACES_MAX_ARGS; z++)
		    {
		      start = z == 0 ? "" : ", ";
		      
		      snprintf(bufex, BUFSIZ - 1, 
			       "\tisstring = 0;\n"
			       "\treadable = check_read_ptr((void *)a%d.ptr, MAX_CHECK_CHAR+1);\n"
			       "\tif (readable)\n\t\tisstring = is_string((void *)a%d.ptr);\n"
			       "\tT_snprintf(logbuf, LOGBUFSIZ - 1, \"%s", z, z, start);
		      fwrite(bufex, strlen(bufex), sizeof(char), fp);
		      
		      if (ret_trace->type == ELFSH_ARG_TYPE_BASED)
			{
			  snprintf(bufex, BUFSIZ - 1, "%s %s: ", 
				   ret_trace->arguments[z].typename,
				   ret_trace->arguments[z].name);
			  fwrite(bufex, strlen(bufex), sizeof(char), fp);
			}

		      snprintf(bufex, BUFSIZ - 1, 
			       "%%s0x%%x%%s%%s%%s\", "
			       "readable ? \"*\" : \"\", "
			       "(void*)a%d.ptr, "
			       "isstring ? \" \\\"\" : \"\", "
			       "(isstring ? (char*)a%d.ptr : (readable ? "
			       "hex_to_str((void *)a%d.ptr) : \"\")), "
			       "isstring ? \"\\\"\" : \"\");\n"
			       "\tWRITENOW();\n", 
			       z, z, z);
		      fwrite(bufex, strlen(bufex), sizeof(char), fp);
		    }
		}
	      
	      snprintf(bufex, BUFSIZ - 1, 
		       "\tT_snprintf(logbuf, LOGBUFSIZ - 1, \")\\n%%s\", \"\");\n"
		       "\tWRITENOW();\n"
		       "\tT_gettimeofday(&before, NULL);\n"
		       "\tret = old_%s(", ret_trace->funcname);
	      fwrite(bufex, strlen(bufex), sizeof(char), fp);

	      /* Send arguments */
	      if (ret_trace->argc > 0)
		{
		  for (z = 0; z < ret_trace->argc && z < ELFSH_TRACES_MAX_ARGS; z++)
		    {
		      start = z == 0 ? "" : ", ";
		      
		      snprintf(bufex, BUFSIZ - 1, "%sa%d", start, z);
		      fwrite(bufex, strlen(bufex), sizeof(char), fp);
		    }
		}

	      snprintf(bufex, BUFSIZ - 1,
		       ");\n"
		       "\tT_gettimeofday(&after, NULL);\n"
		       "\tisstring = 0;\n"
		       "\treadable = check_read_ptr((void *)ret, MAX_CHECK_CHAR+1);\n"
		       "\tif (readable)\n\t\tisstring = is_string((void *)ret);\n"
		       "\tgettrace_time();\n"
		       "\tT_snprintf(logbuf, LOGBUFSIZ -1, \"%%s - %s = \", "
		       "pad_print(0));\n"
		       "\tWRITENOW();\n"
		       "\tT_snprintf(logbuf, LOGBUFSIZ -1, \"%%s0x%%x%%s%%s%%s "
		       "[time spent=%%d.%%06d]\\n\", "
		       "readable ? \"*\" : \"\", "
		       "(void*)ret, "
		       "isstring ? \" \\\"\" : \"\", "
		       "(isstring ? (char*)ret : (readable ? "
		       "hex_to_str((void *)ret) : \"\")), "
		       "isstring ? \"\\\"\" : \"\", after.tv_sec - before.tv_sec, "
		       "after.tv_usec - before.tv_usec);\n"
		       "\tWRITENOW();\n"
		       "\treturn ret;\n}\n",
		       ret_trace->funcname);
	      fwrite(bufex, strlen(bufex), sizeof(char), fp);
	    }
	}
      
      hash_free_keys(keys);
    }  

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Proceed tracing elements on the file during save cmd 
 * @param file target file that is a copy of the original
 * @see cmd_save
 */
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

  /* Check every used functions */
  elfsh_check_trace_functions(file);

  // Write basic stuff, include headers and
  //   pad functions (to have a tree like form)
  snprintf(buf, BUFSIZ - 1, 
	   "#include <stdio.h>\n"
	   "#include <setjmp.h>\n"
	   "#include <signal.h>\n"
	   "#include <string.h>\n"
	   "#include <stdlib.h>\n"
	   "#include <unistd.h>\n"
	   "#include <sys/time.h>\n"
	   "#include <sys/resource.h>\n\n"
	   "#define T_longjmp %slongjmp\n"
	   "#define T_setjmp %s_setjmp\n"
	   "#define T_signal %ssignal\n"
	   "#define T_snprintf %ssnprintf\n"
	   "#define T_memset %smemset\n"
	   "#define T_gettimeofday %sgettimeofday\n"
	   "#define T_write %swrite\n"
	   "#define MAX_CHECK_CHAR 3\n"
	   "#define PRINTABLE(c) (c >= 32 && c <= 126)\n"
	   "#define STDERR 2\n"
	   "#define ARG(_size) trace_arg_##_size\n"
	   "#define WRITENOW() T_write(STDERR, logbuf, strlen(logbuf)+1)\n"
	   "jmp_buf jBuf;\n"
	   "#define LOGBUFSIZ 256\n"
	   "char logbuf[LOGBUFSIZ];\n"
	   "suseconds_t msec;\n"
	   "time_t sec;\n"
	   "unsigned char is_string(const void *ptr)\n{\n"
	   "\tint count;\n"
	   "\tchar *cptr = (char*) ptr;\n\n"
	   "\tfor (count = 0; PRINTABLE(cptr[count]) && cptr[count] != 0x00; count++)\n\t{\n"
	   "\t\tif (count >= MAX_CHECK_CHAR)\n"
	   "\t\t\treturn 1;\n"
	   "\t}\n\n"
	   "\treturn 0;\n"
	   "}\n\n"
	   "void check_ptr_failed(int nSig)\n{\n"
	   "\tlongjmp(jBuf, 1);\n"
	   "}\n\n"
	   "unsigned char check_read_ptr(const void *ptr, unsigned long size)\n{\n"
	   "\tint index;\n"
	   "\tchar elem;\n"
	   "\tvoid (*prev_sig) (int sig);\n\n"
	   "\tif (!ptr || size <= 0 || ptr < (void*)0x5000)\n"
	   "\t\treturn 0;\n\n"
	   "\tif(T_setjmp(jBuf))\n"
	   "\t\treturn 0;\n\n"
	   "\tprev_sig = T_signal(SIGSEGV, check_ptr_failed);\n\n"
	   "\tfor (index = 0; index < size; index++)\n"
	   "\t\telem = ((char *)ptr)[index];\n\n"
	   "\tT_signal(SIGSEGV, prev_sig);\n"
	   "\treturn 1;\n"
	   "}\n\n"
	   "char tmpbuf[256];\n"
	   "char *hex_to_str(const void *ptr)\n{\n"
	   "\tint count;\n"
	   "\tchar *cptr = (char*) ptr;\n\n"
	   "\tT_snprintf(tmpbuf, 255, \" = 0x%%02x 0x%%02x 0x%%02x 0x%%02x\", "
	   "cptr[0] & 0xFF, cptr[1] & 0xFF, cptr[2] & 0xFF, cptr[3] & 0xFF);\n"
	   "\treturn tmpbuf;\n"
	   "}\n\n"
	   "char pad_str[64];\n"
	   "int pad_count = 0;\n\n"
	   "char *pad_print(int inc)\n{\n"
	   "\tint i;\n"
	   "\tif (inc == 0)\n\t\tpad_count -= 2;\n"
	   "\t\tT_memset(pad_str, ' ', pad_count);\n"
	   "\t\tpad_str[pad_count] = 0;\n"
	   "\tif (inc == 1)\n\t\tpad_count += 2;\n"
	   "\treturn pad_str;\n}\n\n"
	   "int setup = 0;\n"
	   "void gettrace_time()\n{\n"
	   "\tdouble difftime = 0;\n"
	   "\tstruct timeval now;\n"
	   "\tT_gettimeofday(&now, NULL);\n"
	   "\tif (setup == 0)\n\t{\n"
	   "\t\tmsec = now.tv_usec;\n"
	   "\t\tsec = now.tv_sec;\n"
	   "\t}\n"
	   "\tT_snprintf(logbuf, LOGBUFSIZ - 1, \"%%4d.%%06d \", now.tv_sec - sec, now.tv_usec - msec);\n"
	   "\tWRITENOW();\n"
	   "\tif (setup)\n\t{\n"
	   "\t\tmsec = now.tv_usec;\n"
	   "\t\tsec = now.tv_sec;\n"
	   "\t}\n"
	   "\tsetup = 1;\n"
	   "}\n",
	   FUNC_BEGIN(longjmp),
	   FUNC_BEGIN(_setjmp),
	   FUNC_BEGIN(signal),
	   FUNC_BEGIN(snprintf),
	   FUNC_BEGIN(memset),
	   FUNC_BEGIN(gettimeofday),
	   FUNC_BEGIN(write));
  fwrite(buf, strlen(buf), sizeof(char), fp);

  /* Setup structures */
  for (index = sizeof(elfsh_Addr); index < 255; index++)
    {
      if (index > sizeof(elfsh_Addr))
	{
	  snprintf(buf, BUFSIZ - 1, 
		   "typedef struct trace_s_%1$d { void *ptr; char elm[%2$d]; } trace_arg_%1$d;\n",
		   index, index - sizeof(elfsh_Addr));
	}
      else
	{
	  snprintf(buf, BUFSIZ - 1, 
		   "\ntypedef struct trace_s_%1$d { void *ptr; } trace_arg_%1$d;\n",
		   index);
	}
      fwrite(buf, strlen(buf), sizeof(char), fp);
    }
  
  /* Add a new line */
  fwrite("\n", 1, sizeof(char), fp);

  /* Iterate */
  for (index = 0; index < keynbr; index++)
    {
      table = hash_get(&traces_table, keys[index]);
      
      if (table)
	elfsh_traces_save_table(fp, file, table);
    }

  hash_free_keys(keys);
  fclose(fp);

  if (queue_count == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "No functions found", (-1));

  // Compile the tmp c file to create a relocatable file to inject
  if (rename(tfname, rtfname) < 0)
    {
      elfsh_traces_queue_clean();
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
      elfsh_traces_queue_clean();
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
	elfsh_traces_queue_clean();
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
	elfsh_traces_queue_clean();
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Failed to hijack a function", -1);
      }
  }

  elfsh_traces_queue_clean();

  /* Save procedure already relocate but we made some modifications too
     then we restart this procedure.
   */
  if (elfsh_save_relocate(file) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Failed to relocate traced binary", -1);

  /* Ask for rewrite the SHT */
  file->hdr->e_shoff = 0;
  
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

/**
 * Retreive a trace from its name 
 * @param trace trace name
 * @return trace hash table
 */
hash_t			*elfsh_traces_gettrace(char *trace)
{
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

  elfsh_traces_inittrace();

  /* Default trace by default */
  if (!trace)
    trace = ELFSH_TRACES_TYPE_DEFAULT;

  table = hash_get(&traces_table, trace);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, table);
}

/**
 * Search a symbol on dependences file 
 * @param file parent file
 * @param name symbol name
 * @return file object that containt the symbol (on dynsym)
 */
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

static int		elfsh_traces_add_untracable(char *name)
{
  NOPROFILER_IN();

  hash_add(&traces_untracable, strdup(name), (void *)1);

  NOPROFILER_ROUT(0);
}

static int		elfsh_traces_untracable(elfshobj_t *file, char *name)
{
  u_char		ostype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ostype = elfsh_get_ostype(file);

  /* Init the table if needed */
  if (traces_untracable.ent == NULL || ostype != untracable_ostype)
    {
      if (traces_untracable.ent == NULL)
	hash_empty(TRACE_UNTRACABLE_NAME);

      hash_init(&traces_untracable, TRACE_UNTRACABLE_NAME, 11, ASPECT_TYPE_UNKNOW);
      
      if (ostype == ELFSH_OS_LINUX)
	{
	  elfsh_traces_add_untracable("__libc_start_main");
	  elfsh_traces_add_untracable("_start");
	}

      if (ostype == ELFSH_OS_FREEBSD)
	{
	  // x86 failed on this function
	  // TODO: correct them
	  elfsh_traces_add_untracable("getcwd");
	  elfsh_traces_add_untracable("printf");
	}

      untracable_ostype = ostype;
    }

  if (hash_get(&traces_untracable, name) != NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
}

/**
 * Can we trace this symbol ? 
 * @param file target file
 * @param name function name
 * @param symtab symbol table (symtab or dynsym)
 * @param num number of symbol on the table
 * @param dynsym 0 = symtab 1 = dynsym (internal/external)
 * @param vaddr fill symbol virtual address
 * @see elfsh_traces_tracable
 */
static int		elfsh_traces_tracable_sym(elfshobj_t *file, char *name, elfsh_Sym *symtab,
						  int num, u_char dynsym, elfsh_Addr *vaddr)
{
  u_int			index;
  elfshsect_t		*sect;
  char			*sect_name;
  char			*func_name;
  u_char		bind;

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

      // FIXME: A function name containing "." will not be compiled right with gcc 
      if (strstr(func_name, "."))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "We didn't support function name with a '.'", -1);

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
 * Check if a function can be traced or not 
 * @param file target file
 * @param name function name
 * @param vaddr pointer where save the function virtual address
 * @param external pointer where save the status internal / external of the function
 * @see elfsh_traces_tracable_sym
 */
int 			elfsh_traces_tracable(elfshobj_t *file, char *name,
					      elfsh_Addr *vaddr, u_char *external)
{
  elfsh_Sym		*symtab, *dynsym;
  int			symnum = 0, dynsymnum = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Some function can't be traced */
  if (elfsh_traces_untracable(file, name) == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Untracable function", -2);    

  /* Retrieve symbol tables pointer / number */
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

/**
 * Add the function on the trace table 
 * @param trace trace name
 * @param name function name
 * @param newtrace allocated structure that describe the trace
 * @return newtrace pointer or NULL in case of failure
 */
elfshtraces_t 		*elfsh_traces_funcadd(char *trace, 
					      char *name,
					      elfshtraces_t *newtrace)
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

/**
 * Delete the function from the trace table 
 * @param trace trace name
 * @param name function name
 */
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

/**
 * Enable the function from the trace table 
 * @param trace trace name
 * @param name function name
 */
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

/**
 * Change the status of a whole trace 
 * @param table trace hash table
 * @param status new status (0 = disable, 1 = enable)
 * @see elfsh_traces_funcenableall
 * @see elfsh_traces_funcdisableall
 */
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

  elfsh_traces_inittrace();

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

/**
 * Enable all functions of a trace 
 * @param trace trace name
 * @see elfsh_traces_funcsetstatus
 */
int			elfsh_traces_funcenableall(char *trace)
{
  u_int			index;
  int			keynbr;
  char			**keys;
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!trace)
    trace = ELFSH_TRACES_TYPE_DEFAULT;

  elfsh_traces_inittrace();

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

/* Disable all functions of a trace 
 * @param trace trace name 
 * @see elfsh_traces_funcsettatus
 */
int			elfsh_traces_funcdisableall(char *trace)
{
  u_int			index;
  int			keynbr;
  char			**keys;
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!trace)
    trace = ELFSH_TRACES_TYPE_DEFAULT;

  elfsh_traces_inittrace();

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

/**
 * Disable a functions of a trace 
 * @param trace trace name
 * @param name function name
 */
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

/**
 * Delete all functions of a trace 
 * @param trace trace name
 */
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

/**
 * Delete a trace 
 * @param trace trace name
 */
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
