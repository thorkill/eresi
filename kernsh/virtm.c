/**
 * @defgroup kernsh Kernsh
 */
/**
 *
** @file virtm.c
** @ingroup kernsh
**
*/

#include "kernsh.h"
#include "libkernsh.h"
#include "libkernsh-virtm.h"

int		cmd_kvirtm_info()
{
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

 
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int		cmd_kvirtm_loadme()
{
  int		ret;
  char		buff[BUFSIZ];
  char		buff2[BUFSIZ];
  char		*filename;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = -1;
  memset(buff, '\0', sizeof(buff));
  memset(buff2, '\0', sizeof(buff2));

  filename = world.curjob->curcmd->param[0];

  if (filename != NULL)
    {
      snprintf(buff, sizeof(buff), "%s %s hijack_sct=1 sct_value=0x%lx free_syscall=%d",
	       (char *)config_get_data(LIBKERNSH_VMCONFIG_KLOAD),
	       filename,
	       libkernshworld.sct,
	       (int)config_get_data(LIBKERNSH_VMCONFIG_VIRTM_NIL_SYSCALL));
      
      snprintf(buff2, sizeof(buff2),
	       "Loading %s with : %s\n\n",
	       filename,
	       revm_colorstr(buff));
      revm_output(buff2);

      ret = revm_system(buff);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int		cmd_kvirtm_dump()
{
  int		ret;
  char		*pid, *filename;

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

int		cmd_kvirtm_read_pid()
{
  int		ret;
  char		*pid, *addr, *len;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = -1;
  pid = addr = len = NULL;

  pid = world.curjob->curcmd->param[0];
  addr = world.curjob->curcmd->param[1];
  len = world.curjob->curcmd->param[2];

  if (pid && addr && len)
    {
      ret = kernsh_virtm_read_pid(atoi(pid), strtoul(addr, NULL, 16), atoi(len));
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


int		cmd_kvirtm_write_pid()
{
  revmexpr_t	*e1;
  revmexpr_t    *e2;
  revmobj_t     *o1;
  revmobj_t     *o2;
  void          *dat;
  int           ret, pid, size;
  elfsh_Addr	addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (world.curjob->curcmd->param[0] == NULL ||
      world.curjob->curcmd->param[1] == NULL ||
      world.curjob->curcmd->param[2] == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Needs 3 parameters", -1);

  pid = atoi(world.curjob->curcmd->param[0]);

  if (IS_VADDR(world.curjob->curcmd->param[1]))
    {
      if (sscanf(world.curjob->curcmd->param[1] + 2, AFMT, &addr) != 1)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Invalid virtual address requested",
		     -1);
    }
  else
    {
      e1 = revm_lookup_param(world.curjob->curcmd->param[1]);
      o1 = e1->value;
      switch (o1->otype->type)
	{
	case ASPECT_TYPE_LONG:
	case ASPECT_TYPE_CADDR:
	case ASPECT_TYPE_DADDR:
	  addr = (o1->immed ? o1->immed_val.ent : o1->get_obj(o1->parent));
	      break;
	      
	case ASPECT_TYPE_INT:
	  addr = (o1->immed ? o1->immed_val.word : o1->get_obj(o1->parent));
	  break;
	}
    }

  e2 = revm_lookup_param(world.curjob->curcmd->param[2]); 

  o2 = e2->value;

 /* Convert Integers into raw data */
  if (o2->otype->type != ASPECT_TYPE_RAW && o2->otype->type != ASPECT_TYPE_STR)
    if (revm_convert_object(e2, ASPECT_TYPE_RAW) < 0)
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                        "Unable to convert dest object to RAW", (-1));

  /* Get the source buff */
  dat = (o2->immed                ? o2->immed_val.str                  :
         o2->otype->type == ASPECT_TYPE_STR ? o2->get_name(o2->root, o2->parent) :
         o2->get_data(o2->parent, o2->off, o2->sizelem));

  /* Set size */
  size = o2->size;

  if (size <= 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Source offset too big", -1);

  printf("WRITE @ %d 0x%lx %d %d\n",
	 pid,
	 (unsigned long)addr,
	 size,
	 strlen(world.curjob->curcmd->param[2]));

  ret = kernsh_virtm_write_pid(pid, addr, (char *)dat, size);

  if (ret != size)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Impossible to write mem",
		   -1);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		cmd_kvirtm_disasm_pid()
{
  int		ret;
  char		*pid, *addr, *len;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = -1;
  pid = addr = len = NULL;

  pid = world.curjob->curcmd->param[0];
  addr = world.curjob->curcmd->param[1];
  len = world.curjob->curcmd->param[2];

  if (pid && addr && len)
    {
      ret = kernsh_virtm_disasm_pid(atoi(pid), strtoul(addr, NULL, 16), atoi(len));
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int		kernsh_virtm_dump_elf(pid_t pid, char *filename)
{
  int		fd, len, ret;
  char		*proc_entry_root_tmp;
  char		buff[BUFSIZ];

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
  int		fd, len, ret;
  char		*proc_entry_root_tmp;
  char		buff[BUFSIZ];

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

int		kernsh_virtm_read_pid(pid_t pid, unsigned long addr, int len)
{
  char		 *new_buff;

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

int		kernsh_virtm_write_pid(pid_t pid, unsigned long addr, char *buffer, int len)
{
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  kernsh_virtm_read_pid(pid, addr, len+10);

  ret = kernsh_kvirtm_write_virtm(pid, addr, buffer, len);

  kernsh_virtm_read_pid(pid, addr, len+10);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int		kernsh_virtm_disasm_pid(pid_t pid, unsigned long addr, int len)
{
  char		*new_buff;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(__FILE__, __FUNCTION__, __LINE__,
	 new_buff,
	 len,
	 -1);

  memset(new_buff, '\0', len);

  kernsh_kvirtm_read_virtm(pid, addr, new_buff, len);

  kernsh_disasm(new_buff, len, addr);

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
