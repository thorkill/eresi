/*
** libkernsh.h for libkernsh
**
** $Id: libkernsh.h,v 1.9 2007-08-26 18:07:09 pouik Exp $
**
*/
#ifndef __LIBKERNSH_H__
 #define __LIBKERNSH_H__

#include "libaspect.h"
#include "libelfsh.h"

#if defined(__NetBSD__)
#include <kvm.h>
#include <nlist.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/syscall.h>
#endif

#if defined(__linux__)

#define SYSCALL0(num) asm volatile \
( "\n\tint $0x80" : "=a"(__ret) : "0"(num) );

#define SYSCALL1(num,arg0) asm volatile \
( "\n\tint $0x80": "=a"(__ret) : "0"(num), "b" (arg0) );

#define SYSCALL2(num,arg0,arg1) asm volatile \
( "\n\tint $0x80": "=a"(__ret) : "0"(num),"b" (arg0),"c"(arg1) );

#define SYSCALL3(num,arg0,arg1,arg2) asm volatile \
( "\n\tint $0x80": "=a"(__ret) : "0"(num),"b" (arg0),"c"(arg1),"d"(arg2) );

#define SYSCALL4(num,arg0,arg1,arg2,arg3) asm volatile \
( "\n\tint $0x80": "=a"(__ret) : "0"(num),"b" (arg0),"c"(arg1),"d"(arg2),"S"(arg3) );

#define SYSCALL5(num,arg0,arg1,arg2,arg3,arg4) asm volatile \
( "\n\tint $0x80": "=a"(__ret) : "0"(num),"b" (arg0),"c"(arg1),"d"(arg2),"S"(arg3),"D"(arg4) );

#endif

#define GET_32BIT_LSB_FIRST(cp) \
	(((unsigned long)(unsigned char)(cp)[0]) | \
	((unsigned long)(unsigned char)(cp)[1] << 8) | \
	((unsigned long)(unsigned char)(cp)[2] << 16) | \
	((unsigned long)(unsigned char)(cp)[3] << 24))

#define PUT_32BIT_LSB_FIRST(cp, value) do { \
	(cp)[0] = (value); \
	(cp)[1] = (value) >> 8; \
	(cp)[2] = (value) >> 16; \
	(cp)[3] = (value) >> 24; } while (0)

/* The four core functions - F1 is optimized somewhat */

#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

/* This is the central step in the MD5 algorithm. */
#define MD5STEP(f, w, x, y, z, data, s) \
	( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )

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
    LIBKERNSH_OS_FREEBSD,
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
    LIBKERNSH_STATIC_MODE
  } libkernsh_e_debug_type;

#define	__DEBUG_KERNSH__			0

#define LIBKERNSH_VMCONFIG_WITHOUT_KERNEL	"libkernsh.without_kernel"
#define LIBKERNSH_VMCONFIG_DEVICE		"libkernsh.device"
#define LIBKERNSH_VMCONFIG_MODE			"libkernsh.mode"
#define LIBKERNSH_VMCONFIG_SYSTEMMAP		"libkernsh.systemmap"
#define LIBKERNSH_VMCONFIG_MMAP			"libkernsh.mmap"
#define LIBKERNSH_VMCONFIG_MMAP_SIZE		"libkernsh.mmapsize"
#define LIBKERNSH_VMCONFIG_KERNEL	        "libkernsh.kernel"
#define LIBKERNSH_VMCONFIG_GZIP			"libkernsh.gzipcmd"
#define LIBKERNSH_VMCONFIG_OBJCOPY		"libkernsh.objcopycmd"
#define LIBKERNSH_VMCONFIG_LD			"libkernsh.ldcmd"
#define LIBKERNSH_VMCONFIG_KERNELGZ		"libkernsh.kernelgz"
#define LIBKERNSH_VMCONFIG_KERNELELF		"libkernsh.kernelelf"
#define LIBKERNSH_VMCONFIG_USE_KERNEL		"libkernsh.use_kernel"
#define LIBKERNSH_VMCONFIG_STORAGE_PATH		"libkernsh.storagepath"
#define LIBKERNSH_VMCONFIG_NB_SYSCALLS		"libkernsh.nbsyscalls"
#define LIBKERNSH_VMCONFIG_NIL_SYSCALL		"libkernsh.nilsyscall"
#define LIBKERNSH_VMCONFIG_KERNEL_START		"libkernsh.kernel_start"
#define LIBKERNSH_VMCONFIG_KERNEL_END		"libkernsh.kernel_end"
#define LIBKERNSH_VMCONFIG_ALLOC		"libkernsh.alloc"
#define LIBKERNSH_VMCONFIG_KLOAD		"libkernsh.kload"
#define LIBKERNSH_VMCONFIG_KUNLOAD		"libkernsh.kunload"

