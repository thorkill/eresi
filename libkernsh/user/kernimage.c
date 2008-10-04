/*
** @file kernimage.c
** @ingroup libkernsh_user
**
*/
#include "libkernsh.h"

/**
 * @brief Extract and gunzip the kernel 
 * @return 0 on success, -1 on error 
 */
int kernsh_decompkernel()
{
  int ret;
  unsigned int dim[1];
  vector_t *decomp;
  int (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__  
  printf("DECOMP KERNEL\n");
#endif

  decomp = aspect_vector_get(LIBKERNSH_VECTOR_NAME_DECOMPKERNEL);

  dim[0] = libkernshworld.os;

  fct = aspect_vectors_select(decomp, dim);

  ret = fct();

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Extract and gunzip the linux kernel\n
 * Configure :\n
 * LIBKERNSH_VMCONFIG_KERNEL, LIBKERNSH_VMCONFIG_STORAGE_PATH, LIBKERNSH_VMCONFIG_KERNELGZ, LIBKERNSH_VMCONFIG_KERNELELF, LIBKERNSH_VMCONFIG_OBJCOPY, LIBKERNSH_VMCONFIG_GZIP
 * @return 0 on success, -1 on error 
 */
int kernsh_decompkernel_linux()
{
  char magic[] = { 0x1f, 0x8b, 0x08 };
  char buf[256];
  char bufgz[256];
  char bufelf[256];
  char decomp[256];
  struct stat st;
  int fd, fz;
  int size;
  char *begin, *zone;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__
  printf("DECOMP KERNEL LINUX\n");
#endif

        
  XOPEN(fd, 
	(char *) config_get_data(LIBKERNSH_VMCONFIG_KERNEL), 
	O_RDONLY, 
	0, 
	-1);

#if __DEBUG_KERNSH__
  printf("OPEN KERNEL @ %s\n", (char *) config_get_data(LIBKERNSH_VMCONFIG_KERNEL));
#endif

  if(fstat(fd, &st) == -1)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "fd error", -1);
    }
  
#if defined(__linux__)  
  XMMAP(zone, NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0, -1);
#endif

#if __DEBUG_KERNSH__
  printf("MMAP size %d @ 0x%lx\n", (int)st.st_size, (unsigned long) zone);
#endif

  begin = kernsh_find_pattern(zone, st.st_size, magic, 3);

#if __DEBUG_KERNSH__
  printf("FIND MAGIC GZIP PATTERN @ 0x%lx\n", (unsigned long) begin);
#endif

  size = st.st_size - ((int)begin - (int)zone);   

  memset(buf, '\0', sizeof(buf));
  snprintf(buf, sizeof(buf), "%s%s", 
	   (char *) config_get_data(LIBKERNSH_VMCONFIG_STORAGE_PATH),
	   (char *) config_get_data(LIBKERNSH_VMCONFIG_KERNELGZ));

  XOPEN(fz, buf, O_CREAT|O_RDWR, 0777, -1);

#if __DEBUG_KERNSH__
  printf("OPEN GZ KERNEL @ %s\n", buf);
#endif

  XWRITE(fz, begin, size, -1);
  XCLOSE(fz, -1);

#if defined(__linux__)  
  XMUNMAP(zone, st.st_size, -1);
#endif

  XCLOSE(fd, -1);
  
  memset(decomp, '\0', sizeof(decomp));
  snprintf(decomp, sizeof(decomp), "%s -d -f %s%s", 
	   (char *) config_get_data(LIBKERNSH_VMCONFIG_GZIP), 
	   (char *) config_get_data(LIBKERNSH_VMCONFIG_STORAGE_PATH),
	   (char *) config_get_data(LIBKERNSH_VMCONFIG_KERNELGZ));

#if __DEBUG_KERNSH__
  printf("DECOMP %s\n", decomp);
#endif

  system(decomp);

  memset(bufgz, '\0', sizeof(bufgz));
  snprintf(bufgz, sizeof(bufgz), "%s%s", 
	   (char *) config_get_data(LIBKERNSH_VMCONFIG_STORAGE_PATH),
	   (char *) config_get_data(LIBKERNSH_VMCONFIG_KERNELGZ));

  bufgz[strlen(bufgz) - 3] = '\0';


  memset(bufelf, '\0', sizeof(bufelf));
  snprintf(bufelf, sizeof(bufelf), "%s%s",
	   (char *) config_get_data(LIBKERNSH_VMCONFIG_STORAGE_PATH),
	   (char *) config_get_data(LIBKERNSH_VMCONFIG_KERNELELF));

  memset(buf, '\0', sizeof(buf));
  snprintf(buf, sizeof(buf) , "%s -B i386 -I binary -O elf32-i386 %s %s",
	 (char *) config_get_data(LIBKERNSH_VMCONFIG_OBJCOPY),
	 bufgz,
	 bufelf);

#if __DEBUG_KERNSH__
  printf("EXTRACT ELF FROM DATA @ %s%s\n", (char *) config_get_data(LIBKERNSH_VMCONFIG_STORAGE_PATH), (char *) config_get_data(LIBKERNSH_VMCONFIG_KERNELELF));
#endif

  system(buf);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

