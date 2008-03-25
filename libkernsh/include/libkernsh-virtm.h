#ifndef __LIBKERNSH_VIRTM_H__
 #define __LIBKERNSH_VIRTM_H__

#define PROC_ENTRY_ROOT					"/proc/"
#define PROC_ENTRY_KERNSH_VIRTM				"kernsh_virtm"
#define PROC_ENTRY_KERNSH_VIRTM_DUMP_ELF		"dump_elf"
#define PROC_ENTRY_KERNSH_VIRTM_VIO_INFO		"vio_info"
#define PROC_ENTRY_KERNSH_VIRTM_VIO			"vio"
#define PROC_ENTRY_KERNSH_VIRTM_INFO			"info"

#define PROC_ENTRY_KERNSH_VIRTM_MAX PROC_ENTRY_KERNSH_VIRTM

enum libkernsh_e_virtm_action_type
  {
    LIBKERNSH_VIRTM_READ_MEM,
    LIBKERNSH_VIRTM_WRITE_MEM,
    LIBKERNSH_VIRTM_READ_MEM_PID,
    LIBKERNSH_VIRTM_WRITE_MEM_PID,
    LIBKERNSH_VIRTM_TASK_PID,
    LIBKERNSH_VIRTM_DUMP_ELF_PID
  };

enum libkernsh_e_virtm_type
  {
    LIBKERNSH_KERNEL_MODE,
    LIBKERNSH_USER_MODE,
    LIBKERNSH_VIRTMNUM
  };

typedef struct s_kvirtm_virtual_task_struct
{
  unsigned long state;
  unsigned long flags;
  unsigned long ptrace;


  /* struct mm_struct *mm */
  unsigned long mmap_base;
  unsigned long task_size;
  
  unsigned long start_code, end_code, start_data, end_data;
  
} kvirtm_virtual_task_struct_t;

typedef struct s_libkernshvma_struct
{

  unsigned long vm_start;
  unsigned long vm_end;

} libkernshvma_struct_t;

typedef struct s_kvirtm_action
{
  int action;
  unsigned long addr;
  int len;
  pid_t pid;
  char filename[256];

} kvirtm_action_t;

#endif /* __LIBKERNSH_VIRTM_H_ */