#define LIBKERNSH_DEFAULT_LINUX_KERNEL		"/boot/vmlinuz"
#define LIBKERNSH_DEFAULT_LINUX_MAP		"/boot/System.map"
#define LIBKERNSH_DEFAULT_LINUX_KERNELGZ	"vmlinuz.gz"
#define LIBKERNSH_DEFAULT_LINUX_KERNELELF	"vmlinux"
#define LIBKERNSH_DEFAULT_LINUX_NB_SYSCALLS	320
#define LIBKERNSH_DEFAULT_LINUX_NIL_SYSCALL	17
#define LIBKERNSH_DEFAULT_LINUX_MMAP_SIZE	0x3e800000
#define LIBKERNSH_DEFAULT_LINUX_INSMOD		"/sbin/insmod"
#define LIBKERNSH_DEFAULT_LINUX_RMMOD		"/sbin/rmmod"
#define LIBKERNSH_DEFAULT_STORAGE_PATH		"/tmp/"
#define LIBKERNSH_DEFAULT_GZIP			"/bin/gzip"
#define LIBKERNSH_DEFAULT_OBJCOPY		"/usr/bin/objcopy"
#define LIBKERNSH_DEFAULT_LD			"/usr/bin/ld"

#define LIBKERNSH_STRING_DEVICE_MEM		"/dev/mem"
#define LIBKERNSH_STRING_DEVICE_KMEM		"/dev/kmem"
#define LIBKERNSH_STRING_DEVICE_KCORE		"/proc/kcore"
#define LIBKERNSH_STRING_REL_GNU		".rel.gnu.linkonce.this_module"

#define LIBKERNSH_I386_LINUX_START		0xc0000000      
#define LIBKERNSH_I386_LINUX_END	      	0xc1000000

#define LIBKERNSH_PAGE_I386_LINUX_OFFSET	0xc0000000


#define GFP_KERNEL				208
#define NAMESIZ					64

/* Idtr segment struct */
struct 
{
	unsigned short limit;
	unsigned long base;
} __attribute__((packed)) idtr;

/* Interrupt descriptor */
struct 
{
	unsigned short off1;
	unsigned short sel;
	unsigned char none,flags;
	unsigned short off2;
} __attribute__ ((packed)) idt;

/* gdtr */
struct 
{
	unsigned short limit;
	unsigned long base;
} __attribute__ ((packed))gdtr;

/* Kmalloc struct */
typedef struct s_libkernshkma 
{
	unsigned long ( *kexec )( size_t, int );     /* Execution of kmalloc */
	int size;	/* Size to alloc */
	int flags;	/* Flags, by default GFP_KERNEL */
	unsigned long mem; /* The address where the allocation has been done */
} libkernshkma_t ;

/* Kfree struct */
typedef struct s_libkernshkfr 
{
	unsigned long ( *kexec )( const void * );     /* Execution of kfree */
	const void * address;			      /* Address to be free */
} libkernshkfr_t;

/* Vmalloc struct */
typedef struct s_libkernshvma 
{
	unsigned long ( *kexec )( size_t );	/* Execution of vmalloc */
	int size;				/* Size to alloc */
	unsigned long mem; /* The address where the allocation has been done */
} libkernshvma_t;

/* Vfree struct */
typedef struct s_libkernshvfr 
{
	unsigned long ( *kexec )( void * );	/* Execution of vfree */
	void * address;				/* Address to be free */
} libkernshvfr_t;

/* Syscall struct */
typedef struct s_libkernshsyscall
{
	unsigned long addr;
	char name[NAMESIZ];
} libkernshsyscall_t;

/* Interrupt struct */
typedef struct s_libkernshint
{
	unsigned long addr;
	char name[NAMESIZ];
} libkernshint_t;

/* Gdt segment struct */
typedef struct s_libkernshsgdt
{
	unsigned long addr;
	unsigned long deb;
	unsigned long fin;
} libkernshsgdt_t;


typedef struct s_libkernshmd5context
{
  unsigned long buf[4];
  unsigned long bits[2];
  unsigned char in[64];
} libkernshmd5context_t;


typedef struct s_dectask
{
	int dec_list;
	int dec_pid;
	int dec_uid;
} dectask_t;

typedef struct s_autotask
{
	int offset_name;
        int offset_list;
	int offset_next;
	int offset_pid;
	int offset_uid;

	dectask_t dectask;
} autotask_t;

