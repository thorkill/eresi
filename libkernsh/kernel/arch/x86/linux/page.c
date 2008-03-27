/*
** @file page.c
** @ingroup libkernsh_kernel
**
*/
#include "libkernsh-kernel.h"
#include "libkernsh-kernel-linux.h"

/**
 * @brief Get a page of a process id
 * @param task The task_struct
 * @param addr The addr to get the page
 * @return page on success, NULL on error
 */
struct page *kernsh_get_page_from_pid(int pid, unsigned long addr)
{
  struct task_struct *task;
  struct mm_struct *mm;
  struct page *page_at_addr;
  pgd_t * pgd;
  pmd_t * pmd;
  pte_t * pte;
  pud_t *pud;

  page_at_addr = NULL;

  printk(KERN_ALERT "[+] kernsh_get_page_from_task ENTER !!\n");

  task = find_task_by_pid(pid);

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
