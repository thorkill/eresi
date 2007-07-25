/*
** kernel.c for libkernsh
**
** $Id: kernel.c,v 1.1 2007-07-25 19:53:01 pouik Exp $
**
*/
#include "libkernsh.h"
#include "libaspect.h"
#include "libaspect-profiler.h"

int kernsh_decompkernel()
{
  int ret;
  u_int dim[1];
  vector_t *decomp;
  int (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  revm_output("DECOMP KERNEL\n");

  decomp = aspect_vector_get("decompkernel");

  dim[0] = libkernshworld.os;

  fct = aspect_vectors_select(decomp, dim);

  ret = fct();

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int kernsh_decompkernel_linux()
{
  char magic[] = { 0x1f, 0x8b, 0x08 };
  char buf[256];
  char decomp[256];
  struct stat st;
  int fd, fz;
  int size;
  char *begin, *zone;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_output("DECOMP KERNEL LINUX\n");
        
  XOPEN(fd, (char *) config_get_data(LIBKERNSH_VMCONFIG_KERNEL), O_RDONLY, 0, -1);

  if(fstat(fd, &st) == -1)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "fd error", -1);
    }
  
  //printf("SIZE %d\n", st.st_size);
  
  XMMAP(zone, NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0, -1);

  begin = kernsh_find_pattern(zone, st.st_size, magic, 3);
  //printf("0x%x 0x%x %d\n", zone, begin, (int)begin - (int)zone);
  size = st.st_size - ((int)begin - (int)zone);
  //printf("SIZE %d\n", size);
   

  memset(buf, '\0', sizeof(buf));
  snprintf(buf, sizeof(buf), "%s%s", 
	   (char *) config_get_data(LIBKERNSH_VMCONFIG_STORAGE_PATH),
	   (char *) config_get_data(LIBKERNSH_VMCONFIG_KERNELGZ));

  //printf("BUF %s\n", buf);

  XOPEN(fz, buf, O_CREAT|O_RDWR, 0777, -1);
      
  XWRITE(fz, begin, size, -1);
  XCLOSE(fz, -1);
  
  XMUNMAP(zone, st.st_size, -1);
        
  XCLOSE(fd, -1);
  
  memset(decomp, '\0', sizeof(decomp));
  snprintf(decomp, sizeof(decomp), "%s -d -f %s%s", 
	   (char *) config_get_data(LIBKERNSH_VMCONFIG_GZIP), 
	   (char *) config_get_data(LIBKERNSH_VMCONFIG_STORAGE_PATH),
	   (char *) config_get_data(LIBKERNSH_VMCONFIG_KERNELGZ));
  //printf("DECOMP %s\n", decomp);
  system(decomp);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_loadkernel()
{
  int ret;
  u_int dim[2];
  vector_t *load;
  int (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_output("LOAD KERNEL\n");

  load = aspect_vector_get("loadkernel");

  dim[0] = libkernshworld.type;
  dim[1] = libkernshworld.os;

  fct = aspect_vectors_select(load, dim);

  ret = fct();

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int kernsh_loadkernel_linux()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_file_load("./vmlinux", 0, NULL);

  libkernshworld.open_static = 1;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_loadkernel_linux_old()
{
  char bufgz[256];
  char bufelf[256];
  char buf[1024];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_output("LOAD KERNEL LINUX\n");

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

  //printf("BUF %s\n", buf);
  system(buf);


  int fd;
  struct stat st;
  char image[4096000];
  unsigned long _text = 0xc0100000;
  unsigned long _etext = 0xc02fd465;
  unsigned long _edata = 0xc03908b4;
  unsigned long offset;
  unsigned long diff;


  fd = open(bufelf, O_RDONLY);
  fstat(fd, &st);
  read(fd, image, st.st_size);
  close(fd);

  printf("0x%lx\n", image);
  
  offset = (unsigned long)kernsh_find_pattern((void *)image, sizeof(image), &_text, 4);
  diff = (unsigned long)offset - (unsigned long)image;
  printf("_TEXT OFFSET 0x%lx 0x%lx\n", offset, diff);

  offset = (unsigned long)kernsh_find_pattern((void *)image, sizeof(image), &_etext, 4);
  diff = (unsigned long)offset - (unsigned long)image;
  printf("_ETEXT OFFSET 0x%lx 0x%lx\n", offset, diff);

  offset = (unsigned long)kernsh_find_pattern((void *)image, sizeof(image), &_edata, 4);
  diff = (unsigned long)offset - (unsigned long)image;
  printf("_EDATA OFFSET 0x%lx 0x%lx\n", offset, diff);

  //  revm_output("\n");
  //revm_load_file(bufelf, 0, NULL);
  //revm_output("\n");

  elfshobj_t    *file;
  file = elfsh_map_obj("/tmp/vmlinux");
  
  printf("0x%lx\n", elfsh_get_entrypoint(file));
  //elfsh_set_entrypoint(file, 0x100000);
  printf("0x%lx\n", elfsh_get_entrypoint(file));
  
  elfsh_save_obj(file, "/tmp/prout");

  //revm_load_file("/tmp/prout", 0, NULL);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

// A FINIR
int kernsh_unloadkernel()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  libkernshworld.open_static = 0;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
