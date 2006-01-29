/*
 * Allocator wrapper for the Embedded ELF Debugger
 *
 * Select the allocator depending on the thread ID of the caller
 *
 * Started Fri Oct  7 21:05:26 CEST 2005 crew
 */
#include "elfsh.h"


/* Debugger Thread specific information */
void		vm_dbgid_set(u_int pid) { e2dbgworld.dbgpid = pid; }
u_int		vm_dbgid_get()          { return (e2dbgworld.dbgpid); }	


/* Wrapper for malloc */
void		*malloc(size_t t)
{
  void		*(*mallocptr)();
  void		*chunk;

#if __DEBUG_EMALLOC__
  write(1, "Calling HOOKED malloc\n", 22);
#endif
  
  //e2dbg_self();

  if (!e2dbgworld.mallocsym)
    e2dbg_dlsym_init();

  if (!vm_dbgid_get() || vm_dbgid_get() != pthread_self())
    {
#if __DEBUG_EMALLOC__
      write(1, "\033[1;41m", 7);
      write(1, "LIBC malloc used\n", 17);
      write(1, "\033[00m", 5);
#endif
      mallocptr = (void *) e2dbgworld.mallocsym;
      if (!e2dbgworld.mallocsym)
	{
	  write(1, " [!] Unable to use original malloc \n", 36);
	  return (NULL);
	}
      chunk = mallocptr(t);
    }
  else
    {
#if __DEBUG_EMALLOC__
      write(1, "E2DBG malloc used\n", 18);
#endif
      chunk = elfsh_malloc(t);
    }
  
  if (!chunk)
    write(1, " [!] Malloc failed \n", 20);

#if __DEBUG_EMALLOC__
  write(1, "Finished HOOKED malloc\n", 23);
#endif

  return (chunk);
}




/* Wrapper for valloc */
void		*valloc(size_t t)
{
  void		*(*vallocptr)();
  void		*chunk;

#if __DEBUG_EMALLOC__
  write(1, "Calling HOOKED valloc\n", 22);
#endif

  if (!e2dbgworld.vallocsym)
    e2dbg_dlsym_init();

  if (!vm_dbgid_get() || vm_dbgid_get() != pthread_self())
    {
#if __DEBUG_EMALLOC__
      write(1, "\033[1;41m", 7);
      write(1, "LIBC valloc used\n", 17);
      write(1, "\033[00m", 5);
#endif
      vallocptr = (void *) e2dbgworld.vallocsym;
      if (!e2dbgworld.vallocsym)
	{
	  write(1, " [!] Unable to use original valloc \n", 36);
	  return (NULL);
	}
      chunk = vallocptr(t);
    }
  else
    {
#if __DEBUG_EMALLOC__
      write(1, "E2DBG valloc used\n", 18);
#endif
      chunk = (void *) elfsh_valloc(t);
    }
  
  if (!chunk)
    write(1, " [!] Valloc failed \n", 20);

#if __DEBUG_EMALLOC__
  write(1, "Finished HOOKED valloc\n", 23);
#endif

  return (chunk);
}





/* Wrapper for calloc */
void		*calloc(size_t t, u_int nbr)
{
  void		*(*callocptr)();
  void		*chunk;
#if __DEBUG_EMALLOC__
  u_int		len;

  write(1, "Calling HOOKED calloc \n", 23);
#endif

  /*
  if (t == 1)
    {
      t = nbr;
      nbr = 1;
    }
  */

  //e2dbg_self();

  if (!e2dbgworld.callocsym)
    e2dbg_dlsym_init();

  if (!vm_dbgid_get() || vm_dbgid_get() != pthread_self())
    {
#if __DEBUG_EMALLOC__
      write(1, "\033[1;41m", 7);
      write(1, "LIBC calloc used\n", 17);
      write(1, "\033[00m", 5);
#endif
      callocptr = (void *) e2dbgworld.callocsym;
      if (!e2dbgworld.callocsym)
	{
	  write(1, " [!] Unable to use original calloc \n", 36);
	  return (NULL);
	}
      write(1, "Calling libc calloc\n", 20);
      chunk = callocptr(t, nbr);
      write(1, "Libc calloc returned\n", 21);
    }
  else
    {
#if __DEBUG_EMALLOC__
      write(1, "E2DBG calloc used\n", 18);
#endif
      chunk = elfsh_calloc(t, nbr);
    }

#if __DEBUG_EMALLOC__
  /*  if (!chunk)
    {
      char buff[256];
      len = snprintf(buff, sizeof(buff), " ! Calloc failed (%u * %u sz) \n", 
		     t, nbr);
      write(1, buff, len);
      }
  */
  write(1, "Finished HOOKED calloc \n", 24);
#endif


  return (chunk);
}



