/*
** trace.c for elfsh
**
** All functions that help to trace content
**
** Started Jul 2 2005 00:03:44 mxatone
**
*/
#include "elfsh.h"

#define TRACE_CFLOW 	1
#define TRACE_PLT 	2
#define TRACE_GOT 	3

typedef struct 	s_trace
{
  char 		*funcname;
  int		redirtype;
  elfsh_Addr	addr;
  
} e2dbgtraced_t;

/* XXX: The good syntax should be : 
** 
** trace <add>     funcname     [optional_trace_name]  : add a function to a given trace
** trace <rm>      funcname     [optional_trace_name]  : remove a function from a given trace
** trace <enable>  funcname|all [optional_trace_name]  : enable tracing for a function in a given trace (enabled by default after a add)
** trace <disable> funcname|all [optional_trace_name]  : disable tracing for a function in a given trace
** trace <create>  tracename    <optionals funcnames>  : create a new trace with a pool of traced functions by default
** trace <delete>  tracename                           : delete a trace
** trace <flush>   tracename                           : remove all functions from a given trace
** trace <list>                                        : list all available traces
**
*/                             

/* XXX: Need to use libgcc and not gcc directly */
FILE		*vm_trace_init(char *tfname, char *rsofname, char *rtfname)
{
  int		rs = 0, fd;
  FILE 		*fp;
  char		buf[BUFSIZ + 1];
  int		osize = strlen(tfname)+3;
  struct stat	s;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
    
 fretry:
  if (rs > 30)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot create temporary file (retry more than 30 times)", 
		      NULL);
  
  rs++;

  /* Generate a random temporary name file */
  fd = mkstemp(tfname);

  if (fd == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot create temporary file", NULL);

  fp = fdopen(fd, "w");

  if (fp == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot read temporary file", NULL);

  /* gcc needs rights extentions */
  snprintf(rtfname, osize, "%s.c", tfname);
  snprintf(rsofname, osize, "%s.o", tfname);

  /* If one of this two files exists, we retry */
  if (lstat(rtfname, &s) == 0 
      || lstat(rsofname, &s) == 0)
    goto fretry;

#if __DEBUG_TRACE__
  printf("[DEBUG TRACE] Open trace temporary filename: %s\n", tfname);
  printf("[DEBUG TRACE] Futur trace filename: %s\n", rtfname);
  printf("[DEBUG TRACE] Relocatable filename: %s\n", rsofname);
#endif

  /* Write basic stuff, include headers and
     pad functions (to have a tree like form)
   */
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

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, fp);
}



