/**
 * @file traces.c
** traces.c for libelfsh
** 
** All functions that help to trace content
** 
** Started Jul 2 2005 00:03:44 mxatone
** 
**
** $Id: traces.c,v 1.17 2007-07-07 17:57:33 mxatone Exp $
**
*/
#include "libelfsh.h"
#include "libasm.h"

#define TRACES_CFLOW 	1
#define TRACES_PLT 	2
#define TRACES_GOT 	3

char buf[BUFSIZ];

#define TRACE_USED_longjmp		0
#define TRACE_USED__setjmp		1
#define TRACE_USED_sigaction		2
#define TRACE_USED_snprintf		3
#define TRACE_USED_memset		4
#define TRACE_USED_gettimeofday		5
#define TRACE_USED_write		6
#define TRACE_USED_exit			7
#define TRACE_USED_MAX			8

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
    TRACE_FUNCTIONS_ADD(sigaction),
    TRACE_FUNCTIONS_ADD(snprintf),
    TRACE_FUNCTIONS_ADD(memset),
    TRACE_FUNCTIONS_ADD(gettimeofday),
    TRACE_FUNCTIONS_ADD(write),
    TRACE_FUNCTIONS_ADD(exit),
    { "", 0 }
  };

/**
 * Store every traces, this table store another hash table for each key 
 */
hash_t traces_table;

/**
 * Exclude hash table which contain regex
 */
hash_t exclude_table;

/**
 * Whole active elements 
 */
int trace_enabled_count = 0;

#define ELFSH_TRACES_TABLE_NAME 	"elfsh_traces_table"
#define ELFSH_TRACES_EXCLUDE_TABLE_NAME "elfsh_traces_exclude_table"
#define ELFSH_TRACES_PATTERN 		"traces_%s"

/**
 * Static part of the file
 */
