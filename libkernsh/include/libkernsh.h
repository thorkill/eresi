/*
** libkernsh.h for libkernsh
**
** $Id: libkernsh.h,v 1.1 2007-07-25 19:53:01 pouik Exp $
**
*/
#ifndef __LIBKERNSH_H__
 #define __LIBKERNSH_H__

#include "revm.h"

#if defined(__NetBSD__)
#include <kvm.h>
#include <nlist.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/syscall.h>
#endif

#if defined(__linux__)
#define SYSCALL0(num) asm volatile ( "\n\tint $0x80" : "=a"(__ret) : "0"(num) );
#define SYSCALL1(num,arg0) asm volatile ( "\n\tint $0x80": "=a"(__ret) : "0"(num), "b" (arg0) );
#define SYSCALL2(num,arg0,arg1) asm volatile ( "\n\tint $0x80": "=a"(__ret) : "0"(num),"b" (arg0),"c"(arg1) );
#define SYSCALL3(num,arg0,arg1,arg2) asm volatile ( "\n\tint $0x80": "=a"(__ret) : "0"(num),"b" (arg0),"c"(arg1),"d"(arg2) );
#define SYSCALL4(num,arg0,arg1,arg2,arg3) asm volatile ( "\n\tint $0x80": "=a"(__ret) : "0"(num),"b" (arg0),"c"(arg1),"d"(arg2),"S"(arg3) );
#define SYSCALL5(num,arg0,arg1,arg2,arg3,arg4) asm volatile ( "\n\tint $0x80": "=a"(__ret) : "0"(num),"b" (arg0),"c"(arg1),"d"(arg2),"S"(arg3),"D"(arg4) );
#endif

enum
  {
    LIBKERNSH_ARCH_I386,
    LIBKERNSH_ARCH_X86_64,
    LIBKERNSH_ARCHNUM
  } libkernsh_e_arch_type;

enum
  {
    LIBKERNSH_OS_LINUX_2_6,
    LIBKERNSH_OS_LINUX_2_4,
    LIBKERNSH_OS_NETBSD,
    LIBKERNSH_OSNUM
  } libkernsh_e_os_type;

enum
  {
    LIBKERNSH_DEVICE_KMEM,
    LIBKERNSH_DEVICE_MEM,
    LIBKERNSH_DEVICE_KCORE,
    LIBKERNSH_DEVICENUM
  } libkernsh_e_device_type;

enum
  {
    LIBKERNSH_MEM_MODE,
    LIBKERNSH_STATIC_MODE,
  } libkernsh_e_debug_type;

#define LIBKERNSH_VMCONFIG_NOSTATIC		"libkernsh.nostatic"
#define LIBKERNSH_VMCONFIG_DEVICE		"libkernsh.device"
#define LIBKERNSH_VMCONFIG_MODE			"libkernsh.mode"
#define LIBKERNSH_VMCONFIG_SYSTEMMAP		"libkernsh.systemmap"
#define LIBKERNSH_VMCONFIG_MMAP			"libkernsh.mmap"
#define LIBKERNSH_VMCONFIG_MMAP_SIZE		"libkernsh.mmapsize"
#define LIBKERNSH_VMCONFIG_KERNEL	        "libkernsh.kernel"
#define LIBKERNSH_VMCONFIG_GZIP			"libkernsh.gzipcmd"
#define LIBKERNSH_VMCONFIG_OBJCOPY		"libkernsh.objcopycmd"
#define LIBKERNSH_VMCONFIG_KERNELGZ		"libkernsh.kernelgz"
#define LIBKERNSH_VMCONFIG_KERNELELF		"libkernsh.kernelelf"

#define LIBKERNSH_VMCONFIG_STORAGE_PATH		"libkernsh.storagepath"
#define LIBKERNSH_VMCONFIG_NB_SYSCALLS		"libkernsh.nbsyscalls"
#define LIBKERNSH_VMCONFIG_NIL_SYSCALL		"libkernsh.nilsyscall"

