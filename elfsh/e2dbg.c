/*
** e2dbg.c for elfsh
**
** The main debugger commands
** 
** Started on  Fri Jun 05 15:21:56 2005 mayhem
**
*/
#include "elfsh.h"



e2dbgworld_t	e2dbgworld;



/* Change between static (ondisk) and dynamic (memory) mode */
int		cmd_mode()
{
  char		*param;
  char		buf[256];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  param = world.curjob->curcmd->param[0];
  if (!param)
    {
      if (elfsh_is_static_mode())
	param = "STATIC";
      else if (elfsh_is_debug_mode())
	param = "DYNAMIC";
      else
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unknown e2dbg mode", -1);
      snprintf(buf, sizeof(buf), " [*] e2dbg is in %s MODE \n\n", param);
      vm_output(buf);
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  
  if (*param == 's' || *param == 'S')
    {
      elfsh_set_static_mode();
      vm_output(" [*] e2dbg is now in STATIC mode\n\n"); 
    }
  else if (*param == 'd' || *param == 'D')
    {
      if (world.curjob)
	{
	  if (world.curjob->current)
	    if (!world.curjob->current->linkmap && !world.curjob->current->base)
	      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
				"Can't switch to dynamic"
				" mode on not mapped file.", -1);
	}
      elfsh_set_debug_mode();
      vm_output(" [*] e2dbg is now in DYNAMIC mode\n\n"); 
    }
  else
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unknown mode for E2DBG", -1);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Dump the linkmap */
int		 vm_linkmap(elfshobj_t *file)
{
  char		 logbuf[BUFSIZ];
  elfshlinkmap_t *actual;
  int		 i = 1;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!file)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid argument", -1);

  if (!file->linkmap)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No linkmap", -1);

  vm_output(" .::. Linkmap entries .::. \n");

  for (actual = elfsh_linkmap_get_lprev(file->linkmap); actual != NULL; 
       actual = elfsh_linkmap_get_lprev(actual), i++)
    {
      snprintf(logbuf, BUFSIZ - 1, 
	       " [%02u] addr : " XFMT " dyn : " XFMT " - %s\n", i, 
	       (elfsh_Addr) elfsh_linkmap_get_laddr(actual), 
	       (elfsh_Addr) elfsh_linkmap_get_lld(actual),
	       elfsh_linkmap_get_lname(actual));
      vm_output(logbuf);
    }

  for (actual = file->linkmap; actual != NULL; 
       actual = elfsh_linkmap_get_lnext(actual), i++)
    {
      snprintf(logbuf, BUFSIZ - 1, 
	       " [%02u] addr : "XFMT" dyn : "XFMT" - %s\n", i, 
	       (elfsh_Addr) elfsh_linkmap_get_laddr(actual), 
	       (elfsh_Addr) elfsh_linkmap_get_lld(actual),
	       elfsh_linkmap_get_lname(actual));
      vm_output(logbuf);
    }

  vm_output("\n");
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Linkmap dump command */
int		cmd_linkmap()
{
  int		old;
  int		ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (elfsh_is_debug_mode())
    {
      if (!world.curjob)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "No current workspace", -1);
      
      if (!world.curjob->current)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "No current file", -1);
      
      old = world.curjob->current->id;

      /* switch to the main program file */
      vm_doswitch(1);
      ret = vm_linkmap(world.curjob->current);
      vm_doswitch(old);
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
    }
  else
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Debug mode only command", (-1));
}



