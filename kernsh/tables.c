/*
** @file tables.c
** @ingroup kernsh
**
*/
#include "kernsh.h"

/* Setup the command hash table */
void	setup_local_cmdhash()
{
  revm_command_add(CMD_OPENMEM , (void *) cmd_openmem, 
		   (void *) NULL, 
		   0, 
		   HLP_OPENMEM);
  
  revm_command_add(CMD_CLOSEMEM, (void *) cmd_closemem,
		   (void *) NULL, 
		   0, 
		   HLP_CLOSEMEM);
  
  revm_command_add(CMD_KMODE, (void *)cmd_kmode, 
		   (void*) revm_getvarparams, 
		   0, 
		   HLP_KMODE);
  
  revm_command_add(CMD_SCT, (void *)cmd_sct, (void *) NULL, 0, HLP_SCT);
  
  revm_command_add(CMD_IDT, (void *)cmd_idt, (void *) NULL, 0, HLP_IDT);
  
  revm_command_add(CMD_GDT, (void *)cmd_gdt, (void *) NULL, 0, HLP_GDT);
  
  revm_command_add(CMD_KALLOC, (void *)cmd_kalloc, 
		   (void*) revm_getvarparams, 
		   0, 
		   HLP_KALLOC);

  revm_command_add(CMD_KFREE, (void *)cmd_kfree, 
		   (void*) revm_getvarparams, 
		   0, 
		   HLP_KFREE);

  revm_command_add(CMD_KALLOCNC, (void *)cmd_kallocnc, 
		   (void*) revm_getvarparams, 
		   0, 
		   HLP_KALLOCNC);

  revm_command_add(CMD_KFREENC, (void *)cmd_kfreenc, 
		   (void*) revm_getvarparams, 
		   0, 
		   HLP_KFREENC);

  revm_command_add(CMD_KSYM, (void *)cmd_ksym, 
		   (void*) revm_getvarparams, 
		   0, 
		   HLP_KSYM);

  revm_command_add(CMD_KMODULE_RELINK, (void *)cmd_kmodule_relink, 
		   (void*) revm_getvarparams, 
		   0, 
		   HLP_KMODULE_RELINK);

  revm_command_add(CMD_KMODULE_INFECT, (void *)cmd_kmodule_infect, 
		   (void*) revm_getvarparams, 
		   0, 
		   HLP_KMODULE_INFECT);

  revm_command_add(CMD_KMODULE_LOAD, (void *)cmd_kmodule_load, 
		   (void*) revm_getvarparams, 
		   0, 
		   HLP_KMODULE_LOAD);

  revm_command_add(CMD_KMODULE_UNLOAD, (void *)cmd_kmodule_unload, 
		   (void*) revm_getvarparams, 
		   0, 
		   HLP_KMODULE_UNLOAD);

  revm_command_add(CMD_AUTOTYPES, (void *)cmd_autotypes, 
		   (void *) NULL,
		   0,
		   HLP_AUTOTYPES);

  revm_command_add(CMD_KMEM_HASH, (void *)cmd_kmem_hash, 
		   (void *) revm_getdisasm,
		   0,
		   HLP_KMEM_HASH);

  revm_command_add(CMD_KMEM_CHASH, (void *)cmd_kmem_chash, 
		   (void*) revm_getvarparams,
		   0,
		   HLP_KMEM_CHASH);


  revm_command_add(CMD_KVIRTM_INFO, (void *)cmd_kvirtm_info, 
		   (void*) revm_getvarparams,
		   0,
		   HLP_KVIRTM_INFO);
  
  revm_command_add(CMD_KVIRTM_LOADME, (void *)cmd_kvirtm_loadme, 
		   (void*) revm_getvarparams,
		   0,
		   HLP_KVIRTM_LOADME);

  revm_command_add(CMD_KVIRTM_READ_PID, (void *)cmd_kvirtm_read_pid, 
		   (void*) revm_getdisasm,
		   0,
		   HLP_KVIRTM_READ_PID);
  
  revm_command_add(CMD_KVIRTM_WRITE_PID, (void *)cmd_kvirtm_write_pid, 
		   (void*) revm_getvarparams,
		   0,
		   HLP_KVIRTM_WRITE_PID);

  revm_command_add(CMD_KVIRTM_DISASM_PID, (void *)cmd_kvirtm_disasm_pid, 
		   (void*) revm_getdisasm,
		   0,
		   HLP_KVIRTM_DISASM_PID);

  revm_command_add(CMD_KVIRTM_TASK_PID, (void *)cmd_kvirtm_task_pid, 
		   (void*) revm_getvarparams,
		   0,
		   HLP_KVIRTM_TASK_PID);

  revm_command_add(CMD_KVIRTM_DUMP, (void *)cmd_kvirtm_dump, 
		   (void*) revm_getvarparams,
		   0,
		   HLP_KVIRTM_DUMP);

  revm_command_add(CMD_KMEM_READ, (void *)cmd_kmem_read, 
		   (void *) revm_getdisasm,
		   0,
		   HLP_KMEM_READ);
  
  revm_command_add(CMD_KMEM_WRITE, (void *)cmd_kmem_write, 
		   (void*) revm_getvarparams,
		   0,
		   HLP_KMEM_WRITE);

  revm_command_add(CMD_KMEM_DISASM, (void *)cmd_kmem_disasm, 
		   (void *) revm_getdisasm,
		   0,
		   HLP_KMEM_DISASM);

  revm_command_add(CMD_KMEM_INFO, (void *)cmd_kmem_info, 
		   (void *) NULL,
		   0,
		   HLP_KMEM_INFO);

  revm_command_add(CMD_KDUMP_GET_VMA, (void *)cmd_kdump_get_vma, 
		   (void*) revm_getvarparams,
		   0,
		   HLP_KDUMP_GET_VMA);
  
  revm_command_add(CMD_KDUMP_VMA, (void *)cmd_kdump_vma, 
		   (void*) revm_getvarparams,
		   0,
		   HLP_KDUMP_VMA);

  revm_command_add(CMD_RPHT, (void *) cmd_rpht, 
		   revm_getregxoption, 
		   1, 
		   HLP_RPHT);
  
  revm_command_add(CMD_RSHT, (void *) cmd_rsht, 
		   revm_getregxoption, 
		   1, 
		   HLP_RSHT);
}