#define LIBKERNSH_DEFAULT_LINUX_KERNEL		"/boot/vmlinuz"
#define LIBKERNSH_DEFAULT_LINUX_MAP		"/boot/System.map"
#define LIBKERNSH_DEFAULT_LINUX_KERNELGZ	"vmlinuz.gz"
#define LIBKERNSH_DEFAULT_LINUX_KERNELELF	"vmlinux"
#define LIBKERNSH_DEFAULT_STORAGE_PATH		"/tmp/"
#define LIBKERNSH_DEFAULT_LINUX_NB_SYSCALLS	320
#define LIBKERNSH_DEFAULT_LINUX_NIL_SYSCALL	17

#define LIBKERNEL_I386_LINUX_START		0xc0000000      
#define LIBKERNEL_I386_LINUX_END	      	0xc1000000

#define LIBPAGE_I386_LINUX_OFFSET		0xc0000000
#define LIBPAGE_I386_LINUX_MAX			0xffffffff

#define LIBKERNEL_AMD64_LINUX_START		0xffffffff80000000
#define LIBKERNEL_AMD64_LINUX_END		0xffffffff81000000

#define LIBPAGE_AMD64_LINUX_OFFSET		0xffff810000000000
#define LIBPAGE_AMD64_LINUX_MAX			0xffffffffffffffff

#define GFP_KERNEL 208

#define LIBKERNSH_CONFIG			".kernshrc"

struct {
  unsigned short limit;
  unsigned long base;
} __attribute__((packed)) idtr;

struct {
  unsigned short off1;
  unsigned short sel;
  unsigned char none,flags;
  unsigned short off2;
} __attribute__ ((packed)) idt;

typedef struct s_libkernshkma 
{
  unsigned long ( *kexec )( size_t, int );
  int size;
  int flags;
  unsigned long mem;
} libkernshkma_t ;

typedef struct s_libkernshkfr 
{
  unsigned long ( *kexec )( const void * );
  const void * address;
} libkernshkfr_t;

typedef struct s_libkernshvma 
{
  unsigned long ( *kexec )( size_t );
  int size;
  unsigned long mem;
} libkernshvma_t;

typedef struct s_libkernshvfr 
{
  unsigned long ( *kexec )( void * );
  void * address;
} libkernshvfr_t;

typedef struct s_libkernshsyscall
{
  unsigned long addr;
  char name[64];
} libkernshsyscall_t;

typedef struct s_libkernshint
{
  unsigned long addr;
  char name[64];
} libkernshint_t;

typedef struct s_libkernshworld
{

  u_int type;
  u_int os;
  u_int device;

  int open;
  int open_static;

  char *release;
  char *mode;

  int fd;
  int fdmode;
  int protmode;
  int flagsmode; 

  int mmap;
  int mmap_size;
  void *ptr;

#if defined(__linux__)
  char *systemmap;
#endif

  unsigned long kernel_start;
  unsigned long kernel_end;
  unsigned long page_offset;
  unsigned long page_max;

  int mem;

  unsigned long idt_base;
  int idt_limit;

  unsigned long sct;

} libkernshworld_t;

extern libkernshworld_t     libkernshworld;

/* Init lib */
int kernsh_init_i386(char *, char *);
int kernsh_del_i386();
int kernsh_config();
void *kernsh_elfsh_get_raw(void *);
void *kernsh_revm_get_raw(void *);
int kernsh_info();
int kernsh_info_linux();
int kernsh_info_netbsd();
int kernsh_info_freebsd();

/* Memory or Static mode */
int kernsh_is_mem_mode();
int kernsh_set_mem_mode();
int kernsh_is_static_mode();
int kernsh_set_static_mode();
int kernsh_is_nostatic_mode();

