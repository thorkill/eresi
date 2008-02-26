#ifndef __VIRTM_H__
 #define __VIRTM_H__

#define PROC_ENTRY_ROOT					"/proc/"
#define PROC_ENTRY_KERNSH_VIRTM				"kernsh_virtm"
#define PROC_ENTRY_KERNSH_VIRTM_DUMP_ELF		"dump_elf"
#define PROC_ENTRY_KERNSH_VIRTM_SYSCALL_TABLE		"sys_call_table"
#define PROC_ENTRY_KERNSH_VIRTM_INFO			"info"

#define PROC_ENTRY_KERNSH_VIRTM_MAX PROC_ENTRY_KERNSH_VIRTM_SYSCALL_TABLE

struct mem_addr {
  unsigned long vm_start;
  unsigned long vm_end;
  unsigned long vm_start_cache;
  unsigned long vm_end_cache;
  int map_count;
  unsigned long startcode;
  unsigned long endcode;
  unsigned long startdata;
  unsigned long enddata;
  unsigned long startbrk;
  unsigned long startstack;
  unsigned long arg_start;
  unsigned long arg_end;
  unsigned long env_start;
  unsigned long env_end;
  unsigned long rss;
  unsigned long total_vm;
  unsigned long locked_vm;
  unsigned long swap_address;
};

#endif /* __VIRTM_H_ */
