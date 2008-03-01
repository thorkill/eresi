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

#include "../include/libkernsh-virtm.h"

MODULE_DESCRIPTION("Kernsh Virtual Memory !");
MODULE_AUTHOR("pouik@kernsh.org");
MODULE_LICENSE("GPL");

static pid_t current_pid;

static struct proc_dir_entry *proc_entry_kernsh_virtm;
static struct proc_dir_entry *proc_entry_kernsh_virtm_dump_elf;
static struct proc_dir_entry *proc_entry_kernsh_virtm_vio;
static struct proc_dir_entry *proc_entry_kernsh_virtm_info;

static kvirtm_action_t current_kvirtm;
static char current_filename[256];

//static unsigned long sys_call_table;
#define SYS_CALL_TABLE 0xc02aa440
static void **sys_call_table = SYS_CALL_TABLE;

int asmlinkage (*o_syscallnill)(void);

int kernsh_get_elf_image(pid_t, const char *);
int kernsh_view_vmaps(pid_t);
int asmlinkage kernsh_read_virtm(pid_t, unsigned long, char *, int);
int asmlinkage kernsh_read_virtm_proc(pid_t, unsigned long, char *, int);

static int my_atoi(const char *name)
{
    int val = 0;

    for (;; name++) {
        switch (*name) {
            case '0' ... '9':
                val = 10*val+(*name-'0');
                break;
            default:
                return val;
        }
    }
}

ssize_t kernsh_virtm_dump_elf_write(struct file *filp, 
				    const char __user *buff, 
				    unsigned long len, 
				    void *data)
{
  int dump;
 
  dump = my_atoi(buff);

  if (dump == 1)
    {
      if (kernsh_get_elf_image(current_pid, current_filename) < 0)
	return -EFAULT;
    }

  if (dump == 2)
    {
      kernsh_view_vmaps(current_pid);
    }

  return len;
}

int kernsh_virtm_dump_elf_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
  int len;

  len = sprintf(page, "Please write 1 to dump elf file\n");

  return len;
}

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
	  new_pid = my_atoi(token);
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

