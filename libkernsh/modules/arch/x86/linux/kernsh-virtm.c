#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <asm/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/threads.h>
#include <linux/string.h>
#include <linux/bio.h>

#include "libkernsh-kernel.h"

MODULE_DESCRIPTION("Kernsh Virtual Memory !");
MODULE_AUTHOR("pouik@kernsh.org");
MODULE_LICENSE("GPL");

#define FREE_SYSCALL_DEFAULT 31
#define SYS_CALL_TABLE_DEFAULT 0xc02aa440

static int hijack_sct;
static unsigned long sct_value;
static int free_syscall;

module_param(hijack_sct, int, 0);
MODULE_PARM_DESC(hijack_sct, "Hijack sct");

module_param(sct_value, long, 0);
MODULE_PARM_DESC(sct_value, "sct_value");

module_param(free_syscall, int, 0);
MODULE_PARM_DESC(free_syscall, "free_syscall");

static pid_t current_pid;

static struct proc_dir_entry *proc_entry_kernsh_virtm;
static struct proc_dir_entry *proc_entry_kernsh_virtm_vio;
static struct proc_dir_entry *proc_entry_kernsh_virtm_vio_info;
static struct proc_dir_entry *proc_entry_kernsh_virtm_info;

static kvirtm_action_t current_kvirtm;
static char current_filename[256];

static void **sys_call_table;

asmlinkage int (*o_syscallnill)(void);

asmlinkage int kernsh_read_virtm_syscall(pid_t, unsigned long, char *, int);
asmlinkage int kernsh_read_mem_syscall(unsigned long, char *, int);
asmlinkage int kernsh_write_virtm_syscall(pid_t, unsigned long, const char *, int);
asmlinkage int kernsh_write_mem_syscall(unsigned long, const char *, int);

ssize_t kernsh_virtm_info_write(struct file *filp, 
				const char __user *buff, 
				unsigned long len, 
				void *data)
{
  char *token;
  char *new_buff;
  int i;
  pid_t new_pid;

  new_buff = kmalloc(len, GFP_KERNEL);
  if (copy_from_user(new_buff, buff, len)) 
    {
      return -EFAULT;
    }

  i = 0;
  while((token = strsep(&new_buff, ":")) != NULL) 
    {
      switch(i)
	{
	case 0 :
	  new_pid = kernsh_atoi(token);
	  if (new_pid > 0 && new_pid <= PID_MAX_DEFAULT)
	    {
	      current_pid = new_pid;
	    }
	  else
	    {
	      kfree(new_buff);
	      return -EFAULT;
	    }
	  break;
	case 1 :
	  if (strlen(token) <= 0 || strlen(token) >= sizeof(current_filename))
	    {
	      kfree(new_buff);
	      return -EFAULT;
	    }
	  
	  memset(current_filename, '\0', sizeof(current_filename));
	  memcpy(current_filename, token, strlen(token));

	  break;
	}

      i++;
    }

  kfree(new_buff);
  return len;
}

int kernsh_virtm_info_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
  int len;

  len = sprintf(page, "PID = %d\nFILENAME = %s\n", current_pid, current_filename);

  return len;
}

ssize_t kernsh_virtm_vio_info_input(struct file *filp, 
				    const char __user *buff, 
				    unsigned long len, 
				    void *data)
{
  char *token;
  char *new_buff;
  int i;
  pid_t new_pid;

  printk(KERN_ALERT "kernsh_virtm_vio_info_input ENTER !!\n"); 

  new_buff = kmalloc(len, GFP_KERNEL);
  if (copy_from_user(new_buff, buff, len))
      return -EFAULT;

  i = 0;
  while((token = strsep(&new_buff, ":")) != NULL) 
    {
      switch(i)
	{
	  /* Action */
	case 0 :
	  current_kvirtm.action = kernsh_atoi(token);
	  break;

	  /* Addr */
	case 1 :
	  current_kvirtm.addr = simple_strtoul(token, NULL, 16);
	  break;
	  
	  /* Len */
	case 2 :
	  current_kvirtm.len = kernsh_atoi(token);
	  break;

	  /* Additional flags */
	case 3 :
	  new_pid = kernsh_atoi(token);
	  if (new_pid > 0 && new_pid <= PID_MAX_DEFAULT)
	    {
	      current_kvirtm.pid = new_pid;
	    }
	  break;
	case 4 :
	  if (strlen(token) > 0 && strlen(token) <= sizeof(current_kvirtm.filename))
	    {
	      memset(current_kvirtm.filename, '\0', sizeof(current_kvirtm.filename));
	      memcpy(current_kvirtm.filename, token, strlen(token));
	    }
	  break;
	default :
	  break;
	}
      i++;
    }

  printk(KERN_ALERT "KVIRTM %d 0x%lx %d %d %s\n", 
	 current_kvirtm.action,
	 current_kvirtm.addr,
	 current_kvirtm.len,
	 current_kvirtm.pid,
	 current_kvirtm.filename);

  kfree(new_buff);
  return len;
}

int kernsh_virtm_vio_info_output(char *page, char **start, off_t off, int count, int *eof, void *data)
{

  return 0;
}

