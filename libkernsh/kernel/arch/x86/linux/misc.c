/**
 * @defgroup libkernsh_kernel libkernsh_kernel
 */
/*
** @file misc.c
** @ingroup libkernsh_kernel
**
*/
#include "libkernsh-kernel.h"
#include "libkernsh-kernel-linux.h"

/**
 * @brief Convert a string to an integer
 * @param name The string
 * @return value
 */
int kernsh_atoi(const char *name)
{
  int val = 0;
  
  for (;; name++) 
    {
      switch (*name) 
	{
	case '0' ... '9':
	  val = 10*val+(*name-'0');
	break;
	default:
	  return val;
	  }
    }
}

/**
 * @brief Get task_struct from userland
 * @param pid task_struct of the process id 
 * @param buffer Store the task_struct in a buffer
 * @param len Count bytes of the buffer
 * @param mode The mode to write into the buffer
 * @return 0 on success, -1 on error
 */
asmlinkage int kernsh_task_pid(int pid, char *buffer, int len, int mode)
{
  kvirtm_virtual_task_struct_t kvtst;
  int write, cur_write;
  struct task_struct *task;
  struct mm_struct *mm;
  unsigned long *tmp;
  unsigned long size;
  char *ptr;

  task = find_task_by_pid(pid);
  if (task == NULL)
    {
      printk(KERN_ALERT "Couldn't find pid %d\n", pid);
      return -EFAULT;
    }

  if (task->mm == NULL)
    {
      printk(KERN_ALERT "Couldn't find mm_struct %d\n", pid);
      return -EFAULT;
    }

  mm = task->mm;
  memset(&kvtst, '\0', sizeof(kvirtm_virtual_task_struct_t));
  
  kvtst.state = task->state;
  kvtst.flags = task->flags;
  kvtst.ptrace = task->ptrace;
  kvtst.mmap_base = mm->mmap_base;
  kvtst.task_size = mm->task_size;

  kvtst.start_code = mm->start_code;
  kvtst.end_code = mm->end_code;
  kvtst.start_data = mm->start_data;
  kvtst.end_data = mm->end_data;

  write = 0;
    
  tmp = (unsigned long *)&kvtst;
  ptr = buffer;
  size = write = cur_write = 0;

  while (write < len && size < sizeof(kvirtm_virtual_task_struct_t))
    {
      cur_write = sprintf(ptr, "0x%lx:", *tmp);
      size += sizeof(unsigned long);
      tmp++;
      ptr += cur_write;
      write += cur_write;
    }
  
  return write;
}

/**
 * @brief Display vma of a process id
 * @param pid the process id 
 * @return 0 on success, -1 on error
 */
int kernsh_view_vmaps(pid_t pid)
{
  struct task_struct *task;
  struct mm_struct *mm;
  struct vm_area_struct *vma;
  
  task = find_task_by_pid(pid);
  if (task == NULL)
    {
      printk(KERN_ALERT "Couldn't find pid %d\n", pid);
      return -1;
    }
  
  if (task->mm == NULL)
    return -1;
  
  mm = task->mm;
  
  printk(KERN_ALERT "START CODE 0x%lx START END 0x%lx\n", mm->start_code, mm->end_code);
  printk(KERN_ALERT "DATA CODE 0x%lx DATA END 0x%lx\n", mm->start_data, mm->end_data);
  
  if (mm)
    {
      for(vma = mm->mmap; vma; vma = vma->vm_next)
	{
	  printk(KERN_ALERT "VM_START @ 0x%lx VM_END @ 0x%lx VM_FLAGS 0x%lx VM_FILE 0x%lx\n", 
		 vma->vm_start, 
		 vma->vm_end,
		 vma->vm_flags,
		 (unsigned long)vma->vm_file);
	        }
    }
  
  return 0;
}

/**
 * @brief Valid a physical address range
 * @param addr
 * @param count 
 * @return 0 on success, -1 on error
 */
int valid_phys_addr_range(unsigned long addr, int count)
{
  if (addr + count > __pa(high_memory))
    return 0;
  
  return 1;
}