/* Display backtrace with sym resolution */
int		vm_bt()
{
  void		*frame;
  void		*ret;
  char		*name;
  int		off;
  char		logbuf[BUFSIZ];
  int		i = 0;
  elfshpath_t	*ssp;
  elfsh_Addr	savedsp;
  char		printed;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  if (!world.curjob)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No current workspace", -1);
  if (!world.curjob->current)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No current file", -1);
  ssp = hash_get(&vars_hash, ELFSH_SSPVAR);
  if (!ssp)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No saved SP", -1);
  savedsp = ssp->immed_val.ent;
  printed = 0;

  /* Get the current frame */
  frame = elfsh_bt_get_frame();
  
  /* Backtrace frames */
  do {

    /* XXX probably arch dependant */
    /* assume that the ret address is one word behind frame pointer */
    /* Need to add backtrace hook */
    if (!printed && savedsp <= ((elfsh_Addr) frame + 4))
      {
	vm_output(" [**] <----- Here start e2dbg internal bt ----->\n");
	if (e2dbgworld.curbp)
	  {
	    snprintf(logbuf, BUFSIZ, " [**] " XFMT " %s\n",
		     e2dbgworld.curbp->addr, e2dbgworld.curbp->symname);
	    vm_output(logbuf);
	  }
	printed = 1;
      }

    ret = (void *) (*((long *) frame + 1));
    
    /* Resolve and print current trace frame */
    name = vm_resolve(world.curjob->current, (elfsh_Addr) ret, &off);
    if (!name)
      name = "?";
    if (off)
      snprintf(logbuf, BUFSIZ - 1, " [%02d] "XFMT" <%s + %d>\n", i, 
	       (elfsh_Addr) ret, 
	       name, 
	       off);
    else
      snprintf(logbuf, BUFSIZ - 1, " [%02d] "XFMT" <%s>\n", i, 
	       (elfsh_Addr) ret, 
	       name);
    vm_output(logbuf);

    /* This seems to be the signal vsyscall return address */
    if (ret == E2DBG_VSYSCALL_RETADDR)
      vm_output(" [**] Dectected VSYSCALL return address : one function missing\n");

    frame = elfsh_bt(frame);    
    i++;

  } while (frame); 

  vm_output("\n");
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Backtrace command */
int		cmd_bt()
{
  int		ret;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!elfsh_is_debug_mode())
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Not in dynamic or debugger mode", -1);
  vm_output(" .:: Backtrace ::. \n");
  ret = vm_bt();
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}



/* Symbol matching on the stack content. Always useful */
int		vm_dumpstack(u_int32_t size, elfsh_Addr start)
{
  long		*i;
  char		logbuf[BUFSIZ];
  char		*name;
  int		off;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  vm_output(" .:: Stack ::.\n");
  
  /* Just a simple loop that dump resolved stack content */
  for (i = (long *) start; i < (long *) start + size; i++)
    {
      if ((elfsh_Addr) i >= E2DBG_KERNELBASE)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Cannot dump anymore : end of stack", -1);

      name = vm_resolve(world.curjob->current, *i, &off);
      if (!name)
	name = "?";
      if (off)
	snprintf(logbuf, BUFSIZ - 1, " " XFMT " " XFMT " <%s + %d> \n", 
		 (elfsh_Addr) i, (elfsh_Addr) *i, name, off);
      else
	snprintf(logbuf, BUFSIZ - 1, " " XFMT " " XFMT " <%s> \n", 
		 (elfsh_Addr) i, (elfsh_Addr) *i, name);      
      vm_output(logbuf);
    }
  vm_output("\n");
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Stack display with symbols */
int		cmd_stack()
{
  char		*param;
  elfsh_Addr	  size;
  elfshpath_t	*ssp;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  if (!elfsh_is_debug_mode())
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Command only available in e2dbg", (-1));
  param = world.curjob->curcmd->param[0];
  if (!param)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid argument", (-1));
  ssp = hash_get(&vars_hash, ELFSH_SSPVAR);
  if (!ssp)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No saved SP", -1);

  /* Dump debuggee stack */
  if (vm_isnbr(param))
    {
      if (sscanf(param, UFMT, &size) != 1)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid argument", (-1));
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			 (vm_dumpstack(size, ssp->immed_val.ent));
    }  
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Invalid parameter", -1);
}




/* Debugger stack display with symbols */
int		cmd_dbgstack()
{
  char		*param;
  elfsh_Addr	  size;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  if (!elfsh_is_debug_mode())
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Command only available in e2dbg", (-1));
  param = world.curjob->curcmd->param[0];
  if (!param)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid argument", (-1));

  /* Dump debugger stack */
  if (vm_isnbr(param))
    {
      if (sscanf(param, UFMT, &size) != 1)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid argument", (-1));
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			 (vm_dumpstack(size, (elfsh_Addr) &param)));
    }  
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		    "Invalid parameter", -1);
}





/* Run the binary */
int		cmd_run()
{
  int		ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!world.curjob->curcmd->param[0])
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", -1);

#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif

  ret = execv(world.curjob->curcmd->param[0], 
	      world.curjob->curcmd->param);


#if defined(USE_READLN)
  rl_callback_handler_install(vm_get_prompt(), vm_ln_handler);
#endif

  if (ret)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot execute ELF binary", -1);
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}





/* Add a breakpoint without using a script command */
int		vm_bp_add(elfsh_Addr addr)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (elfsh_bp_add(&e2dbgworld.bp, world.curjob->current, addr)));
}




