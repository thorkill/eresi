/*
** virtm.c for kernsh
** 
** $Id: virtm.c,v 1.0 2008-02-25 20:05:00 pouik Exp $
**
*/

#include "kernsh.h"
#include "virtm.h"
#include "libkernsh.h"

int		cmd_kvirtm()
{
  int	ret;
  char  *param, *param2, *param3, *param4;
  char	buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  memset(buff, '\0', sizeof(buff));

  param = world.curjob->curcmd->param[0];
  param2 = world.curjob->curcmd->param[1];
  param3 = world.curjob->curcmd->param[2];
  param4 = world.curjob->curcmd->param[3];

  if (param)
    {
      if (param2 && param3 && !strcmp(param, "-d"))
	{
	  kernsh_virtm_dump_elf(atoi(param2), param3);
	}
      if (param2 && !strcmp(param, "-v"))
	{
	  kernsh_virtm_view_vmaps(atoi(param2));
	}
      if (param2 && !strcmp(param, "-s"))
	{
	  kernsh_virtm_get_virtaddr(atoi(param2));
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
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
  snprintf(buff, sizeof(buff), "%d:%s", pid, filename);
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

int kernsh_virtm_get_virtaddr(pid_t pid)
{
  struct mem_addr tmp;
  unsigned int arg[2];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  memset(&tmp, '\0', sizeof(tmp));

  arg[0] = (unsigned int)&pid;
  arg[1] = (unsigned int)&tmp;

  printf("VM_START 0x%lx\n", tmp.vm_start);

  kernsh_syscall(221, 2, arg);

  printf("VM_START 0x%lx\n", tmp.vm_start);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

