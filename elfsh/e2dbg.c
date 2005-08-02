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

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  param = world.curjob->curcmd->param[0];
  if (!param)
    {
      if (elfsh_is_static_mode())
	param = "STATIC";
      else if (elfsh_is_debug_mode())
	param = "DYNAMIC";
      else
	ELFSH_SETERROR(" [*] Unknown e2dbg mode ! \n", -1);
      snprintf(buf, sizeof(buf), " [*] e2dbg is in %s MODE \n\n", param);
      vm_output(buf);
      return (0);
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
	      ELFSH_SETERROR("[elfsh:cmd_mode] Can't switch to dynamic"
			     " mode on not mapped file.\n", -1);
	}
      elfsh_set_debug_mode();
      vm_output(" [*] e2dbg is now in DYNAMIC mode\n\n"); 
    }
  else
    ELFSH_SETERROR("[elfsh:cmd_mode] Unknown mode for E2DBG\n", -1);

  return (0);
}


/* Dump the linkmap */
int		 vm_linkmap(elfshobj_t *file)
{
  char		 logbuf[BUFSIZ];
  elfshlinkmap_t *actual;
  int		 i = 1;

  if (!file)
    ELFSH_SETERROR("[elfsh:linkmap] Invalid argument\n", -1);

  if (!file->linkmap)
    ELFSH_SETERROR("[elfsh:linkmap] No linkmap\n", -1);

  vm_output(" .::. Linkmap entries .::. \n");

  for (actual = elfsh_linkmap_get_lprev(file->linkmap); actual != NULL; actual = elfsh_linkmap_get_lprev(actual), i++)
    {
      snprintf(logbuf, BUFSIZ - 1, " [%02u] addr : " XFMT " dyn : " XFMT " - %s\n", i, 
	       (elfsh_Addr) elfsh_linkmap_get_laddr(actual), 
	       (elfsh_Addr) elfsh_linkmap_get_lld(actual),
	       elfsh_linkmap_get_lname(actual));
      vm_output(logbuf);
    }

  for (actual = file->linkmap; actual != NULL; actual = elfsh_linkmap_get_lnext(actual), i++)
    {
      snprintf(logbuf, BUFSIZ - 1, " [%02u] addr : "XFMT" dyn : "XFMT" - %s\n", i, 
	       (elfsh_Addr) elfsh_linkmap_get_laddr(actual), 
	       (elfsh_Addr) elfsh_linkmap_get_lld(actual),
	       elfsh_linkmap_get_lname(actual));
      vm_output(logbuf);
    }

  vm_output("\n");
  return (0);
}

/* Linkmap dump command */
int		cmd_linkmap()
{
  int		old;
  int		ret;

  if (elfsh_is_debug_mode())
    {
      if (!world.curjob)
	ELFSH_SETERROR("[elfsh:cmd_linkmap] No current workspace \n", -1);
      
      if (!world.curjob->current)
	ELFSH_SETERROR("[elfsh:cmd_linkmap] No current file \n", -1);
      
      old = world.curjob->current->id;

      /* switch to the main program file */
      vm_doswitch(1);
      ret = vm_linkmap(world.curjob->current);
      vm_doswitch(old);
      return ret;
    }
  else
    {
      vm_output("You must be in DYNAMIC mode\n\n");
      return (-1);
    }
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

  if (!world.curjob)
    ELFSH_SETERROR("[elfsh:cmd_bt] No current workspace \n", -1);
  
  if (!world.curjob->current)
    ELFSH_SETERROR("[elfsh:cmd_bt] No current file \n", -1);

  /* get the current frame */
  frame = elfsh_bt_get_frame();
  
  /* backtrace frames */
  do {

    /* XXX probably arch dependant */
    /* assume that the ret address is one word behind frame pointer */
    ret = (void *) (*((long *) frame + 1));
    
    name = vm_resolve(world.curjob->current, (elfsh_Addr) ret, &off);
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

    /* this seems to be the signal vsyscall return address */
    if (ret == (void *) 0xFFFFE420)
      vm_output(" [!!] Warning : One function is missing here \n");

    frame = elfsh_bt(frame);    
    i++;
  } while(frame); 

  vm_output("\n");
  return (0);
}


/* Backtrace command */
int		cmd_bt()
{
  int		ret;
  
  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!elfsh_is_debug_mode())
    ELFSH_SETERROR("[elfsh:cmd_bt] Not in dynamic or debugger mode \n", -1);
  vm_output(" .:: Backtrace ::. \n");
  ret = vm_bt();
  return (ret);
}


/* Symbol matching on the stack content. Always useful */
int		vm_dumpstack(u_int32_t size)
{
  long		c;
  long		*i;
  char		logbuf[BUFSIZ];
  char		*name;
  int		off;

  vm_output(" .:: Stack ::.\n");
  for (i = &c; i < &c + size; i+=1)
    {
      name = vm_resolve(world.curjob->current, *i, &off);
      if (off)
	snprintf(logbuf, BUFSIZ - 1, " " XFMT " " XFMT " <%s + %d> \n", 
		 (elfsh_Addr) i, (elfsh_Addr) *i, name, off);
      else
	snprintf(logbuf, BUFSIZ - 1, " " XFMT " " XFMT " <%s> \n", 
		 (elfsh_Addr) i, (elfsh_Addr) *i, name);      
      vm_output(logbuf);
    }
  vm_output("\n");
  return (0);
}



/* Stack display with symbols */
int		cmd_stack()
{
  char		*param;
  elfsh_Addr	  size;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* XXX : error handling */
  if (!elfsh_is_debug_mode())
    return (-1);
  
  param = world.curjob->curcmd->param[0];
  if (!param)
    ELFSH_SETERROR("[elfsh:cmd_stack] Invalid argument\n", (-1));
  if (vm_isnbr(param))
    {
      if (sscanf(param, UFMT, &size) != 1)
	ELFSH_SETERROR("[elfsh:cmd_stack] Invalid argument\n", (-1));
      return (vm_dumpstack(size));
    }  
  return (0);
}