/* Add a function to the tracing file */
int		vm_trace_add(FILE *fp, int *argcount, char *func_name)
{
  int		z = 0;
  char		buf[BUFSIZ + 1];
  char		bufex[BUFSIZ + 1];
  char		args[BUFSIZ + 1];
  char		argsproto[BUFSIZ + 1];
  char		argshexa[BUFSIZ + 1];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  argshexa[0] = 0;
  argsproto[0] = 0;
  args[0] = 0;

  /* Do we have arguments in this function ? */
  if (argcount[0] != NULL)
  {
    snprintf(argshexa, BUFSIZ, "%%%%%s", "08x");
    snprintf(argsproto, BUFSIZ, "char a%d[%d]", 0, argcount[0]);
    snprintf(args, BUFSIZ, "a%d", 0);
	
    /* Arguments by arguments */
    for (z = 1; z < ELFSH_TRACE_MAX_ARGS && argcount != NULL; z++)
    {
      if (argcount[z] == NULL)
	break;

      /* char type is just what we need to setup a good size */
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

  /* Tracing function with a human readable form */
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

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, z);
}




/* Prepare to trace functions */
int			vm_trace_functions(FILE *fp, elfsh_Sym *symtab, 
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

  /* Some preliminaries checks */
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  called++;
  if (symtab == NULL || symnum == 0)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  for (index = 0, count = 1; index < symnum; index++)
    {
      /* Only function symbols */
      if (elfsh_get_symbol_type(symtab + index) != STT_FUNC)
	continue;
      
      sect = elfsh_get_parent_section(world.curjob->current, 
				      symtab[index].st_value, NULL);
      
      /* Try to get the section using index */
      if (sect == NULL && symtab[index].st_shndx)
	sect = elfsh_get_section_by_index(world.curjob->current,
					  symtab[index].st_shndx, NULL, NULL);
      if (sect == NULL)
	continue;
      sect_name = elfsh_get_section_name(world.curjob->current, sect);
      
      /* Only global, plt & text */
      /* Make sure we look at the beginning of the name, including the .*/
      if (elfsh_get_symbol_bind(symtab + index) != STB_GLOBAL
	  || (strncmp(sect_name, ".plt", 4) && strncmp(sect_name, ".text", 5)))
	continue;
      
      /* Switch between symbol table and dynamic symbol table */
      switch (called)
	{
	case 0:
	  func_name = elfsh_get_symbol_name(world.curjob->current, symtab + index);
	  break;
	default:
	  func_name = elfsh_get_dynsymbol_name(world.curjob->current, symtab + index);
	  break;
	}

      /* FIXME: A function name containing "." will not be compiled right with gcc */
      if (strstr(func_name, "."))
	continue;
      
      /* have we chosen which function will be traced ? */
      for (z = 0; world.curjob->curcmd->param[z]; z++)
	if (!strcmp(world.curjob->curcmd->param[z], func_name))
	  break;
      
      /* This function should not be traced */
      if (world.curjob->curcmd->param[z])
	continue;
      
      off = (int) elfsh_get_raw(sect) + symtab[index].st_value;
      off -= (sect->parent->rhdr.base + sect->shdr->sh_addr);
      
      /* Get args number table */
      argcount = elfsh_args_count(world.curjob->current, off, symtab[index].st_value);
      if (argcount == NULL)
	continue;
      selfuncs[count - 1] = elfsh_strdup(func_name);
      
      /* Add function handle into tmp file */
      numargs = vm_trace_add(fp, argcount, func_name);
      
      /* Print some informations */
#if 0
      snprintf(buf, BUFSIZ, "[%03d] %-24s => %-20s argnum = %d \n", 
	       count, func_name, sect_name, numargs);      
      vm_output(buf);      
#endif
      count++;
    }
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, count - 1);
}

