/*
** hash.c for kernsh
** 
** $Id: hash.c,v 1.4 2007-11-29 15:33:39 may Exp $
**
*/
#include "kernsh.h"
#include "libkernsh.h"

/* Make hash ! */
int		cmd_kmem_hash()
{
  int		i, fd, len, new_size;
  revmlist_t    *actual, *second;
  char          buff[BUFSIZ];
  char		buffhash[BUFSIZ];
  elfsh_Addr    vaddr;
  unsigned char *hashbuffer;
  char		*tmp;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  memset(buff, '\0', sizeof(buff));
  memset(buffhash, '\0', sizeof(buffhash));

  vaddr = -1;  
  actual = world.curjob->curcmd->disasm + 0;
  second = world.curjob->curcmd->disasm + 1;

  if (actual->rname)
    {
      kernsh_addrlen(actual, &vaddr, &len);

      hashbuffer = kernsh_hash(vaddr+actual->off, len, &new_size);

      if (hashbuffer == NULL)
	{
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Unable to make md5", -1);
	}

      i = 0;
      tmp = buffhash;
      while(hashbuffer[i] != '\0')
	{
	  sprintf(tmp, "%02x", hashbuffer[i]);
	  i++;
	  tmp += 2;
	}

      snprintf(buff, sizeof(buff), 
	       "MD5 @ %s : \n\t%s\n\n",
	       revm_coloraddress(XFMT, (elfsh_Addr) vaddr),
	       revm_colorstr((char *)buffhash));
      revm_output(buff);
      
      snprintf(buff, sizeof(buff),
	       "%s:%s:%s:%s:%s:%s\n\n",
	       revm_coloraddress(XFMT, (elfsh_Addr) vaddr),
	       revm_colornumber("%u", libkernshworld.mem),
	       revm_colornumber("%u", (int)config_get_data(LIBKERNSH_VMCONFIG_HASH)),
	       revm_colornumber("%u", new_size),
	       revm_colornumber("%u", actual->off),
	       revm_colorstr((char *)buffhash));
      revm_output(buff);

      snprintf(buff, sizeof(buff),
		   "0x%lx:%d:%d:%d:%d:%s\n",
		   (unsigned long) vaddr,
		   libkernshworld.mem,
	       (int)config_get_data(LIBKERNSH_VMCONFIG_HASH),
		   new_size,
		   actual->off,
		   buffhash);


      revm_setvar_str(REVM_VAR_RESULT, (char *)buffhash);

      export_var((char *)buffhash, 0, 0, buff, 2);

      if (second != NULL && second->rname != NULL)
	{
	  XOPEN(fd, second->rname, O_CREAT|O_APPEND|O_RDWR, 0777, -1);
	  
	  memset(buff, '\0', sizeof(buff));
	  snprintf(buff, sizeof(buff),
		   "0x%lx:%d:%d:%d:%d:%s\n",
		   (unsigned long) vaddr,
		   libkernshworld.mem,
		   (int)config_get_data(LIBKERNSH_VMCONFIG_HASH),
		   new_size,
		   actual->off,
		   buffhash);
	  XWRITE(fd, buff, strlen(buff), -1);
	  XCLOSE(fd, -1);
	}
    }

  XFREE(__FILE__, __FUNCTION__, __LINE__, hashbuffer);

  revm_endline();
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		extract_info(char *origbuf,
			     unsigned long *addr, 
			     int *mode, 
			     int *type,
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
	  *type = atoi(p);
	  break;

	case 3 :
	  *size = atoi(p);
	  break;
	  
	case 4 :
	  *off = atoi(p);
	  break;
	  
	case 5 :
	  memset(buffer, '\0', sizeb);
	  memcpy(buffer, p, sizeb);
	  break;
	}
      
      p = (char *)strtok(NULL, ":");
      i++;
    }

  if (i != 6)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Check an md5sum */