/* Find a breakpoint by various ways */
elfshbp_t	*vm_lookup_bp(char *name)
{
  elfsh_Addr	addr;
  elfsh_Sym	*sym;
  elfshbp_t	*bp;
  uint16_t	bpid;
  char		straddr[32];
  char		logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  bp = NULL;
  
  /* Lookup by vaddr */
  if (IS_VADDR(name))
    {
      
      if (sscanf(name + 2, AFMT, &addr) != 1)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Invalid virtual address requested", 
			  NULL);
    }

  /* Try to lookup by ID */
  else if (vm_isnbr(name))
    {
      bpid = atoi(name);
      bp   = vm_get_bp_by_id(bpid);
      if (!bp)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                          "Invalid breakpoint ID", NULL);
    }

  /* Resolve symbol */
  /* Sometimes we fix symbols on the disk only */
  /* This avoid a mprotect */
   else
    {
      sym = elfsh_get_metasym_by_name(world.curjob->current, 
				      name);
      if (!sym || sym->st_value == NULL)
	{
	  elfsh_toggle_mode();
	  sym = elfsh_get_metasym_by_name(world.curjob->current,
					  name);
	  elfsh_toggle_mode();
	}
      if (!sym)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "No symbol by that name in the current file", 
			  NULL);

      if (sym->st_value == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Requested symbol address unknown",
			  NULL);
      addr = sym->st_value;
    }

  /* Get the breakpoint */
  if (!bp)
    {
      snprintf(straddr, sizeof(straddr), XFMT, addr);
      bp = hash_get(&e2dbgworld.bp, straddr);
      if (!bp)
	{
	  snprintf(logbuf, BUFSIZ, 
		   "\n [!] No breakpoint set at addr %08X \n\n", addr);
	  vm_output(logbuf);
	  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			    "No breakpoint at this address", NULL);
	}
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, bp);
}





/* Breakpoint command */
int		cmd_bp()
{
  char		*str;
  int		ret;
  elfsh_Addr	addr;
  elfsh_Sym	*sym;
  char		logbuf[BUFSIZ];
  int		idx;
  int		index;
  hashent_t	*actual;
  int		idx2 = 0;
  int		off = 0;
  char		*name;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* build argc */
  for (idx = 0; world.curjob->curcmd->param[idx] != NULL; idx++);
  str = world.curjob->curcmd->param[0];

  /* List breakpoints */
  if (idx == 0)
    {  
      vm_output(" .:: Breakpoints ::.\n\n");	      
      for (index = 0; index < e2dbgworld.bp.size; index++)
	{
	  for (actual = e2dbgworld.bp.ent + index; 
	       actual != NULL && actual->key != NULL; 
	       actual = actual->next)
	    {
	      idx2++;
	      name = vm_resolve(world.curjob->current, 
				(elfsh_Addr) ((elfshbp_t *) 
					      actual->data)->addr, &off);

	      if (off)
		snprintf(logbuf, BUFSIZ, " [%02u] " XFMT " <%s + %d>\n", 
			 ((elfshbp_t *) actual->data)->id, 
			 ((elfshbp_t *) actual->data)->addr, 
			 name, 
			 off);
	      else
		snprintf(logbuf, BUFSIZ, " [%02u] " XFMT " <%s>\n", 
			 ((elfshbp_t *) actual->data)->id,
			 ((elfshbp_t *) actual->data)->addr, 
			 name);
		  
	      vm_output(logbuf);
	    }
	}

      if (!idx2)
	vm_output(" [*] No breakpoints\n");
      
      vm_output("\n");
    }

  /* Supply a new breakpoint */
  else if (idx == 1)
    {
      if (!elfsh_is_debug_mode())
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Not in dynamic or debugger mode", -1);
  
      if (!str || !(*str))
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid argument", -1);

      /* Break on a supplied virtual address */
      if (IS_VADDR(str))
	{
	  if (sscanf(str + 2, AFMT, &addr) != 1)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Invalid virtual address requested", (-1));
	}

      /* Resolve first a function name */
      else
	{
	  sym = elfsh_get_metasym_by_name(world.curjob->current, str);

	  /* Sometimes we fix symbols on the disk */
	  /* We avoid a mprotect */
	  if (!sym || sym->st_value == NULL)
	    {
	      elfsh_toggle_mode();
	      sym = elfsh_get_metasym_by_name(world.curjob->current, str);
	      elfsh_toggle_mode();
	    }
	  
	  if (!sym)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "No symbol by that name in the current file", (-1));
	  if (sym->st_value == NULL)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Requested symbol address unknown", (-1));
	  
	  addr = sym->st_value;
	}

      /* Add the breakpoint */
      ret = vm_bp_add(addr);
      if (ret < 0)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Breakpoint insertion failed\n", (-1));
      if (ret >= 0)
	{
	  name = vm_resolve(world.curjob->current, addr, &off);
	  if (!off)
	      snprintf(logbuf, BUFSIZ - 1, 
		       " [*] Breakpoint added at <%s> (" XFMT ")\n\n", 
		       name, addr);
	    else
	      snprintf(logbuf, BUFSIZ - 1, 
		       " [*] Breakpoint added at <%s + %u> (" XFMT ")\n\n", 
		       name, off, addr);
	  vm_output(logbuf);
	}
    } 
  else 
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Wrong arg number", (-1));
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}