char trace_file_base[] = "#include <stdio.h>\n"
"#include <setjmp.h>\n"
"#include <signal.h>\n"
"#include <string.h>\n"
"#include <stdlib.h>\n"
"#include <unistd.h>\n"
"#include <sys/time.h>\n"
"#include <sys/resource.h>\n"
"\n"
"#define T_longjmp %slongjmp\n"
"#define T_setjmp %s_setjmp\n"
"\n"
"#define T_sigaction %ssigaction\n"
"#define T_snprintf %ssnprintf\n"
"#define T_memset %smemset\n"
"#define T_gettimeofday %sgettimeofday\n"
"#define T_write %swrite\n"
"#define T_exit %sexit\n"
"\n"
"#define MAX_CHECK_CHAR 3\n"
"#define PRINTABLE(c) (c >= 32 && c <= 126)\n"
"#define STDERR 2\n"
"#define ARG(_size) trace_arg_##_size\n"
"#define WRITENOW() do { T_write(STDERR, logbuf, strlen(logbuf)+1); } while(0)\n"
"#define MAX_CHAR_STR_PRINT 40\n"
"#define STR_REPEAT_CHAR 4\n"
"jmp_buf jBuf;\n"
"#define LOGBUFSIZ 256\n"
"char logbuf[LOGBUFSIZ];\n"
"suseconds_t msec;\n"
"time_t sec;\n"
"u_char siginit = %d;\n"
"u_char no_trace = 0;\n"
"\n"
"#define INITSIGNAL() \\\ndo { if (siginit == 0) { initsignal(); siginit = 1; } } while(0)\n"
"\n"
"static void sigcrash(int signal, siginfo_t *si, void *context)\n"
"{\n"
"  snprintf(logbuf, LOGBUFSIZ - 1, \" [!] The tracer received a signal %%s. \"\n"
"	   \"Currently, the tracer is unable to follow context dependent functions.\\n\"\n"
"	   \"\\t~> Please check previous functions.\\n\", \n"
"	   (signal == SIGILL ? \"SIGILL\" : (signal == SIGFPE ? \"SIGFPE\" : \n"
"					     (signal == SIGBUS ? \"SIGBUS\" : \"SIGSEGV\"))));\n"
"  WRITENOW();\n"
"  T_exit(-1);\n"
"}\n"
"\n"
"static void initsignal()\n"
"{\n"
"  struct sigaction sa;\n"
"	\n"
"  sa.sa_sigaction = (void *) sigcrash;\n"
"  sa.sa_flags = SA_ONSTACK | SA_SIGINFO | SA_NODEFER;\n"
"  \n"
"  sigemptyset(&sa.sa_mask);\n"
"  \n"
"  T_sigaction(SIGSEGV, &sa, NULL);\n"
"  T_sigaction(SIGFPE, &sa, NULL);\n"
"  T_sigaction(SIGILL, &sa, NULL);\n"
"  T_sigaction(SIGBUS, &sa, NULL);\n"
"}\n"
"\n"
"static unsigned char is_string(const void *ptr)\n"
"{\n"
"  int count;\n"
"  char *cptr = (char*) ptr;\n"
"  \n"
"  for (count = 0; PRINTABLE(cptr[count]) && cptr[count] != 0x00; count++)\n"
"    {\n"
"      if (count >= MAX_CHECK_CHAR)\n"
"	return 1;\n"
"    }\n"
"\n"
"  return 0;\n"
"}\n"
"\n"
"static void check_ptr_failed(int nSig)\n"
"{\n"
"  longjmp(jBuf, 1);\n"
"}\n"
"\n"
"static unsigned char check_read_ptr(const void *ptr, unsigned long size)\n"
"{\n"
"  int index;\n"
"  char elem;\n"
"  struct sigaction sa, segvoldsa, busoldsa;\n"
"\n"
"  if (!ptr || size <= 0 || ptr < (void*)0x5000 || ptr == (void *)-1)\n"
"    return 0;\n"
"\n"
"  if(T_setjmp(jBuf))\n"
"    return 0;\n"
"\n"
"  sa.sa_handler = (void *) check_ptr_failed;\n"
"  sa.sa_flags = SA_ONSTACK | SA_RESETHAND | SA_NODEFER;\n"
"  sigemptyset(&sa.sa_mask);\n"
"\n"
"  if (T_sigaction(SIGSEGV, &sa, &segvoldsa) < 0)\n"
"    return 0;\n"
"\n"
"  if (T_sigaction(SIGBUS, &sa, &busoldsa) < 0)\n"
"    return 0;\n"
"\n"
"  for (index = 0; index < size; index++)\n"
"    elem = ((char *)ptr)[index];\n"
"\n"
"  T_sigaction(SIGSEGV, &segvoldsa, NULL);\n"
"  T_sigaction(SIGBUS, &busoldsa, NULL);\n"
"\n"
"  return 1;\n"
"}\n"
"\n"
"static void hex_to_str(const void *ptr)\n"
"{\n"
"  int count;\n"
"  char *cptr = (char*) ptr;\n"
"  unsigned int i;\n"
"\n"
"  T_snprintf(logbuf, LOGBUFSIZ - 1, \" = \");\n"
"  WRITENOW();\n"
"\n"
"  for (i = 0; i < 4; i++)\n"
"  {\n"
"    T_snprintf(logbuf, LOGBUFSIZ - 1, \"0x%%02x\", cptr[i] & 0xFF);\n"
"    WRITENOW();\n"
"\n"
"    if (cptr[i] == '\\n')\n"
"    {\n"
"      \n"
"      T_snprintf(logbuf, LOGBUFSIZ - 1, \" '\\\\n'\"); \n"
"      WRITENOW();\n"
"    }\n"
"    else if (cptr[i] == '\\r')\n"
"    {\n"
"      \n"
"      T_snprintf(logbuf, LOGBUFSIZ - 1, \" '\\\\r'\"); \n"
"      WRITENOW();\n"
"    }\n"
"    else if (cptr[i] == '\\t')\n"
"    {\n"
"      \n"
"      T_snprintf(logbuf, LOGBUFSIZ - 1, \" '\\\\t'\"); \n"
"      WRITENOW();\n"
"    }\n"
"    else if (PRINTABLE(cptr[i]))\n"
"    {\n"
"      T_snprintf(logbuf, BUFSIZ - 1, \" '%%c'\", cptr[i]);\n"
"      WRITENOW();\n"
"    }\n"
"\n"
"    if (i != 3)\n"
"    {\n"
"      T_snprintf(logbuf, BUFSIZ - 1, \" \");\n"
"      WRITENOW();\n"
"    }\n"
"  }\n"
"}\n"
"\n"
"char pad_str[64];\n"
"int pad_count = 0;\n"
"static char *pad_print(int inc)\n"
"{\n"
"  int i;\n"
"\n"
"  if (inc == 0 && pad_count >= 2)\n"
"    pad_count -= 2;\n"
"\n"
"  T_memset(pad_str, ' ', pad_count);\n"
"  pad_str[pad_count] = 0;\n"
"\n"
"  if (inc == 1 && pad_count <= (0xFFFF - 2))\n"
"    pad_count += 2;\n"
"\n"
"  return pad_str;\n"
"}\n"
"\n"
"int setup = 0;\n"
"static void gettrace_time()\n"
"{\n"
"  double difftime = 0;\n"
"  struct timeval now;\n"
"\n"
"  T_gettimeofday(&now, NULL);\n"
"\n"
"  if (setup == 0)\n"
"    {\n"
"      msec = now.tv_usec;\n"
"      sec = now.tv_sec;\n"
"    }\n"
"\n"
"  T_snprintf(logbuf, LOGBUFSIZ - 1, \"%%4d.%%06d \", now.tv_sec - sec, now.tv_usec - msec);\n"
"  WRITENOW();\n"
"\n"
"  if (setup)\n"
"    {\n"
"      msec = now.tv_usec;\n"
"      sec = now.tv_sec;\n"
"    }\n"
"\n"
"  setup = 1;\n"
"}\n"
"static void func_prstr(char *str)\n"
"{\n"
"  unsigned int i, z, count;\n"
"\n"
"  for (i = 0; i < MAX_CHAR_STR_PRINT && str[i] != 0x00; i++, count = 0)\n"
"    {\n"
"      if (str[i+1] == str[i])\n"
"	{\n"
"	  for (count = 0, z = i; z < MAX_CHAR_STR_PRINT && str[z] == str[i]; count++, z++);\n"
"\n"
"	  if (count >= STR_REPEAT_CHAR)\n"
"	    i = z - 1;\n"
"	}\n"
"\n"
"      if (str[i] == '\\n')\n"
"	{\n"
"	  T_snprintf(logbuf, LOGBUFSIZ - 1, \"\\\\n\");\n"
"	  WRITENOW();\n"
"	}\n"
"      else if (str[i] == '\\r')\n"
"	{\n"
"	  T_snprintf(logbuf, LOGBUFSIZ - 1, \"\\\\r\");\n"
"	  WRITENOW();\n"
"	}\n"
"      else if (str[i] == '\\t')\n"
"	{\n"
"	  T_snprintf(logbuf, LOGBUFSIZ - 1, \"\\\\t\");\n"
"	  WRITENOW();\n"
"	}\n"
"      else if (!PRINTABLE(str[i]))\n"
"	{\n"
"	  T_snprintf(logbuf, LOGBUFSIZ - 1, \"\\\\%%d\", (int) (str[i] & 0xFF));\n"
"	  WRITENOW();\n"
"	}\n"
"	else\n"
"	{\n"
"	  T_snprintf(logbuf, LOGBUFSIZ - 1, \"%%c\", str[i]);\n"
"	  WRITENOW();\n"
"	}\n"
"\n"
"      if (count >= STR_REPEAT_CHAR)\n"
"	{\n"
"	  T_snprintf(logbuf, LOGBUFSIZ - 1, \"{%%d times}\", count);\n"
"	  WRITENOW();\n"
"	}\n"
"    }\n"
"\n"
"  if (i >= MAX_CHAR_STR_PRINT)\n"
"    {\n"
"      T_snprintf(logbuf, LOGBUFSIZ - 1, \"...\");	  \n"
"      WRITENOW();\n"
"    }\n"
"}\n"
"\n"
"static void func_intro(const char *fname)\n"
"{\n"
"  INITSIGNAL();\n"
"  gettrace_time();\n"
"  T_snprintf(logbuf, LOGBUFSIZ - 1, \"%%s + %%s(\", pad_print(1), fname);\n"
"  WRITENOW();\n"
"}\n"
"\n"
"static void func_fmtptr(void *ptr, unsigned char readable, unsigned char isstring)\n"
"{\n"
"  T_snprintf(logbuf, LOGBUFSIZ - 1, \"%%s0x%%x\", \n"
"	     readable ? \"*\" : \"\", ptr);\n"
"  WRITENOW();\n"
"\n"
"  if (isstring)\n"
"    {\n"
"      T_snprintf(logbuf, LOGBUFSIZ - 1, \" \\\"\");\n"
"      WRITENOW();	   \n"
"\n"
"      func_prstr((char *)ptr);\n"
"\n"
"      T_snprintf(logbuf, LOGBUFSIZ - 1, \"\\\"\");\n"
"      WRITENOW();	   \n"
"    }\n"
"  else if (readable)\n"
"    {\n"
"      hex_to_str(ptr);\n"
"    }\n"
"}\n"
"  \n"
"static void func_argu(void *ptr, unsigned char first, char *tname, char *aname)\n"
"{\n"
"  unsigned char readable, isstring = 0;\n"
"  readable = check_read_ptr(ptr, MAX_CHECK_CHAR+1);\n"
"\n"
"  if (readable)\n"
"    isstring = is_string(ptr);\n"
"\n"
"  if (!first)\n"
"    {\n"
"      T_snprintf(logbuf, LOGBUFSIZ - 1, \", \");\n"
"      WRITENOW();\n"
"    }\n"
"  \n"
"  if (tname != NULL || aname != NULL)\n"
"    {\n"
"      T_snprintf(logbuf, LOGBUFSIZ - 1, \"%%s %%s: \", tname, aname);\n"
"      WRITENOW();\n"
"    }\n"
"\n"
"  func_fmtptr(ptr, readable, isstring);\n"
"}\n"
"\n"
"static void func_conclu()\n"
"{\n"
"  T_snprintf(logbuf, LOGBUFSIZ - 1, \")\\n%%s\", \"\");\n"
"  WRITENOW(); \n"
"}\n"
"\n"
"static void func_retu(const char *fname, void *ret, int t1, int t2)\n"
"{\n"
"  unsigned char readable, isstring = 0;\n"
"\n"
"  readable = check_read_ptr((void *)ret, MAX_CHECK_CHAR+1);\n"
"\n"
"  if (readable)\n"
"    isstring = is_string(ret);\n"
"\n"
"  gettrace_time();\n"
"\n"
"  T_snprintf(logbuf, LOGBUFSIZ -1, \"%%s - %%s = \", pad_print(0), fname);\n"
"  WRITENOW();\n"
"  func_fmtptr(ret, readable, isstring);\n"
"  T_snprintf(logbuf, LOGBUFSIZ -1, \" [time spent=%%d.%%06d]\\n\", t1, t2);\n"
"  WRITENOW();\n"
"}\n";

