/*
** memory.c for libkernsh
**
** $Id: memory.c,v 1.1 2007-07-25 19:53:01 pouik Exp $
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

  revm_output("OPENMEM\n");

  if (libkernshworld.os == LIBKERNSH_OS_LINUX_2_6 || 
      libkernshworld.os == LIBKERNSH_OS_LINUX_2_4)
    {
      //CHECK configure device, mode etc

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
      
      //check device and mode
      if (!strcmp(device, "/dev/kmem"))
	{
	  libkernshworld.device = LIBKERNSH_DEVICE_KMEM;
	}
      else if (!strcmp(device, "/dev/mem"))
	{
	  libkernshworld.device = LIBKERNSH_DEVICE_MEM;
	}
      else if (!strcmp(device, "/proc/kcore"))
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
      
      printf("SYSTEM %s\n", (char *) config_get_data(LIBKERNSH_VMCONFIG_SYSTEMMAP));

    }
  else if (libkernshworld.os == LIBKERNSH_OS_NETBSD)
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
  dim[0] = libkernshworld.type;
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
  dim[0] = libkernshworld.type;
  dim[1] = libkernshworld.os;
  dim[2] = libkernshworld.device;
  
  fct = aspect_vectors_select(mem, dim);
  
  ret = fct();
  
  kernsh_set_static_mode();

  libkernshworld.open = 0;

  XFREE(__FILE__, __FUNCTION__, __LINE__, libkernshworld.systemmap);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

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
  dim[0] = libkernshworld.type;
  dim[1] = libkernshworld.os;
  dim[2] = libkernshworld.device;
  
  fct = aspect_vectors_select(mem, dim);
  
  ret = fct(offset, buf, size);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

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
  dim[0] = libkernshworld.type;
  dim[1] = libkernshworld.os;
  dim[2] = libkernshworld.device;
  
  fct = aspect_vectors_select(mem, dim);
  
  ret = fct(offset, buf, size);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}