ssize_t kernsh_virtm_vio_input(struct file *filp, 
			       const char __user *buff, 
			       unsigned long len, 
			       void *data)
{

  printk(KERN_ALERT "kernsh_virtm_vio_input buff 0x%lx COUNT 0x%lx\n", (unsigned long)buff, len);

  switch (current_kvirtm.action)
    {
    case LIBKERNSH_VIRTM_WRITE_MEM_PID :
      printk(KERN_ALERT "kvvo -> kernsh_write_virtm\n");
      kernsh_write_virtm(current_kvirtm.pid,
			 current_kvirtm.addr,
			 buff,
			 current_kvirtm.len,
			 LIBKERNSH_KERNEL_MODE);
      break;
    case LIBKERNSH_VIRTM_WRITE_MEM :
      printk(KERN_ALERT "kvvo -> kernsh_write_mem\n");
      kernsh_write_mem(current_kvirtm.addr,
		       buff,
		       current_kvirtm.len,
		       LIBKERNSH_KERNEL_MODE);
      break;
    }


  return len;
}

int kernsh_virtm_vio_output(char *page, char **start, off_t off, int count, int *eof, void *data)
{
  char *new_buff;
  int len;

  len = 0;

  new_buff = NULL;

  printk(KERN_ALERT "kernsh_virtm_vio_output PAGE 0x%lx COUNT %d\n", (unsigned long)page, count);

  switch (current_kvirtm.action)
    {
    case LIBKERNSH_VIRTM_READ_MEM : 
      printk(KERN_ALERT "kvvo -> kernsh_read_mem\n");
      new_buff = kmalloc(current_kvirtm.len, GFP_KERNEL);
      kernsh_read_mem(current_kvirtm.addr,
		      new_buff,
		      current_kvirtm.len,
		      LIBKERNSH_KERNEL_MODE);
      len = current_kvirtm.len;
      break;
    case LIBKERNSH_VIRTM_READ_MEM_PID :
      printk(KERN_ALERT "kvvo -> kernsh_read_virtm\n");	    
      new_buff = kmalloc(current_kvirtm.len, GFP_KERNEL);
      len = kernsh_read_virtm(current_kvirtm.pid,
			      current_kvirtm.addr,
			      new_buff,
			      current_kvirtm.len,
			      LIBKERNSH_KERNEL_MODE);
      break;
    case LIBKERNSH_VIRTM_TASK_PID :
      printk(KERN_ALERT "kvvo -> kernsh_task_pid\n");	    
      new_buff = kmalloc(current_kvirtm.len, GFP_KERNEL);
      len = kernsh_task_pid(current_kvirtm.pid, new_buff, current_kvirtm.len, LIBKERNSH_KERNEL_MODE);
      break;
    case LIBKERNSH_VIRTM_DUMP_ELF_PID :
      len = kernsh_dump_elf_pid(current_kvirtm.pid, current_kvirtm.filename);
      break;
    default :
      return -EFAULT;
      break;
    }
 
  if (len > 0)
    memcpy(page, new_buff, len);

  kfree(new_buff);

  return len;
}

asmlinkage int kernsh_read_virtm_syscall(pid_t pid, unsigned long addr, char *buffer, int len)
{
  return kernsh_read_virtm(pid, addr, buffer, len, LIBKERNSH_USER_MODE);
}

asmlinkage int kernsh_write_virtm_syscall(pid_t pid, unsigned long addr, const char *buffer, int len)
{
  return kernsh_write_virtm(pid, addr, buffer, len, LIBKERNSH_USER_MODE);
}

asmlinkage int kernsh_read_mem_syscall(unsigned long addr, char *buffer, int len)
{
  return kernsh_read_mem(addr, buffer, len, LIBKERNSH_USER_MODE);
}

asmlinkage int kernsh_write_mem_syscall(unsigned long addr, const char *buffer, int len)
{
  return kernsh_write_mem(addr, buffer, len, LIBKERNSH_USER_MODE);
}

asmlinkage int kernsh_task_pid_syscall(pid_t pid, char *buffer, int len)
{
  return kernsh_task_pid(pid, buffer, len,  LIBKERNSH_USER_MODE);
}

asmlinkage int kernsh_dump_elf_pid_syscall(pid_t pid, const char *filename)
{
  return kernsh_dump_elf_pid(pid, filename);
}

