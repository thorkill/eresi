/*
** memory.c for libkernsh
**
** $Id: memory.c,v 1.5 2007-07-31 12:31:54 pouik Exp $
**
*/
#include "libkernsh.h"
#include "libaspect.h"

/* Open kernel memory						*/
/* Configure : Device = "/dev/kmem", "/dev/mem", "/proc/kcore"	*/
/*          Mode = "read", "write"				*/
int kernsh_openmem()
{
  int ret, mmap, mmap_size;

  char *device, *mode;
  u_int         dim[4];
  vector_t      *mem;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__
  printf("OPENMEM\n");
#endif

  libkernshworld.kernel_start = (unsigned long) 
    config_get_data(LIBKERNSH_VMCONFIG_KERNEL_START);
  
  libkernshworld.kernel_end = (unsigned long) 
    config_get_data(LIBKERNSH_VMCONFIG_KERNEL_END);


#if __DEBUG_KERNSH__
  printf("KERNEL_START 0x%lx\n", libkernshworld.kernel_start);
  printf("KERNEL_END   0x%lx\n", libkernshworld.kernel_end);
#endif

  if (libkernshworld.os == LIBKERNSH_OS_LINUX_2_6 || 
      libkernshworld.os == LIBKERNSH_OS_LINUX_2_4)
    {
      /* Check configure device, mode etc */

      device = (char *) config_get_data(LIBKERNSH_VMCONFIG_DEVICE);
      mode = (char *) config_get_data(LIBKERNSH_VMCONFIG_MODE);
      mmap = (int) config_get_data(LIBKERNSH_VMCONFIG_MMAP);
      mmap_size = (int) config_get_data(LIBKERNSH_VMCONFIG_MMAP_SIZE);

      if (device == NULL)
	{
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Device is NULL", -1);
	}
      
      if (mode == NULL)
	{
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Mode is NULL", -1);
	}
      
      /* check device and mode */
      if (!strcmp(device, LIBKERNSH_STRING_DEVICE_KMEM))
	{
	  libkernshworld.device = LIBKERNSH_DEVICE_KMEM;
	}
      else if (!strcmp(device, LIBKERNSH_STRING_DEVICE_MEM))
	{
	  libkernshworld.device = LIBKERNSH_DEVICE_MEM;
	}
      else if (!strcmp(device, LIBKERNSH_STRING_DEVICE_KCORE))
	{
	  libkernshworld.device = LIBKERNSH_DEVICE_KCORE;
	}
      else
	{
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Invalid device", -1);
	}  
  
      if (!strcmp(mode, "read"))
	{
	  libkernshworld.fdmode = O_RDONLY;
	  libkernshworld.protmode = PROT_READ;
	  libkernshworld.flagsmode = MAP_PRIVATE;
	}
      else if (!strcmp(mode, "write"))
	{
	  libkernshworld.fdmode = O_RDWR;
	  libkernshworld.protmode = PROT_WRITE;
	  libkernshworld.flagsmode = MAP_SHARED;
	}
      else
	{
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Invalid mode", -1);
	}
      
      libkernshworld.mmap = mmap;
      libkernshworld.mmap_size = mmap_size;

#if  __DEBUG_KERNSH__     
      printf("SYSTEM %s\n", (char *) config_get_data(LIBKERNSH_VMCONFIG_SYSTEMMAP));
#endif

    }
  else if (libkernshworld.os == LIBKERNSH_OS_NETBSD)
    {

    }
  else if (libkernshworld.os == LIBKERNSH_OS_FREEBSD)
    {
      
    }
  else
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "OS not supported", -1);
    }

  if (libkernshworld.open)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Memory already open, you must close it !", -1);
    }

  mem = aspect_vector_get("openmem");
  dim[0] = libkernshworld.arch;
  dim[1] = libkernshworld.os;
  dim[2] = libkernshworld.device;

  fct = aspect_vectors_select(mem, dim);
  
  ret = fct();

  if(ret == 0)
    {
      libkernshworld.open = 1;
      kernsh_info();
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/* Close kernel memory */
int kernsh_closemem()
{
  int ret;
  u_int         dim[4];
  vector_t      *mem;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!libkernshworld.open)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Memory not open !", -1);
    }

  mem = aspect_vector_get("closemem");
  dim[0] = libkernshworld.arch;
  dim[1] = libkernshworld.os;
  dim[2] = libkernshworld.device;
  
  fct = aspect_vectors_select(mem, dim);
  
  ret = fct();

  libkernshworld.open = 0;
  libkernshworld.idt_base = 0;
  libkernshworld.idt_limit = 0;
  libkernshworld.sct = 0;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/* Read kernel memory */
int kernsh_readmem(unsigned long offset, void *buf, int size)
{
  int ret;
  u_int         dim[4];
  vector_t      *mem;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!libkernshworld.open)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Memory not open !", -1);
    }

  mem = aspect_vector_get("readmem");
  dim[0] = libkernshworld.arch;
  dim[1] = libkernshworld.os;
  dim[2] = libkernshworld.device;
  
  fct = aspect_vectors_select(mem, dim);
  
  ret = fct(offset, buf, size);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/* Write in kernel memory */
int kernsh_writemem(unsigned long offset, void *buf, int size)
{
  int ret;
  u_int         dim[4];
  vector_t      *mem;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!libkernshworld.open)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Memory not open !", -1);
    }

  mem = aspect_vector_get("writemem");
  dim[0] = libkernshworld.arch;
  dim[1] = libkernshworld.os;
  dim[2] = libkernshworld.device;
  
  fct = aspect_vectors_select(mem, dim);
  
  ret = fct(offset, buf, size);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}