/*
** TODO: 
**  - Module more
**  - Support arguments
**  - use libgcc and not gcc with file
*/
int		trace_add(const char *name, const char *optarg)
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

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  elfsh_setup_hooks();

  vm_output(" .: Tracing Functions :.\n\n");

  /* Do we have symbols ? */
  symtab = elfsh_get_symtab(world.curjob->current, &symnum);
  dynsym = elfsh_get_dynsymtab(world.curjob->current, &dynsymnum);

  if (symnum + dynsymnum == 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No symbols found", (-1));

  /* Initialize the tracing */
  fp = vm_trace_init(tfname, rsofname, rtfname);
  if (fp == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot init tmp files", (-1));

  /* Create temporary storage */
  char 		*selfuncs[symnum + dynsymnum + 1];

  for (index = 0; index < symnum + dynsymnum + 1; index++)
    selfuncs[index] = NULL;

  /* Call on both symbol tables */
  count += vm_trace_functions(fp, symtab, symnum, selfuncs);
  count += vm_trace_functions(fp, dynsym, dynsymnum, selfuncs + count);
  fclose(fp);

#if __DEBUG_TRACE__
  printf("[DEBUG TRACE] TRACED FUNCTIONS COUNT = %d\n", count);
#endif

  /* If we didn't trace a function, we don't need to continue */
  if (!count)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No function found", (-1));
  
#if __DEBUG_TRACE__
  printf("[DEBUG TRACE] RENAME %s to %s\n", tfname, rtfname);
#endif

  /* Compile the tmp c file to create a relocatable file to inject */
  if (rename(tfname, rtfname) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Rename failed", (-1));

  snprintf(buf, BUFSIZ, "gcc -c %s -o %s", rtfname, rsofname);
  vm_system(buf);

  /* Load the new relocatable file for ET_REL injection */
  tobj = elfsh_map_obj(rsofname);
  if (!tobj)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Failed to load relocatable file", (-1));

  idx = elfsh_inject_etrel(world.curjob->current, tobj);	  
  if (idx < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Failed to inject ET_REL with workspace", (-1));

  /* Hijack functions with the new functions injected */
  for (index = 0; index < count; index++)
  {
    snprintf(buf, BUFSIZ, "%s_trace", selfuncs[index]);	  

    /* Retrieve symbol */
    dst = elfsh_get_symbol_by_name(world.curjob->current, buf);
    if (dst == NULL)
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Failed to find trace function", (-1));
    addr = dst->st_value;

#if __DEBUG_TRACE__
    printf("[DEBUG TRACE] (%03u) HIJACK: %s / %s\n", index, selfuncs[index], buf);
#endif

    /* Start to hijack the function */
    err = elfsh_hijack_function_by_name(world.curjob->current, 
					ELFSH_HIJACK_TYPE_FLOW,
					selfuncs[index], 
					addr, NULL);
    if (err < 0)
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Failed to hijack a function", (-1));
  }
  
  vm_output("\n [*] Binary ready to be traced. You can now save and run it.\n\n");

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* TODO: make it ! */
int		trace_rm(const char *name, const char *optarg)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  vm_output("trace: rm\n\n");

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* TODO: make it ! */
int		trace_enable(const char *name, const char *optarg)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  vm_output("trace: enable\n\n");

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* TODO: make it ! */
int		trace_disable(const char *name, const char *optarg)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  vm_output("trace: disable\n\n");

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* TODO: make it ! */
int		trace_create(const char *name, const char *optarg)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  vm_output("trace: create\n\n");

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* TODO: make it ! */
int		trace_delete(const char *name, const char *optarg)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  vm_output("trace: delete\n\n");

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* TODO: make it ! */
int		trace_flush(const char *name, const char *optarg)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  vm_output("trace: flush\n\n");

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* TODO: make it ! */
int		trace_list(const char *name, const char *optarg)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  vm_output("trace: list\n\n");

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Create the structure that store trace cmd informations */
elfshtracecmd_t   *trace_create_CMDENT(int	(*exec)(char *name, char *optarg),
				       char	flagName,
				       char 	flagArg)
{
  elfshtracecmd_t *new;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(new, sizeof(elfshtracecmd_t), NULL);
  new->exec	= exec;
  new->flagName	= flagName;
  new->flagArg	= flagArg;
 
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}

/* Add a command */
int 		trace_addcmd(char *cmd, void *exec, char flagName, char flagArg)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  hash_add(&trace_cmd_hash, cmd, (void *) trace_create_CMDENT(exec, flagName, flagArg));

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int 			cmd_trace()
{
  elfshtracecmd_t	*cmd;
  char			logbuf[BUFSIZ];
  char			*fArg, *sArg;
  int			ret = 0;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* classic msg */
  if (!world.curjob->curcmd->param[0])
    {
      snprintf(logbuf, BUFSIZ - 1,
	       "\t Trace command options: \n\n"
	       " add     funcname     [optional_trace_name]  : add a function to a given trace\n"
	       " rm      funcname     [optional_trace_name]  : remove a function from a given trace\n"
	       " enable  funcname|all [optional_trace_name]  : enable tracing for a function in a given trace (enabled by default after a add)\n"
	       " disable funcname|all [optional_trace_name]  : disable tracing for a function in a given trace\n"
	       " create  tracename    <optionals funcnames>  : create a new trace with a pool of traced functions by default\n"
	       " delete  tracename                           : delete a trace\n"
	       " flush   tracename                           : remove all functions from a given trace\n"
	       " list                                        : list all available traces\n\n");
      
      vm_output(logbuf);
    }
  else
    {
      cmd = hash_get(&trace_cmd_hash, world.curjob->curcmd->param[0]);
      
      if (!cmd)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Unknown command", -1);

      fArg = sArg = NULL;

      if (cmd->flagName > 0)
	{
	  /* First argument is needed and doesn't submited */
	  if (!world.curjob->curcmd->param[1] && cmd->flagName == 2)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			      "First argument is needed", -1);

	  sArg = world.curjob->curcmd->param[1];

	  if (cmd->flagArg > 0)
	    {
	      /* Second argument is needed and doesn't submited */
	      if (!world.curjob->curcmd->param[2] && cmd->flagArg == 2)
		ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
				  "Second argument is needed", -1);

	      fArg = world.curjob->curcmd->param[2];
	    }
	}

      ret = cmd->exec(fArg, sArg);
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}
