/*
** open.c for kernsh
** 
** $Id: open.c,v 1.4 2007-08-06 15:40:39 pouik Exp $
**
*/
#include "kernsh.h"
#include "libkernsh.h"

/* Open the memory device */
int		cmd_openmem()
{
  int		ret;
  char		buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif

  /* Check if no static is set */
  if (!(int) config_get_data(LIBKERNSH_VMCONFIG_WITHOUT_KERNEL))
    {
      if(!(int) config_get_data(LIBKERNSH_VMCONFIG_USE_KERNEL))
	{
	  /* Gunzip/Extract the kernel */
	  ret = kernsh_decompkernel();
	}
      
      memset(buff, '\0', sizeof(buff));
      snprintf(buff,
	       sizeof(buff),
	       "%s%s",
	       (char *) config_get_data(LIBKERNSH_VMCONFIG_STORAGE_PATH),
	       (char *) config_get_data(LIBKERNSH_VMCONFIG_KERNELELF));
      
      ret = revm_file_load(buff, 0, NULL);

      if (ret)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to load kernel", -1);
    
      libkernshworld.root = revm_lookup_file(buff);
      
      /*elfsh_Shdr toto = elfsh_create_shdr(0, SHT_PROGBITS, SHF_EXECINSTR | SHF_ALLOC, 0xc0000000, 0, 1073741823, 0, 0, 0, 0);
      elfshsect_t     *new = elfsh_create_section("totosec");
      
      ret = elfsh_insert_runtime_shdr(libkernshworld.root, toto, libkernshworld.root->rhdr.rshtnbr, new->name, 1);
      
      ret = elfsh_add_runtime_section(libkernshworld.root, new, 1,libkernshworld.ptr);
      */

      libkernshworld.open_static = 1;
    }
  else 
    {
      kernsh_set_mem_mode();
    }

  /* Open memory */
  ret = kernsh_openmem();

  if (ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot open memory", -1);

  memset(buff, '\0', sizeof(buff));
  snprintf(buff, sizeof(buff), 
	   "%s\n\n",
	   revm_colorfieldstr("[+] OPEN MEMORY"));
  revm_output(buff);
  revm_endline();

#if defined(USE_READLN)
  rl_callback_handler_install(vm_get_prompt(), vm_ln_handler);
  readln_column_update();
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