int asmlinkage kernsh_kvirtm_syscall_wrapper(pid_t pid, 
					     unsigned long addr, 
					     char *buffer, 
					     int len, 
					     int type)
{
  printk(KERN_ALERT "[+] kernsh_kvirtm_syscall_wrapper\n");

  switch (type)
    {
    case LIBKERNSH_VIRTM_READ_MEM :
      printk(KERN_ALERT "[+] kksw -> kernsh_read_mem_syscall\n");
      return kernsh_read_mem_syscall(addr, buffer, len);
      break;
    case LIBKERNSH_VIRTM_WRITE_MEM :
      printk(KERN_ALERT "[+] kksw -> kernsh_write_mem_syscall\n");
      return kernsh_write_mem_syscall(addr, buffer, len);
      break;
    case LIBKERNSH_VIRTM_READ_MEM_PID :
      printk(KERN_ALERT "[+] kksw -> kernsh_read_virtm_syscall\n");
      return kernsh_read_virtm_syscall(pid, addr, buffer, len);
      break;
    case LIBKERNSH_VIRTM_WRITE_MEM_PID :
      printk(KERN_ALERT "[+] kksw -> kernsh_write_virtm_syscall\n");
      return kernsh_write_virtm_syscall(pid, addr, buffer, len);
      break;
    case LIBKERNSH_VIRTM_TASK_PID :
      printk(KERN_ALERT "[+] kksw -> kernsh_task_pid_syscall\n");
      return kernsh_task_pid_syscall(pid, buffer, len);
      break;
    case LIBKERNSH_VIRTM_DUMP_ELF_PID :
      printk(KERN_ALERT "[+] kksw -> kernsh_dump_pid_syscall\n");
      return kernsh_dump_elf_pid_syscall(pid, buffer);
      break;
    default :
      break;
    }

  return -1;
}

static int kernsh_virtm_init(void)
{
  printk(KERN_ALERT "Kernsh Virtm Init\n");

  memset(&current_kvirtm, '\0', sizeof(current_kvirtm));

  current_pid = -1;
  memset(current_filename, '\0', sizeof(current_filename));

  proc_entry_kernsh_virtm = proc_mkdir(PROC_ENTRY_KERNSH_VIRTM, &proc_root);

  if (proc_entry_kernsh_virtm == NULL)
    {
      printk(KERN_ALERT "Couldn't create kernsh virtm proc entry\n");
      return -1;
    }

  proc_entry_kernsh_virtm_info = 
    create_proc_entry(PROC_ENTRY_KERNSH_VIRTM_INFO, 0644, proc_entry_kernsh_virtm);
  proc_entry_kernsh_virtm_vio =
    create_proc_entry(PROC_ENTRY_KERNSH_VIRTM_VIO, 0644, proc_entry_kernsh_virtm);
  proc_entry_kernsh_virtm_vio_info =
    create_proc_entry(PROC_ENTRY_KERNSH_VIRTM_VIO_INFO, 0644, proc_entry_kernsh_virtm);

  if (proc_entry_kernsh_virtm_info == NULL ||
      proc_entry_kernsh_virtm_vio_info == NULL ||
      proc_entry_kernsh_virtm_vio == NULL)
    {
      printk(KERN_ALERT "Couldn't create sub proc entry\n");
      return -1;
    }
  

  proc_entry_kernsh_virtm_info->read_proc = kernsh_virtm_info_read;
  proc_entry_kernsh_virtm_info->write_proc = kernsh_virtm_info_write;
  proc_entry_kernsh_virtm_info->owner = THIS_MODULE;

  proc_entry_kernsh_virtm_vio_info->read_proc = kernsh_virtm_vio_info_output;
  proc_entry_kernsh_virtm_vio_info->write_proc = kernsh_virtm_vio_info_input;
  proc_entry_kernsh_virtm_vio_info->owner = THIS_MODULE;

  proc_entry_kernsh_virtm_vio->read_proc = kernsh_virtm_vio_output;
  proc_entry_kernsh_virtm_vio->write_proc = kernsh_virtm_vio_input;
  proc_entry_kernsh_virtm_vio->owner = THIS_MODULE;

  if (hijack_sct)
    {
      if (!sct_value)
	sct_value = SYS_CALL_TABLE_DEFAULT;

      printk(KERN_ALERT "HIJACK sys_call_table @ 0x%lx ", sct_value);
      sys_call_table = (void **)sct_value;

      if (!free_syscall)
	free_syscall = FREE_SYSCALL_DEFAULT;

      printk(KERN_ALERT "and use syscall %d\n", free_syscall);

      o_syscallnill = sys_call_table[free_syscall];
      sys_call_table[free_syscall] = &kernsh_kvirtm_syscall_wrapper;
    }

  return 0;
}

static void kernsh_virtm_exit(void)
{
  remove_proc_entry(PROC_ENTRY_KERNSH_VIRTM_DUMP_ELF, proc_entry_kernsh_virtm);
  remove_proc_entry(PROC_ENTRY_KERNSH_VIRTM_INFO, proc_entry_kernsh_virtm);
  remove_proc_entry(PROC_ENTRY_KERNSH_VIRTM_VIO_INFO, proc_entry_kernsh_virtm);
  remove_proc_entry(PROC_ENTRY_KERNSH_VIRTM_VIO, proc_entry_kernsh_virtm);

  remove_proc_entry(PROC_ENTRY_KERNSH_VIRTM, &proc_root);

  if (hijack_sct)
    {
      printk(KERN_ALERT "UNHIJACK sys_call_table @ 0x%lx\n", sct_value);
      sys_call_table[free_syscall] = o_syscallnill;
    }

  printk(KERN_ALERT "Kernsh Virtm Exit\n");
}


module_init(kernsh_virtm_init);
module_exit(kernsh_virtm_exit);
