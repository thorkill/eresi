/*
** ia32.c for libelfsh
**
** Started on  Fri Jan 11 03:05:37 2003 mayhem
**
*/
#include "libelfsh.h"
#include "libasm.h"



static asm_processor	proc;



/* Write a breakpoint 0xCC in memory */
/* One of the 2 breakpoint technique of e2dbg */
int			elfsh_break_ia32(elfshobj_t *f,
					 elfshbp_t  *bp)
{
  int			prot;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
#if __DEBUG_BREAKPOINTS__
  printf("[ia32::break_ia32] bp->addr %08X \n", bp->addr);
#endif

  bp->savedinstr[0] = (*(char *) bp->addr);
  prot = elfsh_munprotect(f, bp->addr, 4);
  (*(char *) bp->addr) = 0xCC;
  elfsh_mprotect(bp->addr, 4, prot);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}



/* Static hooking for IA32 */
int			elfsh_cflow_ia32(elfshobj_t	*file,
					 char		*name,
					 elfsh_Sym	*symbol,
					 elfsh_Addr	addr)
{
  elfshsect_t		*hooks;
  elfshsect_t		*source;
  asm_instr		instrs[5];
  char			buff[32];
  int			ret, len;
  int			off;
  int			idx;
  char			*hookbuf;
  char			*hook;
  char			bufname[BUFSIZ];
  int			prot;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

#if	__DEBUG_CFLOW__      
  printf("[DEBUG_CFLOW] Requesting hijack addr = %08X, sym.st_value = %08X, name = %s\n", 
	 addr, symbol->st_value, name);
#endif

  /* Resolve parameters */
  off = elfsh_get_foffset_from_vaddr(file, symbol->st_value);
  if (!off) 
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid address to hijack", -1);

  ret = elfsh_raw_read(file, off, buff, 32);
  if (ret != 32)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Function too small to be hijacked", -1);

  /* If the hook section does not exist, create it */
  hooks = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_HOOKS, 
				    0, 0, 0); 

  if (!hooks)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get and inject .hooks", -1);

  hook = elfsh_get_raw(hooks) + hooks->curend;

  /* Determine the minimal aligned length */
  asm_init_i386(&proc);
  for (idx = ret = 0; ret < 5 && idx < 5; idx++)
    ret += asm_read_instr(instrs + idx, buff + ret, 32 - ret, &proc);

  /* Create the hook for this function */
  prot = elfsh_munprotect(file, (elfsh_Addr) hook, 16);
  memset(hook, 0x90, 40);
  memcpy(hook, "\xe9\x00\x00\x00\x00", 5);
  *(uint32_t *) ((char *) hook + 1) = addr - (hooks->shdr->sh_addr + (hook + 1 - (char *) elfsh_get_raw(hooks)) + 4);
  memcpy(hook + 5, buff, ret);
  memcpy(hook + 5 + ret, "\xe9\x00\x00\x00\x00", 5);
  *(uint32_t *) ((char *) hook + 6 + ret) = symbol->st_value - (hooks->shdr->sh_addr + hooks->curend + 10);
  elfsh_mprotect((elfsh_Addr) hook, 16, prot);

  /* We need to grab the parent section to compute the remaining offset */
  source = elfsh_get_parent_section_by_foffset(file, off, NULL);
  if (!source)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find parent section for hooked addr", -1);

  /* Install the hook */
#if	__DEBUG_CFLOW__      
  printf("[DEBUG_CFLOW] Installing hook with addr %08X (curhookent addr = %08X with curend = %u symbol.st_value %08X) \n", 
	 ((hooks->shdr->sh_addr + hooks->curend) - (symbol->st_value + 5)),
	 (hooks->shdr->sh_addr + hooks->curend), hooks->curend, symbol->st_value);
#endif

  /* Ret: the minimal instruction aligned length for installing the hook caller-side properly */
  hookbuf = alloca(ret);

  //prot = elfsh_munprotect(hookbuf, 16);
  memcpy(hookbuf, "\xe9\x00\x00\x00\x00", 5);
  *(uint32_t *) ((char *) hookbuf + 1) = (hooks->shdr->sh_addr + hooks->curend) - (symbol->st_value + 5);
  memset(hookbuf + 5, 0x90, ret - 5);
  len = elfsh_raw_write(file, off, hookbuf, ret);
  if (len != ret)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Error during hook installation", -1);
  //elfsh_mprotect(hookbuf, 16, prot);

  /* Insert the old symbol on the original saved bytes (do the insertion AFTER all use of symbol !!) */
  snprintf(bufname, BUFSIZ, "old_%s", name);
  elfsh_insert_funcsym(file, bufname, hooks->shdr->sh_addr + hooks->curend + 5,
		       ret + 5, hooks->index);
  snprintf(bufname, BUFSIZ, "hook_%s", name);
  elfsh_insert_funcsym(file, bufname, hooks->shdr->sh_addr + hooks->curend,
		       ret + 10, hooks->index);

