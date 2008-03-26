/*
** @file virtm.c
** @ingroup libkernsh_kernel
**
*/

#include "libkernsh-kernel.h"

/*
 * Dictracy Loadable Kernel Module
 *                          by -  twiz   - twiz@email.it
 *                                thefly - thefly@acaro.org
 *
 * That module let you investigate, read, search, dump and write the virtual
 * address space of a process running.
 *
 * From the idea exposed by vecna in rmfbd :
 *    http://www.s0ftpj.org/bfi/dev/BFi11-dev-06
 *
 * Thanks : silvio, Liv Tyler (by thefly)
 */

/**
 * @brief Dump elf binary of a pid into a file
 * @param pid The process id
 * @param filename The filename to write elf binary
 * @return 0 on success, -1 on error
 */
int kernsh_dump_elf_pid(pid_t pid, const char *filename)
{
  struct task_struct *task;
  struct file *file;
  struct mm_struct *mm;
  struct vm_area_struct *vma;
  unsigned char c;
  ssize_t ssize;
  int i;
  mm_segment_t fs;

  if (filename == NULL || strlen(filename) == 0)
    return -1;

  printk(KERN_ALERT "DUMP pid %d @ %s\n", pid, filename);

  task = find_task_by_pid(pid);
  if (task == NULL)
    {
      printk(KERN_ALERT "Couldn't find pid %d\n", pid);
      return -1;
    }

  if (task->mm == NULL)
    return -1;
  
  file = filp_open(filename, O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO );
  if (file == NULL)
    {
      printk(KERN_ALERT "Couldn't create %s\n", filename);
      return -1;
    }

  get_file(file);
  
  fs = get_fs();
  set_fs(KERNEL_DS);
  
  mm = task->mm;

  if (mm)
    {
      for(vma = mm->mmap; vma; vma = vma->vm_next)
	{
	  printk(KERN_ALERT "VM_START @ 0x%lx VM_END @ 0x%lx VM_FLAGS 0x%lx VM_FILE 0x%lx\n", 
		 vma->vm_start, 
		 vma->vm_end,
		 vma->vm_flags,
		 (unsigned long)vma->vm_file);
	 
	  if((vma->vm_flags & VM_EXECUTABLE) && 
	     (vma->vm_flags & VM_EXEC) &&
	     (vma->vm_file))
	    {
	      vma->vm_file->f_pos = 0;
	      ssize = vma->vm_file->f_op->read(vma->vm_file, &c, sizeof(c), 
					       &vma->vm_file->f_pos);
	      if(ssize > 0)
		{
		  while(ssize != 0) 
		    {
		      i = file->f_op->write(file, &c, sizeof(c), &file->f_pos );
		      if (i != ssize) 
			{
			  set_fs(fs);
			  atomic_dec(&file->f_count);
			  filp_close(file, 0);
			  return -1;
			}
		      ssize = vma->vm_file->f_op->read(vma->vm_file, &c, sizeof(c), 
						       &vma->vm_file->f_pos );
		    }
		}  
	    }
	}

    }

  set_fs(fs);
  atomic_dec(&file->f_count);
  filp_close(file, 0);

  return 0;
}

/**
 * @brief Read virtual memory of a pid
 * @param pid The process id
 * @param addr The address to read
 * @param buffer Read virtual memory into the buffer
 * @param len Count bytes to read
 * @param mode The mode to write into the buffer
 * @return 0 on success, -1 on error
 */
