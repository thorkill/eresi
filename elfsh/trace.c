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

int    		cmd_trace()
{
  int		z;
  int	       	index;
  int		symnum;
  int		count;
  int		*argc;
  elfsh_Sym	*symtab;
  elfshsect_t	*sect;
  char		*sect_name;
  char		*func_name;
  char		buf[BUFSIZ + 1];
  char		bufex[BUFSIZ + 1];
  char		args[BUFSIZ + 1];
  char		argsproto[BUFSIZ + 1];
  char		argshexa[BUFSIZ + 1];
  FILE		*fp;
  elfshobj_t	*tobj;
  int		idx;
  u_int		off;
  int		err;
  elfsh_Addr	addr;
  elfsh_Sym	*dst;
  int		entries = 0;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  for (entries = 0; world.curjob->curcmd->param[entries] != NULL; entries++);

  elfsh_setup_hooks();

  vm_output(" .: Tracing Functions :.\n\n");

  /* Do we have symbols ? */
  if ((symtab = elfsh_get_symtab(world.curjob->current, &symnum)) != NULL && symnum > 0)
    {
      fp = fopen("/tmp/tracing.c", "w");

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

      int 	is_plt[symnum + 1];
      char 	*selfuncs[symnum + 1];

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

	  sect = elfsh_get_parent_section(world.curjob->current, symtab[index].st_value, NULL);

	  if (sect == NULL && symtab[index].st_shndx)
	    sect = elfsh_get_section_by_index(world.curjob->current,
					      symtab[index].st_shndx, NULL, NULL);

	  if (sect != NULL)
	    sect_name = elfsh_get_section_name(world.curjob->current, sect);
	  else
	    continue;
	  
	  /* Only global */
	  if (elfsh_get_symbol_bind(symtab + index) != 1)
	    continue;

	  /* Only plt & text */
	  if (!strstr(sect_name, "plt") && !strstr(sect_name, "text"))
	    continue;

	  func_name = elfsh_get_symbol_name(world.curjob->current, symtab + index);

	  /* A function name containing "." will not be compiled right with gcc 
	     XXX: resolv it */
	  if (strstr(func_name, "."))
	    continue;

	  if (entries > 0)
	    {
	      for (z = 0; z < entries; z++)
		{
		  if (!strcmp(world.curjob->curcmd->param[z], func_name))
		    break;
		}

	      if (z >= entries)
		continue;
	    }

	  off = elfsh_get_raw(sect);
	  off += symtab[index].st_value;
	  off -= sect->parent->rhdr.base;
	  off -= sect->shdr->sh_addr;
	    
	  /* Get args number table */
	  argc = elfsh_args_count(world.curjob->current, 
				  off,
				  symtab[index].st_value);
	  
	  /* Bug ? */
	  if (argc == NULL)
	    continue;

	  if (strstr(sect_name, "plt"))
	    is_plt[count] = 1;

	  selfuncs[count - 1] = elfsh_strdup(func_name);

	  argshexa[0] = 0;
	  argsproto[0] = 0;
	  args[0] = 0;
	  if (argc[0] != NULL)
	    {
	      snprintf(argshexa, BUFSIZ, "%%%%%s", "08x");
	      snprintf(argsproto, BUFSIZ, "char a%d[%d]", 0, argc[0]);
	      snprintf(args, BUFSIZ, "a%d", 0);
	      
	      for (z = 1; z < ELFSH_TRACE_MAX_ARGS && argc != NULL; z++)
		{
		  if (argc[z] == NULL)
		    break;

		  snprintf(buf, BUFSIZ, ", char a%d[%d]", z, argc[z]);

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
	  
	  snprintf(buf, BUFSIZ, "[%03d] %-24s arg:%d => %s\n", 
		   count, 
		   func_name,
		   z,
		   sect_name);

	  vm_output(buf);

	  count++;
	}

      fclose(fp);

      if (count == 1 || selfuncs == NULL || selfuncs[0] == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "No function found", (-1));

      snprintf(buf, BUFSIZ,
	       "gcc -c /tmp/tracing.c -o /tmp/tracing.o");

      vm_system(buf);

      tobj = elfsh_map_obj("/tmp/tracing.o");

      if (!tobj)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Failed to load relocatable file", (-1));

      idx = elfsh_inject_etrel(world.curjob->current, tobj);
	  
      if (idx < 0)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Failed to inject ET_REL with workspace", (-1));

      for (index = 0; index < symnum && selfuncs[index] != NULL; index++)
	{
	  snprintf(buf, BUFSIZ, "%s_trace", selfuncs[index]);
	  
	  
	  if (is_plt[index] == 1)
	    dst = elfsh_get_dynsymbol_by_name(world.curjob->current, buf);
	  else
	    dst = elfsh_get_symbol_by_name(world.curjob->current, buf);

	  if (dst == NULL)
	    {
	      elfsh_toggle_mode();

	      if (is_plt[index] == 1)
		dst = elfsh_get_dynsymbol_by_name(world.curjob->current, buf);
	      else
		dst = elfsh_get_symbol_by_name(world.curjob->current, buf);

	      elfsh_toggle_mode();
	    }

	  if (dst == NULL)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Failed to find trace function", (-1));

	  addr = dst->st_value;

	  printf("HIJACK = %s / %s\n", selfuncs[index], buf);

	  err = elfsh_hijack_function_by_name(world.curjob->current, 
					      ELFSH_HIJACK_TYPE_FLOW,
					      selfuncs[index], 
					      addr, NULL);

	  if (err < 0)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Failed to hijack a function", (-1));
	}
    }
  else
    {
      vm_output(" No symbols found\n");
    }

  vm_output("\n");

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int	vm_validtrace(elfshobj_t *file, elfsh_Sym *sym, 
		      elfshsect_t *sect, char *func_name,
		      char *sect_name)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);



  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


