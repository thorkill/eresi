/**
 * @file save.c
** @ingroup libetrace
**
** @brief This file contain trace save function.
**
** $Id: save.c,v 1.2 2007-11-29 10:25:02 rival Exp $
**
*/
#include "libetrace.h"


/**
 * Check if this function name is excluded
 * @param name function name
 */
static int		etrace_check_exclude(char *name)
{
  unsigned int			index;
  int			keynbr;
  char			**keys;
  regex_t		*preg;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (name && exclude_table.ent)
    {
      keys = hash_get_keys(&exclude_table, &keynbr);
      
      if (keys)
	{
	  for (index = 0; index < keynbr; index++)
	    {
	      preg = hash_get(&exclude_table, keys[index]);

	      if (preg)
		{
		  /* We exclude this function */
		  if (regexec(preg, name, 0, 0, 0) == 0)
		    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);
		}
	    }

	  hash_free_keys(keys);
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Check every function used on the new module if there already exist or not
 * @param file host file
 */
static int		elfsh_check_trace_functions(elfshobj_t *file)
{
  unsigned int			index;

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

/* Queue pointers */
trace_t **trace_queue = NULL;
int queue_count = 0;
#define ETRACE_DEFAULT_STEP 20
int queue_step = 0;

/**
 * The queue is used to store function that will be traced.
 * The aglorithm is two differents function then I don't wanna
 * recheck which function must be traced.
 * @param elm function to queue
 * @see etrace_save_tracing_table
 * @see etrace_save_tracing
 */
static int		etrace_queue_add(trace_t *elm)
{
  unsigned int			index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (trace_queue == NULL)
    {
      queue_step = ETRACE_DEFAULT_STEP;
      XALLOC(__FILE__, __FUNCTION__, __LINE__, trace_queue, 
	     sizeof(trace_t)*queue_step, -1);
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
      
      queue_step += ETRACE_DEFAULT_STEP;
      XREALLOC(__FILE__, __FUNCTION__, __LINE__, trace_queue, trace_queue,
	     sizeof(trace_t)*queue_step, -1);
    }

  trace_queue[queue_count++] = elm;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Clean the queue if we setup one 
 */
static int		etrace_queue_clean() 
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (trace_queue)
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__, trace_queue);
      trace_queue = NULL;
      queue_count = 0;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Generate each function on the file 
 * @param fp file pointer
 * @param file object (target)
 * @param table a trace hash table
 */
static int		etrace_save_tracing_table(FILE *fp, elfshobj_t *file, hash_t *table)
{
  int			z = 0;
  unsigned int			index;
  int			keynbr;
  char			**keys;
  trace_t		*ret_trace;
  char			*start;
  int			ret;
  unsigned char		typed;
  char			bufex[BUFSIZ];


  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  keys = hash_get_keys(table, &keynbr);

  if (keys)
    {
      for (index = 0; index < keynbr; index++)
	{
	  ret_trace = hash_get(table, keys[index]);

	  if (ret_trace && ret_trace->enable && ret_trace->file->id == file->id)
	    {
	      /* We have an exclude hash table to check. This way, user has a total control
		 on traced function using etrace for example. */
	      if (etrace_check_exclude(ret_trace->funcname) < 0)
		continue;

	      /* Add in the queue */
	      ret = etrace_queue_add(ret_trace);

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
		  for (z = 0; z < ret_trace->argc && z < ETRACE_MAX_ARGS; z++)
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
		       "\tint ret;\n"
		       "\tstruct timeval before, after;\n"
		       "\tconst char fname[] = \"%s\";\n\n"
		       "\tif (!no_trace)\n\t{\n"
		       "\t\tno_trace = 1;\n"
		       "\t\tfunc_intro(fname);\n",
		       ret_trace->funcname);
	      fwrite(bufex, strlen(bufex), sizeof(char), fp);

	      /* Printf arguments */
	      if (ret_trace->argc > 0)
		{
		  for (z = 0; z < ret_trace->argc && z < ETRACE_MAX_ARGS; z++)
		    {
		      typed = (ret_trace->type == ELFSH_ARG_TYPE_BASED);

		      snprintf(bufex, BUFSIZ - 1, 
			       "\t\tfunc_argu((void *) a%d.ptr, %d, %s%s%s, %s%s%s);\n",
			       z, (z == 0), 
			       typed ? "\"" : "",
			       typed ? ret_trace->arguments[z].typename : "NULL",
			       typed ? "\"" : "",
			       typed ? "\"" : "",
			       typed ? ret_trace->arguments[z].name : "NULL",
			       typed ? "\"" : ""
			       );
		      fwrite(bufex, strlen(bufex), sizeof(char), fp);
		    }
		}
	      
	      snprintf(bufex, BUFSIZ - 1, 
		       "\t\tfunc_conclu();\n"
		       "\t\tT_gettimeofday(&before, NULL);\n"
		       "\t\tno_trace = 0;\n"
		       "\t}\n\tret = old_%s(", 
		       ret_trace->funcname);
	      fwrite(bufex, strlen(bufex), sizeof(char), fp);

	      /* Send arguments */
	      if (ret_trace->argc > 0)
		{
		  for (z = 0; z < ret_trace->argc && z < ETRACE_MAX_ARGS; z++)
		    {
		      start = z == 0 ? "" : ", ";
		      
		      snprintf(bufex, BUFSIZ - 1, "%sa%d", start, z);
		      fwrite(bufex, strlen(bufex), sizeof(char), fp);
		    }
		}

	      snprintf(bufex, BUFSIZ - 1,
		       ");\n"
		       "\tif (!no_trace)\n\t{\n"
		       "\t\tno_trace = 1;\n"
		       "\t\tT_gettimeofday(&after, NULL);\n"
		       "\t\tfunc_retu(fname, (void *) ret, after.tv_sec - before.tv_sec, "
		       "after.tv_usec - before.tv_usec);\n"
		       "\t\tno_trace = 0;\n"
		       "\t}\n"
		       "\treturn ret;\n}\n");
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
int			etrace_save_tracing(elfshobj_t *file)
{
  unsigned int			index;
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
  eresi_Addr		addr;
  elfsh_Sym		*dst;
  int			err;
  char			*system[6];
  char			buf[BUFSIZ];

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
  /*
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
	   "#define T_sigaction %ssigaction\n"
	   "#define T_snprintf %ssnprintf\n"
	   "#define T_memset %smemset\n"
	   "#define T_gettimeofday %sgettimeofday\n"
	   "#define T_write %swrite\n"
	   "#define T_exit %sexit\n"
	   "#define MAX_CHECK_CHAR 3\n"
	   "#define PRINTABLE(c) (c >= 32 && c <= 126)\n"
	   "#define STDERR 2\n"
	   "#define ARG(_size) trace_arg_##_size\n"
	   "#define WRITENOW() do { T_write(STDERR, logbuf, strlen(logbuf)+1); } while(0)\n"
	   "#define MAX_CHAR_STR_PRINT 256\n"
	   "#define STR_REPEAT_CHAR 4\n"
	   "jmp_buf jBuf;\n"
	   "#define LOGBUFSIZ 256\n"
	   "char logbuf[LOGBUFSIZ];\n"
	   "suseconds_t msec;\n"
	   "time_t sec;\n"
	   "unsigned char siginit = %d;\n"
	   "unsigned char no_trace = 0;\n"
	   "#define INITSIGNAL() \\\ndo { if (siginit == 0) { initsignal(); siginit = 1; } } while(0)\n"
	   "static void sigcrash(int signal, siginfo_t *si, void *context)\n{\n"
	   "\tsnprintf(logbuf, LOGBUFSIZ - 1, \n"
	   "\t\" [!] The tracer received a signal %%s. "
	   "Currently, the tracer is unable to follow context dependent functions.\\n\"\n"
	   "\t\"\t~> Please check previous functions.\\n\", "
	   "(signal == SIGILL ? \"SIGILL\" : (signal == SIGFPE ? \"SIGFPE\" : "
	   "(signal == SIGBUS ? \"SIGBUS\" : \"SIGSEGV\"))));\n"
	   "\tWRITENOW();\n"
	   "\tT_exit(-1);\n"
	   "}\n\n"
	   "static void initsignal()\n{\n"
	   "\tstruct sigaction sa;\n"
	   "\tsa.sa_sigaction = (void *) sigcrash;\n"
	   "\tsa.sa_flags = SA_ONSTACK | SA_SIGINFO | SA_NODEFER;\n"
	   "\tsigemptyset(&sa.sa_mask);\n"
	   "\tT_sigaction(SIGSEGV, &sa, NULL);\n"
	   "\tT_sigaction(SIGFPE, &sa, NULL);\n"
	   "\tT_sigaction(SIGILL, &sa, NULL);\n"
	   "\tT_sigaction(SIGBUS, &sa, NULL);\n"
	   "}\n\n"
	   "static unsigned char is_string(const void *ptr)\n{\n"
	   "\tint count;\n"
	   "\tchar *cptr = (char*) ptr;\n\n"
	   "\tfor (count = 0; PRINTABLE(cptr[count]) && cptr[count] != 0x00; count++)\n\t{\n"
	   "\t\tif (count >= MAX_CHECK_CHAR)\n"
	   "\t\t\treturn 1;\n"
	   "\t}\n\n"
	   "\treturn 0;\n"
	   "}\n\n"
	   "static void check_ptr_failed(int nSig)\n{\n"
	   "\tlongjmp(jBuf, 1);\n"
	   "}\n\n"
	   "static unsigned char check_read_ptr(const void *ptr, unsigned long size)\n{\n"
	   "\tint index;\n"
	   "\tchar elem;\n"
	   "\tstruct sigaction sa, segvoldsa, busoldsa;\n"
	   "\tif (!ptr || size <= 0 || ptr < (void*)0x5000 || ptr == (void *)-1)\n"
	   "\t\treturn 0;\n\n"
	   "\tif(T_setjmp(jBuf))\n"
	   "\t\treturn 0;\n\n"
	   "\tsa.sa_handler = (void *) check_ptr_failed;\n"
	   "\tsa.sa_flags = SA_ONSTACK | SA_RESETHAND | SA_NODEFER;\n"
	   "\tsigemptyset(&sa.sa_mask);\n"
	   "\tif (T_sigaction(SIGSEGV, &sa, &segvoldsa) < 0)\n"
	   "\t\treturn 0;\n"
	   "\tif (T_sigaction(SIGBUS, &sa, &busoldsa) < 0)\n"
	   "\t\treturn 0;\n"
	   "\tfor (index = 0; index < size; index++)\n"
	   "\t\telem = ((char *)ptr)[index];\n\n"
	   "\tT_sigaction(SIGSEGV, &segvoldsa, NULL);\n"
	   "\tT_sigaction(SIGBUS, &busoldsa, NULL);\n"
	   "\treturn 1;\n"
	   "}\n\n"
	   "char tmpbuf[256];\n"
	   "static char *hex_to_str(const void *ptr)\n{\n"
	   "\tint count;\n"
	   "\tchar *cptr = (char*) ptr;\n\n"
	   "\tT_snprintf(tmpbuf, 255, \"= 0x%%02x 0x%%02x 0x%%02x 0x%%02x\", "
	   "cptr[0] & 0xFF, cptr[1] & 0xFF, cptr[2] & 0xFF, cptr[3] & 0xFF);\n"
	   "\treturn tmpbuf;\n"
	   "}\n\n"
	   "char pad_str[64];\n"
	   "int pad_count = 0;\n\n"
	   "static char *pad_print(int inc)\n{\n"
	   "\tint i;\n"
	   "\tif (inc == 0 && pad_count >= 2)\n\t\tpad_count -= 2;\n"
	   "\t\tT_memset(pad_str, ' ', pad_count);\n"
	   "\t\tpad_str[pad_count] = 0;\n"
	   "\tif (inc == 1 && pad_count <= (0xFFFF - 2))\n\t\tpad_count += 2;\n"
	   "\treturn pad_str;\n}\n\n"
	   "int setup = 0;\n"
	   "static void gettrace_time()\n{\n"
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
	   "}\n"
	   "static void func_prstr(char *str)\n{\n"
	   "\tunsigned int i, z, count;\n"
	   "\tfor (i = 0; i < MAX_CHAR_STR_PRINT && str[i] != 0x00; i++, count = 0)\n\t{\n"
	   "\t\tif (str[i+1] == str[i])\n\t\t{\n"
	   "\t\t\tfor (count = 0, z = i; z < MAX_CHAR_STR_PRINT && str[z] == str[i]; count++, z++);\n"
	   "\t\t\tif (count >= STR_REPEAT_CHAR)\n"
	   "\t\t\t\ti = z - 1;\n"
	   "\t\t}\n"
	   "\t\tif (str[i] == '\\n')\n\t\t{\n\t\t\tT_snprintf(logbuf, 255, \"\\\\n\");\n\t\t\tWRITENOW();\n\t\t}\n"
	   "\t\telse if (str[i] == '\\r')\n\t\t{\n\t\t\t\tT_snprintf(logbuf, 255, \"\\\\r\");\n\t\t\tWRITENOW();\n\t\t}\n"
	   "\t\telse if (!PRINTABLE(str[i]))\n\t\t{\n\t\t\tT_snprintf(logbuf, 255, \"\\\\%%d\", (int) (str[i] & 0xFF));\n\t\t\tWRITENOW();\n\t\t}\n"
	   "\t\telse\n\t\t{\n"
	   "\t\t\tT_snprintf(logbuf, 255, \"%%c\", str[i]);\n"
	   "\t\t\tWRITENOW();\n"
	   "\t\t}\n"
	   "\t\tif (count >= STR_REPEAT_CHAR)\n\t\t{\n"
	   "\t\t\tT_snprintf(logbuf, 255, \"{%%d times}\", count);\n"
	   "\t\t\tWRITENOW();\n"
	   "\t\t}\n"
	   "\t}\n"
	   "\tif (i >= MAX_CHAR_STR_PRINT)\n\t{\n"
	   "\t\tT_snprintf(logbuf, LOGBUFSIZ - 1, \"...\");\n"	  
	   "\t\tWRITENOW();\n"
	   "\t}\n"
	   "}\n"
	   "static void func_intro(const char *fname)\n{\n"
	   "\t\n"
	   "\tINITSIGNAL();\n"
	   "\tgettrace_time();\n"
	   "\tT_snprintf(logbuf, LOGBUFSIZ - 1, \"%%s + %%s(\", pad_print(1), fname);\n"
	   "\tWRITENOW();\n"
	   "}\n"
	   "static void func_fmtptr(void *ptr, unsigned char readable, unsigned char isstring)\n{\n"
	   "\tT_snprintf(logbuf, LOGBUFSIZ - 1, \"%%s0x%%x \", "
	   "readable ? \"*\" : \"\", "
	   "ptr);\n"
	   "\tWRITENOW();\n"
	   "\tif (isstring)\n\t{\n"
	   "\t\tT_snprintf(logbuf, LOGBUFSIZ - 1, \"\\\"\");\n"
	   "\t\tWRITENOW();\n"	   
	   "\t\tfunc_prstr((char *)ptr);\n"
	   "\t\tT_snprintf(logbuf, LOGBUFSIZ - 1, \"\\\"\");\n"
	   "\t\tWRITENOW();\n"	   
	   "\t}\n"
	   "\telse if (readable)\n\t{\n"
	   "\t\tT_snprintf(logbuf, LOGBUFSIZ - 1, \"%%s\", hex_to_str(ptr));\n"
	   "\t\tWRITENOW();\n"	   
	   "\t}\n"
	   "}\n"
	   "static void func_argu(void *ptr, unsigned char first, char *tname, char *aname)\n{\n"
	   "\tunsigned char readable, isstring = 0;\n"
	   "\treadable = check_read_ptr(ptr, MAX_CHECK_CHAR+1);\n"
	   "\tif (readable)\n\t\tisstring = is_string(ptr);\n"
	   "\tif (!first)\n\t{\n"
	   "\t\tT_snprintf(logbuf, LOGBUFSIZ - 1, \", \");\n"
	   "\t\tWRITENOW();\n"
	   "\t}\n"
	   "\tif (tname != NULL || aname != NULL)\n\t{\n"
	   "\t\tT_snprintf(logbuf, LOGBUFSIZ - 1, \"%%s %%s: \", tname, aname);\n"
	   "\t\tWRITENOW();\n"
	   "\t}\n"
	   "\tfunc_fmtptr(ptr, readable, isstring);\n"
	   "}\n"
	   "static void func_conclu()\n{\n"
	   "\tT_snprintf(logbuf, LOGBUFSIZ - 1, \")\\n%%s\", \"\");\n"
	   "\tWRITENOW();\n" 
	   "}\n"
	   "static void func_retu(const char *fname, void *ret, int t1, int t2)\n{\n"
	   "\tunsigned char readable, isstring = 0;\n"
	   "\treadable = check_read_ptr((void *)ret, MAX_CHECK_CHAR+1);\n"
	   "\tif (readable)\n\t\tisstring = is_string(ret);\n"
	   "\tgettrace_time();\n"
	   "\tT_snprintf(logbuf, LOGBUFSIZ -1, \"%%s - %%s = \", "
	   "pad_print(0), "
	   "fname);\n"
	   "\tWRITENOW();\n"
	   "\tfunc_fmtptr(ret, readable, isstring);\n"
	   "\tT_snprintf(logbuf, LOGBUFSIZ -1, \" [time spent=%%d.%%06d]\\n\", t1, t2);\n"
	   "\tWRITENOW();\n"
	   "}\n",
	   FUNC_BEGIN(longjmp),
	   FUNC_BEGIN(_setjmp),
	   FUNC_BEGIN(sigaction),
	   FUNC_BEGIN(snprintf),
	   FUNC_BEGIN(memset),
	   FUNC_BEGIN(gettimeofday),
	   FUNC_BEGIN(write),
	   FUNC_BEGIN(exit),
	   0);
  */
  snprintf(buf, BUFSIZ - 1, trace_file_base, 
	   FUNC_BEGIN(longjmp),
	   FUNC_BEGIN(_setjmp),
	   FUNC_BEGIN(sigaction),
	   FUNC_BEGIN(snprintf),
	   FUNC_BEGIN(memset),
	   FUNC_BEGIN(gettimeofday),
	   FUNC_BEGIN(write),
	   FUNC_BEGIN(exit),
	   0);
  fwrite(buf, strlen(buf), sizeof(char), fp);

  /* Setup structures */
  for (index = sizeof(eresi_Addr); index < 512; index++)
    {
      if (index > sizeof(eresi_Addr))
	{
	  snprintf(buf, BUFSIZ - 1, 
		   "typedef struct trace_s_%1$d { void *ptr; char elm[%2$d]; } trace_arg_%1$d;\n",
		   index, index - sizeof(eresi_Addr));
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
	etrace_save_tracing_table(fp, file, table);
    }

  hash_free_keys(keys);
  fclose(fp);

  if (queue_count == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "No functions found", (-1));

  // Compile the tmp c file to create a relocatable file to inject
  if (rename(tfname, rtfname) < 0)
    {
      etrace_queue_clean();
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

  /* Load the new relocatable file for ET_REL injection */
  tobj = elfsh_map_obj(rsofname);
  if (!tobj)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Failed to load relocatable file", -1);

  /* Inject the loaded file */
  idx = elfsh_inject_etrel(file, tobj);

  if (idx < 0)
    {
      etrace_queue_clean();
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Failed to inject ET_REL with workspace", -1);
    }

  last_parsed_function = NULL;

  /* Hijack functions with the new functions injected */
  for (index = 0; index < queue_count && trace_queue[index]; index++)
  {
    /* Keep function name for etrace */
    last_parsed_function = trace_queue[index]->funcname;

    snprintf(buf, BUFSIZ, "%s_trace", trace_queue[index]->funcname);	  

    /* Retrieve symbol */
    dst = elfsh_get_symbol_by_name(file, buf);

    if (dst == NULL)
      {
	etrace_queue_clean();
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Failed to find trace function", -1);
      }

    addr = dst->st_value;

#if __DEBUG_TRACES__
    printf("[DEBUG TRACE] (%03u) HIJACK: %s / %s\n", index, trace_queue[index]->funcname, buf);
#endif

    /* Start to hijack the function */
    err = elfsh_hijack_function_by_name(file, 
					ELFSH_HIJACK_TYPE_FLOW,
					trace_queue[index]->funcname, 
					addr, NULL);
    if (err < 0)
      {
	etrace_queue_clean();
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Failed to hijack a function", -1);
      }
  }

  last_parsed_function = NULL;

  etrace_queue_clean();

  /* Save procedure already relocate but we made some modifications too
     then we restart this procedure.
   */
  if (elfsh_save_relocate(file) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Failed to relocate traced binary", -1);

  /* Clean temp files */
  unlink(rtfname);
  unlink(rsofname);

  /* Ask for rewrite the SHT */
  file->hdr->e_shoff = 0;
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/**
 * @brief Save a binary file on disk for tracing (interface function).
 * @param file The file to be traced
 * @param name Name for saved file (full path).
 * @return Success (0) or Error (-1).
 */
int		etrace_save_obj(elfshobj_t *file, char *name)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Sanity checks */
  if (file == NULL || file->sht == NULL || file->sectlist == NULL ||
      (file->pht == NULL && elfsh_get_objtype(file->hdr) != ET_REL))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid elfshobj_t parameter",  -1);

  file = elfsh_save_preconds(file);
  if (!file)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to prepare file for saving",  -1);
  
  /* Apply awaiting function tracing hooks */
  if (file->hdr->e_type != ET_REL && etrace_save_tracing(file) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Tracing failed", -1);
  
  if (elfsh_store_obj(file, name) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Failed to save object", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
