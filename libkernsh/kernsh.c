/*
** kernsh.c for libkernsh : initialisation, get_raw and mode switch
**
** $Id: kernsh.c,v 1.1 2007-07-25 19:53:01 pouik Exp $
**
*/
#include "libkernsh.h"
#include "libaspect-profiler.h"

libkernshworld_t libkernshworld;

int kernsh_init_i386(char *os, char *release)
{
  char buffer[256];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  memset(&libkernshworld, '\0', sizeof(libkernshworld_t));

  libkernshworld.type = LIBKERNSH_ARCH_I386;

  /* By default we use static kernel */
  kernsh_set_static_mode();
  
  config_add_item(LIBKERNSH_VMCONFIG_NOSTATIC,
		  CONFIG_TYPE_INT,
		  CONFIG_MODE_RW,
		  (void *) 0);
  
  if (!strcmp(os, "Linux"))
    {
      if (!strncmp(release, "2.6", 3))
	{
	  libkernshworld.os = LIBKERNSH_OS_LINUX_2_6;
	}
      else if (!strncmp(release, "2.4", 3))
	{
	  libkernshworld.os = LIBKERNSH_OS_LINUX_2_4;
	}
      else 
	{
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Release not supported", -1);
	}
 
      config_add_item(LIBKERNSH_VMCONFIG_DEVICE,
		      CONFIG_TYPE_STR,
		      CONFIG_MODE_RW,
		      (char *)"/dev/mem");
      
      config_add_item(LIBKERNSH_VMCONFIG_MODE,
		      CONFIG_TYPE_STR,
		      CONFIG_MODE_RW,
		      (char *)"write");

      config_add_item(LIBKERNSH_VMCONFIG_MMAP,
		      CONFIG_TYPE_INT,
		      CONFIG_MODE_RW,
		      (void *) 1);
      
      config_add_item(LIBKERNSH_VMCONFIG_MMAP_SIZE,
		      CONFIG_TYPE_INT,
		      CONFIG_MODE_RW,
		      (void *)(200 * 1024 * 1024));

      memset(buffer, '\0', sizeof(buffer));
      snprintf(buffer, sizeof(buffer), "%s-%s", LIBKERNSH_DEFAULT_LINUX_KERNEL, release);
      
      config_add_item(LIBKERNSH_VMCONFIG_KERNEL,
		      CONFIG_TYPE_STR,
		      CONFIG_MODE_RW,
		      (void *) buffer);
      
      config_add_item(LIBKERNSH_VMCONFIG_KERNELGZ,
		      CONFIG_TYPE_STR,
		      CONFIG_MODE_RW,
		      (void *) LIBKERNSH_DEFAULT_LINUX_KERNELGZ);
      
      config_add_item(LIBKERNSH_VMCONFIG_KERNELELF,
		      CONFIG_TYPE_STR,
		      CONFIG_MODE_RW,
		      (void *) LIBKERNSH_DEFAULT_LINUX_KERNELELF);
      
      config_add_item(LIBKERNSH_VMCONFIG_STORAGE_PATH,
		      CONFIG_TYPE_STR,
		      CONFIG_MODE_RW,
		      (void *) LIBKERNSH_DEFAULT_STORAGE_PATH);
      
      memset(buffer, '\0', sizeof(buffer));
      snprintf(buffer, sizeof(buffer), "%s-%s", LIBKERNSH_DEFAULT_LINUX_MAP, release);
  
      config_add_item(LIBKERNSH_VMCONFIG_SYSTEMMAP,
		      CONFIG_TYPE_STR,
		      CONFIG_MODE_RW,
		      (char *) buffer);
      
      config_add_item(LIBKERNSH_VMCONFIG_GZIP,
		      CONFIG_TYPE_STR,
		      CONFIG_MODE_RW,
		      (char *) "gzip");
      
      config_add_item(LIBKERNSH_VMCONFIG_OBJCOPY,
		      CONFIG_TYPE_STR,
		      CONFIG_MODE_RW,
		      (char *) "objcopy");

      config_add_item(LIBKERNSH_VMCONFIG_NB_SYSCALLS,
		      CONFIG_TYPE_INT,
		      CONFIG_MODE_RW,
		      (void *) LIBKERNSH_DEFAULT_LINUX_NB_SYSCALLS);

      config_add_item(LIBKERNSH_VMCONFIG_NIL_SYSCALL,
		      CONFIG_TYPE_INT,
		      CONFIG_MODE_RW,
		      (void *) LIBKERNSH_DEFAULT_LINUX_NIL_SYSCALL);

      libkernshworld.kernel_start = LIBKERNEL_I386_LINUX_START;
      libkernshworld.kernel_end = LIBKERNEL_I386_LINUX_END;
      libkernshworld.page_offset = LIBPAGE_I386_LINUX_OFFSET;
      libkernshworld.page_max = LIBPAGE_I386_LINUX_MAX;
    }
  else
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Os not supported", -1);
    }
  


  XALLOC(__FILE__, __FUNCTION__, __LINE__, libkernshworld.release, 
	 strlen(release) + 1, -1);
  
  if (memcpy(libkernshworld.release, release, strlen(release)) == NULL)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Memcpy failed", -1);
    }

  printf("RELEASE %s\n", libkernshworld.release);
  
  kernsh_init_vectors();
  kernsh_register_vectors();

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


