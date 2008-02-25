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
    strlen(PROC_ENTRY_KERNSH_VIRTM_FILENAME) + 
    2;

  XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	 proc_entry_root_tmp, 
	 len, 
	 -1);
  
  memset(proc_entry_root_tmp, '\0', len);
 
  snprintf(proc_entry_root_tmp, len, "%s%s/%s", 
	   PROC_ENTRY_ROOT, 
	   PROC_ENTRY_KERNSH_VIRTM,
	   PROC_ENTRY_KERNSH_VIRTM_PID);
 
  XOPEN(fd, proc_entry_root_tmp, O_RDWR, 0777, -1);
  snprintf(buff, sizeof(buff), "%d", pid);
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
	   PROC_ENTRY_KERNSH_VIRTM_FILENAME);

  XOPEN(fd, proc_entry_root_tmp, O_RDWR, 0777, -1);
  ret = write(fd, filename, strlen(filename));
  if (ret != strlen(filename))
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Impossible to set filename",
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
