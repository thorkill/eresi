/*
** kernsh.h for kernsh
** 
** $Id: kernsh.h,v 1.6 2007-09-02 21:47:25 pouik Exp $
**
*/
#ifndef __KERNSH_H__
 #define __KERNSH_H__

/* User defined configuration */
#include "libstderesi.h"

/* Help strings */
#include "kernsh-help.h"

#define KERNSH_SNAME	       	"kernsh"
#define KERNSH_NAME		"Kernel shell"
#define KERNSH_CONFIG		".kernshrc"

/* Commands */
#define	CMD_OPENMEM		"openmem"
#define CMD_CLOSEMEM            "closemem"
#define CMD_KMODE		"mode"

#define CMD_SCT		        "sct"
#define CMD_IDT			"idt"
#define CMD_GDT			"gdt"

#define CMD_KALLOC		"kalloc"
#define CMD_KFREE		"kfree"
#define CMD_KALLOCNC		"kallocnc"
#define CMD_KFREENC		"kfreenc"

#define CMD_KSYM		"ksym"

#define CMD_KMODULE_RELINK	"kmodule_relink"
#define CMD_KMODULE_INFECT	"kmodule_infect"
#define CMD_KMODULE_LOAD	"kmodule_load"
#define CMD_KMODULE_UNLOAD	"kmodule_unload"

#define CMD_AUTOTYPES		"autotypes"

#define CMD_KMEM_HASH		"kmem_hash"
#define CMD_KMEM_CHASH		"kmem_chash"

#define CMD_KVIRTM_INFO		"kvirtm_info"
#define CMD_KVIRTM_LOADME	"kvirtm_loadme"
#define CMD_KVIRTM_READ_PID	"kvirtm_read_pid"
#define CMD_KVIRTM_WRITE_PID	"kvirtm_write_pid"
#define CMD_KVIRTM_DISASM_PID	"kvirtm_disasm_pid"
#define CMD_KVIRTM_DUMP		"kvirtm_dump"

#define CMD_KMEM_READ		"kmem_read"
#define CMD_KMEM_WRITE		"kmem_write"
#define CMD_KMEM_DISASM		"kmem_disasm"
#define CMD_KMEM_INFO		"kmem_info"

/* Commands execution handlers, each in their respective file */
int	cmd_openmem();
int     cmd_closemem();
int	cmd_kmode();
int     cmd_sct();
int	cmd_idt();
int	cmd_gdt();
int	cmd_kalloc();
int	cmd_kfree();
int	cmd_kallocnc();
int	cmd_kfreenc();
int	cmd_ksym();
int	cmd_kmodule_relink();
int	cmd_kmodule_infect();
int	cmd_kmodule_load();
int	cmd_kmodule_unload();
int	cmd_autotypes();
int	cmd_kmem_hash();
int	cmd_kmem_chash();
int	cmd_kvirtm_info();
int	cmd_kvirtm_loadme();
int	cmd_kvirtm_read_pid();
int	cmd_kvirtm_write_pid();
int	cmd_kvirtm_disasm_pid();
int	cmd_kvirtm_dump();

int	cmd_kmem_read();
int	cmd_kmem_write();
int	cmd_kmem_disasm();
int	cmd_kmem_info();

int	kernsh_virtm_dump_elf(pid_t, char *);
int	kernsh_virtm_view_vmaps(pid_t);
int	kernsh_virtm_get_virtaddr(pid_t);
int	kernsh_virtm_read_pid(pid_t, unsigned long, int);
int	kernsh_virtm_disasm_pid(pid_t, unsigned long, int);

int	export_var(char *, unsigned long, int, char *, int);
int	extract_info(char *, 
		     unsigned long *, 
		     int *, 
		     int *,
		     int *, 
		     int *, 
		     unsigned char *, size_t);

int kernsh_hexdump(unsigned char *, unsigned int, size_t);
int kernsh_disasm(char *, int, unsigned long);
int kernsh_addrlen(revmlist_t *, elfsh_Addr *, int *);

int	kernsh_config();

/* Top skeleton functions */
int	vm_main(int ac, char **av);

/* Commands registration */
void	setup_local_cmdhash();

#endif /* __KERNSH_H_ */

