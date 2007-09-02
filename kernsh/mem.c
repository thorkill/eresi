/*
** mem.c for kernsh
** 
** $Id: mem.c,v 1.8 2007-09-02 21:47:25 pouik Exp $
**
*/
#include "kernsh.h"
#include "libkernsh.h"

/* Handle the sys_call_table */
int		cmd_sct()
{
  int		ret;
  int		index;
  listent_t     *actual;
  list_t	*h;
  libkernshsyscall_t *sct;
  char		buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif

  XALLOC(__FILE__, 
	 __FUNCTION__, 
	 __LINE__,
	 h,
	 sizeof(list_t),
	 -1);

  list_init(h, "cmd_sct_list", ASPECT_TYPE_UNKNOW);

  ret = kernsh_sct(h);

  memset(buff, '\0', sizeof(buff));
  snprintf(buff, sizeof(buff), 
	   "%s\n\n",
	   revm_colorfieldstr("[+] SYS_CALL_TABLE"));
  revm_output(buff);

  for (index = 0, actual = h->head; 
       index < h->elmnbr; 
       index++, actual = actual->next)
    {
      sct = (libkernshsyscall_t *) actual->data;
      memset(buff, '\0', sizeof(buff));
      snprintf(buff, sizeof(buff),
	       "%s %-40s %s %s\n", 
	       revm_colornumber("id:%-10u", (unsigned int)index),
	       revm_colortypestr_fmt("%s", sct->name),
	       revm_colorstr("@"),
	       revm_coloraddress(XFMT, (elfsh_Addr) sct->addr));
      revm_output(buff);
      revm_endline();
      
    }
  
  revm_output("\n");

  list_destroy(h);

  
#if defined(USE_READLN)
  rl_callback_handler_install(revm_get_prompt(), revm_ln_handler);
  readln_column_update();
#endif

  if (ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get syscalltable", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Handle the idt */
int		cmd_idt()
{
  int		ret;
  int		index;
  listent_t     *actual;
  list_t	*h;
  libkernshint_t *dint;
  char		buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif

  XALLOC(__FILE__, 
	 __FUNCTION__, 
	 __LINE__,
	 h,
	 sizeof(list_t),
	 -1);

  list_init(h, "cmd_idt_list", ASPECT_TYPE_UNKNOW);

  ret = kernsh_idt(h);
        
  memset(buff, '\0', sizeof(buff));
  snprintf(buff, sizeof(buff), 
	   "%s\n\n",
	   revm_colorfieldstr("[+] IDT"));
  revm_output(buff);
  
  for (index = 0, actual = h->head; 
       index < h->elmnbr; 
       index++, actual = actual->next)
    {
      dint = (libkernshint_t *) actual->data;

      snprintf(buff, sizeof(buff),
	       "%s %-40s %s %s\n",
	       revm_colornumber("id:%-10u", (unsigned int)index),
	       revm_colortypestr_fmt("%s", dint->name),
	       revm_colorstr("@"),
	       revm_coloraddress(XFMT, (elfsh_Addr) dint->addr));
      revm_output(buff);
      revm_endline();
    }

  revm_output("\n");
  list_destroy(h);

#if defined(USE_READLN)
  rl_callback_handler_install(revm_get_prompt(), revm_ln_handler);
  readln_column_update();
#endif

  if (ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get idt", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Handle the gdt */
int		cmd_gdt()
{
  int		ret;
  int		index;
  int		i;
  listent_t     *actual;
  list_t	*h;
  libkernshsgdt_t *sgdt;
  char		buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif

  XALLOC(__FILE__, 
	 __FUNCTION__, 
	 __LINE__,
	 h,
	 sizeof(list_t),
	 -1);

  list_init(h, "cmd_gdt_list", ASPECT_TYPE_UNKNOW);

  ret = kernsh_gdt(h);
        
  memset(buff, '\0', sizeof(buff));
  snprintf(buff, sizeof(buff), 
	   "%s\n\n",
	   revm_colorfieldstr("[+] GDT"));
  revm_output(buff);
  
  for (i = 0, index = 0, actual = h->head; 
       index < h->elmnbr; 
       i+=8, index++, actual = actual->next)
    {
      sgdt = (libkernshsgdt_t *) actual->data;
      
      snprintf(buff, sizeof(buff),
	       "%s%s %s %s\n",
	       revm_coloraddress("%.8lX", (elfsh_Addr) sgdt->deb),
	       revm_coloraddress("%.8lX", (elfsh_Addr) sgdt->fin),
	       revm_colorstr("@"),
	       revm_coloraddress(XFMT, (elfsh_Addr) sgdt->addr));

      revm_output(buff);
      
      revm_endline();
    } 
	     
  revm_output("\n");

  list_destroy(h);

#if defined(USE_READLN)
  rl_callback_handler_install(revm_get_prompt(), revm_ln_handler);
  readln_column_update();
#endif

  if (ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get gdt", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Alloc contiguous kernel memory */
int		cmd_kalloc()
{
  char          *param;
  char		buff[BUFSIZ];
  unsigned long	addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif

  param = world.curjob->curcmd->param[0];

  if (param)
    {
      if (kernsh_alloc_contiguous(atoi(param), &addr))
	    {
	      revm_setvar_int("_", -1);
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			   "Cannot alloc contiguous memory", -1);
	    }
      memset(buff, '\0', sizeof(buff));
      snprintf(buff, sizeof(buff), 
	       "%s %s %s %s %s %s\n\n",
	       revm_colorfieldstr("ALLOCATE"),
	       revm_colornumber("%u", (unsigned int)atoi(param)),
	       revm_colorfieldstr("octet(s)"),
	       revm_colorfieldstr("OF CONTIGUOUS MEMORY"),
	       revm_colorstr("@"),
	       revm_coloraddress(XFMT, (elfsh_Addr) addr));
      revm_output(buff);
      revm_setvar_long("_", addr);
    }
   
#if defined(USE_READLN)
  rl_callback_handler_install(revm_get_prompt(), revm_ln_handler);
  readln_column_update();
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Free contiguous kernel memory */
int		cmd_kfree()
{
  char          *param;
  char		buff[BUFSIZ];
  unsigned long	addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif

  param = world.curjob->curcmd->param[0];

  if (param)
    {
      addr = strtoul( param, NULL, 16 );
      if(kernsh_free_contiguous(addr))
	{
	  revm_setvar_int("_", -1);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Cannot free contiguous memory", -1);
	}
      memset(buff, '\0', sizeof(buff));
      snprintf(buff, sizeof(buff), 
	       "%s %s %s %s\n\n",
	       revm_colorfieldstr("FREE"),
	       revm_colorfieldstr("CONTIGUOUS MEMORY"),
		   revm_colorstr("@"),
	       revm_coloraddress(XFMT, (elfsh_Addr) addr));
      revm_output(buff);
      revm_setvar_int("_", 0);
    }

#if defined(USE_READLN)
  rl_callback_handler_install(revm_get_prompt(), revm_ln_handler);
  readln_column_update();
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Alloc non contiguous kernel memory */
int		cmd_kallocnc()
{
  char          *param;
  char		buff[BUFSIZ];
  unsigned long	addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif

  param = world.curjob->curcmd->param[0];

  if (param)
    {
      if(kernsh_alloc_noncontiguous(atoi(param), &addr))
	{
	  revm_setvar_int("_", -1);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Cannot get alloc non contiguous memory", -1);
	}
      memset(buff, '\0', sizeof(buff));
      snprintf(buff, sizeof(buff), 
		   "%s %s %s %s %s %s\n\n",
	       revm_colorfieldstr("ALLOCATE"),
	       revm_colornumber("%u", (unsigned int)atoi(param)),
	       revm_colorfieldstr("octet(s)"),
	       revm_colorfieldstr("OF NON CONTIGUOUS MEMORY"),
	       revm_colorstr("@"),
	       revm_coloraddress(XFMT, (elfsh_Addr) addr));
      revm_output(buff);
      revm_setvar_long("_", addr);
    }
  
#if defined(USE_READLN)
  rl_callback_handler_install(revm_get_prompt(), revm_ln_handler);
  readln_column_update();
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Free non contiguous kernel memory */
int		cmd_kfreenc()
{
  char          *param;
  char		buff[BUFSIZ];
  unsigned long	addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif

  param = world.curjob->curcmd->param[0];

  if (param)
    {
      addr = strtoul( param, NULL, 16 );
      if(kernsh_free_noncontiguous(addr))
	{
	  revm_setvar_int("_", -1);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Cannot get free non contiguous memory", -1);
	}
      memset(buff, '\0', sizeof(buff));
      snprintf(buff, sizeof(buff), 
	       "%s %s %s %s\n\n",
	       revm_colorfieldstr("FREE"),
	       revm_colorfieldstr("NONCONTIGUOUS MEMORY"),
	       revm_colorstr("@"),
	       revm_coloraddress(XFMT, (elfsh_Addr) addr));
      revm_output(buff);
      revm_setvar_int("_", 0);
    }
 
#if defined(USE_READLN)
  rl_callback_handler_install(revm_get_prompt(), revm_ln_handler);
  readln_column_update();
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Handle kernel symbol */
int		cmd_ksym()
{
  char          *param;
  char		buff[BUFSIZ];
  unsigned long	addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif

  param = world.curjob->curcmd->param[0];

  if (param)
    {
      if(kernsh_get_addr_by_name(param, &addr, strlen(param)))
	{
	  revm_setvar_int("_", -1);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Cannot get symbol", -1);
	}
      memset(buff, '\0', sizeof(buff));
      snprintf(buff, sizeof(buff), 
	       "%s %s %s %s %s\n\n",
	       revm_colorfieldstr("SYMBOL"),
	       revm_colorstr(param),
	       revm_colorfieldstr("is"),
	       revm_colorstr("@"),
	       revm_coloraddress(XFMT, (elfsh_Addr) addr));
      revm_output(buff);
      revm_setvar_long("_", addr);
    }

#if defined(USE_READLN)
  rl_callback_handler_install(revm_get_prompt(), revm_ln_handler);
  readln_column_update();
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Handle loadable kernel module */
int		cmd_kmodule()
{
  int	ret;
  char  *param, *param2, *param3, *param4;
  char	buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif

  memset(buff, '\0', sizeof(buff));

  param = world.curjob->curcmd->param[0];
  param2 = world.curjob->curcmd->param[1];
  param3 = world.curjob->curcmd->param[2];
  param4 = world.curjob->curcmd->param[3];

  if (param)
    {
      if (param2 && !strcmp(param, "-l"))
	{
	  ret = kernsh_kload_module(param2);
	  if (ret == 0)
	    {
	      snprintf(buff, sizeof(buff), 
		       "Module %s is loaded\n\n",
		       revm_colorstr(param2));
	      revm_output(buff);
	    }
	    
	}
      else if (param2 && !strcmp(param, "-u"))
	{
	  ret = kernsh_kunload_module(param2);
	  if (ret == 0)
	    {
	      snprintf(buff, sizeof(buff), 
		       "Module %s is unloaded\n\n",
		       revm_colorstr(param2));
	      revm_output(buff);
	    }
	}
      else if (param2 && param3 && !strcmp(param, "-r"))
	{
	  ret = kernsh_relink_module(param2, param3, param4);
	  if (ret == 0)
	    {
	      if (param4 == NULL)
		param4 = param2;
	      snprintf(buff, sizeof(buff), 
		       "Module %s and %s is linked in %s\n\n",
		       revm_colorstr(param2),
		       revm_colorstr(param3),
		       revm_colorstr(param4));
	      revm_output(buff);
	    }
	}
      else if (param2 && param3 && param4 && !strcmp(param, "-i"))
	{
	  ret = kernsh_infect_module(param2, param3, param4);
	  if (ret == 0)
	    {
	      snprintf(buff, sizeof(buff), 
		       "%s have been replaced by %s in %s\n\n",
		       revm_colorstr(param3),
		       revm_colorstr(param4),
		       revm_colorstr(param2));
	      revm_output(buff);
	    }
	}
    }

#if defined(USE_READLN)
  rl_callback_handler_install(revm_get_prompt(), revm_ln_handler);
  readln_column_update();
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/* Make md5 ! */
int		cmd_kmd5()
{
  int	ret;
  revmlist_t    *actual, *second;
  revmobj_t     *obj;
  char          buff[BUFSIZ];
  elfsh_Addr    vaddr;
  int		fd;

  unsigned char md5buffer[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif

  memset(buff, '\0', sizeof(buff));

  vaddr = -1;
  
  actual = world.curjob->curcmd->disasm + 0;
  second = world.curjob->curcmd->disasm + 1;

  if (actual->rname)
    {
      /* Is it directly an addr ? */
      if (IS_VADDR(actual->rname))
	{
	  if (sscanf(actual->rname + 2, AFMT, &vaddr) != 1)
            PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Invalid virtual address requested",
			 -1);
	}
      /* Get the value of the object */
      else
	{
	  obj = revm_lookup_param(actual->rname);

	  switch (obj->type)
	    {
	    case ASPECT_TYPE_LONG:
	    case ASPECT_TYPE_CADDR:
	    case ASPECT_TYPE_DADDR:
	      vaddr = (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));
	      break;

	    case ASPECT_TYPE_INT:
	      vaddr = (obj->immed ? obj->immed_val.word : obj->get_obj(obj->parent));
	      break;
	    }
	}

      /* Create the md5 */
      ret = kernsh_md5(vaddr+actual->off, actual->size, md5buffer);
      if (ret <= 0)
	{
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Unable to make md5", -1);
	}
      snprintf(buff, sizeof(buff), 
	       "MD5 @ %s : \n\t%s\n\n",
	       revm_coloraddress(XFMT, (elfsh_Addr) vaddr),
	       revm_colorstr((char *)md5buffer));
      revm_output(buff);
      
      snprintf(buff, sizeof(buff),
	       "%s:%s:%s:%s:%s\n\n",
	       revm_coloraddress(XFMT, (elfsh_Addr) vaddr),
	       revm_colornumber("%u", libkernshworld.mem),
	       revm_colornumber("%u", ret),
	       revm_colornumber("%u", actual->off),
	       revm_colorstr((char *)md5buffer));
      revm_output(buff);

      snprintf(buff, sizeof(buff),
		   "0x%lx:%d:%d:%d:%s\n",
		   (unsigned long) vaddr,
		   libkernshworld.mem,
		   ret,
		   actual->off,
		   md5buffer);


      revm_setvar_str("_", (char *)md5buffer);

      export_var((char *)md5buffer, 0, 0, buff, 2);

      if (second != NULL && second->rname != NULL)
	{
	  XOPEN(fd, second->rname, O_CREAT|O_APPEND|O_RDWR, 0777, -1);
	  
	  memset(buff, '\0', sizeof(buff));
	  snprintf(buff, sizeof(buff),
		   "0x%lx:%d:%d:%d:%s\n",
		   (unsigned long) vaddr,
		   libkernshworld.mem,
		   ret,
		   actual->off,
		   md5buffer);
	  XWRITE(fd, buff, strlen(buff), -1);
	  XCLOSE(fd, -1);
	}
    }
  
#if defined(USE_READLN)
  rl_callback_handler_install(revm_get_prompt(), revm_ln_handler);
  readln_column_update();
#endif

  revm_endline();

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		extract_info(char *origbuf,
			     unsigned long *addr, 
			     int *mode, 
			     int *size, 
			     int *off, 
			     unsigned char *buffer,
			     size_t sizeb)
{
  int i;
  char *p;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  i = 0;
  p = (char *)strtok(origbuf, ":");
  while (p != NULL)
    {
      switch(i)
	{
	case 0 :
	  *addr = strtoul(p, NULL, 16);
	  break;
	
	case 1 :
	  *mode = atoi(p);
	  break;

	case 2 :
	  *size = atoi(p);
	  break;
	  
	case 3 :
	  *off = atoi(p);
	  break;
	  
	case 4 :
	  memset(buffer, '\0', sizeb);
	  memcpy(buffer, p, sizeb);
	  break;
	}
      
      p = (char *)strtok(NULL, ":");
      i++;
    }

  if (i != 5)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Check an md5sum */
int		cmd_kcmd5()
{
  int	ret, mode, size, off, origmode, val;
  char  buff[BUFSIZ], buff2[256];
  char	*param, *str;
  unsigned long addr;
  revmobj_t *obj;
  
  unsigned char md5buffer[BUFSIZ];
  unsigned char cmd5buffer[BUFSIZ];

  FILE *fd;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);


#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif
  
  val = 0;
  memset(buff, '\0', sizeof(buff));
  memset(buff2, '\0', sizeof(buff2));

  param = world.curjob->curcmd->param[0];
  
  if (param != NULL)
    {
      obj = revm_lookup_param(param);
      if (obj->type == ASPECT_TYPE_STR)
	{
	  str = (obj->immed ? obj->immed_val.str : 
		 obj->get_name(obj->root, obj->parent));
	  
	  printf("STR %s\n", str);
	  memcpy(buff2, str, sizeof(buff2));
	  printf("BUFF2 %s\n", buff2);  
	  param = buff2;
	}

      /* We must get md5 in a file ? */
      if (!strstr(param, ":"))
	{
	  fd = fopen(param, "r");
	  if (fd == NULL)
	    {
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			   "Unable to open file",
			   -1);
	    }

	  while(fgets(buff, sizeof(buff), fd) != NULL)
	    {
	      buff[strlen(buff) - 1] = '\0';
	      
	      if (buff[0] == '#')
		continue;

	      if (extract_info(buff, 
			       &addr, 
			       &mode, 
			       &size, 
			       &off, 
			       md5buffer, 
			       sizeof(md5buffer)))
		{
		  
		  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			       "Bad format", -1);
		}
	      /* Switch to the mode where the md5 has been done */
	      origmode = kernsh_get_mode();
	      kernsh_set_mode(mode);
	      
	      ret = kernsh_md5(addr+off, size, cmd5buffer);
	      
	      kernsh_set_mode(origmode);

	      memset(buff, '\0', sizeof(buff));
	      
	      if (!strncmp((const char *)md5buffer, 
			   (const char *)cmd5buffer, 
			   sizeof(md5buffer)))
		{
		  snprintf(buff, sizeof(buff),
			   "%s @ %s with size = %s and offset = %s\n",
			   revm_colorstr("MD5 MATCH @ !"),
			   revm_coloraddress(XFMT, (elfsh_Addr) addr),
			   revm_colornumber("%u", size),
			   revm_colornumber("%u", off));
		  revm_output(buff);
		}
	      else
		{
		  snprintf(buff, sizeof(buff),
			   "%s @ %s with size = %s and offset = %s\n %s != %s\n",
			   revm_colorstr("MD5 MISMATCH @ !"),
			   revm_coloraddress(XFMT, (elfsh_Addr) addr),
			   revm_colornumber("%u", size),
			   revm_colornumber("%u", off),
			   revm_colorstr((char *)md5buffer),
			   revm_colorstr((char *)cmd5buffer));
		  revm_output(buff);
		  val++;
		}
	    }

	  fclose(fd);
	}
      else
	{
	  if (extract_info(param, 
			   &addr, 
			   &mode, 
			   &size, 
			   &off, 
			   md5buffer, 
			   sizeof(md5buffer)))
	  {
	    
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			 "Bad format", -1);
	  }

	  origmode = kernsh_get_mode();
	  kernsh_set_mode(mode);

	  ret = kernsh_md5(addr+off, size, cmd5buffer);

	  kernsh_set_mode(origmode);

	  if (ret <= 0)
	    {
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			   "Unable to make md5", -1);
	    }

	  memset(buff, '\0', sizeof(buff));

	  if (!strncmp((const char *)md5buffer, 
		       (const char *)cmd5buffer, 
		       sizeof(md5buffer)))
	    {
	      snprintf(buff, sizeof(buff),
		       "%s @ %s with size = %s and offset = %s\n\n",
		       revm_colorstr("MD5 MATCH @ !"),
		       revm_coloraddress(XFMT, (elfsh_Addr) addr),
		       revm_colornumber("%u", size),
		       revm_colornumber("%u", off));
	      revm_output(buff);
	    }
	  else
	    {
	      snprintf(buff, sizeof(buff),
		       "%s @ %s with size = %s and offset = %s\n %s != %s\n\n",
		       revm_colorstr("MD5 MISMATCH @ !"),
		       revm_coloraddress(XFMT, (elfsh_Addr) addr),
		       revm_colornumber("%u", size),
		       revm_colornumber("%u", off),
		       revm_colorstr((char *)md5buffer),
		       revm_colorstr((char *)cmd5buffer));
	      revm_output(buff);
	      val++;
	    }
	}
    }

#if defined(USE_READLN)
  rl_callback_handler_install(revm_get_prompt(), revm_ln_handler);
  readln_column_update();
#endif

  revm_setvar_int("_", val);
  
  revm_endline();
    
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}
