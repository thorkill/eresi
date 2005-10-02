/*
** e2dbg.c for e2dbg
**
** Misc routines for the debugger
** 
** Started on  Fri Jun 05 15:21:56 2005 mayhem
**
*/
#include "elfsh.h"




/* Debugger library initialisation */
/* Only useful when debugger takes control by .ctors */
void		e2dbg_init(void)
{

#if __DEBUG_E2DBG__
  printf("[e2dbg_init] there\n");
#endif

  memset((void *) &e2dbgworld, 0, sizeof(e2dbgworld_t));

  /* installing sigsev handler */
  SETSIG; 
}



/* Configure and launch the VM from the debugger */
int		e2dbg_entry(int ac, char **av)
{
#if __DEBUG_E2DBG__
  printf("[e2dbg_entry] CHECKPOINT 1\n");
#endif

  /* We have a debugger script pending, continue it */
  if (e2dbgworld.sourcing)
    {
      vm_execscript();
      if (e2dbgworld.sourcing)
	return (0);
    }
  
  /* If the breakpoint is a watchpoint, do not call the debugger */
  /* Displays have been executed already so its no problem to ret */
  if (vm_is_watchpoint(e2dbgworld.curbp))
    return (0);

  vm_setup(ac, av);
  
#if __DEBUG_E2DBG__
  printf("[e2dbg_entry] CHECKPOINT 2\n");
#endif

  /* Only on first execution */
  if (ac == 2)
    {
      /* Debugger only script commands */
      vm_addcmd(CMD_MODE     , (void *) cmd_mode     , vm_getvarparams, 0, HLP_MODE);
      vm_addcmd(CMD_LINKMAP  , (void *) cmd_linkmap  , NULL,            1, HLP_LINKMAP);
      vm_addcmd(CMD_BT       , (void *) cmd_bt       , NULL,            1, HLP_BT);
      vm_addcmd(CMD_BT2      , (void *) cmd_bt       , NULL,            1, HLP_BT);
      vm_addcmd(CMD_BP       , (void *) cmd_bp       , vm_getvarparams, 1, HLP_BP);
      vm_addcmd(CMD_WATCH    , (void *) cmd_watch    , vm_getvarparams, 1, HLP_WATCH);
      vm_addcmd(CMD_BP2      , (void *) cmd_bp       , vm_getvarparams, 1, HLP_BP);
      vm_addcmd(CMD_STACK    , (void *) cmd_stack    , vm_getoption,    1, HLP_STACK);
      vm_addcmd(CMD_DBGSTACK , (void *) cmd_dbgstack , vm_getoption,    1, HLP_DBGSTACK);
      vm_addcmd(CMD_DUMPREGS , (void *) cmd_dumpregs , NULL,            1, HLP_DUMPREGS);
      vm_addcmd(CMD_DELETE   , (void *) cmd_delete   , vm_getoption,    1, HLP_DELETE);
      vm_addcmd(CMD_CONTINUE , (void *) cmd_quit     , (void *) NULL,   1, HLP_CONTINUE);
      vm_addcmd(CMD_CONTINUE2, (void *) cmd_quit     , (void *) NULL,   1, HLP_CONTINUE);
      vm_addcmd(CMD_STEP     , (void *) cmd_step     , (void *) NULL,   1, HLP_STEP);
      vm_addcmd(CMD_DISPLAY  , (void *) cmd_display  , vm_getvarparams, 1, HLP_DISPLAY);
      vm_addcmd(CMD_UNDISPLAY, (void *) cmd_undisplay, vm_getvarparams, 1, HLP_UNDISPLAY);
      vm_addcmd(CMD_RSHT     , (void *) cmd_rsht     , vm_getregxoption, 1, HLP_RSHT);
      vm_addcmd(CMD_RPHT     , (void *) cmd_rpht     , vm_getregxoption, 1, HLP_RPHT);
    }


  if (ac == 2 && !e2dbgworld.step)
    vm_print_banner(av[1]);


  if (world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER)
    if (e2dbg_setup(av[1]) < 0)
      {
	elfsh_error();
	exit(-1);
      }
  vm_doswitch(1);

#if __DEBUG_E2DBG__
  printf("[e2dbg_entry] CHECKPOINT 3\n");
#endif

  return (vm_run(ac, av));
}


