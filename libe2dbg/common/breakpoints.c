/*
** breakpoints.c for e2dbg
**    
** Started on  Tue Aug 16 09:38:03 2005 mayhem                                                                                                                   
**
** $Id: breakpoints.c,v 1.1 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libe2dbg.h"


/* Simple shared flags for watch/breakpoints */
static unsigned char	watchflag;



/**
 * Add a breakpoint 
 */
int		elfsh_bp_add(hash_t	*bps, 
			     elfshobj_t *file, 
			     char	*resolv, 
			     eresi_Addr addr, 
			     unsigned char	flags)
{
  static int	lastbpid = 1;
  elfshbp_t	*bp;
  char		tmp[32];
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || addr == 0 || bps == 0) 
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", -1);

  /* Breakpoints handlers must be initialized */
  elfsh_setup_hooks();
  XALLOC(__FILE__, __FUNCTION__, __LINE__,bp , sizeof(elfshbp_t), (-1));

  bp->obj     = file;
  bp->type    = INSTR;
  bp->addr    = addr;
  bp->symname = strdup(resolv);
  bp->flags   = flags;

  snprintf(tmp, 32, XFMT, addr);   
  if (hash_get(bps, tmp))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Breakpoint already exist", -1);

  /* Call the architecture dependent hook for breakpoints */
  ret = e2dbg_setbreak(file, bp);
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Breakpoint insertion failed", (-1));
  
  /* Add new breakpoint to hash table */
  bp->id = lastbpid++;
  hash_add(bps, strdup(tmp), bp); 
 
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Add a breakpoint without using a script command */
int		e2dbg_breakpoint_add(eresi_Addr addr, unsigned char flags)
{
  int		err;
  char		buf[BUFSIZ];
  char		*name;
  elfsh_SAddr	off;
  elfshobj_t	*file;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Resolve source file */
  file = e2dbg_get_parent_object(addr);
  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot resolve parent file for bp", -1);

  /* Resolve breakpoint address */
  name = revm_resolve(file, addr, &off);
  if (off)
    snprintf(buf, BUFSIZ, "<%s + " DFMT ">", name, off);
  else
    snprintf(buf, BUFSIZ, "<%s>", name);

  /* Really put the breakpoint */
  err = elfsh_bp_add(&e2dbgworld.bp, file, buf, addr, flags);
  if (err < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot add breakpoint", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Return 1 if the breakpoint is a watchpoint */
int		e2dbg_is_watchpoint(elfshbp_t *b)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     b && b->flags & ELFSH_BP_WATCH);
}





/* Find breakpoint by ID */
elfshbp_t	*e2dbg_breakpoint_from_id(uint32_t bpid)
{
  elfshbp_t	*cur;
  int           index;
  char		**keys;
  int		keynbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  keys = hash_get_keys(&e2dbgworld.bp, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      cur = hash_get(&e2dbgworld.bp, keys[index]);
      if (cur->id == bpid)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		      cur);
    }
  hash_free_keys(keys);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                    "Unable to find breakpoing by ID", NULL);
}



/* Find a breakpoint by various ways */
elfshbp_t	*e2dbg_breakpoint_lookup(char *name)
{
  eresi_Addr	addr;
  elfshbp_t	*bp;
  uint16_t	bpid;
  char		straddr[32];
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  bp = NULL;
  
  /* Lookup by vaddr */
  if (IS_VADDR(name))
    {
      
      if (sscanf(name + 2, AFMT, &addr) != 1)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Invalid virtual address requested", 
			  NULL);
    }

  /* Try to lookup by ID */
  else if (revm_isnbr(name))
    {
      bpid = atoi(name);
      bp   = e2dbg_breakpoint_from_id(bpid);
      if (!bp)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                          "Invalid breakpoint ID", NULL);
    }

  /* Resolve symbol */
  /* Here we fix symbols on the disk only ! This avoid a mprotect */
  else
    {      
      addr = e2dbg_breakpoint_find_addr(name);
      if (!addr)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Requested symbol address unknown",
			  NULL);
    }

  /* Get the breakpoint */
  if (!bp)
    {
      snprintf(straddr, sizeof(straddr), XFMT, addr);
      bp = hash_get(&e2dbgworld.bp, straddr);
      if (!bp)
	{
	  snprintf(logbuf, BUFSIZ, 
		   "\n [!] No breakpoint set at addr " AFMT " \n\n", addr);
	  e2dbg_output(logbuf);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			    "No breakpoint at this address", NULL);
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, bp);
}





/* Find the correct location for a breakpoint. Avoid putting breakpoints 
   on plt entries when possible */