/* Add a breakpoint without using a script command */
int		vm_bp_add(elfsh_Addr addr)
{
  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  return (elfsh_bp_add(&e2dbgworld.bp, world.curjob->current, addr));
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

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* build argc */
  for (idx = 0; world.curjob->curcmd->param[idx] != NULL; idx++);

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
	      name = vm_resolve(world.curjob->current, 
				(elfsh_Addr) ((elfshbp_t *) 
					      actual->data)->addr, &off);

	      if (off)
		snprintf(logbuf, BUFSIZ, " [%02u] " XFMT " <%s + %d>\n", 
			 idx2++,
			 ((elfshbp_t *) actual->data)->addr, 
			 name, 
			 off);
	      else
		snprintf(logbuf, BUFSIZ, " [%02u] " XFMT " <%s>\n", 
			 idx2++, 
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
      str = world.curjob->curcmd->param[0];

      if (!elfsh_is_debug_mode())
	ELFSH_SETERROR("[elfsh:cmd_bp_add] Not in dynamic or debugger mode \n", -1);
  
      if (!str || !(*str))
	ELFSH_SETERROR("[elfsh:cmd_bp_add] Invalid argument\n", -1);

      /* Break on a supplied virtual address */
      if (IS_VADDR(str))
	{
	  if (sscanf(str + 2, "%X", &addr) != 1)
	    ELFSH_SETERROR("[elfsh:cmd_bp_add] Invalid virtual address requested\n", (-1));
	}

      /* Resolve first a function name */
      else
	{
	  sym = elfsh_get_metasym_by_name(world.curjob->current, str);
	  if (!sym)
	    ELFSH_SETERROR("[elfsh:cmd_bp_add] No symbol by that name in the current file\n", (-1));
	  
	  addr = sym->st_value;
	}
      
      /* XXX: Need to find the parent object where the breakpoint is set into */
      ret = vm_bp_add(addr);
      
      if (ret >= 0)
	{
	  name = vm_resolve(world.curjob->current, addr, &off);
	  if (!off)
	      snprintf(logbuf, BUFSIZ - 1, " [*] Breakpoint added at <%s> (" XFMT ")\n\n", 
		       name, addr);
	    else
	      snprintf(logbuf, BUFSIZ - 1, " [*] Breakpoint added at <%s + %u> (" XFMT ")\n\n", 
		       name, off, addr);
	  vm_output(logbuf);
	}
    } 
  else 
    {
      vm_output("Wring arg number\n\n");
      ELFSH_SETERROR("[elfsh:cmd_bp_add] wrong arg number \n", (-1));
    }
  
  return (ret);
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

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || addr == 0 || bps == 0) 
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  elfsh_setup_hooks();

  archtype = elfsh_get_archtype(file);
  objtype = elfsh_get_elftype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      objtype  == ELFSH_TYPE_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid target", -1);

  XALLOC(bp , sizeof(elfshbp_t), (-1));
  bp->type = INSTR;
  bp->addr = addr;
  bp->obj  = vm_get_parent_object(addr);
  if (!bp->obj)
    printf("Warning : Unknown parent object for breakpoint\n");

  snprintf(tmp, 32, XFMT, addr);   
  if (hash_get(bps, tmp))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Breakpoint already exist", -1);

  /* Call the architecture dependent hook for backtrace */
  ret = (*hook_break[archtype][objtype][ostype])(file, bp);
  if (ret < 0)
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Breakpoint insertion failed", (-1));

  /* XXX: Should not be put in here ! */
  hash_add(bps, strdup(tmp), bp); 
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Get the parent object of a breakpoint */
/* Thats needed for the mprotect stuff inside the breakpoint handler */
elfshobj_t	*vm_get_parent_object(elfsh_Addr addr)
{
  elfsh_Phdr	*cur;
  elfshobj_t	*curfile;
  elfshsect_t	*cursect;

  for (curfile = world.curjob->list; curfile != NULL; curfile = curfile->next)
    {
      cursect = elfsh_get_parent_section(curfile, addr, NULL);
      if (cursect)
	{
	  cur = elfsh_get_parent_segment(curfile, cursect);
	  if (cur)
	    return (curfile);
	}
    }

  /* Parent object not found */
  return (NULL);
}



/* Delete a breakpoint */
int		cmd_delete()
{
  elfsh_Addr	addr;
  elfshbp_t	*bp;
  int		prot;
  char		*name;
  int		off;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (sscanf(world.curjob->curcmd->param[0], "%X", &addr) != 1)
    ELFSH_SETERROR("[elfsh:cmd_delete] Invalid virtual address \n", 
		   (-1));
  
  bp = hash_get(&e2dbgworld.bp, world.curjob->curcmd->param[0]);
  if (!bp)
    {
      printf("Warning: No breakpoint set at addr %08X \n",
	     addr);
      return (-1);
    }

  /* Delete the breakpoint */
  prot = elfsh_munprotect(bp->obj, addr,  1);
  *(u_char *) addr = bp->savedinstr[0];
  elfsh_mprotect(addr, 1, prot);

  name = vm_resolve(bp->obj, addr, &off);
  hash_del(&e2dbgworld.bp, world.curjob->curcmd->param[0]);

  if (off)
    printf(" [*] Breakpoint at %08X <%s + %u> removed\n\n", 
	 addr, name, off);
  else
    printf(" [*] Breakpoint at %08X <%s> removed\n\n", 
	 addr, name);

  return (0);
}