char *last_parsed_function = NULL;

/**
 * When an error occur in etrace. This function is used to show buggy function name.
 * @return function name
 */
char			*elfsh_traces_geterrfunc()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, last_parsed_function);
}

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
 * Check if this function name is excluded
 * @param name function name
 */
static int		elfsh_traces_check_exclude(char *name)
{
  u_int			index;
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
  u_char		typed;

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
	      if (elfsh_traces_check_exclude(ret_trace->funcname) < 0)
		continue;

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
		  for (z = 0; z < ret_trace->argc && z < ELFSH_TRACES_MAX_ARGS; z++)
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
		  for (z = 0; z < ret_trace->argc && z < ELFSH_TRACES_MAX_ARGS; z++)
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
	   "u_char siginit = %d;\n"
	   "u_char no_trace = 0;\n"
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
  for (index = sizeof(elfsh_Addr); index < 512; index++)
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

  /* Load the new relocatable file for ET_REL injection */
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
	elfsh_traces_queue_clean();
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
	elfsh_traces_queue_clean();
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Failed to hijack a function", -1);
      }
  }

  last_parsed_function = NULL;

  elfsh_traces_queue_clean();

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

      /* Not support printf like functions */
      elfsh_traces_add_untracable("printf");
      elfsh_traces_add_untracable("fprintf");
      elfsh_traces_add_untracable("sprintf");
      elfsh_traces_add_untracable("snprintf");
      elfsh_traces_add_untracable("vprintf");
      elfsh_traces_add_untracable("vfprintf");
      elfsh_traces_add_untracable("vsprintf");
      elfsh_traces_add_untracable("vsnprintf");

      /* sscanf use a printf like prototype */
      elfsh_traces_add_untracable("scanf");
      elfsh_traces_add_untracable("fscanf");
      elfsh_traces_add_untracable("sscanf");
      elfsh_traces_add_untracable("vscanf");
      elfsh_traces_add_untracable("vsscanf");
      elfsh_traces_add_untracable("vfscanf");

      /* ioctl use a printf like prototype */
      elfsh_traces_add_untracable("ioctl");
      
      if (ostype == ELFSH_OS_LINUX)
	{
	  elfsh_traces_add_untracable("__libc_start_main");
	  elfsh_traces_add_untracable("_start");
	  elfsh_traces_add_untracable("_init");
	  elfsh_traces_add_untracable("_fini");
	}

      if (ostype == ELFSH_OS_FREEBSD)
	{
	  // x86 failed on this function
	  // TODO: correct them
	  elfsh_traces_add_untracable("getcwd");
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
 * Setup asm_processor structure correctly
 * @param proc pointer to asm_processor structure
 */
static int		elfsh_traces_setup_proc(elfshobj_t *file, asm_processor *proc)
{
  u_int         	arch;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!file || !proc)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);
  
  arch = elfsh_get_arch(file->hdr);

  switch(arch)
    {
    case EM_SPARC:
    case EM_SPARCV9:
    case EM_SPARC32PLUS:
      asm_init_sparc(proc);
      break;
    case EM_386:
      asm_init_i386(proc);
      break;
      /* Not ready yet ?
    case EM_MIPS:
      asm_init_mips(proc);
      break;
      */
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unsupported architecture for address tracing", -1);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Get function addr list from call search basic
 * @param file target file
 * @param addr address list
 */
int			elfsh_addr_get_func_list(elfshobj_t *file, elfsh_Addr **addr)
{
  int 			ret;
  int			index;
  asm_instr		instr;
  elfsh_SAddr		foffset;
  elfsh_Word		len;
  char			*base;
  asm_processor		proc;
  elfsh_Addr		base_vaddr, caddr;
  u_char		found = 0;
  elfshsect_t		*text;
  elfsh_Addr		*vaddr;
  const int		astep = 20;
  u_int			apos = 0;
  btree_t		*broot = NULL;
  u_int			diff;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!file || !addr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  /* Search entrypoint section, our address must be in this section */
  text = elfsh_get_parent_section(file, elfsh_get_entrypoint(file->hdr), &foffset);

  if (!text)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot find parent section from entry point", -1);
  
  if (!elfsh_get_anonymous_section(file, text))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to get an anonymous section", -1);
  
  base = elfsh_get_raw(text);
  len = text->shdr->sh_size;

  /* Get the virtual address */
  base_vaddr = (elfsh_is_debug_mode() && !elfsh_section_is_runtime(text) ?
		file->rhdr.base + elfsh_get_section_addr(text->shdr) :
		elfsh_get_section_addr(text->shdr));

  /* Setup asm_processor structure */
  if (elfsh_traces_setup_proc(file, &proc) < 0)
        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed during proc structure setup", -1);

  XALLOC(__FILE__, __FUNCTION__, __LINE__, vaddr, sizeof(elfsh_Addr)*astep, -1);
  
  /* Despite the fact that we choose the right architecture to init asm,
     Our approach is totally architecture independant as we search using
     global type ASM_TYPE_CALLPROC and we know that op1.imm will contain a
     relative value. */
  for (index = 0; index < len; index += ret)
    {
      /* Read an instruction */
      if ((ret = asm_read_instr(&instr, (u_char *) (base + index), len -  index, &proc)))
	{
	  /* Global assembler filter */
	  if ((instr.type & ASM_TYPE_CALLPROC)
	      && instr.op1.imm != 0)
	    {
	      caddr = base_vaddr + index + instr.op1.imm + instr.len;

	      /* Found a call check its local */
	      if (INTERVAL(base_vaddr, caddr, base_vaddr + len))
		{
		  found = 1;

		  diff = (u_int) caddr;

		  /* Avoid double entrie */
		  if (btree_get_elem(broot, diff) != NULL)
		    goto next;

		  btree_insert(&broot, diff, (void *)0x1);

		  /* Next will be the last of the current list
		     then realloc */
		  if ((apos+1) % astep == 0)
		    {
		      XREALLOC(__FILE__, __FUNCTION__, __LINE__, vaddr, vaddr,
			       sizeof(elfsh_Addr)*(apos+1+astep), -1);

		      /* Blank new elements */
		      memset(&vaddr[apos], 0x00, astep*sizeof(elfsh_Addr));
		    }

		  vaddr[apos++] = caddr;
		}
	    }
	}

    next:

      if (ret <= 0)
	ret = 1;
    }

  /* If nothing found we free allocated buffer and
     return an error */
  if (!found)
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__, vaddr);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "No call internal found", -3);
    }
  
  btree_free(broot, 0);

  *addr = vaddr;
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Search a call for a given address
 * @param file target file
 * @param addr supose to be a function
 */
