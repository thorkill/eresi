/**
 * @file alloc-proxy.c
 * Allocator wrapper for the Embedded ELF Debugger
 * 
 * Select the allocator depending on the thread ID of the caller
 * 
 * Started Fri Oct  7 21:05:26 CEST 2005 crew
 *
 * $Id: alloc-proxy.c,v 1.1 2008-02-16 12:32:27 thor Exp $
 *
 */
#include "libe2dbg.h"


/**
 * Wrapper for malloc 
 * @param t
 * @return
 */
void		*malloc(size_t t)
{
  void		*(*mallocptr)();
  void		*chunk;
#if __DEBUG_EMALLOC__
  char		buf[256];
  unsigned int		len;
#endif

#if __DEBUG_EMALLOC__
  write(2, "Calling HOOKED malloc\n", 22);
#endif

  if (!e2dbgworld.syms.mallocsym)
    e2dbg_dlsym_init();

  if (e2dbg_presence_get() == 0)
    {
#if __DEBUG_EMALLOC__
      write(2, "\033[1;41m", 7);
      write(2, "LIBC malloc used\n", 17);
      write(2, "\033[00m", 5);
#endif
      mallocptr = (void *) e2dbgworld.syms.mallocsym;
      if (!e2dbgworld.syms.mallocsym)
	{
	  write(2, " [!] Unable to use original malloc \n", 36);
	  return (NULL);
	}
      chunk = mallocptr(t);
    }
  else
    {
      chunk = aproxy_malloc(t);
#if __DEBUG_EMALLOC__
      len = snprintf(buf, sizeof(buf), 
		     "E2DBG malloc used [ret = %08X, len = %u] \n", 
		     (eresi_Addr) chunk, t);
      write(2, buf, len);
#endif
    }
  
  if (!chunk)
    write(2, " [!] Malloc failed \n", 20);

#if __DEBUG_EMALLOC__
  write(2, "Finished HOOKED malloc\n", 23);
#endif

  return (chunk);
}




/**
 * Wrapper for valloc 
 * @param t
 * @return
 */
void		*valloc(size_t t)
{
  void		*(*vallocptr)();
  void		*chunk;

#if __DEBUG_EMALLOC__
  write(2, "Calling HOOKED valloc\n", 22);
#endif

  if (!e2dbgworld.syms.vallocsym)
    e2dbg_dlsym_init();

  if (e2dbg_presence_get() == 0)
    {
#if __DEBUG_EMALLOC__
      write(2, "\033[1;41m", 7);
      write(2, "LIBC valloc used\n", 17);
      write(2, "\033[00m", 5);
#endif
      vallocptr = (void *) e2dbgworld.syms.vallocsym;
      if (!e2dbgworld.syms.vallocsym)
	{
	  write(2, " [!] Unable to use original valloc \n", 36);
	  return (NULL);
	}
      chunk = vallocptr(t);
    }
  else
    {
#if __DEBUG_EMALLOC__
      write(2, "E2DBG valloc used\n", 18);
#endif
      chunk = (void *) aproxy_valloc(t);
    }
  
  if (!chunk)
    write(2, " [!] Valloc failed \n", 20);

#if __DEBUG_EMALLOC__
  write(2, "Finished HOOKED valloc\n", 23);
#endif

  return (chunk);
}





/**
 * Wrapper for calloc 
 * @param t
 * @param nbr
 * @return
 */