/* Add a breakpoint */
int		elfsh_bp_add(hash_t *bps, elfshobj_t *file, elfsh_Addr addr)
{
  u_char	archtype;
  u_char	objtype;
  u_char	ostype;
  elfshbp_t	*bp;
  char		tmp[32];
  int		ret;
  static int	lastbpid = 1;
  char		buf[BUFSIZ];
  char		*name;
  int		off;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || addr == 0 || bps == 0) 
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  /* Breakpoints handlers must be initialized */
  elfsh_setup_hooks();
  archtype = elfsh_get_archtype(file);
  objtype = elfsh_get_elftype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      objtype  == ELFSH_TYPE_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid target", -1);

  /* Resolve breakpoint address */
  name = vm_resolve(file, addr, &off);
  if (off)
    snprintf(buf, BUFSIZ, "<%s + %d>", name, off);
  else
    snprintf(buf, BUFSIZ, "<%s>", name);

  XALLOC(bp , sizeof(elfshbp_t), (-1));
  bp->type    = INSTR;
  bp->addr    = addr;
  bp->symname = elfsh_strdup(buf);
  bp->obj     = vm_get_parent_object(addr);
  if (!bp->obj)
    vm_output("Warning : Unknown parent object for breakpoint\n");

  snprintf(tmp, 32, XFMT, addr);   
  if (hash_get(bps, tmp))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Breakpoint already exist", -1);

  /* Call the architecture dependent hook for backtrace */
  ret = (*hook_break[archtype][objtype][ostype])(file, bp);
  if (ret < 0)
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Breakpoint insertion failed", (-1));

  /* Add new breakpoint to hash table */
  bp->id = lastbpid++;
  hash_add(bps, elfsh_strdup(tmp), bp); 
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
	    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (curfile));
	}
    }

  /* Parent object not found */
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to get parent object addr", (NULL));
}




/* Find breakpoint by ID */
elfshbp_t	*vm_get_bp_by_id(uint32_t bpid)
{
  hashent_t     *actual;
  elfshbp_t	*cur;
  int           index;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  for (index = 0; index < e2dbgworld.bp.size; index++)
    for (actual = e2dbgworld.bp.ent + index;
	 actual != NULL && actual->key != NULL;
	 actual = actual->next)
      {
	cur = (elfshbp_t *) actual->data;
	if (cur->id == bpid)
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (cur));
      }

  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                    "Unable to find breakpoing by ID", NULL);
}




/* Print all display commands */
void		vm_print_display()
{
  char		buff[BUFSIZ];
  int		idx;
  int		printed;

  hashent_t     *actual;
  elfshbp_t     *curbp;
  int		idx2;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Global displays */
  printed = 0;
  for (idx = 0; idx < E2DBG_STEPCMD_MAX; idx++)
    {
      if (!e2dbgworld.displaycmd[idx])
	continue;
      if (!printed)
	{
	  vm_output("\n\t .:: e2dbg global displayed commands \n");
	  printed = 1;
	}
      snprintf(buff, BUFSIZ, "\t %u: %s \n", 
	       idx, e2dbgworld.displaycmd[idx]);
      vm_output(buff);
    }
  if (!printed)
    vm_output("\n\t .:: e2dbg currently has no global displays \n\n");
  
  /* Display breakpoint commands */
  printed = 0;
  for (idx = 0; idx < e2dbgworld.bp.size; idx++)
    for (actual = e2dbgworld.bp.ent + idx;
	 actual != NULL && actual->key != NULL;
	 actual = actual->next)
      {
	curbp = (elfshbp_t *) actual->data;
	for (idx2 = 0; idx2 < 10; idx2++)
	  if (curbp->cmd[idx2])
	    {
	      if (!printed)
		{
		  snprintf(buff, BUFSIZ, 
			   "\n\t .:: e2dbg display for breakpoint %u [" XFMT "] %s \n",
			   curbp->id, curbp->addr, curbp->symname);
		  vm_output(buff);
		  printed = 1;
		}
	      snprintf(buff, BUFSIZ, "\t %u: %s \n",
		       idx2, curbp->cmd[idx2]);
	      vm_output(buff);
	    }
	printed = 0;
      }

  vm_output("\n");
  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}