int			elfsh_addr_is_called(elfshobj_t *file, elfsh_Addr addr)
{
  int 			ret;
  int			index;
  asm_instr		instr;
  elfsh_SAddr		foffset;
  elfsh_Word		len;
  char			*base;
  asm_processor		proc;
  elfsh_Addr		base_vaddr;
  u_char		found = 0;
  elfshsect_t		*text;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!file)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameter", -1);

  /* Search entrypoint section, our address must be in this section */
  text = elfsh_get_parent_section(file, elfsh_get_entrypoint(file->hdr), &foffset);

  if (!text)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot find parent section from entry point", -1);
  
  if (!elfsh_get_anonymous_section(file, text))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to get an anonymous section", -1);
  
  base = elfsh_get_raw(text);
  len = text->shdr->sh_size;

  /* Get the virtual address */
  base_vaddr = (elfsh_is_debug_mode() && !elfsh_section_is_runtime(text) ?
		file->rhdr.base + elfsh_get_section_addr(text->shdr) :
		elfsh_get_section_addr(text->shdr));

  /* Our address is valid ? */
  if (!INTERVAL(base_vaddr, addr, (base_vaddr + len)))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Not in entrypoint section", -4);

  /* Setup asm_processor structure */
  if (elfsh_traces_setup_proc(file, &proc) < 0)
        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed during proc structure setup", -1);
  
  /* Despite the fact that we choose the right architecture to init asm,
     Our approach is totally architecture independant as we search using
     global type ASM_TYPE_CALLPROC and we know that op1.imm will contain a
     relative value. */
  for (index = 0; index < len; index += ret)
    {
      /* Read an instruction */
      if ((ret = asm_read_instr(&instr, (u_char *) (base + index), len -  index, &proc)))
	{
	  /* Global assembler filter */
	  if ((instr.type & ASM_TYPE_CALLPROC)
	      && instr.op1.imm != 0)
	    {
	      /* Found the correct call */
	      if (base_vaddr + index + instr.op1.imm + instr.len == addr)
		{
		  found = 1;
		  break;
		}
	    }
	}

      if (ret <= 0)
	ret = 1;
    }

  if (!found)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "No call found", -3);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Check if a function address is valid or not
 * @param file target file
 * @param addr function address
 * @param vaddr returned virtual address
 */