void		*calloc(size_t t, size_t nbr)
{
  void		*(*callocptr)();
  void		*chunk;

  //static int	cnt = 0;

#if __DEBUG_EMALLOC__
  unsigned int		len;

  write(2, "Calling HOOKED calloc \n", 23);
#endif

  /*
  if (t == 1)
    {
      t = nbr;
      nbr = 1;
    }
  */

  //e2dbg_self();

  if (!e2dbgworld.syms.callocsym)
    e2dbg_dlsym_init();

  if (e2dbg_presence_get() == 0)
    {
#if __DEBUG_EMALLOC__
      write(2, "\033[1;32m", 7);
      write(2, "LIBC calloc used\n", 17);
      write(2, "\033[00m", 5);
#endif

      /*
      if (cnt < 2)
	{
	  callocptr = (void *) (*(long *) e2dbgworld.mallochooksym);
	  cnt++;
	}
      else
      */
      callocptr = (void *) e2dbgworld.syms.mallocsym;

      if (!callocptr)
	{
	  write(2, " [!] Unable to use original calloc \n", 36);
	  return (NULL);
	}

      //write(2, "Calling libc calloc \n", 20);

#if __DEBUG_EMALLOC__
      {
	char buff[256];
	len = snprintf(buff, sizeof(buff), 
		       "Calling LIBC calloc at addr %08X\n", 
		       (eresi_Addr) callocptr);
	write(2, buff, len);
      } 
#endif

      chunk = callocptr(t * nbr);
#if __DEBUG_EMALLOC__
      write(2, "Libc m/calloc returned\n", 23);
#endif
      if (chunk)
	memset(chunk, 0x00, t * nbr);
    }
  else
    {
#if __DEBUG_EMALLOC__
      write(2, "\033[1;31m", 7);
      write(2, "E2DBG calloc used\n", 18);
      write(2, "\033[00m", 5);
#endif
      //chunk = aproxy_calloc(t, nbr);
      chunk = aproxy_malloc(t * nbr);
      if (chunk)
	memset(chunk, 0x00, t * nbr);
    }

#if __DEBUG_EMALLOC__
  if (!chunk)
    {
      char buff[256];
      len = snprintf(buff, sizeof(buff), 
		     " ! Calloc failed (%u * %u sz) \n", 
		     t, nbr);
      write(2, buff, len);
    }
  else
    {
      char buff[256];
      len = snprintf(buff, sizeof(buff), 
		     " Calloc (%u * %u sz) returned %08X\n", 
		     t, nbr, (eresi_Addr) chunk);
      write(2, buff, len);
    }
  write(2, "Finished HOOKED calloc \n", 24);
#endif

  return (chunk);
}



/**
 * Wrapper for memalign 
 * @param t
 * @param nbr
 */
void		*memalign(size_t t, unsigned int nbr)
{
  void		*(*memalignptr)();
  void		*chunk;

  //static int	cnt = 0;

#if __DEBUG_EMALLOC__
  unsigned int		len;

  write(2, "Calling HOOKED memalign \n", 23);
#endif

  if (!e2dbgworld.syms.memalignsym)
    e2dbg_dlsym_init();

  if (e2dbg_presence_get() == 0)
    {
#if __DEBUG_EMALLOC__
      write(2, "\033[1;32m", 7);
      write(2, "LIBC memalign used\n", 17);
      write(2, "\033[00m", 5);
#endif

      /*
      if (!cnt)
	{
	  memalignptr = (void *) (*(long *) e2dbgworld.memalignhooksym);
	  cnt++;
	}
      else
      */
      
      memalignptr = (void *) e2dbgworld.syms.memalignsym;
      
      if (!memalignptr)
	{
	  write(2, " [!] Unable to use original memalign \n", 36);
	  return (NULL);
	}
      chunk = (void *) memalignptr(t, nbr);
    }
  else
    {
#if __DEBUG_EMALLOC__
      write(2, "\033[1;31m", 7);
      write(2, "E2DBG memalign used\n", 18);
      write(2, "\033[00m", 5);
#endif
      chunk = (void *) aproxy_memalign(t, nbr);
    }

#if __DEBUG_EMALLOC__
  if (!chunk)
    {
      char buff[256];
      len = snprintf(buff, sizeof(buff), 
		     " ! Memalign failed (%u * %u sz) \n", 
		     t, nbr);
      write(2, buff, len);
    }
  write(2, "Finished HOOKED memalign \n", 24);
#endif

  return (chunk);
}



/**
 * Wrapper for realloc 
 * @param a
 * @param t
 * @return
 */
