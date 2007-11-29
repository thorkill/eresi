/*
** md5.c for kernsh
** 
** $Id: md5.c,v 1.3 2007-11-29 14:01:55 may Exp $
**
*/
#include "kernsh.h"
#include "libkernsh.h"

/* Make md5 ! */
int		cmd_kmd5()
{
  int		ret;
  revmlist_t    *actual, *second;
  revmobj_t     *obj;
  char          buff[BUFSIZ];
  elfsh_Addr    vaddr;
  int		fd;
  unsigned char md5buffer[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  revm_callback_handler_remove();
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

	  switch (obj->otype->type)
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
  
  revm_callback_handler_install(revm_get_prompt(), revm_ln_handler);
  revm_column_update();
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
  revm_callback_handler_remove();
  
  val = 0;
  memset(buff, '\0', sizeof(buff));
  memset(buff2, '\0', sizeof(buff2));

  param = world.curjob->curcmd->param[0];
  
  if (param != NULL)
    {
      obj = revm_lookup_param(param);
      if (obj->otype->type == ASPECT_TYPE_STR)
	{
	  str = (obj->immed ? obj->immed_val.str : 
		 obj->get_name(obj->root, obj->parent));
	  
	  memcpy(buff2, str, sizeof(buff2));
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
		  revm_endline();
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
		  revm_endline();
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

  revm_callback_handler_install(revm_get_prompt(), revm_ln_handler);
  revm_column_update();
  revm_setvar_int("_", val);  
  revm_endline();
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}