/* Wrapper for memalign */
void		*memalign(size_t t, u_int nbr)
{
  void		*(*memalignptr)();
  void		*chunk;
#if __DEBUG_EMALLOC__
  u_int		len;

  write(1, "Calling HOOKED memalign \n", 23);
#endif

  if (!e2dbgworld.memalignsym)
    e2dbg_dlsym_init();

  if (!vm_dbgid_get() || vm_dbgid_get() != pthread_self())
    {
#if __DEBUG_EMALLOC__
      write(1, "\033[1;41m", 7);
      write(1, "LIBC memalign used\n", 17);
      write(1, "\033[00m", 5);
#endif
      memalignptr = (void *) e2dbgworld.memalignsym;
      if (!e2dbgworld.memalignsym)
	{
	  write(1, " [!] Unable to use original memalign \n", 36);
	  return (NULL);
	}
      chunk = (void *) memalignptr(t, nbr);
    }
  else
    {
#if __DEBUG_EMALLOC__
      write(1, "E2DBG memalign used\n", 18);
#endif
      chunk = (void *) elfsh_memalign(t, nbr);
    }

#if __DEBUG_EMALLOC__
  if (!chunk)
    {
      char buff[256];
      len = snprintf(buff, sizeof(buff), " ! Memalign failed (%u * %u sz) \n", 
		     t, nbr);
      write(1, buff, len);
    }
  write(1, "Finished HOOKED memalign \n", 24);
#endif

  return (chunk);
}



/* Wrapper for realloc */
void	*realloc(void *a, size_t t)
{
  void	*b;
  void	*(*reallocptr)();

  //e2dbg_self();

#if __DEBUG_EMALLOC__
  write(1, "Calling HOOKED realloc\n", 23);
#endif

  if (!e2dbgworld.reallocsym)
    e2dbg_dlsym_init();

  if (!vm_dbgid_get() || vm_dbgid_get() != pthread_self())
    {
#if __DEBUG_EMALLOC__
      write(1, "\033[1;41m", 7);
      write(1, "LIBC realloc used\n", 18);
      write(1, "\033[00m", 5);
#endif
      reallocptr = (void *) e2dbgworld.reallocsym;
      if (!e2dbgworld.reallocsym)
	{
	  write(1, " [!] Unable to use original realloc \n", 37);
	  return (NULL);
	}
      b = reallocptr(a, t);
    }
  else
    {
#if __DEBUG_EMALLOC__
      write(1, "E2DBG realloc used\n", 19);
#endif
      b = elfsh_realloc(a, t);
    }
  
  if (!b)
    write(1, " [!] Realloc failed \n", 20);
  
#if __DEBUG_EMALLOC__
  write(1, "Finished HOOKED realloc\n", 24);
#endif

  return (b);
}

/* Wrapper for free */
void	free(void *a)
{
  void	*(*freeptr)();

  //e2dbg_self();

#if __DEBUG_EMALLOC__
  write(1, "Calling HOOKED free\n", 20);
#endif

  if (!e2dbgworld.freesym)
    e2dbg_dlsym_init();

  if (!vm_dbgid_get() || vm_dbgid_get() != pthread_self())
    {
#if __DEBUG_EMALLOC__
      write(1, "\033[1;41m", 7);
      write(1, "LIBC free used\n", 15);
      write(1, "\033[00m", 5);
#endif
      freeptr = (void *) e2dbgworld.freesym;
      if (!e2dbgworld.freesym)
	{
	  write(1, " [!] Unable to use original free \n", 34);
	  return;
	}
      freeptr(a);
    }
  else
    {
#if __DEBUG_EMALLOC__
      write(1, "E2DBG free used\n", 16);
#endif
      XFREE(a);
    }

#if __DEBUG_EMALLOC__
  write(1, "Finished HOOKED free\n", 21);
#endif
}


void	wait4exit(void *a)
{
  //printf("\n [*] Debuggee wait4exit called \n");
  while (!e2dbgworld.exited)
    sleep(1);
  exit(0);
}


/* Wrapper for _exit */
void		_exit(int err)
{
  while (1)
    if (e2dbgworld.exited)
      {
	if (!e2dbgworld.debuggee_exited)
	  write(1, " [*] Legit program terminating\n\n", 32);
	e2dbgworld.debuggee_exited = 1;
	syscall(1, 0);
	raise(SIGKILL);
      }
    else
      {
	e2dbgworld.exited = 1;
	write(1, " [*] Debugger exited\n", 21);
	syscall(1, 0);
	raise(SIGKILL); 
      }
}




/* Not sure it is useful / bugless, just a try */
/*
void*		_int_malloc(size_t p)             { return (malloc(p));      }
void*		_int_valloc(size_t p)             { return (valloc(p));      }
void*		_int_calloc(size_t p, size_t n)   { return (calloc(p, n));   }
void*		_int_realloc(char *p, size_t n)   { return (realloc(p, n));  }
void*		_int_memalign(size_t p, size_t n) { return (memalign(p, n)); }
void		_int_free(void *p)                { free(p);                 }  
*/
