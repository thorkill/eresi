/*
** libelfsh-internal.h for elfsh
** 
** Started on  Wed Jan  1 07:51:24 2003 mayhem
** Last update Thu Mar 20 06:19:53 2003 mayhem
*/

#ifndef __LIBELFSH_INTERN_
 #define __LIBELFSH_INTERN_

extern char	*elfsh_error_msg;


#if defined(sun) || defined(__linux__)
 #define		ELFSH_ERR_ARRAY		strerror(errno)
#else
 #define		ELFSH_ERR_ARRAY		sys_errlist[sys_nerr]
#endif


/* Safe calloc() */
#define		XALLOC(a, b, c)				\
do							\
{							\
  if ((a = calloc(b, 1)) == NULL)			\
    ELFSH_SETERROR("libelfsh: Out of memory .", c);	\
}							\
while (0)

/* Safe realloc() */
#define		XREALLOC(a, b, c, d)			\
do							\
{							\
  if ((a = realloc(b, c)) == NULL)			\
    ELFSH_SETERROR("libelfsh: Out of memory .", d);	\
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
    ELFSH_SETERROR(ELFSH_ERR_ARRAY, d);		\
}						\
while (0)

/* Safe lseek() */
#define		XSEEK(a, b, c, d)		\
do						\
{						\
  if (lseek(a, b, c) == (off_t) -1)	        \
    ELFSH_SETERROR(ELFSH_ERR_ARRAY, d);		\
}						\
while (0)

/* Safe write */
#define		XWRITE(a, b, c, d)		\
do						\
{						\
  if (write(a, b, c) != c)			\
    ELFSH_SETERROR(ELFSH_ERR_ARRAY, d);		\
 if (fsync(a))					\
    ELFSH_SETERROR(ELFSH_ERR_ARRAY, d);		\
}						\
while (0)

/* Safe close */
#define		XCLOSE(a, b)			\
do						\
{						\
  if (close(a))					\
    ELFSH_SETERROR(ELFSH_ERR_ARRAY, b);		\
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
#define ELFSH_SETERROR(b, c)		\
do					\
{					\
  elfsh_error_msg = (char *) b;		\
  return (c);				\
}					\
while (0)			

#endif /* __LIBELFSH_INTERN_ */