/* Load all process files in the debugger */
int			e2dbg_setup(char *name)
{
  elfshsect_t		*got;
  elfsh_Addr		*linkmap_entry;
  void			*data;
  elfshlinkmap_t	*actual;
  char			*gotname;
  char			*ename;

      elfsh_Ehdr	*hdr;
      u_int		elftypenum;
      void		*handle;
      elfshsect_t	*sct;
      elfsh_Sym		*dynsymtab;
      elfsh_Sym		*table;
      u_int		num;
      u_int		bindnum;
      u_int		typenum;
      u_int		index;
      char		*symname;
      void		*symaddr;
      elfsh_Phdr	*gotseg;
      elfsh_Phdr	*sctseg;
      elfsh_Addr	symvalue;
  



  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_E2DBG__
  printf("[e2dbg_setup] there\n");
#endif

  e2dbg_setup_hooks();
  //vm_config();

  printf("DEBUG : name %s\n", name);

  /* Load debugged file */
  if (name != NULL)
    {

      /* No need to fill ET_EXEC base addr */
      /* XXX: FIXME base addr for PIE binaries is unknown */ 
      if (!vm_is_loaded(name) && vm_load_file(name, NULL, NULL) < 0)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Cannot load file", -1);


#if __DEBUG_E2DBG__
      printf("[e2dbg_setup] file %s loaded\n", name);
#endif

      world.curjob->current->linkmap = E2DBG_DYNAMIC_LINKMAP;
    }

  /* Switch to obj 1 */
  if (vm_doswitch(1) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot switch on object 1", -1);    
  
  /* Get ALTGOT or GOT if we used LD_PRELOAD */
  if (!e2dbgworld.preloaded)
    {
      gotname = ELFSH_SECTION_NAME_ALTGOT;
      got = elfsh_get_section_by_name(world.curjob->current, 
				      gotname, NULL, NULL, NULL);
    }
  else
    got = elfsh_get_gotsct(world.curjob->current);
	  
	  
#if __DEBUG_E2DBG__
	  printf("[e2dbg_setup] %s section at " XFMT "\n",
		 got->name, (elfsh_Addr) got);
#endif
	  

  /* Fix first file linkmap entry */
  if (world.curjob->current->linkmap == E2DBG_DYNAMIC_LINKMAP)
    {

      hdr = elfsh_get_hdr(world.curjob->current);
      if (!hdr)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Cannot get ELF header", -1);    

      elftypenum = elfsh_get_objtype(hdr);

      /* Find the PIE binary linkmap */
      if (elftypenum == ET_DYN)
	{
	  elfsh_set_static_mode();
      
	  handle = dlopen(NULL, RTLD_NOW | RTLD_LOCAL);
	  
	  if (!handle)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "dlopen failed", -1);    
	  
	  dynsymtab = elfsh_get_dynsymtab(world.curjob->current, &num);
	  if (!dynsymtab)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Cannot get dynsymtab", -1);    

	  sct = elfsh_get_section_by_name(world.curjob->current, 
					  ELFSH_SECTION_NAME_ALTDYNSYM, 
					  NULL, NULL, &num);

	  if (!sct)
	    sct = elfsh_get_section_by_type(world.curjob->current, SHT_DYNSYM, 
					    NULL, NULL, NULL, &num);

	  if (!sct)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Cannot get dynsym section", -1);    
	    
	  num = num / sizeof(elfsh_Sym);

	  table = (elfsh_Sym *) (sct->shdr->sh_addr ? elfsh_get_raw(sct) : sct->data);
	  if (!table)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Cannot get dynsym data", -1);     
	  
	  gotseg = elfsh_get_parent_segment(world.curjob->current, got);

	  /* Browse symtab */
	  for (index = 0; index < num; index++)
	    {
	      /* Retreive names */
	      typenum = elfsh_get_symbol_type(table + index);
	      bindnum = elfsh_get_symbol_bind(table + index);
	      symname = elfsh_get_dynsymbol_name(world.curjob->current, table + index);
	      
	      sct = elfsh_get_parent_section(world.curjob->current, 
					     table[index].st_value, 
					     NULL);

	      if (sct == NULL && table[index].st_shndx)
		sct = elfsh_get_section_by_index(world.curjob->current, 
						 table[index].st_shndx,
						 NULL, NULL);

	      /* Try to compute base address */
	      if (symname != NULL && *symname)
		{
		  symaddr = dlsym(handle, symname);
		  sctseg = elfsh_get_parent_segment(world.curjob->current, sct);
		  symvalue = elfsh_get_symbol_value(table + index);
		  
		  if (symaddr && sctseg == gotseg && symvalue)
		    {
		      world.curjob->current->rhdr.base = symaddr - symvalue;
		      break;
		    }
		}
	      
	    }
	  
	  if (!world.curjob->current->rhdr.base)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			       "Base address not found", -1);    

	  elfsh_set_debug_mode();
	}

      
      /* Get ALTGOT entry */
      data          = elfsh_get_raw(got);
      linkmap_entry = elfsh_get_got_entry_by_index(data, 1);
      world.curjob->current->linkmap = (elfshlinkmap_t *) 
	elfsh_get_got_val(linkmap_entry);
      
    }

