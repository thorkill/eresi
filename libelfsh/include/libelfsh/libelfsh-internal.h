/*
** libelfsh-internal.h for elfsh
** 
** Started on  Wed Jan  1 07:51:24 2003 mayhem
** Last update Thu Mar 20 06:19:53 2003 mayhem
*/

#ifndef __LIBELFSH_INTERN_
 #define __LIBELFSH_INTERN_

#if defined(sun) || defined(__linux__) || defined(__BEOS__) || defined(HPUX)
 #define		ELFSH_ERR_ARRAY		strerror(errno)
#else
 #define		ELFSH_ERR_ARRAY		sys_errlist[sys_nerr]
#endif


/* Safe calloc() */
#define		XALLOC(a, b, c)									\
do												\
{												\
  if ((a = calloc(b, 1)) == NULL)								\
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Out of memory .", c);			\
}												\
while (0)

/* Safe realloc() */
#define		XREALLOC(a, b, c, d)								\
do												\
{												\
  if ((a = realloc(b, c)) == NULL)								\
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Out of memory .", d);			\
}												\
while (0)

/* Safe open() */
#define		XOPEN(a, b, c, d, e)	\
do					\
{					\
  if ((a = open(b, c, d)) < 0)		\
    return (e);				\
}					\
while (0)
     
/* Safe read() */
#define		XREAD(a, b, c, d)							\
do											\
{											\
  if (read(a, b, c) != c)								\
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, ELFSH_ERR_ARRAY, d);		\
}											\
while (0)

/* Safe lseek() */
#define		XSEEK(a, b, c, d)							\
do											\
{											\
  if (lseek(a, b, c) == (off_t) -1)							\
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, ELFSH_ERR_ARRAY, d);		\
}											\
while (0)

/* Safe write */
#define		XWRITE(a, b, c, d)							\
do											\
{											\
  if (write(a, b, c) != c)								\
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, ELFSH_ERR_ARRAY, d);		\
 if (fsync(a))										\
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, ELFSH_ERR_ARRAY, d);		\
}											\
while (0)

/* Safe close */
#define		XCLOSE(a, b)								\
do											\
{											\
  if (close(a))										\
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, ELFSH_ERR_ARRAY, b);		\
}											\
while (0)



/* Simple useful macro */
#define	INTERVAL(a, b, c) (a <= b && b < c)


/* Profiling macros */
#define	ELFSH_PROFILE_IN(file, func, line)	elfsh_profile_out(file, (char*) func, line)
#define	ELFSH_PROFILE_OUT(file, func, line)     do { return; } while (0)
#define	ELFSH_PROFILE_ROUT(file, f, l, ret)	do { return ret; } while (0)
#define	ELFSH_PROFILE_ERR(file, f, l, m, r)	do { elfsh_error = m; elfsh_profile_err(file, (char*) f, l, m); return r; } while (0)

// To remove one day
#define	ELFSH_SETERROR(msg, ret)	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, msg, ret)

#endif /* __LIBELFSH_INTERN_ */