eresi_Addr	e2dbg_breakpoint_find_addr(char *str)
{
  elfsh_Sym	*sym;
  elfsh_Sym	*bsym;
  elfshsect_t	*sect;
  elfshobj_t	*parent;
  char		**keys;
  int		keynbr;
  int		index;
  eresi_Addr	addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  sym = bsym = NULL;

  /* Sometimes we fix symbols on the disk : we avoid mprotect */
  /* Only return success early if not a plt symbol */
  keys = NULL;
  parent = world.curjob->curfile;
  sym = elfsh_get_metasym_by_name(parent, str);
  if (!sym || !sym->st_value)
    {
      elfsh_toggle_mode();
      sym = elfsh_get_metasym_by_name(parent, str);
      elfsh_toggle_mode();
    }
  
  if (sym && parent->hdr->e_type == ET_DYN)
    sym->st_value += parent->rhdr.base;

  if (sym && sym->st_value)
    {
      sect = elfsh_get_parent_section(parent, sym->st_value, NULL);
      if (!elfsh_is_plt(parent, sect))
	goto end;
    }

  /* Try to look in other objects */
  keys = hash_get_keys(&world.curjob->loaded, &keynbr);
  
  for (index = 0; index < keynbr; index++)
    {
      if (strstr(keys[index], E2DBG_ARGV0))
	continue;

      parent = hash_get(&world.curjob->loaded, keys[index]);
      bsym = elfsh_get_metasym_by_name(parent, str);
      if (!bsym || !bsym->st_value)
	{
	  elfsh_toggle_mode();
	  bsym = elfsh_get_metasym_by_name(parent, str);
	  elfsh_toggle_mode();
	  if (bsym && bsym->st_value)
	    {
	      sect = elfsh_get_parent_section(parent, bsym->st_value, NULL);
	      if (!elfsh_is_plt(parent, sect))
		{
		  sym = bsym;
		  if (strstr(parent->name, "libc.so"))
		    goto end;
		}
	      if (!sym)
		sym = bsym;
	    }
	}
      else
	{
	  sect = elfsh_get_parent_section(parent, bsym->st_value, NULL);
	  if (!elfsh_is_plt(parent, sect))
	    {
	      sym = bsym;
	      if (strstr(parent->name, "libc.so"))
		goto end;
	    }
	  if (!sym)
	    sym = bsym;
	}

#if __DEBUG_BP__
      printf("BPSym after %-30s = " XFMT "\n", parent->name, (eresi_Addr) sym);
#endif

    }

  /* Return error or success, dont forget to free the keys */
 end:
  if (keys)
    hash_free_keys(keys);

  if (!sym || !sym->st_value)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "No symbol by that name in the current file", 0);
  addr = sym->st_value;
  if (elfsh_get_objtype(parent->hdr) == ET_DYN)
    {
#if __DEBUG_BP__
      printf(" [*] Adding base addr " XFMT "\n", parent->rhdr.base);
#endif
      addr += parent->rhdr.base;
    }
  
#if __DEBUG_BP__
  printf(" [*] Will set breakpoint on " XFMT " (parent = %s) \n", 
	 addr, parent->name);
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, addr);
}




/* Breakpoint command */
int		cmd_bp()
{
  char		*str;
  int		ret;
  eresi_Addr	addr;
  char		logbuf[BUFSIZ];
  int		idx;
  int		index;
  elfsh_SAddr	off = 0;
  char		*name;
  elfshbp_t	*cur;
  char		**keys;
  int		keynbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* build argc */
  for (idx = 0; world.curjob->curcmd->param[idx] != NULL; idx++)
    ;
  str = revm_lookup_string(world.curjob->curcmd->param[0]);

  fprintf(stderr, "Putting breakpoint on %s \n", str);

  /* Select subcommand */
  switch (idx)
    {
      
      /* List breakpoints */
    case 0:
      e2dbg_output(" .:: Breakpoints & Watchpoints ::.\n\n");	      
      keys = hash_get_keys(&e2dbgworld.bp, &keynbr);
      for (index = 0; index < keynbr; index++)
	{
	  cur = hash_get(&e2dbgworld.bp, keys[index]);
	  name = revm_resolve(world.curjob->curfile, 
			    (eresi_Addr) cur->addr, &off);
	  if (off)
	    snprintf(logbuf, BUFSIZ, " %c [%02u] " XFMT " <%s + " UFMT ">\n", 
		     (e2dbg_is_watchpoint(cur) ? 'W' : 'B'),
		     cur->id, cur->addr, name, off);
	  else
	    snprintf(logbuf, BUFSIZ, " %c [%02u] " XFMT " <%s>\n", 
		     (e2dbg_is_watchpoint(cur) ? 'W' : 'B'),
		     cur->id, cur->addr, name);
	  e2dbg_output(logbuf);
	}
      hash_free_keys(keys);
      if (!index)
	e2dbg_output(" [*] No breakpoints\n");
      e2dbg_output("\n");
      break;
      
      /* Supply a new breakpoint */
    case 1:
      if (!elfsh_is_debug_mode())
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Not in dynamic or debugger mode", -1);
      if (!str || !(*str))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Invalid argument", -1);
      
      /* Break on a supplied virtual address */
      if (IS_VADDR(str))
	{
	  if (sscanf(str + 2, AFMT, &addr) != 1)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			 "Invalid virtual address requested", (-1));
	}
      
      /* Resolve first a function name */
      else
	{
	  addr = e2dbg_breakpoint_find_addr(str);
	  if (addr == 0)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			 "Requested symbol address unknown", -1);
	}
      
      /* Add the breakpoint */
      ret = e2dbg_breakpoint_add(addr, watchflag);
      if (ret < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Breakpoint insertion failed\n", (-1));
      if (ret >= 0)
	{
	  name = revm_resolve(world.curjob->curfile, addr, &off);
	  if (!off)
	    snprintf(logbuf, BUFSIZ - 1, 
		     " [*] %spoint added at <%s> (" XFMT ")\n\n", 
		     (watchflag ? "Watch" : "Break"), name, addr);
	  else
	    snprintf(logbuf, BUFSIZ - 1, 
		     " [*] %spoint added at <%s + " UFMT "> (" XFMT ")\n\n", 
		     (watchflag ? "Watch" : "Break"), name, off, addr);
	  e2dbg_output(logbuf);
	}
      break;

      /* Wrong command syntax */
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Wrong arg number", (-1));
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}



/* Watchpoint command -- FIXME: need to use debug registers ! */
int		cmd_watch()
{
  int		err;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  watchflag = 1;
  err = cmd_bp();
  watchflag = 0;
  if (err < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed to install watchpoint", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (err));
}