int kernsh_del_i386()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  XFREE(__FILE__, __FUNCTION__, __LINE__, libkernshworld.release);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_config()
{
  char          buff[BUFSIZ];
  char          *home;
  int           ret;
  static int    done = 0;
  revmargv_t    *new;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (done)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  
  ret = -1;
  home = getenv("HOME");
  if (home)
    {
      snprintf(buff, sizeof(buff), "%s/%s", home, LIBKERNSH_CONFIG);
      XALLOC(__FILE__, __FUNCTION__, __LINE__,
             new, sizeof(revmargv_t), -1);
      memset(new, 0, sizeof(revmargv_t));
      world.curjob->curcmd = new;
      world.curjob->curcmd->param[0] = buff;
      ret = cmd_source();
      world.curjob->curcmd = NULL;
      XFREE(__FILE__, __FUNCTION__, __LINE__,new);
    }
  if (ret < 0)
    revm_output("\n [*] No configuration in ~/" LIBKERNSH_CONFIG " \n\n");
  done = 1;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_is_mem_mode()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		(libkernshworld.mem == LIBKERNSH_MEM_MODE));
}

int kernsh_set_mem_mode()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  libkernshworld.mem= LIBKERNSH_MEM_MODE;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_is_static_mode()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		(libkernshworld.mem == LIBKERNSH_STATIC_MODE));
}

int kernsh_set_static_mode()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  libkernshworld.mem= LIBKERNSH_STATIC_MODE;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_is_nostatic_mode()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		((int) config_get_data(LIBKERNSH_VMCONFIG_NOSTATIC)));
}

void *kernsh_elfsh_get_raw(void *addr)
{
  void *dataptr;
  elfshsect_t *sect;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (libkernshworld.open && kernsh_is_mem_mode())
    {
      sect = (elfshsect_t *)addr;   
      dataptr = libkernshworld.ptr - libkernshworld.kernel_start;
      sect->parent->rhdr.base = 0;
      sect->shdr->sh_addr = 0;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, dataptr);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}

void *kernsh_revm_get_raw(void *addr)
{
  //elfshsect_t	*sect;
  //elfsh_SAddr	offset;
  void *dataptr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (libkernshworld.open && kernsh_is_mem_mode())
    {    
      //sect = elfsh_get_parent_section(world.curjob->current,
      //			      (elfsh_Addr) addr, &offset);
      
      dataptr = libkernshworld.ptr + ((elfsh_Addr) addr - libkernshworld.kernel_start);
      
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, dataptr);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}

int kernsh_info()
{
  int ret;
  u_int         dim[3];
  vector_t      *info;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
    
  info = aspect_vector_get("info");
  dim[0] = libkernshworld.type;
  dim[1] = libkernshworld.os;
  
  fct = aspect_vectors_select(info, dim);
  
  ret = fct();
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int kernsh_info_linux()
{
  unsigned long system_call;
  char buffer[256];
  char *p;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(__linux__)
  asm("sidt %0" : "=m" (idtr));
#endif
  
  libkernshworld.idt_base = idtr.base;
  libkernshworld.idt_limit = idtr.limit;

  printf("IDTR BASE 0x%lx LIMIT 0x%x\n", idtr.base,idtr.limit);
      
  kernsh_readmem(idtr.base+(2*sizeof(unsigned long))*0x80, 
		 &idt, 
		 sizeof(idt));
  system_call = (idt.off2 << 16) | idt.off1;
  
  printf("idt80: flags = %d flags=%X sel=%X off1=%x off2=%X\n",
	 idt.flags,(unsigned)idt.flags,(unsigned)idt.sel,(unsigned)idt.off1, 
	 (unsigned)idt.off2);
  printf("SYSTEM_CALL : 0x%lx\n", system_call);
  
  kernsh_readmem(system_call, buffer, 255);
  p = (char *)kernsh_find_pattern(buffer,255,"\xff\x14\x85",3);
      
  if (p == NULL)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Cannot get sct", -1);
    }
  
  libkernshworld.sct = *(unsigned long *)(p + 3);
  printf("Sys Call Table 0x%lx\n", libkernshworld.sct);
    
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_info_netbsd()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_info_freebsd()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