int		cmd_kmem_chash()
{
  int	i, ret, mode, stype, type, nsize, size, off, origmode, val;
  char  buff[BUFSIZ], buff2[256];
  char	*param, *str;
  unsigned long addr;
  revmexpr_t *expr;
  revmobj_t *obj;

  unsigned char origbuffer[BUFSIZ];
  char	buffhash[BUFSIZ];
  unsigned char *hashbuffer;
  char *tmp;

  FILE *fd;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  val = 0;
  memset(buff, '\0', sizeof(buff));
  memset(buff2, '\0', sizeof(buff2));

  param = world.curjob->curcmd->param[0];

  stype = (int)config_get_data(LIBKERNSH_VMCONFIG_HASH);

  if (param != NULL)
    {
      expr = revm_lookup_param(param);
      obj = expr->value;

      if (obj->otype->type == ASPECT_TYPE_STR)
	{
	  str = (obj->immed ? obj->immed_val.str : 
		 obj->get_name(obj->root, obj->parent));
	  
	  memcpy(buff2, str, sizeof(buff2));
	  param = buff2;
	}

      /* We must get hash in a file ? */
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
			       &type,
			       &size, 
			       &off, 
			       origbuffer, 
			       sizeof(origbuffer)))
		{
		  
		  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			       "Bad format", -1);
		}
	      /* Switch to the mode where the md5 has been done */
	      origmode = kernsh_get_mode();
	      kernsh_set_mode(mode);

	      config_update_key(LIBKERNSH_VMCONFIG_HASH, (void *)type);
	      hashbuffer = kernsh_hash(addr+off, size, &nsize);	      
	      	      
	      kernsh_set_mode(origmode);

	      i = 0;
	      tmp = buffhash;
	      while((i < sizeof(buffhash)) && (hashbuffer[i] != '\0'))
		{
		  sprintf(tmp, "%02x", hashbuffer[i]);
		  i++;
		  tmp += 2;
		}

	      memset(buff, '\0', sizeof(buff));
	      
	      if (!strncmp((const char *)origbuffer, 
			   (const char *)buffhash, 
			   sizeof(origbuffer)))
		{
		  snprintf(buff, sizeof(buff),
			   "%s @ %s with size = %s and offset = %s\n",
			   revm_colorstr("HASH MATCH @ !"),
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
			   revm_colorstr("HASH MISMATCH @ !"),
			   revm_coloraddress(XFMT, (elfsh_Addr) addr),
			   revm_colornumber("%u", size),
			   revm_colornumber("%u", off),
			   revm_colorstr((char *)origbuffer),
			   revm_colorstr((char *)buffhash));
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
			   &type,
			   &size, 
			   &off, 
			   origbuffer, 
			   sizeof(origbuffer)))
	  {
	    
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			 "Bad format", -1);
	  }

	  origmode = kernsh_get_mode();
	  kernsh_set_mode(mode);
	  config_update_key(LIBKERNSH_VMCONFIG_HASH, (void *)type);
	  hashbuffer = kernsh_hash(addr+off, size, &nsize);

	  kernsh_set_mode(origmode);
	  i = 0;
	  tmp = buffhash;
	  while((i < sizeof(buffhash)) && (hashbuffer[i] != '\0'))
	    {
	      sprintf(tmp, "%02x", hashbuffer[i]);
	      i++;
	      tmp += 2;
	    }

	  if (hashbuffer == NULL)
	    {
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			   "Unable to make hash", -1);
	    }

	  memset(buff, '\0', sizeof(buff));

	  if (!strncmp((const char *)origbuffer, 
		       (const char *)buffhash, 
		       sizeof(origbuffer)))
	    {
	      snprintf(buff, sizeof(buff),
		       "%s @ %s with size = %s and offset = %s\n\n",
		       revm_colorstr("HASH MATCH @ !"),
		       revm_coloraddress(XFMT, (elfsh_Addr) addr),
		       revm_colornumber("%u", size),
		       revm_colornumber("%u", off));
	      revm_output(buff);
	    }
	  else
	    {
	      snprintf(buff, sizeof(buff),
		       "%s @ %s with size = %s and offset = %s\n %s != %s\n\n",
		       revm_colorstr("HASH MISMATCH @ !"),
		       revm_coloraddress(XFMT, (elfsh_Addr) addr),
		       revm_colornumber("%u", size),
		       revm_colornumber("%u", off),
		       revm_colorstr((char *)origbuffer),
		       revm_colorstr((char *)buffhash));
	      revm_output(buff);
	      val++;
	    }
	}
    }

  config_update_key(LIBKERNSH_VMCONFIG_HASH, (void *)stype);

  revm_setvar_int(REVM_VAR_RESULT, val);  

  revm_endline();
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}
