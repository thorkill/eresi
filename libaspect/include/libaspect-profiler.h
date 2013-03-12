/*
* @file libaspect/include/libaspect-profiler.h
** 
** Started on  Wed Jan  1 07:51:24 2003 jfv
** Last update Thu Mar 20 06:19:53 2003 jfv
**
** $Id$
**
*/

#ifndef __LIBASPECT_INTERN_
 #define __LIBASPECT_INTERN_

#if defined(sun) || defined(__linux__) || defined(__BEOS__) || defined(HPUX) || defined(__FreeBSD__)
 #define     PROFILER_ERRORS_ARRAY		strerror(errno)
#else
 #define     PROFILER_ERRORS_ARRAY		sys_errlist[sys_nerr]
#endif

/* Extern variables */
extern int		profiler_depth;
extern char*		profiler_error_str;
extern int		aspect_type_nbr; 
extern hash_t		*vector_hash;
extern hash_t		types_hash;


/**
 * Type of warnings that emit the allocation profiler 
 */
#define			PROFILER_WARNING_UNKNOW	0
#define			PROFILER_WARNING_LAST	1
#define			PROFILER_WARNING_FIRST	2

/**
 * The structure for an allocation entry 
 */
typedef	struct		s_allocentry
{
#define			PROFILER_ALLOC_UNKNOW	0
#define			PROFILER_ALLOC_LEGIT	1
#define			PROFILER_ALLOC_PROXY	2
  u_char		alloctype;	/*!< Inform about the allocator */

#define			PROFILER_OP_UNKNOW	0
#define			PROFILER_OP_ALLOC	1
#define			PROFILER_OP_REALLOC	2
#define			PROFILER_OP_FREE	3
  u_char		optype;		/*!< Inform about alloc/free/etc */
  char			*filename;	/*!< Inform about file location */
  char			*funcname;	/*!< Inform about func location */
  u_int			linenbr;	/*!< Inform about line number */
  u_long		addr;		/*!< Address of allocation */
}			profallocentry_t;

/**
 * Maximum function calls patterns depth 
 */
#define		PROFILER_MAX_PATTERN	10 
#define		PROFILER_MAX_ALLOC	2000


/**
 * Safe calloc() 
 */
#define		XALLOC(f, fc, l, a, b, c)		    \
do							    \
{							    \
  if ((a = (void *) calloc(b, 1)) == NULL)		    \
  {							    \
    int wret = write(1, "Out of memory\n", 14);		    \
    exit(wret);						    \
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,          \
		      (char *)"Out of memory .", c);	    \
  }							    \
  if (profiler_started())				    \
    profiler_alloc_update(f, (char *) fc, l, (u_long) a,    \
		          PROFILER_ALLOC_PROXY,		    \
		          PROFILER_OP_ALLOC);		    \
}							    \
while (0)

/**
 * Safe realloc() 
 */
#define		XREALLOC(f, fc, l, a, b, c, d)		    \
do							    \
{							    \
  if ((a = (void *) realloc(b, c)) == NULL)		    \
  {							    \
    int wret = write(1, "Out of memory\n", 14);		    \
    exit(wret);						    \
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,	    \
		      "Out of memory .", d);		    \
  }							    \
  if (profiler_started())				    \
    profiler_alloc_update(f, (char *) fc, l, (u_long) a,    \
			  PROFILER_ALLOC_PROXY,		    \
		          PROFILER_OP_REALLOC);		    \
}							    \
while (0)

/**
 * Our free() 
 */
#define		XFREE(f, fc, l, a)			    \
do							    \
{							    \
  if (profiler_started())				    \
    profiler_alloc_update(f, (char *) fc, l, (u_long) a,    \
		          PROFILER_ALLOC_PROXY,		    \
		          PROFILER_OP_FREE);		    \
  free(a);						    \
  a = 0;						    \
}							    \
while (0)

/**
 * Our strdup() 
 */
#define		XSTRDUP(f, fc, l, a, b)			    \
do							    \
{							    \
  if (profiler_started())				    \
    profiler_alloc_update(f, (char *) fc, l, (u_long) a,    \
		          PROFILER_ALLOC_PROXY,		    \
		          PROFILER_OP_ALLOC);		    \
  a = strdup(b);					    \
}							    \
while (0)

/**
 * Safe open() 
 */
#define		XOPEN(a, b, c, d, e)			    \
do							    \
{							    \
  if ((a = open(b, c, d)) < 0)				    \
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,          \
		      "Cannot open file", e);		    \
}							    \
while (0)
     
/**
 * Safe read() 
 */
#define		XREAD(a, b, c, d)			    \
do							    \
{							    \
  if (read(a, b, c) != c)				    \
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,          \
		      (char *) PROFILER_ERRORS_ARRAY, d);   \
}							    \
while (0)

/**
 * Safe lseek() 
 */
#define		XSEEK(a, b, c, d)			    \
do							    \
{							    \
  if (lseek(a, b, c) == (off_t) -1)			    \
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,          \
		      (char *) PROFILER_ERRORS_ARRAY, d);   \
}							    \
while (0)

/**
 * Safe write 
 */
#define		XWRITE(a, b, c, d)			    \
do							    \
{							    \
  if (write(a, b, c) != c)				    \
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,          \
		      (char *) PROFILER_ERRORS_ARRAY, d);   \
 if (fsync(a))						    \
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,	    \
		      (char *) PROFILER_ERRORS_ARRAY, d);   \
}							    \
while (0)

/**
 * Safe close 
 */