int			elfsh_traces_valid_faddr(elfshobj_t *file, elfsh_Addr addr,
						 elfsh_Addr *vaddr, u_char *dynsym)
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

  if (!file || !name || !vaddr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

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

  if (!name || !newtrace)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", NULL);

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

  if (!trace || !name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

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
 * Exclude functions by regex during the last stage
 * @param regstr reg string
 */
int			elfsh_traces_funcexclude(char *regstr)
{
  char			funcreg[256];
  size_t		len;
  regex_t		preg, *set;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (!regstr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  /* Init table if needed */
  if (exclude_table.ent == NULL)
    hash_init(&exclude_table, ELFSH_TRACES_EXCLUDE_TABLE_NAME, 30, ASPECT_TYPE_UNKNOW);

  len = strlen(regstr);
  snprintf(funcreg, 255, "%s%s%s", 
	   regstr[0] != '^' ? "^" : "",
	   regstr,
	   regstr[len-1] != '$' ? "$" : "");

  /* Check if its a valid regexec */
  if (regcomp(&preg, funcreg, 0) != 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid regex", -1);

  /* Alloc and copy our regex */
  XALLOC(__FILE__, __FUNCTION__, __LINE__, set, sizeof(regex_t), -1);
  memcpy(set, &preg, sizeof(regex_t));

  /* Add it to the hash table */
  hash_add(&exclude_table, strdup(regstr), set);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Remove an exclude function by regex during the last stage
 * @param regstr reg string
 */
int			elfsh_traces_funcrmexclude(char *regstr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (!regstr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  if (exclude_table.ent)
    hash_del(&exclude_table, regstr);

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

  if (!trace || !name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

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

  if (!table)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Can't found this trace", -1);

  hash_destroy(table);
  hash_del(&traces_table, trace);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * If submited function point into the plt, we return the file
 * and its virtual address to point to remote function
 * @param filein base file pointer
 * @param vaddrin base virtual address
 * @param fileout returned file pointer
 * @param vaddrout returned virtual address
 */
int			elfsh_resolv_remote_function(elfshobj_t *filein, elfsh_Addr vaddrin,
						     elfshobj_t **fileout, elfsh_Addr *vaddrout)
{
  elfshobj_t		*file;
  elfshsect_t		*sect;
  char			*data;
  elfsh_SAddr		sfoffset;
  char			*sym_name;
  elfsh_Sym		*sym;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!filein || !fileout || !vaddrout)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  /* Default returned values are -in */
  *fileout = filein;
  *vaddrout = vaddrin;

  /* Get virtual address parent section */
  sect = elfsh_get_parent_section(filein, vaddrin, &sfoffset);

  if (!sect)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Can't get section", -1);

  data = elfsh_get_section_name(filein, sect);

  if (!data)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Can't get section name", -1);

  /* Find the right function on dependencies */
  if (!strncmp(data, ".plt", 4))
    {
      sym_name = elfsh_reverse_dynsymbol(filein, vaddrin, &sfoffset);

      if (!sym_name)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Can't find symbol name", -1);

      file = elfsh_traces_search_sym(filein, sym_name);
	      
      if (!file)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Can't find extern function file", -1);

      sym = elfsh_get_dynsymbol_by_name(file, sym_name);

      if (!sym)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Can't find function symbol on dependencies", -1);

      /* Update pointers */
      *fileout = file;
      *vaddrout = sym->st_value;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
