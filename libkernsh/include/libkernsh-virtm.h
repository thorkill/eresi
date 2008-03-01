#ifndef __LIBKERNSH_VIRTM_H__
 #define __LIBKERNSH_VIRTM_H__

#define PROC_ENTRY_ROOT					"/proc/"
#define PROC_ENTRY_KERNSH_VIRTM				"kernsh_virtm"
#define PROC_ENTRY_KERNSH_VIRTM_DUMP_ELF		"dump_elf"
#define PROC_ENTRY_KERNSH_VIRTM_VIO			"vio"
#define PROC_ENTRY_KERNSH_VIRTM_SYSCALL_TABLE		"sys_call_table"
#define PROC_ENTRY_KERNSH_VIRTM_INFO			"info"

#define PROC_ENTRY_KERNSH_VIRTM_MAX PROC_ENTRY_KERNSH_VIRTM_SYSCALL_TABLE

enum
  {
    LIBKERNSH_VIRTM_READ_MEM,
    LIBKERNSH_VIRTM_WRITE_MEM,
    LIBKERNSH_VIRTM_READ_MEM_PID,
    LIBKERNSH_VIRTM_WRITE_MEM_PID
  } libkernsh_e_virtm_type;

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

typedef struct s_kvirtm_action
{
  int action;
  unsigned long addr;
  int len;
  pid_t pid;
  char filename[256];

} kvirtm_action_t;

#endif /* __LIBKERNSH_VIRTM_H_ */
