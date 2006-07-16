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

int    		cmd_trace()
{
  int		z;
  int	       	index;
  int		symnum;
  int		count;
  int		*argcount;
  int		numargs;
  elfsh_Sym	*symtab;
  elfshsect_t	*sect;
  char		*sect_name;
  char		*func_name;
  FILE		*fp;
  elfshobj_t	*tobj;
  int		idx;
  u_int		off;
  int		err;
  elfsh_Addr	addr;
  elfsh_Sym	*dst;
  int		entries = 0;
  char		buf[BUFSIZ + 1];
  char 		tfname[] = "/tmp/tracingXXXXXX";
  int		osize = strlen(tfname)+3;
  char		rsofname[osize];
  char		rtfname[osize];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  for (entries = 0; world.curjob->curcmd->param[entries] != NULL; entries++);

  elfsh_setup_hooks();

  vm_output(" .: Tracing Functions :.\n\n");

  /* Do we have symbols ? */
  if ((symtab = elfsh_get_symtab(world.curjob->current, &symnum)) == NULL 
      || symnum == 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No symbols found", (-1));


  fp = vm_trace_init(tfname, rsofname, rtfname);

  if (fp == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot init tmp files", (-1));

  int 		is_plt[symnum + 1];
  char 		*selfuncs[symnum + 1];

  for (index = 0; index < symnum + 1; index++)
  {
    selfuncs[index] = NULL;
    is_plt[index] = 0;
  }

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
    if (elfsh_get_symbol_bind(symtab + index) != STB_GLOBAL
	|| (!strstr(sect_name, "plt") && !strstr(sect_name, "text")))
      continue;

    func_name = elfsh_get_symbol_name(world.curjob->current, symtab + index);

    /* A function name containing "." will not be compiled right with gcc 
       XXX: resolv it */
    if (strstr(func_name, "."))
      continue;

    /* Do we have choosed which function will be traced ? */
    if (entries > 0)
    {
      for (z = 0; z < entries; z++)
	if (!strcmp(world.curjob->curcmd->param[z], func_name))
	  break;

      /* This function should not be traced */
      if (z >= entries)
	continue;
    }

    off = (int) elfsh_get_raw(sect) + symtab[index].st_value;
    off -= (sect->parent->rhdr.base + sect->shdr->sh_addr);
	    
    /* Get args number table */
    argcount = elfsh_args_count(world.curjob->current, 
				off,
				symtab[index].st_value);
	  
    /* Bug ? */
    if (argcount == NULL)
      continue;

    is_plt[count] = strstr(sect_name, "plt") ? 1 : 0;

    selfuncs[count - 1] = elfsh_strdup(func_name);

    /* Add function handle into tmp file */
    numargs = vm_trace_add(fp, argcount, func_name);
	 
    /* Print some informations */
    snprintf(buf, BUFSIZ, "[%03d] %-24s arg:%d => %s\n", 
	     count, 
	     func_name,
	     numargs,
	     sect_name);

    vm_output(buf);

    count++;
  }

  fclose(fp);

#if __DEBUG_TRACE__
  printf("[DEBUG TRACE] COUNT = %d\n", count != 1 ? 1 : 0);
  printf("[DEBUG TRACE] SELFUNCS = %d\n", selfuncs ? 1 : 0);
  printf("[DEBUG TRACE] SELFUNCS[0] = %d\n", selfuncs[0] ? 1 : 0);
#endif

  /* If we didn't trace a function, we don't need to continue */
  if (count == 1 || selfuncs == NULL || selfuncs[0] == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No function found", (-1));

#if __DEBUG_TRACE__
  printf("[DEBUG TRACE] RENAME %s to %s\n", tfname, rtfname);
#endif

  if (rename(tfname, rtfname) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Rename failed", (-1));

  snprintf(buf, BUFSIZ, "gcc -c %s -o %s", 
	   rtfname, rsofname);

  /* Compile the tmp c file to create a recotable file to inject */
  vm_system(buf);

  /* Load the new relocatable file for ET_REL injection */
  tobj = elfsh_map_obj(rsofname);

  if (!tobj)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Failed to load relocatable file", (-1));

  /* Inject our handle functions into the traced binarie */
  idx = elfsh_inject_etrel(world.curjob->current, tobj);
	  
  if (idx < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Failed to inject ET_REL with workspace", (-1));

  /* Hijack functions with the new functions injected */
  for (index = 0; index < symnum && selfuncs[index] != NULL; index++)
  {
    snprintf(buf, BUFSIZ, "%s_trace", selfuncs[index]);	  

    /* Retrieve symbol */
    dst = (is_plt[index] == 1 ? 
      elfsh_get_dynsymbol_by_name(world.curjob->current, buf) :
      elfsh_get_symbol_by_name(world.curjob->current, buf));

    /* Try to toggle mode and retry, last chance */
    if (dst == NULL)
    {
      elfsh_toggle_mode();

      dst = (is_plt[index] == 1 ? 
	     elfsh_get_dynsymbol_by_name(world.curjob->current, buf) :
	     elfsh_get_symbol_by_name(world.curjob->current, buf));
	  
      elfsh_toggle_mode();
    }

    if (dst == NULL)
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Failed to find trace function", (-1));

    addr = dst->st_value;

#if __DEBUG_TRACE__
    printf("[DEBUG TRACE] HIJACK: %s / %s\n", selfuncs[index], buf);
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
  
  vm_output("\n");

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
