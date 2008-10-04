/*
** @file autotypes.c
** @ingroup libkernsh_common
**
*/
#include "libkernsh.h"

int kernsh_autotypes()
{
  int ret;
  unsigned int         dim[3];
  vector_t      *autotypes;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!libkernshworld.open)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Memory not open !", -1);
    }

  autotypes = aspect_vector_get(LIBKERNSH_VECTOR_NAME_AUTOTYPES);
  dim[0] = libkernshworld.arch;
  dim[1] = libkernshworld.os;
  
  fct = aspect_vectors_select(autotypes, dim);
  
  ret = fct();

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int kernsh_autotypes_linux_2_6()
{
  int ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  ret = kernsh_autotask_linux_2_6();
  if (ret)
    {

    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_autotask_offsetname_linux_2_6(char *buffer, size_t size)
{
  char *p;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  p = kernsh_find_pattern(buffer, 
			  size, 
			  "\x73\x77\x61\x70\x70\x65\x72", 
			  7);

  if (p == NULL)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Can't find pattern !", -1); 
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (int)p - (int)buffer);
}

int kernsh_autotask_offsetlist_linux_2_6(char *buffer, size_t size)
{
  int i, count, offset;
  unsigned long first_addr, second_addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  count = 0;
  offset = -1;
  first_addr = second_addr = -1;

  for (i = 0; i < size && offset == -1; i += 4)
    {
      first_addr = *(unsigned long *)(buffer+i);

      if (first_addr == second_addr)
	{
	  count++;
	}

      if (count == 2)
	{
	  offset = i;
	}

      if (first_addr > libkernshworld.kernel_start &&
	  first_addr < libkernshworld.kernel_end)
	{
	  second_addr = first_addr;
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, offset);
}

int kernsh_autotask_offsetnext_linux_2_6(char *buffer, size_t size)
{
  int i, offset, find1, find2, diffaddr, jmpfiveaddr;
  unsigned long first_addr,second_addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  diffaddr = 0x10;
  jmpfiveaddr = 0x14;
  offset = find1 = find2 = second_addr = first_addr = 0;

  for(i = 0; i < size; i += 4)
    {
      second_addr = first_addr;
      first_addr = *(unsigned long *)(buffer+i);
    
      if ((first_addr == second_addr) && 
	 (first_addr > libkernshworld.kernel_start))
	{
	  find2 = find1;
	  find1 = i;
	  
	  if((find1-find2) == diffaddr)
	    {
#if __DEBUG_KERNSH__                          
	      printf("[+] POS = 0x%x\n",find2+jmpfiveaddr);
#endif
	      offset = find2+jmpfiveaddr;
	      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, offset);
	    }
	}
    }
    
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);
}

int kernsh_autotask_offsetpid_linux_2_6(char *buffer, size_t size)
{
  int i, offset;
  unsigned long first_addr, second_addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  offset = first_addr = second_addr = -1;

  for (i = 0; i < size && offset == -1 ; i += 4)
    {
      first_addr = *(unsigned long *)(buffer + i);
      

      if(first_addr == 0x1)
	second_addr = first_addr;

      if(first_addr == second_addr)
	offset = i;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, offset);
}

int kernsh_autotask_offsetuid_linux_2_6(char *buffer, size_t size)
{
  unsigned long first_addr, second_addr;
  int i, offset, count;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  offset = -1;
  count = first_addr = second_addr = 0;

  for(i = 0; i < size && offset == -1; i += 4)
    {
      if(first_addr != 0)
	second_addr = first_addr;

      first_addr = *(unsigned long *)(buffer + i);
      if(first_addr == second_addr)
	{
#if __DEBUG_KERNSH__
	  printf("EGAL %d %d\n", i, count);
#endif
	  count++;
	}
      
      if(count == 6)
	{
#if __DEBUG_KERNSH__                
	  printf("COUNT %d\n", i);
#endif
	  offset = i;
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, offset);
}

int kernsh_autotask_linux_2_6()
{
  unsigned long init_task, lst_addr, current_addr;
  char buffer[1024];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  libkernshworld.typetask.dectask.dec_list = 28;
  libkernshworld.typetask.dectask.dec_pid = 4;
  libkernshworld.typetask.dectask.dec_uid = 16;

  kernsh_get_addr_by_name("init_task", &init_task, strlen("init_task"));
  
  kernsh_readmem(init_task, buffer, sizeof(buffer));

  libkernshworld.typetask.offset_name = 
    kernsh_autotask_offsetname_linux_2_6(buffer, sizeof(buffer));

  if (libkernshworld.typetask.offset_name == -1)
    {

    }

#if __DEBUG_KERNSH__
  printf("OFFSET NAME %d\n", libkernshworld.typetask.offset_name);
#endif

  kernsh_readmem(init_task+40, buffer, 200);
  
  libkernshworld.typetask.offset_list = 
    kernsh_autotask_offsetlist_linux_2_6(buffer, 200);

  libkernshworld.typetask.offset_list += 
    libkernshworld.typetask.dectask.dec_list;

#if __DEBUG_KERNSH__
  printf("OFFSET LIST %d\n", libkernshworld.typetask.offset_list);
#endif

  kernsh_readmem(init_task + libkernshworld.typetask.offset_list,
		 &lst_addr,
		 sizeof(unsigned long));

  kernsh_readmem(lst_addr, buffer, 256);

#if __DEBUG_KERNSH__
  printf("LST_ADDR 0x%lx\n", lst_addr);
#endif

  libkernshworld.typetask.offset_next = 
    kernsh_autotask_offsetnext_linux_2_6(buffer, 200);

#if __DEBUG_KERNSH__
  printf("OFFSET_NEXT %d\n", libkernshworld.typetask.offset_next);
#endif


  kernsh_readmem(lst_addr + libkernshworld.typetask.offset_next,
		 &current_addr,
		 sizeof(unsigned long));

#if __DEBUG_KERNSH__
  printf("CURRENT_ADDR 0x%lx\n", current_addr);
#endif

  kernsh_readmem(current_addr + libkernshworld.typetask.offset_list,
		 buffer,
		 sizeof(buffer));

  libkernshworld.typetask.offset_pid = 
    kernsh_autotask_offsetpid_linux_2_6(buffer, 200);

  libkernshworld.typetask.offset_pid += 
    libkernshworld.typetask.offset_list + 
    libkernshworld.typetask.dectask.dec_pid;

#if __DEBUG_KERNSH__
  printf("OFFSET PID %d\n", libkernshworld.typetask.offset_pid);
#endif

  kernsh_readmem(current_addr + libkernshworld.typetask.offset_pid,
		 buffer,
		 sizeof(buffer));

  libkernshworld.typetask.offset_uid = 
    kernsh_autotask_offsetuid_linux_2_6(buffer, 500);

  libkernshworld.typetask.offset_uid +=
    libkernshworld.typetask.offset_pid + libkernshworld.typetask.dectask.dec_uid;
#if __DEBUG_KERNSH__
  printf("OFFSET UID %d\n", libkernshworld.typetask.offset_uid);
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_autotypes_linux_2_4()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_autotask_linux_2_4()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_autotypes_netbsd()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_autotask_netbsd()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_autotypes_freebsd()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_autotask_freebsd()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
