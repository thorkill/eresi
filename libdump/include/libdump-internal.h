/*
** libdump-internal.h for libdump 
** ripped from libelfsh
** Started on Sun Mar 13 17:21:33 2005 ym
** 
*/

#ifndef __LIBDUMP_INTERN_
 #define __LIBDUMP_INTERN_

extern char	*dump_error_msg;


#if defined(sun) || defined(__linux__) || defined(__BEOS__)
 #define		DUMP_ERR_ARRAY		strerror(errno)
#else
 #define		DUMP_ERR_ARRAY		sys_errlist[sys_nerr]
#endif


/* Safe calloc() */
#define		XALLOC(a, b, c)				\
do							\
{							\
  if ((a = calloc(b, 1)) == NULL)			\
    DUMP_SETERROR("libdump: Out of memory .", c);	\
}							\
while (0)

/* Safe realloc() */
#define		XREALLOC(a, b, c, d)			\
do							\
{							\
  if ((a = realloc(b, c)) == NULL)			\
    DUMP_SETERROR("libdump: Out of memory .", d);	\
}							\
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
#define		XREAD(a, b, c, d)		\
do						\
{						\
  if (read(a, b, c) != c)			\
    DUMP_SETERROR(DUMP_ERR_ARRAY, d);		\
}						\
while (0)

/* Safe lseek() */
#define		XSEEK(a, b, c, d)		\
do						\
{						\
  if (lseek(a, b, c) == (off_t) -1)	        \
    DUMP_SETERROR(DUMP_ERR_ARRAY, d);		\
}						\
while (0)

/* Safe write */
#define		XWRITE(a, b, c, d)		\
do						\
{						\
  if (write(a, b, c) != c)			\
    DUMP_SETERROR(DUMP_ERR_ARRAY, d);		\
 if (fsync(a))					\
    DUMP_SETERROR(DUMP_ERR_ARRAY, d);		\
}						\
while (0)

/* Safe close */
#define		XCLOSE(a, b)			\
do						\
{						\
  if (close(a))					\
    DUMP_SETERROR(DUMP_ERR_ARRAY, b);		\
}						\
while (0)



/* Simple useful macro */
#define		INTERVAL(a, b, c) (a <= b && b < c)


/*
** Some error stuffs : set the error offset of the file when calling SETERROR
** a: the file structure
** b: the string error
** c: the return value of the function
*/
#define DUMP_SETERROR(b, c)		\
do					\
{					\
  dump_error_msg = (char *) b;		\
  return (c);				\
}					\
while (0)			


#endif /* __LIBDUMP_INTERN_ */