#if	__DEBUG_CFLOW__      
  printf("[DEBUG_CFLOW] hook_legit_func = %08X, old_legit_func = %08X \n", 
	 hooks->shdr->sh_addr + hooks->curend, 
	 hooks->shdr->sh_addr + hooks->curend + 5);
#endif

  /* Everything OK, ret usually is 6 on x86 */
  hooks->curend += ret + 10;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* 
   PLT hijacking on i386 dyn objects 
   
   Tested on BeoS only 

   ***** Not used for ET_DYN manipulation on other OS than BeOS ******

   Adapted by zagig for BeoS from libelfsh/hijack.c 
   
*/
/*
int		elfsh_hijack_plt_ia32_etdyn(elfshobj_t *file, 
					    elfsh_Sym *symbol,
					    elfsh_Addr addr)
{
  int		foffset;
  uint8_t	opcode = 0xe9;
  uint32_t	displacement;
  elfshsect_t   *rel_plt;
  elfshsect_t   *plt;
  elfshsect_t	*symtab;
  elfshsect_t	*dynsym;
  u_int		off, index, plt_rel_index;
  u_int		entsz;
  int		plt_index;
  char		*name;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  plt = elfsh_get_section_by_name(file,
				  ELFSH_SECTION_NAME_PLT,
				  &plt_index, NULL, NULL);
  if (plt == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Unable to get PLT", -1);
  

  entsz = elfsh_get_pltentsz(file);
  if (entsz < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid PLT entry size", -1);
  if (NULL == elfsh_get_dynsymtab(file, NULL))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to get DYNSYM", -1);
  if (NULL == elfsh_get_symtab(file, NULL))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to get SYMTAB", -1);
  
  if (file->hdr->e_machine != EM_386)     
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "File is not IA32", -1);
  
  dynsym = file->secthash[ELFSH_SECTION_DYNSYM];
  symtab = file->secthash[ELFSH_SECTION_SYMTAB];
  
  rel_plt = elfsh_get_section_by_index(file, plt_index, NULL, NULL);
  if (rel_plt == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find .rel.plt section", -1);
  
  off = entsz;
  for (index = 0; 
       index < (rel_plt->shdr->sh_size / sizeof(elfsh_Rel)); 
       off += entsz, index++)
    {
      elfsh_Rel *rel_entry;
      
      rel_entry = ((elfsh_Rel *) rel_plt->data + index);
      
      if (off == 0)
	continue;
      name  = elfsh_get_symname_from_reloc(file, rel_entry);
      if (!name)
	continue;
      
      if (!strcmp(name, elfsh_get_symbol_name(file, symbol)))
	{

          displacement = addr - (plt->shdr->sh_addr + off) - 5;
          foffset = plt->shdr->sh_addr + off;
          elfsh_raw_write(file, foffset, &opcode, sizeof(opcode));

#if __BYTE_ORDER == __BIG_ENDIAN
          displacement = swap32(displacement);
          elfsh_raw_write(file, foffset + sizeof(opcode), 
			  &displacement, sizeof(displacement));
#else
          elfsh_raw_write(file, foffset + sizeof(opcode), 
			  &displacement, sizeof(displacement));
#endif
          ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	}
    }
 
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Cannot find PLT entry to patch", -1);
}
*/



/* PLT hijacking on i386 for ET_DYN objects */
int		elfsh_hijack_plt_ia32(elfshobj_t *file, 
				      elfsh_Sym *symbol,
				      elfsh_Addr addr)
{
  int		foffset;
  uint8_t	opcode; 
  uint32_t	displacement;
  int		prot;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (file->hdr->e_machine != EM_386)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "File is not IA32", -1);

  /* Compute jmp displacement, 5 is the length of jmp opcode */
  /* Direct jmp with full displacement */
  opcode       = 0xE9;
  displacement = addr - symbol->st_value - 5;
  foffset      = elfsh_get_foffset_from_vaddr(file, symbol->st_value);
  prot         = elfsh_munprotect(file, 
				  symbol->st_value, 
				  elfsh_get_pltentsz(file));
  elfsh_raw_write(file, foffset, &opcode, sizeof(opcode));

  /* Byte order test for cross-endianess ability */
