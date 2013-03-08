/**
* @file libe2dbg/user/linkmap.c
** @ingroup user
*/
/**
* @file libe2dbg/user/linkmap.c
 *    
 * Started on  Tue Aug 16 09:38:03 2005 jfv
 *
 * $Id$
 *
 */
#include "libe2dbg.h"


/** 
 * Load linkmap 
 * @param name
 * @return
 */
int			e2dbg_linkmap_load(char *name)
{
  static int		done	= 0;
  elfshsect_t		*got;
  eresi_Addr		*linkmap_entry;
  void			*data;
#if defined(sun)
  Link_map		*actual;
#else
  elfshlinkmap_t	*actual;
#endif
  char			*gotname;
  char			*ename;
  elfsh_Ehdr		*hdr;
  u_int			elftypenum;
  elfsh_Sym		*endsym;
  char			buff[64];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (done)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);    

#if __DEBUG_LINKMAP__
  fprintf(stderr, "[e2dbg] Starting Loading LINKMAP !! \n");
#endif

  e2dbg_user_hooks_install();
  revm_config(E2DBG_CONFIG);

  /* Load debugged file */
  if (name)
    {

      /* No need to fill ET_EXEC base addr */
      if (!revm_is_loaded(name) && revm_file_load(name, 0, NULL) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Cannot load file", -1);
      
#if __DEBUG_LINKMAP__
      fprintf(stderr, "[e2dbg_linkmap_load] file %s loaded\n", name);
#endif
      
      world.curjob->curfile->linkmap = E2DBG_DYNAMIC_LINKMAP;
      world.curjob->curfile->iotype  = ELFSH_IOTYPE_EMBEDDED;
      world.curjob->curfile->running = 0;
    }
  
#if __DEBUG_LINKMAP__
  fprintf(stderr, "[e2dbg_linkmap_load] Before switch\n");
#endif
  
  /* Switch to obj 1 */
  if (revm_doswitch(1) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot switch on object 1", -1);    
  
#if __DEBUG_LINKMAP__
  fprintf(stderr, "[e2dbg_linkmap_load] After switch \n");
#endif
  
  /* Base address for PIE binaries have to be imported */
  if (world.curjob->curfile->hdr->e_type == ET_DYN &&
      !world.curjob->curfile->rhdr.base)
    {
#if __DEBUG_LINKMAP__
      fprintf(stderr, "[e2dbg_linkmap_load] Inside ET_DYN condition\n");
#endif

      endsym = elfsh_get_symbol_by_name(world.curjob->curfile, "_end");

      fprintf(stderr, "endsym = " AFMT " \n", (eresi_Addr) endsym);
      sleep(1);

#if __DEBUG_LINKMAP__
      fprintf(stderr, "[e2dbg_linkmap_load] Filling PIE base"
	      " (_end ondisk = " AFMT " / _end in memory = " AFMT ") ! \n",
	      endsym->st_value, e2dbgworld.syms.piebase);
#endif 

      world.curjob->curfile->rhdr.base = e2dbgworld.syms.piebase -
	endsym->st_value;
    }

  /* Get ALTGOT or GOT if we used LD_PRELOAD */
  if (!e2dbgworld.preloaded)
    {
      gotname = ELFSH_SECTION_NAME_ALTGOT;
      got = elfsh_get_section_by_name(world.curjob->curfile, 
				      gotname, NULL, NULL, NULL);
    }
  else
    got = elfsh_get_gotsct(world.curjob->curfile);

#if __DEBUG_LINKMAP__
  fprintf(stderr, "[e2dbg_linkmap_load] %s section at " XFMT "\n",
	  got->name, got->shdr->sh_addr);
  fprintf(stderr, "[e2dbg_linkmap_load] BASE = %08x\n", 
	  world.curjob->curfile->rhdr.base);
#endif
  
  
  /* Fix first file linkmap entry */
  if (world.curjob->curfile->linkmap == E2DBG_DYNAMIC_LINKMAP)
    {
      /* Fix first file linkmap entry */
      hdr = elfsh_get_hdr(world.curjob->curfile);
      if (!hdr)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Cannot get ELF header", -1);    
      elftypenum = elfsh_get_objtype(hdr);
      
      //fprintf(stderr, "[e2dbg_linkmap_load] after ELF header \n");

      /* Get ALTGOT entry */
      data          = elfsh_readmem(got);

      //fprintf(stderr, "[e2dbg_linkmap_load] after get_raw (data = %08X) \n", data);

      linkmap_entry = elfsh_get_got_entry_by_index(data, 1);
      
      //fprintf(stderr, "[e2dbg_linkmap_load] after entry_by_index (linkmap_entry = %08x)\n",
      //      linkmap_entry);

#if defined(__FreeBSD__) || defined(__NetBSD__)
      world.curjob->curfile->linkmap = (elfshlinkmap_t *)
	&((Obj_Entry *) elfsh_get_got_val(linkmap_entry))->linkmap;
#elif defined(sun)
      world.curjob->curfile->linkmap = e2dbgworld.syms.map;
#else
      world.curjob->curfile->linkmap = (elfshlinkmap_t *) elfsh_get_got_val(linkmap_entry);
#endif
      
    }
  
#if __DEBUG_LINKMAP__
  else
    fprintf(stderr, "[e2dbg_linkmap_load] Linkmap was -NOT- dynamic\n");

  fprintf(stderr, "[e2dbg_linkmap_load] LINKMAP Found at " XFMT "\n", 
	 world.curjob->curfile->linkmap);
#endif

  revm_doswitch(1);
  
  /* now load all linkmap's files */
  for (actual = elfsh_linkmap_get_lprev(world.curjob->curfile->linkmap);
       actual != NULL; 
       actual = elfsh_linkmap_get_lprev(actual))
    {
      
#if __DEBUG_LINKMAP__
      fprintf(stderr, "[e2dbg_linkmap_load] Running on LINKMAP PREV " XFMT "\n", 
	     actual);
#endif
      
      ename = elfsh_linkmap_get_lname(actual);
      if (ename && *ename && !revm_is_loaded(ename))
	{
	  if (revm_file_load(ename,
			     elfsh_linkmap_get_laddr(actual), 
			     world.curjob->curfile->linkmap) < 0)
	    e2dbg_output(" [EE] Loading failed");
	  world.curjob->curfile->iotype  = ELFSH_IOTYPE_EMBEDDED;
	}      
    }

#if __DEBUG_LINKMAP__
  fprintf(stderr, "[e2dbg_linkmap_load] Running on LINKMAP NEXT\n");
#endif
  
  for (actual = elfsh_linkmap_get_lnext(world.curjob->curfile->linkmap);
       actual != NULL; 
       actual = elfsh_linkmap_get_lnext(actual))
    {

      ename = elfsh_linkmap_get_lname(actual);
     
#if __DEBUG_LINKMAP__
      fprintf(stderr, "[e2dbg_linkmap_load] Running on LINKMAP NEXT " XFMT " (%s baseaddr %08X) \n", 
	      actual, ename, actual->laddr);
#endif

      if (ename && *ename && !revm_is_loaded(ename))
	{
	  if (revm_file_load(ename, elfsh_linkmap_get_laddr(actual), 
			     world.curjob->curfile->linkmap) < 0)
	    e2dbg_output(" [EE] Loading failed");
	  world.curjob->curfile->iotype  = ELFSH_IOTYPE_EMBEDDED;
	}      
    }

  /* Everything was OK */
  e2dbg_output("\n");
  //elfsh_set_runtime_mode();
  revm_doswitch(1);

  snprintf(buff, sizeof(buff), " [*] Target PID = %u \n", getpid());
  e2dbg_output(buff);

  done = 1;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




