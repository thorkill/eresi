/*
** open.c for kernsh
** 
** $Id: open.c,v 1.5 2007-08-26 18:07:09 pouik Exp $
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

      /*
      printf("la\n");
      elfsh_Phdr phdr = elfsh_create_phdr(PT_LOAD, 0, 50, 0);
      printf("la\n");
      elfsh_Phdr *tmp = elfsh_insert_phdr(libkernshworld.root, &phdr);
      printf("0x%lx\n", tmp);

      elfsh_Shdr toto = elfsh_create_shdr(0, SHT_PROGBITS, SHF_EXECINSTR | SHF_ALLOC, 0xc0100000, 0, 1073741823, 0, 0, 0, 0);
      elfshsect_t     *new = elfsh_create_section(".text");
      
      printf("libkernshworld.root->hdr->e_shnum %d\n", libkernshworld.root->hdr->e_shnum);
      ret = elfsh_insert_shdr(libkernshworld.root, toto,  libkernshworld.root->hdr->e_shnum, new->name, 0);
      printf("ret %d\n", ret);
      ret = elfsh_add_section(libkernshworld.root, new, libkernshworld.root->hdr->e_shnum-1, libkernshworld.ptr, ELFSH_SHIFTING_NONE);
      printf("ret %d\n", ret);

      elfsh_set_entrypoint(libkernshworld.root->hdr, (elfsh_Addr)0x100000);
      elfsh_set_objtype(libkernshworld.root->hdr, ET_EXEC);

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
  rl_callback_handler_install(revm_get_prompt(), revm_ln_handler);
  readln_column_update();
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