void	*realloc(void *a, size_t t)
{
  void	*b;
  void	*(*reallocptr)();

  //e2dbg_self();

#if __DEBUG_EMALLOC__
  write(2, "Calling HOOKED realloc\n", 23);
#endif

  if (!e2dbgworld.syms.reallocsym)
    e2dbg_dlsym_init();

  if (e2dbg_presence_get() == 0)
    {
#if __DEBUG_EMALLOC__
      write(2, "\033[1;41m", 7);
      write(2, "LIBC realloc used\n", 18);
      write(2, "\033[00m", 5);
#endif
      reallocptr = (void *) e2dbgworld.syms.reallocsym;
      if (!e2dbgworld.syms.reallocsym)
	{
	  write(2, " [!] Unable to use original realloc \n", 37);
	  return (NULL);
	}
      b = reallocptr(a, t);
    }
  else
    {
#if __DEBUG_EMALLOC__
      write(2, "E2DBG realloc used\n", 19);
#endif
      b = aproxy_realloc(a, t);
    }
  
  if (!b)
    write(2, " [!] Realloc failed \n", 20);
  
#if __DEBUG_EMALLOC__
  write(2, "Finished HOOKED realloc\n", 24);
#endif

  return (b);
}

/**
 * Wrapper for free 
 * @param a Pointer to memory to free.
 * @return
 */
void	free(void *a)
{
  void	(*freeptr)();

  //e2dbg_self();

#if __DEBUG_EMALLOC__
  write(2, "Calling HOOKED free\n", 20);
#endif

  if (!e2dbgworld.syms.freesym)
    e2dbg_dlsym_init();

  if (e2dbg_presence_get() == 0)
    {
#if __DEBUG_EMALLOC__
      write(2, "\033[1;41m", 7);
      write(2, "LIBC free used\n", 15);
      write(2, "\033[00m", 5);
#endif
      freeptr = (void *) e2dbgworld.syms.freesym;
      if (!e2dbgworld.syms.freesym)
	{
	  write(2, " [!] Unable to use original free \n", 34);
	  return;
	}
      freeptr(a);
    }
  else
    {
#if __DEBUG_EMALLOC__
      write(2, "E2DBG free used\n", 16);
#endif
      aproxy_free(a);
    }

#if __DEBUG_EMALLOC__
  write(2, "Finished HOOKED free\n", 21);
#endif
}

/**
 * FIXME
 * @param a
 * @return
 */
void	wait4exit(void *a)
{
  fprintf(stderr, "\n [*] Debuggee wait4exit called \n");
  while (!e2dbgworld.exited)
    sleep(1);
  exit(0);
}


/**
 * Wrapper for _exit 
 * @param err Return value.
 */
void		_exit(int err)
{
  /* If another thread did an exit, just signal it and return */
  if (e2dbg_presence_get() == 0)
    {
      printf(" [*] Thread ID %u exited \n", 
	     (unsigned int) e2dbg_self());
      while (1)
	sleep(1);
    }

  while (1)
    if (e2dbgworld.exited)
      {
	if (!e2dbgworld.debuggee_exited)
	  write(2, " [*] Legit program terminating\n\n", 32);
	e2dbgworld.debuggee_exited = 1;
	syscall(1, 0);
	raise(SIGKILL);
      }
    else
      {
	e2dbgworld.exited = 1;
	write(2, " [*] Debugger exited\n", 21);
	syscall(1, 0);
	raise(SIGKILL); 
      }
}


/**
 * Wrapper for heap initialisation 
 * @param first_time
 * @return
 */
/*
void	__libc_malloc_pthread_startup (int first_time)
{
  void	(*pthstartupptr)();

  if (!e2dbgworld.syms.pthstartupsym)
    e2dbg_dlsym_init();

  pthstartupptr = (void *) e2dbgworld.syms.pthstartupsym;
  write(2, "Calling __libc_malloc_pthread_startup HOOK ! \n", 46);
  pthstartupptr(first_time);
  write(2, "Finished LIBC pthread startup ! \n", 33);
  write(2, "Calling __aproxy_libc_malloc_pthread_startup HOOK ! \n", 52);
  __aproxy_libc_malloc_pthread_startup (first_time);
  write(2,g "Finished OURS pthread startup ! \n", 33);
}
*/


/* Not sure it is useful / bugless, just a try */
/*
void*	  _int_malloc(size_t p)             { return (malloc(p));      }
void*	  _int_valloc(size_t p)             { return (valloc(p));      }
void*	  _int_calloc(size_t p, size_t n)   { return (calloc(p, n));   }
void*	  _int_realloc(char *p, size_t n)   { return (realloc(p, n));  }
void*	  _int_memalign(size_t p, size_t n) { return (memalign(p, n)); }
void	  _int_free(void *p)                { free(p);                 }  
*/