ssize_t kernsh_virtm_vio_input(struct file *filp, 
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
      return -EFAULT;

  i = 0;
  while((token = strsep(&new_buff, ":")) != NULL) 
    {
      switch(i)
	{
	  /* Action */
	case 0 :
	  current_kvirtm.action = my_atoi(token);
	  break;

	  /* Addr */
	case 1 :
	  current_kvirtm.addr = simple_strtoul(token, NULL, 16);
	  break;
	  
	  /* Len */
	case 2 :
	  current_kvirtm.len = my_atoi(token);
	  break;

	  /* Additional flags */
	case 3 :
	  new_pid = my_atoi(token);
	  if (new_pid > 0 && new_pid <= PID_MAX_DEFAULT)
	    {
	      current_kvirtm.pid = new_pid;
	    }
	  else
	    {
	      kfree(new_buff);
	      return -EFAULT;
	    }
	  break;
	case 4 :
	  if (strlen(token) <= 0 || strlen(token) >= sizeof(current_kvirtm.filename))
	    {
	      kfree(new_buff);
	      return -EFAULT;
	    }
	  
	  memset(current_kvirtm.filename, '\0', sizeof(current_kvirtm.filename));
	  memcpy(current_kvirtm.filename, token, strlen(token));

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

int kernsh_virtm_vio_output(char *page, char **start, off_t off, int count, int *eof, void *data)
{
  char *new_buff;
  int len;

  len = 0;

  new_buff = NULL;

  printk(KERN_ALERT "PAGE 0x%lx COUNT %d\n", (unsigned long)page, count);

  switch (current_kvirtm.action)
    {
    case LIBKERNSH_VIRTM_READ_MEM : 
      break;
    case LIBKERNSH_VIRTM_WRITE_MEM :
      break;
    case LIBKERNSH_VIRTM_READ_MEM_PID :
      new_buff = kmalloc(current_kvirtm.len, GFP_KERNEL);
      kernsh_read_virtm_proc(current_kvirtm.pid,
			current_kvirtm.addr,
			new_buff,
			current_kvirtm.len);
      len = current_kvirtm.len;
      break;
    case LIBKERNSH_VIRTM_WRITE_MEM_PID :
      break;
    default :
      break;
    }
  
  //  len = sprintf(page, "PID = %d\nFILENAME = %s\n", current_pid, current_filename);

  memcpy(page, new_buff, len);
  kfree(new_buff);
  return len;
}

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

int kernsh_get_elf_image(pid_t pid, const char *filename)
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

struct page *kernsh_get_page_from_task(struct task_struct *task, unsigned long addr)
{
  struct mm_struct *mm;
  struct page *page_at_addr;
  pgd_t * pgd;
  pmd_t * pmd;
  pte_t * pte;
  pud_t *pud;

  page_at_addr = NULL;

  printk(KERN_ALERT "[+] kernsh_get_page_from_task ENTER !!\n");

  if (task == NULL)
    {
      printk(KERN_ALERT "[-] task_struct is null !!\n");
      return NULL;
    }
  
  if (addr <= 0)
    {
      printk(KERN_ALERT "[-] Addr isn't valid => 0x%lx!\n", addr);
      return NULL;
    }

  mm = task->mm;
    
  task_lock(task);
  if (mm == NULL)
    {
      printk(KERN_ALERT "[-] mm_struct is null !!\n");
      task_unlock(task);
      return NULL;
    }

  atomic_inc(&mm->mm_users);
  spin_lock(&mm->page_table_lock);
  task_unlock(task);

  printk(KERN_ALERT "[+] PGD ENTER\n");
  pgd = pgd_offset(mm, addr);
  if (pgd_none(*pgd))
    {
      printk(KERN_ALERT "[-] PGD_NONE\n");
      goto kernsh_get_page_from_task_error;
    }
  if (pgd_bad(*pgd))
    {
      printk(KERN_ALERT "[-] PGD_BAD\n");
      pgd_clear(pgd);
      goto kernsh_get_page_from_task_error;
    }
  printk(KERN_ALERT "[+] PGD EXIT\n");

  printk(KERN_ALERT "[+] PUD ENTER\n");
  pud = pud_offset(pgd, addr);
  if (pud_none(*pud))
    {
      printk(KERN_ALERT "[-] PUD_NONE\n");
      goto kernsh_get_page_from_task_error;
    }
  if (pud_bad(*pud))
    {
      printk(KERN_ALERT "[-] PUD_BAD\n");
      pud_clear(pud);
      goto kernsh_get_page_from_task_error;
    }
  printk(KERN_ALERT "[+] PUD EXIT\n");

  printk(KERN_ALERT "[+] PMD ENTER\n");
  pmd = pmd_offset(pud, addr);
  if (pmd_none(*pmd))
    {
      printk(KERN_ALERT "[-] PMD_NONE\n");
      goto kernsh_get_page_from_task_error;
    }
  if (pmd_bad(*pmd))
    {
      pmd_clear(pmd);
      goto kernsh_get_page_from_task_error;
    }
  printk(KERN_ALERT "[+] PMD EXIT\n");

  printk(KERN_ALERT "[+] PTE ENTER\n");
  pte = pte_offset_kernel(pmd, addr);
  if (!pte_present(*pte)) 
    {
      printk(KERN_ALERT "[-] PTE_PRESENT\n");
      goto kernsh_get_page_from_task_error;
    }

  printk(KERN_ALERT "[+] PTE EXIT\n");

  page_at_addr = (struct page *)pte_page(*pte);

 kernsh_get_page_from_task_error :
  task_lock(task);  
  spin_unlock(&mm->page_table_lock);
  atomic_dec(&mm->mm_users);
  task_unlock(task);

  printk(KERN_ALERT "[+] kernsh_get_page_from_task EXIT !!\n");

  return page_at_addr;
}

int asmlinkage kernsh_read_virtm(pid_t pid, unsigned long addr, char *buffer, int len)
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
  if(copy_to_user(buffer, kaddr + (addr & ~PAGE_MASK), len))
    {
      printk(KERN_ALERT "[-] copy_from_user error\n");
      kunmap_atomic(kaddr, smp_processor_id());
      return -EFAULT;
    }

  kunmap_atomic(kaddr, smp_processor_id());
  printk(KERN_ALERT "[+] KUNMAP_ATOMIC\n");

  printk(KERN_ALERT "[+] kernsh_read_virtm EXIT !!\n");

  return 0;
}

int asmlinkage kernsh_read_virtm_proc(pid_t pid, unsigned long addr, char *buffer, int len)
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
  memcpy(buffer, kaddr + (addr & ~PAGE_MASK), len);
  /*
  if(copy_to_user(buffer, kaddr + (addr & ~PAGE_MASK), len))
    {
      printk(KERN_ALERT "[-] copy_from_user error\n");
      kunmap_atomic(kaddr, smp_processor_id());
      return -EFAULT;
    }
  */
  kunmap_atomic(kaddr, smp_processor_id());
  printk(KERN_ALERT "[+] KUNMAP_ATOMIC\n");

  printk(KERN_ALERT "[+] kernsh_read_virtm EXIT !!\n");

  return 0;
}

int asmlinkage kernsh_get_virtaddr(pid_t pid, struct mem_addr * temmem)
{
  struct task_struct *task;

  if (temmem == NULL)
    return -EFAULT;

  printk("PID %d\n", pid);
  task = find_task_by_pid(pid);
  if (task == NULL)
    {
      printk(KERN_ALERT "Couldn't find pid %d\n", pid);
      return -1;
    }

  temmem->vm_start = task->mm->mmap->vm_start;

  return 0;
}

int asmlinkage kernsh_read_mem(unsigned long addr, char *buffer, int len)
{

  return 0;
}

int asmlinkage kernsh_write_mem(unsigned long addr, char *buffer, int len)
{

  return 0;
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
  
  proc_entry_kernsh_virtm_dump_elf = 
    create_proc_entry(PROC_ENTRY_KERNSH_VIRTM_DUMP_ELF, 0644, proc_entry_kernsh_virtm);
  proc_entry_kernsh_virtm_info = 
    create_proc_entry(PROC_ENTRY_KERNSH_VIRTM_INFO, 0644, proc_entry_kernsh_virtm);
  proc_entry_kernsh_virtm_vio =
    create_proc_entry(PROC_ENTRY_KERNSH_VIRTM_VIO, 0644, proc_entry_kernsh_virtm);


  if (proc_entry_kernsh_virtm_dump_elf == NULL ||
      proc_entry_kernsh_virtm_info == NULL ||
      proc_entry_kernsh_virtm_vio == NULL)
    {
      printk(KERN_ALERT "Couldn't create sub proc entry\n");
      return -1;
    }
  
  
  proc_entry_kernsh_virtm_dump_elf->read_proc = kernsh_virtm_dump_elf_read;
  proc_entry_kernsh_virtm_dump_elf->write_proc = kernsh_virtm_dump_elf_write;
  proc_entry_kernsh_virtm_dump_elf->owner = THIS_MODULE; 

  proc_entry_kernsh_virtm_info->read_proc = kernsh_virtm_info_read;
  proc_entry_kernsh_virtm_info->write_proc = kernsh_virtm_info_write;
  proc_entry_kernsh_virtm_info->owner = THIS_MODULE;

  proc_entry_kernsh_virtm_vio->read_proc = kernsh_virtm_vio_output;
  proc_entry_kernsh_virtm_vio->write_proc = kernsh_virtm_vio_input;
  proc_entry_kernsh_virtm_vio->owner = THIS_MODULE;

  //  o_syscallnill = sys_call_table[17];
  //sys_call_table[17] = &kernsh_read_virtm;//kernsh_get_virtaddr;

  return 0;
}

static void kernsh_virtm_exit(void)
{
  printk(KERN_ALERT "Kernsh Virtm Exit\n");

  remove_proc_entry(PROC_ENTRY_KERNSH_VIRTM_DUMP_ELF, proc_entry_kernsh_virtm);
  remove_proc_entry(PROC_ENTRY_KERNSH_VIRTM_INFO, proc_entry_kernsh_virtm);
  remove_proc_entry(PROC_ENTRY_KERNSH_VIRTM_VIO, proc_entry_kernsh_virtm);

  remove_proc_entry(PROC_ENTRY_KERNSH_VIRTM, &proc_root);

  //  sys_call_table[17] = o_syscallnill;
}


module_init(kernsh_virtm_init);
module_exit(kernsh_virtm_exit);