#if __BYTE_ORDER == __BIG_ENDIAN
  displacement = swap32(displacement);
  elfsh_raw_write(file, foffset + sizeof(opcode), 
		  &displacement, sizeof(displacement));
#else
  elfsh_raw_write(file, foffset + sizeof(opcode), 
		  &displacement, sizeof(displacement));
#endif

  elfsh_mprotect(symbol->st_value, 
		 elfsh_get_pltentsz(file), prot);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Perform relocation on entry for INTEL architecture */
/* not endianess independant - ym */
int      elfsh_relocate_ia32(elfshsect_t	*new,
			     elfsh_Rel		*cur,
			     elfsh_Addr		*dword,
			     elfsh_Addr		addr,
			     elfshsect_t	*mod)
{
  elfsh_Shdr *section;
  elfsh_Sym  *symbol;	
  char	     *symname;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
   
  switch (elfsh_get_reltype(cur))
    {

      /* Absolute reference */
    case R_386_32:
      *dword = addr;
      break;

      /* Relative reference */
    case R_386_PC32:
      *dword = addr - (new->shdr->sh_addr + cur->r_offset + 4);
      break;

    case R_386_GOT32:   
      section = elfsh_get_sht_entry_by_name(new->parent, ELFSH_SECTION_NAME_GOT);
      if (section == NULL)
         ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			   "Unable to find GOT for GOTOFF", -1);
      *dword = cur->r_offset - section->sh_addr;
      break;
    
    case R_386_PLT32:
      if (elfsh_static_file(new->parent))
	{
#if	__DEBUG_RELADD__      
	  printf("[DEBUG_RELADD] R_386_PLT32 on STATIC file : %s \n", new->parent->name);
#endif      
	  symname = elfsh_get_symname_from_reloc(mod->parent, cur); 
#if	__DEBUG_RELADD__      
	  printf("[DEBUG_RELADD] R_386_PLT32 : %s\n", symname); 
#endif	  
	  if (symname == NULL)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			      "Unable to find symbol name", -1);
	  

	  symbol = elfsh_get_symbol_from_reloc(mod->parent, cur);
	  if (symbol == NULL)
	   ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			      "Unable to find symbol", -1);
	  *dword = symbol->st_value + addr - (new->shdr->sh_addr + cur->r_offset + 4);
	}
      else
	{
	  section = elfsh_get_sht_entry_by_name(new->parent, ELFSH_SECTION_NAME_PLT);
	  if (section == NULL)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			      "Unable to find PLT fot PLT32", -1);
	
#if	__DEBUG_RELADD__        
	  printf("[DEBUG_RELADD] R_386_PLT32: *dword value was %08X \n", *dword);
#endif

	  /* Basicaly the same */
	  *dword = addr - (new->shdr->sh_addr + cur->r_offset + 4);
	  
#if	__DEBUG_RELADD__        
	  printf("[DEBUG_RELADD] R_386_PLT32 on DYNAMIC file : set dword[*%08X] = %08X (reach PLT entry %08X) \n", 
		 new->shdr->sh_addr + cur->r_offset, *dword, addr);
#endif

	}
      break;

    case R_386_GOTOFF:
      section = elfsh_get_sht_entry_by_name(new->parent, ELFSH_SECTION_NAME_GOT);
      if (section == NULL)
         ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			   "Unable to find GOT for GOTOFF", -1);
      *dword = addr - section->sh_addr;
      break;

    case R_386_GOTPC:
      section = elfsh_get_sht_entry_by_name(new->parent, ELFSH_SECTION_NAME_GOT);
      if (section == NULL)
         ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                        "Unable to find GOT for GOTPC", -1);

      *dword = section->sh_addr - (new->shdr->sh_addr + cur->r_offset) + 2;

#if	__DEBUG_RELADD__        
      printf("[DEBUG_RELADD] R_386_GOTPC : *dword=%08X, non added *dword was %08X \n", 
	     section->sh_addr - (new->shdr->sh_addr + cur->r_offset) + 2, *dword);
#endif

      break;

    default:
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unsupported relocation type", -1);
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