#define		XCLOSE(a, b)				    \
do							    \
{							    \
  if (close(a))						    \
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,	    \
		      (char *)PROFILER_ERRORS_ARRAY, b);    \
}							    \
while (0)

/**
 * @brief Safe mmap
 */
#define XMMAP(a, b, c, d, e, f, g, h)			    \
do							    \
{							    \
  if ((a = mmap(b, c, d, e, f, g)) == MAP_FAILED)	    \
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,          \
		 (char *)PROFILER_ERRORS_ARRAY, h);	    \
}							    \
while (0)


/**
 * @brief Safe munmap
 */
#define XMUNMAP(a, b, c)				    \
do							    \
{							    \
  if (munmap(a, b) == -1)				    \
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,          \
		 (char *)PROFILER_ERRORS_ARRAY, c);         \
}							    \
while(0)

/**
 * @brief Safe msync
 */
#define XMSYNC(a, b, c, d)				    \
do							    \
{							    \
  if (msync(a, b, c) == -1)				    \
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,          \
		 (char *)PROFILER_ERRORS_ARRAY, d);	    \
}							    \
while(0)

/**
 * @brief Safe lseek64
 */
#define XLSEEK64(a, b, c, d)				    \
do							    \
{							    \
  if (lseek64(a, (off64_t)b, c) == -1)			    \
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,          \
		 (char *)PROFILER_ERRORS_ARRAY, d);	    \
}							    \
while(0)


/**
 * Simple useful macro 
 */
#define	INTERVAL(a, b, c) (a <= b && b < c)

/**
 * Profiling macros - some artifact to shut gcc up
 */
#define	NOPROFILER_IN()  int profileme = 0

#define	NOPROFILER_OUT()	\
do				\
{				\
  if (profileme);		\
  return;			\
}				\
while (0)

#define	NOPROFILER_ROUT(r)	\
do				\
{				\
  if (profileme);		\
  return (r);			\
}				\
while (0)

#define PROFILER_INQ()				\
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__)

#define	PROFILER_IN(file, fun, lin)		\
int profileme = profiler_depth;			\
do {						\
  if (profiler_started()) {			\
  profiler_updir();				\
  profiler_out(file, (char*) fun, lin);	        \
  profiler_incdepth();	  }			\
} while (0)

#define	PROFILER_OUTQ()				\
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__)

#define	PROFILER_OUT(file, func, line)		\
do {						\
  if (profiler_started()) {			\
  profiler_decdepth();				\
  if (profileme != profiler_depth)		\
  {						\
    printf(" [!] A function called by current"	\
	   "forgot to decrement profiler_depth" \
           "(%d %d)\n",				\
	   profileme, profiler_depth);		\
    printf("     Current FUNCTION %s@%s:%d\n",	\
	   func, file, line);			\
    profiler_depth = profileme;		        \
  }						\
  profiler_out(file, (char*) func, line); }     \
  return;		  			\
} while (0)

#define	PROFILER_ROUTQ(ret)			\
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret)

#define	PROFILER_ROUT(file, f, l, ret)		\
do {						\
  if (profiler_started()) {			\
  profiler_decdepth();				\
  if (profileme != profiler_depth)		\
  {						\
    printf(" [!] A function called by current "	\
	   "forgot to decrement profiler_depth"	\
           "(%d %d)\n", profileme,		\
	   profiler_depth);			\
    printf("     Current FUNCTION %s@%s:%d\n",	\
	   f, file, l);				\
    profiler_depth = profileme;			\
  }						\
  profiler_out(file, (char*) f, l); }           \
  return ret;	          			\
} while (0)					

#define PROFILER_ERRQ(msg, ret)			\
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, msg, ret);

#define	PROFILER_ERR(file, f, l, m, r)		\
do {						\
  if (profiler_started()) {			\
  profiler_decdepth();				\
  if (profileme != profiler_depth)		\
  {						\
    printf(" [!] A function called by current "	\
	   "one forgot to decrement "		\
           "profiler_depth\n");			\
    printf("     Current FUNCTION %s@%s:%d\n",	\
	   f, file, l);				\
    profiler_depth = profileme;			\
  }						\
  profiler_error_str = m;			\
  profiler_err(file, (char*) f, l, m); }	\
  return r;		  			\
} while (0)

#define	PROFILER_RERR(file, f, l, m)		\
do {						\
  if (profiler_started()) {			\
  profiler_decdepth();				\
  if (profileme != profiler_depth)		\
  {						\
    printf("a function called by the current "	\
	   "one forgot to decrement "		\
           "profiler_depth\n");			\
    printf("current FUNCTION %s@%s:%d\n",	\
	   f, file, l);				\
    profiler_depth = profileme;			\
  }						\
  profiler_error_str = m;			\
  profiler_err(file, (char*) f, l, m);	}	\
} while (0)



/**
 * Debugging macro 
 */
#ifdef ELFSH_DEBUG						
#define PROFILER_DEBUG(fi, f, l, fm, a...) do			  \
 {								  \
   if (profiler_started() && dbgworld.proflevel >= PROFILE_DEBUG) \
     {								  \
       char	dbg[BUFSIZ];					  \
       snprintf(dbg, BUFSIZ, " [D]<%s@%s:%d> ", f, fi, l);	  \
       dbgworld.profile(dbg);					  \
       snprintf(dbg, BUFSIZ, fm, ##a);				  \
       dbgworld.profile(dbg);					  \
       dbgworld.profile("\n");					  \
     }								  \
 while (0)
#endif


#endif
