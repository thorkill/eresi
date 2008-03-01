/*
** virtm.c for kernsh
** 
** $Id: virtm.c,v 1.0 2008-02-25 20:05:00 pouik Exp $
**
*/

#include "kernsh.h"
#include "libkernsh.h"
#include "libkernsh-virtm.h"

int		cmd_kvirtm_info()
{
  int	ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

 
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int		cmd_kvirtm_dump()
{
  int ret;
  char *pid, *filename;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = -1;
  pid = filename = NULL;

  pid = world.curjob->curcmd->param[0];
  filename = world.curjob->curcmd->param[1];

  if (pid && filename)
    {
      ret = kernsh_virtm_dump_elf(atoi(pid), filename);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);

  return 0;
}

int		cmd_kvirtm_read()
{
  int ret;
  char *pid, *addr, *len;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = -1;
  pid = addr = len = NULL;

  pid = world.curjob->curcmd->param[0];
  addr = world.curjob->curcmd->param[1];
  len = world.curjob->curcmd->param[2];

  if (pid && addr && len)
    {
      ret = kernsh_virtm_read(atoi(pid), strtoul(addr, NULL, 16), atoi(len));
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int		cmd_kvirtm_read_mem()
{
  int ret;
  char *addr, *len;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = -1;
  addr = len = NULL;

  addr = world.curjob->curcmd->param[0];
  len = world.curjob->curcmd->param[1];

  if (addr && len)
    {
      ret = kernsh_virtm_read_mem(strtoul(addr, NULL, 16), atoi(len));
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int		cmd_kvirtm_write_mem()
{

  return 0;
}

int		cmd_kvirtm_write()
{

  return 0;
}

int		kernsh_virtm_dump_elf(pid_t pid, char *filename)
{
  int fd, len, ret;
  char *proc_entry_root_tmp;
  char	buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  memset(buff, '\0', sizeof(buff));

  if (filename == NULL)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "filename is empty",
		   -1);
    }

  len = strlen(PROC_ENTRY_ROOT) + 
    strlen(PROC_ENTRY_KERNSH_VIRTM) +
    strlen(PROC_ENTRY_KERNSH_VIRTM_MAX) + 
    2;

  XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	 proc_entry_root_tmp, 
	 len, 
	 -1);
  
  memset(proc_entry_root_tmp, '\0', len);
 
  snprintf(proc_entry_root_tmp, len, "%s%s/%s", 
	   PROC_ENTRY_ROOT, 
	   PROC_ENTRY_KERNSH_VIRTM,
	   PROC_ENTRY_KERNSH_VIRTM_INFO);
 
  XOPEN(fd, proc_entry_root_tmp, O_RDWR, 0777, -1);
  snprintf(buff, sizeof(buff), "%d:%s:", pid, filename);
  ret = write(fd, buff, strlen(buff));
  if (ret != strlen(buff))
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Impossible to set info",
		   -1);
    }

  XCLOSE(fd, -1);

  snprintf(proc_entry_root_tmp, len, "%s%s/%s", 
	   PROC_ENTRY_ROOT, 
	   PROC_ENTRY_KERNSH_VIRTM,
	   PROC_ENTRY_KERNSH_VIRTM_DUMP_ELF);

  XOPEN(fd, proc_entry_root_tmp, O_RDWR, 0777, -1);
  snprintf(buff, sizeof(buff), "1");
  ret = write(fd, buff, strlen(buff));
  XCLOSE(fd, -1);

  XFREE(__FILE__, __FUNCTION__, __LINE__, proc_entry_root_tmp);

  if (ret != 1)
    {
      snprintf(buff, sizeof(buff),
	       "%s !!!\n\n",
	       revm_colorstr("Dump failed"));
      revm_output(buff);

      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Dump failed",
		   -1);
    }

  snprintf(buff, sizeof(buff),
	   "%s PID %s @ %s\n\n",
	   revm_colorstr("DUMP"),
	   revm_colornumber("%u", pid),
	   revm_colorstr((char *)filename));
  revm_output(buff);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Recupere les vmaps dans une liste */
int		kernsh_virtm_view_vmaps(pid_t pid)
{
  int fd, len, ret;
  char *proc_entry_root_tmp;
  char	buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  memset(buff, '\0', sizeof(buff));

  len = strlen(PROC_ENTRY_ROOT) + 
    strlen(PROC_ENTRY_KERNSH_VIRTM) + 
    strlen(PROC_ENTRY_KERNSH_VIRTM_MAX) + 
    2;

  XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	 proc_entry_root_tmp, 
	 len, 
	 -1);
  
  memset(proc_entry_root_tmp, '\0', len);
 
  snprintf(proc_entry_root_tmp, len, "%s%s/%s", 
	   PROC_ENTRY_ROOT, 
	   PROC_ENTRY_KERNSH_VIRTM,
	   PROC_ENTRY_KERNSH_VIRTM_INFO);
 
  XOPEN(fd, proc_entry_root_tmp, O_RDWR, 0777, -1);
  snprintf(buff, sizeof(buff), "%d:", pid);
  ret = write(fd, buff, strlen(buff));
  if (ret != strlen(buff))
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Impossible to set pid",
		   -1);
    }

  XCLOSE(fd, -1);

  snprintf(proc_entry_root_tmp, len, "%s%s/%s", 
	   PROC_ENTRY_ROOT, 
	   PROC_ENTRY_KERNSH_VIRTM,
	   PROC_ENTRY_KERNSH_VIRTM_DUMP_ELF);

  XOPEN(fd, proc_entry_root_tmp, O_RDWR, 0777, -1);
  snprintf(buff, sizeof(buff), "2");
  ret = write(fd, buff, strlen(buff));
  XCLOSE(fd, -1);

  XFREE(__FILE__, __FUNCTION__, __LINE__, proc_entry_root_tmp);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_virtm_read(pid_t pid, unsigned long addr, int len)
{
  char *new_buff;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(__FILE__, __FUNCTION__, __LINE__,
	 new_buff,
	 len,
	 -1);

  memset(new_buff, '\0', len);

  kernsh_kvirtm_read_virtm(pid, addr, new_buff, len);

  kernsh_hexdump((unsigned char *)new_buff, len, addr);

  XFREE(__FILE__, __FUNCTION__, __LINE__, new_buff);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_virtm_read_mem(unsigned long addr, int len)
{
  char *new_buff;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(__FILE__, __FUNCTION__, __LINE__,
	 new_buff,
	 len,
	 -1);

  memset(new_buff, '\0', len);

  kernsh_kvirtm_read_mem(addr, new_buff, len);

  kernsh_hexdump((unsigned char *)new_buff, len, addr);

  XFREE(__FILE__, __FUNCTION__, __LINE__, new_buff);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/*int kernsh_virtm_get_virtaddr(pid_t pid)
{
  struct mem_addr tmp;
  unsigned int arg[4];
  char buffer[500];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  memset(&tmp, '\0', sizeof(tmp));

  arg[0] = (unsigned int)pid;
  arg[1] = (unsigned int)0x08048000;
  arg[2] = (unsigned int)buffer;
  arg[3] = (unsigned int)sizeof(buffer);

  //arg[1] = (unsigned int)&tmp;

  //printf("VM_START 0x%lx\n", tmp.vm_start);

  kernsh_syscall(17, 4, arg);
  hexdump(buffer, sizeof(buffer), 0x08048000);
  //printf("VM_START 0x%lx\n", tmp.vm_start);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
*/