/* Execute all displayed commands for a breakpoint or a step */
int		vm_display(char **cmd, u_int nbr)
{
  int		idx;
  elfshargv_t	*cur;
  char		*str;
  char		buf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  for (idx = 0; idx < nbr; idx++)
    if (cmd[idx])
      {
	
	snprintf(buf, BUFSIZ, 
		 "\t .:: Display %u [%s] result ::. \n", 
		 idx, cmd[idx]);
	vm_output(buf);
	
	/* Register displayed command in the script control flow */
	str = elfsh_strdup(cmd[idx]);
	if (vm_exec_str(cmd[idx]) < 0)
	  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Display execrequest failed", -1);
	cmd[idx] = str;
	
	/* Execute displayed command */
	cur = world.curjob->curcmd;
	world.curjob->curcmd = world.curjob->script[world.curjob->sourced]; // instead of script 0
	if (vm_execmd() < 0)
	  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			    "Display execution failed", -1);
	world.curjob->curcmd = cur;
      }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
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
		       elfsh_strdup(buff));
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		    "Empty display", NULL);
}




/* Display on breakpoint or step */
int		cmd_display()
{
  elfshbp_t	*bp;
  char		buf[BUFSIZ];
  char		*str;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* No parameter, list displays */
  if (!world.curjob->curcmd->param[0])
    vm_print_display();
  
  /* Add a global display */
  else if (!vm_isnbr(world.curjob->curcmd->param[0]))
    {
      if (e2dbgworld.displaynbr >= E2DBG_STEPCMD_MAX)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Too many global displays", NULL);
      str = vm_get_string(world.curjob->curcmd->param);
      snprintf(buf, BUFSIZ, "\n [*] Adding global display %u [%s] \n\n", 
	       e2dbgworld.displaynbr, str);
      vm_output(buf);
      e2dbgworld.displaycmd[e2dbgworld.displaynbr++] = str;
    }
  
  /* Add a local display on breakpoint */
  else
    {
      bp = vm_lookup_bp(world.curjob->curcmd->param[0]);
      if (!bp)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Cannot find specified breakpoint", NULL);
      
      if (bp->cmdnbr >= 10)
        ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                          "Too many local displays", NULL);
      
      str = vm_get_string(&world.curjob->curcmd->param[1]);
      snprintf(buf, BUFSIZ, "\n [*] Adding display %u [%s] for breakpoint %u "
	       "[" XFMT "] %s \n\n", bp->cmdnbr, str, 
	       bp->id, bp->addr, bp->symname);
               
      vm_output(buf);
      bp->cmd[(int) bp->cmdnbr++] = str;
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Delete displays */
int		cmd_undisplay()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);



  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}






/* Delete a breakpoint */
int		cmd_delete()
{
  elfsh_Addr	addr;
  elfshbp_t	*bp;
  char		*name;
  int		prot;
  int		off;
  char		logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Lookup breakpoint */
  bp = vm_lookup_bp(world.curjob->curcmd->param[0]);
  if (!bp)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unknown breakpoint", -1);
  addr = bp->addr;
  
  /* Delete the breakpoint */
  prot = elfsh_munprotect(bp->obj, addr,  1);
  *(u_char *) addr = bp->savedinstr[0];
  elfsh_mprotect(addr, 1, prot);

  name = vm_resolve(bp->obj, addr, &off);
  snprintf(logbuf, BUFSIZ, XFMT, addr);
  hash_del(&e2dbgworld.bp, logbuf);

  if (off)
    snprintf(logbuf, BUFSIZ, " [*] Breakpoint at %08X <%s + %u> removed\n\n", 
	 addr, name, off);
  else
    snprintf(logbuf, BUFSIZ, " [*] Breakpoint at %08X <%s> removed\n\n", 
	 addr, name);
  vm_output(logbuf);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