asmlinkage int kernsh_read_virtm(pid_t pid, unsigned long addr, char *buffer, int len, int mode)
{
  struct task_struct *task;
  struct page *mypage;
  void *kaddr;

  printk(KERN_ALERT "[+] kernsh_read_virtm ENTER !!\n");

  printk(KERN_ALERT "Kernsh Read Virtm PID %d @ 0x%lx strlen(%d) in 0x%lx\n", 
	 pid, 
	 addr, 
	 len, 
	 (unsigned long)buffer);
 
  if (addr <= 0)
    {
      printk(KERN_ALERT "[-] Addr isn't valid => 0x%lx!\n", addr);
      return -1;
    }
  
  task = find_task_by_pid(pid);
   
  if (task == NULL)
    {
      printk(KERN_ALERT "[-] Couldn't find pid %d\n", pid);
      return -1;
    }
  
  mypage = kernsh_get_page_from_task(task, addr);
  if (mypage == NULL)
    {
      printk(KERN_ALERT "[-] PAGE NULL\n");	
      return -EFAULT;
    }

  printk(KERN_ALERT "[+] KMAP_ATOMIC\n");
  kaddr = kmap_atomic(mypage, smp_processor_id());
  
  switch(mode)
    {
    case LIBKERNSH_KERNEL_MODE :
      memcpy(buffer, kaddr + (addr & ~PAGE_MASK), len);
      break;
    case LIBKERNSH_USER_MODE :
      if(copy_to_user(buffer, kaddr + (addr & ~PAGE_MASK), len))
	{
	  printk(KERN_ALERT "[-] copy_to_user error\n");
	  kunmap_atomic(kaddr, smp_processor_id());
	  return -EFAULT;
	}
      break;
    }

  kunmap_atomic(kaddr, smp_processor_id());
  printk(KERN_ALERT "[+] KUNMAP_ATOMIC\n");

  printk(KERN_ALERT "[+] kernsh_read_virtm EXIT !!\n");

  return len;
}

/**
 * @brief Write virtual memory of a pid
 * @param pid The process id
 * @param addr The address to write
 * @param buffer Write buffer into virtual memory
 * @param len Count bytes to write
 * @param mode The mode to write into the buffer
 * @return len on success, -1 on error
 */
asmlinkage int kernsh_write_virtm(pid_t pid, unsigned long addr, const char *buffer, int len, int mode)
{
  struct task_struct *task;
  struct page *mypage;
  void *kaddr;

  printk(KERN_ALERT "[+] kernsh_write_virtm ENTER !!\n");

  printk(KERN_ALERT "Kernsh Write Virtm PID %d @ 0x%lx strlen(%d) to 0x%lx\n", 
	 pid, 
	 addr, 
	 len, 
	 (unsigned long)buffer);
 
  if (addr <= 0)
    {
      printk(KERN_ALERT "[-] Addr isn't valid => 0x%lx!\n", addr);
      return -1;
    }
  
  task = find_task_by_pid(pid);
   
  if (task == NULL)
    {
      printk(KERN_ALERT "[-] Couldn't find pid %d\n", pid);
      return -1;
    }
  
  mypage = kernsh_get_page_from_task(task, addr);
  if (mypage == NULL)
    {
      printk(KERN_ALERT "[-] PAGE NULL\n");	
      return -EFAULT;
    }

  if (PageReserved(mypage))
    {
      printk(KERN_ALERT "[-] PAGE RESERVED\n");	
      return -EFAULT;
    }
    
  printk(KERN_ALERT "[+] KMAP_ATOMIC\n");
  kaddr = kmap_atomic(mypage, smp_processor_id());
  
  switch(mode)
    {
    case LIBKERNSH_KERNEL_MODE :
      memcpy(kaddr + (addr & ~PAGE_MASK), buffer, len);
      break;
    case LIBKERNSH_USER_MODE :
      if(copy_from_user(kaddr + (addr & ~PAGE_MASK), buffer, len))
	{
	  printk(KERN_ALERT "[-] copy_from_user error\n");
	  kunmap_atomic(kaddr, smp_processor_id());
	  return -EFAULT;
	}
      break;
    }

  kunmap_atomic(kaddr, smp_processor_id());
  printk(KERN_ALERT "[+] KUNMAP_ATOMIC\n");

  printk(KERN_ALERT "[+] kernsh_write_virtm EXIT !!\n");

  return len;
}
