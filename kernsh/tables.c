/*
** tables.c for kernsh
**
** $Id: tables.c,v 1.5 2007-09-02 21:47:25 pouik Exp $
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

  revm_command_add(CMD_KMODULE, (void *)cmd_kmodule, 
		   (void*) revm_getvarparams, 
		   0, 
		   HLP_KMODULE);

  revm_command_add(CMD_AUTOTYPES, (void *)cmd_autotypes, 
		   (void *) NULL,
		   0,
		   HLP_AUTOTYPES);

  revm_command_add(CMD_KMD5, (void *)cmd_kmd5, 
		   (void *) revm_getdisasm,
		   0,
		   HLP_KMD5);

  revm_command_add(CMD_KCMD5, (void *)cmd_kcmd5, 
		   (void*) revm_getvarparams,
		   0,
		   HLP_KCMD5);


  revm_command_add(CMD_KVIRTM, (void *)cmd_kvirtm, 
		   (void*) revm_getvarparams,
		   0,
		   HLP_KVIRTM);

  revm_command_add(CMD_RPHT, (void *) cmd_rpht, 
		   revm_getregxoption, 
		   1, 
		   HLP_RPHT);
  
  revm_command_add(CMD_RSHT, (void *) cmd_rsht, 
		   revm_getregxoption, 
		   1, 
		   HLP_RSHT);
}