#if __DEBUG_E2DBG__
  printf("[e2dbg_setup] LINKMAP Found at " XFMT "\n", 
	 world.curjob->current->linkmap);
#endif

  vm_doswitch(1);
  elfsh_set_static_mode();

  /* now load all linkmap's files */
  for (actual = elfsh_linkmap_get_lprev(world.curjob->current->linkmap);
       actual != NULL; 
       actual = elfsh_linkmap_get_lprev(actual))
    {
      
#if __DEBUG_E2DBG__
      printf("[e2dbg_setup] Running on LINKMAP PREV " XFMT "\n", 
	     actual);
#endif
      
      ename = elfsh_linkmap_get_lname(actual);
      if (ename && *ename && !vm_is_loaded(ename))
	{
	  if (vm_load_file(ename, elfsh_linkmap_get_laddr(actual), 
			   world.curjob->current->linkmap) < 0)
	    vm_output(" [EE] Loading failed");
	}      
    }

#if __DEBUG_E2DBG__
  printf("[e2dbg_setup] Running on LINKMAP NEXT\n");
#endif

  for (actual = elfsh_linkmap_get_lnext(world.curjob->current->linkmap);
       actual != NULL; 
       actual = elfsh_linkmap_get_lnext(actual))
    {
     
#if __DEBUG_E2DBG__
	  printf("[e2dbg_setup] Running on LINKMAP NEXT " XFMT "\n", 
		 actual);
#endif

      ename = elfsh_linkmap_get_lname(actual);

      if (ename && *ename && !vm_is_loaded(ename))
	{
	  if (vm_load_file(ename, elfsh_linkmap_get_laddr(actual), 
			   world.curjob->current->linkmap) < 0)
	    vm_output(" [EE] Loading failed");
	}      
    }

  /* Everything was OK */
  vm_output("\n");
  elfsh_set_debug_mode();
  vm_doswitch(1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}






/* Get the parent object of a breakpoint */
/* Thats needed for the mprotect stuff inside the breakpoint handler */
elfshobj_t	*vm_get_parent_object(elfsh_Addr addr)
{
  elfsh_Phdr	*cur;
  elfshobj_t	*curfile;
  elfshsect_t	*cursect;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  for (curfile = world.curjob->list; curfile != NULL; curfile = curfile->next)
    {
      cursect = elfsh_get_parent_section(curfile, addr, NULL);
      if (cursect)
	{
	  cur = elfsh_get_parent_segment(curfile, cursect);
	  if (cur)
	    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, curfile);
	}
    }
  
  /* Parent object not found */
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to get parent object addr", (NULL));
}



/* Concatenate all parameters and return a single string */
char	*vm_get_string(char **params)
{
  char	buff[BUFSIZ];
  int	idx;
  int	len;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  for (len = idx = 0; params[idx]; idx++)
    len += snprintf(buff + len, BUFSIZ - len, "%s%s", 
		    (idx ? " " : ""), params[idx]);
   

  if (len)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		       strdup(buff));
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		    "Empty display", NULL);
}