/* Init vectors */
int kernsh_init_vectors();
int kernsh_register_vectors();
int kernsh_register_openmem(u_int, u_int, u_int, void *);
int kernsh_register_closemem(u_int, u_int, u_int, void *);
int kernsh_register_readmem(u_int, u_int, u_int, void *);
int kernsh_register_writemem(u_int, u_int, u_int, void *);
int kernsh_register_sct(u_int, u_int, void *);
int kernsh_register_idt(u_int, u_int, void *);
int kernsh_register_info(u_int, u_int, void *);
int kernsh_register_decompkernel(u_int, void *);
int kernsh_register_loadkernel(u_int, u_int, void *);
int kernsh_register_symbs_abn(u_int, u_int, void *);
int kernsh_register_symbs_nba(u_int, u_int, void *);
int kernsh_register_alloc_contiguous(u_int, void *);
int kernsh_register_alloc_noncontiguous(u_int, void *);
int kernsh_register_free_contiguous(u_int, void *);
int kernsh_register_free_noncontiguous(u_int, void *);

/* Memory */
int kernsh_openmem();
int kernsh_openmem_kmem_linux_2_6();
int kernsh_openmem_mem_linux_2_6();
int kernsh_openmem_kcore_linux_2_6();
int kernsh_openmem_netbsd();

int kernsh_closemem();
int kernsh_closemem_kmem_linux_2_6();
int kernsh_closemem_mem_linux_2_6();
int kernsh_closemem_kcore_linux_2_6();
int kernsh_closemem_netbsd();

int kernsh_readmem(unsigned long, void *, int);
int kernsh_readmem_kmem_linux_2_6(unsigned long, void *, int);
int kernsh_readmem_mem_linux_2_6(unsigned long, void *, int);
int kernsh_readmem_kcore_linux_2_6(unsigned long, void *, int);
int kernsh_readmem_netbsd(unsigned long, void *, int);

int kernsh_writemem(unsigned long, void *, int);
int kernsh_writemem_kmem_linux_2_6(unsigned long, void *, int);
int kernsh_writemem_mem_linux_2_6(unsigned long, void *, int);
int kernsh_writemem_kcore_linux_2_6(unsigned long, void *, int);
int kernsh_writemem_netbsd(unsigned long, void *, int);

/* Syscalls table */
int kernsh_sct();
int kernsh_sct_linux();
int kernsh_sct_netbsd();
int kernsh_sct_freebsd();

int kernsh_syscall(int, int, unsigned int []);

/* IDT */
int kernsh_idt();
int kernsh_idt_linux();
int kernsh_idt_netbsd();
int kernsh_idt_freebsd();


/* Symbols */
int kernsh_get_addr_by_name(char *, unsigned long *, size_t);
int kernsh_get_name_by_addr(unsigned long, char *, size_t);

int kernsh_get_addr_by_name_linux_2_6(char *, unsigned long *, size_t);
int kernsh_get_name_by_addr_linux_2_6(unsigned long, char *, size_t);

unsigned long kernsh_walk_kstrtab(const char *, unsigned long *addr, size_t);

/* Tasks */


/* Alloc-Free */
int kernsh_alloc_contiguous(size_t, unsigned long *);
int kernsh_free_contiguous(unsigned long);
int kernsh_alloc_noncontiguous(size_t, unsigned long *);
int kernsh_free_noncontiguous(unsigned long);

int kernsh_alloc_contiguous_linux(size_t, unsigned long *);
int kernsh_free_contiguous_linux(unsigned long);
int kernsh_alloc_noncontiguous_linux(size_t, unsigned long *);
int kernsh_free_noncontiguous_linux(unsigned long);


/* Hijack */


/* Kernel Decompression */
int kernsh_decompkernel();
int kernsh_decompkernel_linux();

/* Kernel Loader */
int kernsh_loadkernel();
int kernsh_loadkernel_linux();

int kernsh_unloadkernel();

/* Misc */
void *kernsh_find_pattern(const void *, int, const void *, int);
int kernsh_resolve_systemmap(unsigned long, char *, size_t);
int kernsh_rresolve_systemmap(const char *,unsigned long *, size_t);

#endif
