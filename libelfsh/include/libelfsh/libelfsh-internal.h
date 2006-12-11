/*
** libelfsh-internal.h for elfsh
** 
** Started on  Wed Jan  1 07:51:24 2003 mayhem
** Last update Thu Mar 20 06:19:53 2003 mayhem
*/

#ifndef __LIBELFSH_INTERN_
 #define __LIBELFSH_INTERN_

#if defined(sun) || defined(__linux__) || defined(__BEOS__) || defined(HPUX) || defined(__FreeBSD__)
 #define		ELFSH_ERR_ARRAY		strerror(errno)
#else
 #define		ELFSH_ERR_ARRAY		sys_errlist[sys_nerr]
#endif

/* Some calls defined in libmalloc */
void		*elfsh_calloc(size_t, char);
void		*elfsh_realloc(void *, size_t);
void		*elfsh_malloc(size_t);
void		*elfsh_valloc(size_t t);
void		*elfsh_memalign(size_t t, u_int nbr);
void		elfsh_free(void *);
void		__elfsh_libc_malloc_pthread_startup(int first_time);

/* Safe calloc() */
#define		XALLOC(a, b, c)									\
do												\
{												\
  if ((a = (void *)elfsh_calloc(b, 1)) == NULL)						\
  {												\
    write(1, "Out of memory\n", 14);								\
    exit(1);											\
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, (char *)"Out of memory .", c);			\
  }												\
}												\
while (0)
  //printf("Allocated %u bytes at address %08X \n", b, (elfsh_Addr) a);				


/* Safe realloc() */
#define		XREALLOC(a, b, c, d)								\
do												\
{												\
  if ((a = (void *) elfsh_realloc(b, c)) == NULL)						\
  {												\
    write(1, "Out of memory\n", 14);								\
    exit(1);											\
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Out of memory .", d);			\
  }												\
}												\
while (0)
  //printf("ReAllocated %u bytes at address %08X \n", c, (elfsh_Addr) a);			   


/* Our free() */
#define		XFREE(a)	elfsh_free(a); a = 0

/* Safe open() */
#define		XOPEN(a, b, c, d, e)							\
do											\
{											\
  if ((a = open(b, c, d)) < 0)								\
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Cannot open file", e);		\
}											\
while (0)
     
/* Safe read() */
#define		XREAD(a, b, c, d)							\
do											\
{											\
  if (read(a, b, c) != c)								\
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, (char *)ELFSH_ERR_ARRAY, d);		\
}											\
while (0)

/* Safe lseek() */
#define		XSEEK(a, b, c, d)							\
do											\
{											\
  if (lseek(a, b, c) == (off_t) -1)							\
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, (char *)ELFSH_ERR_ARRAY, d);		\
}											\
while (0)

/* Safe write */
#define		XWRITE(a, b, c, d)							\
do											\
{											\
  if (write(a, b, c) != c)								\
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, (char *)ELFSH_ERR_ARRAY, d);		\
 if (fsync(a))										\
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, (char *)ELFSH_ERR_ARRAY, d);		\
}											\
while (0)

/* Safe close */
#define		XCLOSE(a, b)								\
do											\
{											\
  if (close(a))										\
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, (char *)ELFSH_ERR_ARRAY, b);		\
}											\
while (0)


/* Simple useful macro */
#define	INTERVAL(a, b, c) (a <= b && b < c)


/* Profiling macros */
#define	ELFSH_NOPROFILE_IN()    int profileme = 0

#define	ELFSH_NOPROFILE_OUT()	\
do				\
{				\
  profileme = 0;		\
  return;			\
}				\
while (0)

#define	ELFSH_NOPROFILE_ROUT(r) \
do				\
{				\
  profileme = 0;		\
  return (r);			\
}				\
while (0)

#define	ELFSH_PROFILE_IN(file, func, line)	\
int profileme = elfsh_depth;			\
do {						\
  elfsh_updir();				\
  elfsh_profile_out(file, (char*) func, line);	\
  elfsh_incdepth();				\
} while (0)

#define	ELFSH_PROFILE_OUT(file, func, line)     \
do {						\
  elfsh_decdepth();				\
  if (profileme != elfsh_depth)			\
  {						\
    printf(" [!] A function called by current"	\
	   "forgot to decrement elfsh_depth"    \
           "(%d %d)\n", profileme, elfsh_depth);\
    printf("     Current FUNCTION %s@%s:%d\n",	\
	   func, file, line);			\
    elfsh_depth = profileme;		        \
  }						\
  elfsh_profile_out(file, (char*) func, line);	\
  return;					\
} while (0)

#define	ELFSH_PROFILE_ROUT(file, f, l, ret)	\
do {						\
  elfsh_decdepth();				\
  if (profileme != elfsh_depth)			\
  {						\
    printf(" [!] A function called by current "	\
	   "forgot to decrement elfsh_depth"	\
           "(%d %d)\n", profileme, elfsh_depth);\
    printf("     Current FUNCTION %s@%s:%d\n",	\
	   f, file, l);				\
    elfsh_depth = profileme;			\
  }						\
  elfsh_profile_out(file, (char*) f, l);       	\
  return ret;					\
} while (0)					

#define	ELFSH_PROFILE_ERR(file, f, l, m, r)	\
do {						\
  elfsh_decdepth();				\
  if (profileme != elfsh_depth)			\
  {						\
    printf(" [!] A function called by current "	\
	   "one forgot to decrement "		\
           "elfsh_depth\n");			\
    printf("     Current FUNCTION %s@%s:%d\n",	\
	   f, file, l);				\
    elfsh_depth = profileme;			\
  }						\
  elfsh_error_str = m;				\
  elfsh_profile_err(file, (char*) f, l, m);	\
  return r;					\
} while (0)

#define	ELFSH_PROFILE_NERR(file, f, l, m)	\
do {						\
  elfsh_decdepth();				\
  if (profileme != elfsh_depth)			\
  {						\
    printf("a function called by the current "	\
	   "one forgot to decrement "		\
           "elfsh_depth\n");			\
    printf("current FUNCTION %s@%s:%d\n",	\
	   f, file, l);				\
    elfsh_depth = profileme;			\
  }						\
  elfsh_error_str = m;				\
  elfsh_profile_err(file, (char*) f, l, m);	\
} while (0)



/* Debugging macro */
#ifdef ELFSH_DEBUG						
#define ELFSH_PROFILE_DEBUG(fi, f, l, fm, a...) do		\
 {								\
   if (dbgworld.proflevel >= ELFSH_DEBUGPROF)			\
     {								\
       char	dbg[BUFSIZ];					\
       snprintf(dbg, BUFSIZ, " [D]<%s@%s:%d> ", f, fi, l);	\
       dbgworld.profile(dbg);					\
       snprintf(dbg, BUFSIZ, fm, ##a);				\
       dbgworld.profile(dbg);					\
       dbgworld.profile("\n");					\
     }								\
 while (0)
#endif


// To remove one day
#define	ELFSH_SETERROR(msg, ret)	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, msg, ret)

#endif /* __LIBELFSH_INTERN_ */

