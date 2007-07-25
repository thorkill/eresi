/*
** sct.c for libkernsh
**
** $Id: sct.c,v 1.2 2007-07-25 21:55:06 pouik Exp $
**
*/
#include "libkernsh.h"

int kernsh_sct()
{
  int ret;
  u_int         dim[3];
  vector_t      *sct;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  
  sct = aspect_vector_get("sct");
  dim[0] = libkernshworld.type;
  dim[1] = libkernshworld.os;

  fct = aspect_vectors_select(sct, dim);

  ret = fct();

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


int kernsh_sct_linux()
{
  int i;
  char buff[512];

  libkernshsyscall_t syscall;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  printf("SCT LINUX\n");

  if (kernsh_is_static_mode())
    {
      //elfsh_reverse_metasym_by_name
    }
  else 
    {
      if (!libkernshworld.open)
	{
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Memory not open !", -1);
	}

      /*
      memset(buff, '\0', sizeof(buff));
      snprintf(buff, sizeof(buff), 
	       "%s\n\n",
	       revm_colorfieldstr("[+] SYS_CALL_TABLE"));
      revm_output(buff);
      */

      for (i = 0; 
	   i < (int) config_get_data(LIBKERNSH_VMCONFIG_NB_SYSCALLS); 
	   i++)
	{
	  memset(&syscall, '\0', sizeof(syscall));
	  kernsh_readmem(libkernshworld.sct + sizeof(unsigned long) * i, 
			 &syscall.addr, 
			 sizeof(unsigned long));
	  
	  kernsh_resolve_systemmap(syscall.addr, syscall.name, sizeof(syscall.name));
	  /* memset(buff, '\0', sizeof(buff));
	  snprintf(buff, sizeof(buff),
		   "%s %-40s %s %s\n", 
		   revm_colornumber("id:%-10u", (unsigned int)i),
		   revm_colortypestr_fmt("%s", syscall.name),
		   revm_colorstr("@"),
		   revm_coloraddress(XFMT, (elfsh_Addr) syscall.addr));
	  revm_output(buff);
	  revm_endline();*/
	}
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0); 
}

int kernsh_syscall(int num, int argc, unsigned int argv[])
{
  int __ret = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (argc > 5 || argc < 0)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "invalid number of arguments", -1);
    }

  switch(argc) 
    {
    case 0 :
      SYSCALL0(num);
      break;
    case 1 :
      SYSCALL1(num, argv[0]);
      break;
    case 2:
      SYSCALL2( num, argv[0], argv[1] );
      break;
    case 3:
      SYSCALL3 (num, argv[0], argv[1], argv[2] );
      break;
    case 4:
      SYSCALL4 (num, argv[0], argv[1], argv[2], argv[3] );
      break;
    case 5:
      SYSCALL5 (num, argv[0], argv[1], argv[2], argv[3], argv[4] );
      break;
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, __ret);
}