/* World kernsh struct */
typedef struct s_libkernshworld
{
	u_int arch;	/* Arch type */
	u_int os;	/* Os type */
	u_int device;	/* Device type */

	int open;	/* 0 => memory close, 1 => memory open */
	int open_static;	/* 0 => static kernel close,
				   1 => static kernel open */

	char *release;	/* Release name */

	int fd;		/* File descriptor for the memory */
	int fdmode;	/* Mode to open memory (read/write) */
	int protmode;	/* Protection mode to mmap */
	int flagsmode;	/* Flags mode to mmap */

	int mmap;	/* 0 => memory not mmap
			   1 => memory mmap */
	int mmap_size;	/* Size of the mmap */
	void *ptr;	/* Pointer of the mmap*/
  
	unsigned long kernel_start;	/* Address of kernel start */
	unsigned long kernel_end;	/* Address of kernel end */

	int mem;	/* Static/Mem mode */

	int physical;	/* 0 => virtual memory address 
			   1 => physical memory address */

	unsigned long idt_base;	/* Address of idt table */
	unsigned short idt_limit;	/* Length */

	unsigned long sct;	/* Address of syscall table */

	unsigned long gdt_base; /* Address of the gdt table */
	unsigned short gdt_limit; /* Lenght */

	autotask_t typetask;
	
	elfshobj_t *root;	/* Pointer to the kernel's elfshobj_t*/
} libkernshworld_t;

extern libkernshworld_t     libkernshworld; /* Global libkernsh struct
					       W00t W00t use it please !*/

/* Init lib */
int	kernsh_init_i386(char *, char *);
int	kernsh_del_i386();

/* Get raw */
void	*kernsh_elfsh_get_raw(elfshsect_t *);
void	*kernsh_revm_get_raw(void *);

/* Information about kernel */
int	kernsh_info();
int	kernsh_info_linux();
int	kernsh_info_netbsd();
int	kernsh_info_freebsd();

elfshobj_t	*kernsh_load_file(char *);
void		kernsh_unload_file(elfshobj_t *);

/* Memory or Static mode */
int	kernsh_is_mem_mode();
int	kernsh_set_mem_mode();
int	kernsh_is_static_mode();
int	kernsh_set_static_mode();

/* Init vectors */
int	kernsh_init_vectors();
int	kernsh_register_vectors();
int	kernsh_register_openmem(u_int, u_int, u_int, void *);
int	kernsh_register_closemem(u_int, u_int, u_int, void *);
int	kernsh_register_readmem(u_int, u_int, u_int, void *);
int	kernsh_register_writemem(u_int, u_int, u_int, void *);
int	kernsh_register_sct(u_int, u_int, void *);
int	kernsh_register_idt(u_int, u_int, void *);
int	kernsh_register_gdt(u_int, u_int, void *);
int	kernsh_register_info(u_int, u_int, void *);
int	kernsh_register_decompkernel(u_int, void *);
int	kernsh_register_loadkernel(u_int, u_int, void *);
int	kernsh_register_symbs_abn(u_int, u_int, void *);
int	kernsh_register_symbs_nba(u_int, u_int, void *);
int	kernsh_register_alloc_contiguous(u_int, void *);
int	kernsh_register_alloc_noncontiguous(u_int, void *);
int	kernsh_register_free_contiguous(u_int, void *);
int	kernsh_register_free_noncontiguous(u_int, void *);
int	kernsh_register_autotypes(u_int, u_int, void *);
int	kernsh_register_relink(u_int, void *);
int	kernsh_register_infect(u_int, void *);
int	kernsh_register_kload(u_int, void *);
int	kernsh_register_kunload(u_int, void *);


/* Memory */
int	kernsh_openmem();
int	kernsh_openmem_kmem_linux_2_6();
int	kernsh_openmem_kmem_linux_2_4();
int	kernsh_openmem_mem_linux();
int	kernsh_openmem_kcore_linux_2_6();
int	kernsh_openmem_netbsd();

int	kernsh_closemem();
int	kernsh_closemem_kmem_linux_2_6();
int	kernsh_closemem_kmem_linux_2_4();
int	kernsh_closemem_mem_linux();
int	kernsh_closemem_kcore_linux_2_6();
int	kernsh_closemem_netbsd();

int	kernsh_readmem(unsigned long, void *, int);
int	kernsh_readmem_kmem_linux_2_6(unsigned long, void *, int);
int	kernsh_readmem_kmem_linux_2_4(unsigned long, void *, int);
int	kernsh_readmem_mem_linux(unsigned long, void *, int);
int	kernsh_readmem_kcore_linux_2_6(unsigned long, void *, int);
int	kernsh_readmem_netbsd(unsigned long, void *, int);

int	kernsh_writemem(unsigned long, void *, int);
int	kernsh_writemem_kmem_linux_2_6(unsigned long, void *, int);
int	kernsh_writemem_kmem_linux_2_4(unsigned long, void *, int);
int	kernsh_writemem_mem_linux(unsigned long, void *, int);
int	kernsh_writemem_kcore_linux_2_6(unsigned long, void *, int);
int	kernsh_writemem_netbsd(unsigned long, void *, int);

/* Syscalls table */
int	kernsh_sct(list_t *);
int	kernsh_sct_linux(list_t *);
int	kernsh_sct_netbsd(list_t *);
int	kernsh_sct_freebsd(list_t *);

int	kernsh_syscall(int, int, unsigned int []);

/* IDT */
int	kernsh_idt(list_t *);
int	kernsh_idt_linux(list_t *);
int	kernsh_idt_netbsd(list_t *);
int	kernsh_idt_freebsd(list_t *);

/* GDT */
int	kernsh_gdt(list_t *);
int	kernsh_gdt_linux(list_t *);
int	kernsh_gdt_netbsd(list_t *);
int	kernsh_gdt_freebsd(list_t *);

/* Symbols */
int	kernsh_get_addr_by_name(char *, unsigned long *, size_t);
int	kernsh_get_name_by_addr(unsigned long, char *, size_t);

int	kernsh_get_addr_by_name_linux_2_6(char *, unsigned long *, size_t);
int	kernsh_get_name_by_addr_linux_2_6(unsigned long, char *, size_t);
int	kernsh_get_addr_by_name_linux_2_4(char *, unsigned long *, size_t);
int	kernsh_get_name_by_addr_linux_2_4(unsigned long, char *, size_t);

int	kernsh_walk_kstrtab(const char *, unsigned long *, size_t);
int	kernsh_get_kernel_syms(char *, unsigned long *, size_t);

/* Tasks */


/* Alloc-Free */
int	kernsh_alloc(size_t, unsigned long *);
int	kernsh_free(unsigned long);

int	kernsh_alloc_contiguous(size_t, unsigned long *);
int	kernsh_free_contiguous(unsigned long);
int	kernsh_alloc_noncontiguous(size_t, unsigned long *);
int	kernsh_free_noncontiguous(unsigned long);

int	kernsh_alloc_contiguous_linux(size_t, unsigned long *);
int	kernsh_free_contiguous_linux(unsigned long);
int	kernsh_alloc_noncontiguous_linux(size_t, unsigned long *);
int	kernsh_free_noncontiguous_linux(unsigned long);

/* Module */
int kernsh_kload_module(char *);
int kernsh_kload_module_linux(char *);

int kernsh_kunload_module(char *);
int kernsh_kunload_module_linux(char *);

int kernsh_relink_module(char *, char *, char *);
int kernsh_relink_module_linux(char *, char *, char *);

int kernsh_infect_module(char *, char *, char *);
int kernsh_infect_module_linux_2_6(char *, elfshobj_t *, char *, char *);
int kernsh_infect_module_linux_2_4(char *, elfshobj_t *, char *, char *);

/* Hijack */

/* MD5 */

int kernsh_md5init(libkernshmd5context_t *);
int kernsh_md5update(libkernshmd5context_t *, 
		     unsigned char *, 
		     unsigned);
int kernsh_md5final(unsigned char digest[16], libkernshmd5context_t *);
int kernsh_md5transform(unsigned long buf[4], const unsigned char inext[64]);
int kernsh_md5dump(unsigned char *, 
		   int, 
		   unsigned char md5buffer[BUFSIZ]);

/* Auto Types */
int kernsh_autotypes();
int kernsh_autotypes_linux_2_6();
int kernsh_autotypes_linux_2_4();
int kernsh_autotypes_netbsd();
int kernsh_autotypes_freebsd();

int kernsh_autotask_linux_2_6();
int kernsh_autotask_linux_2_4();
int kernsh_autotask_netbsd();
int kernsh_autotask_freebsd();

int kernsh_autotask_offsetname_linux_2_6(char *, size_t);
int kernsh_autotask_offsetlist_linux_2_6(char *, size_t);

/* Kernel Decompression */
int	kernsh_decompkernel();
int	kernsh_decompkernel_linux();

/* Kernel Un/Loader */
int	kernsh_loadkernel();
int	kernsh_loadkernel_linux();

int	kernsh_unloadkernel();

/* Misc */
void	*kernsh_find_pattern(const void *, int, const void *, int);
int	kernsh_resolve_systemmap(unsigned long, char *, size_t);
int	kernsh_rresolve_systemmap(const char *,unsigned long *, size_t);

#endif
