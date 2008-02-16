/**
 * @file libe2dbg/kernel/strings.c
 *
 *
 * Imported from glibc-2.7
 */
#include "ke2dbg.h"

/**
 * Compare S1 and S2, returning less than, equal to or
 * greater than zero if S1 is lexicographically less than,
 * equal to or greater than S2.  
 * @param p1
 * @param p2
 * @return
 */
int	strcmp (const char *p1, const char *p2)
{
  register const unsigned char *s1 = (const unsigned char *) p1;
  register const unsigned char *s2 = (const unsigned char *) p2;
  unsigned reg_char c1, c2;

  do
    {
      c1 = (unsigned char) *s1++;
      c2 = (unsigned char) *s2++;
      if (c1 == '\0')
        return c1 - c2;
    }
  while (c1 == c2);

  return c1 - c2;
}

/**
 *
 * @param s1
 * @param s2
 * @param n
 */
char	*strncat (char *s1, const char *s2, size_t n)
{
  reg_char c;
  char *s = s1;

  /* Find the end of S1.  */
  do
    c = *s1++;
  while (c != '\0');

  /* Make S1 point before next character, so we can increment
     it while memory is read (wins on pipelined cpus).  */
  s1 -= 2;

  if (n >= 4)
    {
      size_t n4 = n >> 2;
      do
        {
          c = *s2++;
          *++s1 = c;
          if (c == '\0')
            return s;
          c = *s2++;
          *++s1 = c;
          if (c == '\0')
            return s;
          c = *s2++;
          *++s1 = c;
          if (c == '\0')
            return s;
          c = *s2++;
          *++s1 = c;
          if (c == '\0')
            return s;
        } while (--n4 > 0);
      n &= 3;
    }

  while (n > 0)
    {
      c = *s2++;
      *++s1 = c;
      if (c == '\0')
        return s;
      n--;
    }

  if (c != '\0')
    *++s1 = '\0';

  return s;
}

/**
 * 
 * @param s1
 * @param s2
 * @param n
 */
char *strncpy (char *s1, const char *s2, size_t n)
{
  reg_char c;
  char *s = s1;

  --s1;

  if (n >= 4)
    {
      size_t n4 = n >> 2;

      for (;;)
        {
          c = *s2++;
          *++s1 = c;
          if (c == '\0')
            break;
          c = *s2++;
          *++s1 = c;
          if (c == '\0')
            break;
          c = *s2++;
          *++s1 = c;
          if (c == '\0')
            break;
          c = *s2++;
          *++s1 = c;
          if (c == '\0')
            break;
          if (--n4 == 0)
            goto last_chars;
        }
      n = n - (s1 - s) - 1;
      if (n == 0)
        return s;
      goto zero_fill;
    }

 last_chars:
  n &= 3;
  if (n == 0)
    return s;

  do
    {
      c = *s2++;
      *++s1 = c;
      if (--n == 0)
        return s;
    }
  while (c != '\0');

 zero_fill:
  do
    *++s1 = '\0';
  while (--n > 0);

  return s;
}

/**
 * Compare no more than N characters of S1 and S2,
 *  returning less than, equal to or greater than zero
 *  if S1 is lexicographically less than, equal to or
 *  greater than S2.  
 *
 * @param s1
 * @param s2
 * @param n
 */
int	strncmp (const char  *s1, const char *s2, size_t n)
{
  unsigned reg_char c1 = '\0';
  unsigned reg_char c2 = '\0';

  if (n >= 4)
    {
      size_t n4 = n >> 2;
      do
        {
          c1 = (unsigned char) *s1++;
          c2 = (unsigned char) *s2++;
          if (c1 == '\0' || c1 != c2)
            return c1 - c2;
          c1 = (unsigned char) *s1++;
          c2 = (unsigned char) *s2++;
          if (c1 == '\0' || c1 != c2)
            return c1 - c2;
          c1 = (unsigned char) *s1++;
          c2 = (unsigned char) *s2++;
          if (c1 == '\0' || c1 != c2)
            return c1 - c2;
          c1 = (unsigned char) *s1++;
          c2 = (unsigned char) *s2++;
          if (c1 == '\0' || c1 != c2)
            return c1 - c2;
        } while (--n4 > 0);
      n &= 3;
    }

  while (n > 0)
    {
      c1 = (unsigned char) *s1++;
      c2 = (unsigned char) *s2++;
      if (c1 == '\0' || c1 != c2)
        return c1 - c2;
      n--;
    }

  return c1 - c2;
}

/** 
 * Append SRC on the end of DEST.  
 *
 *
 */
char	*strcat (char *dest, const char *src)
{
  char *s1 = dest;
  const char *s2 = src;
  reg_char c;

  /* Find the end of the string.  */
  do
    c = *s1++;
  while (c != '\0');

  /* Make S1 point before the next character, so we can increment
     it while memory is read (wins on pipelined cpus).  */
  s1 -= 2;

  do
    {
      c = *s2++;
      *++s1 = c;
    }
  while (c != '\0');

  return dest;
}


/**
 *  Parse S into tokens separated by characters in DELIM.
 *  If S is NULL, the saved pointer in SAVE_PTR is used as
 *  the next starting point.  For example:
 *       char s[] = "-abc-=-def";
 *       char *sp;
 *       x = strtok_r(s, "-", &sp);      // x = "abc", sp = "=-def"
 *       x = strtok_r(NULL, "-=", &sp);  // x = "def", sp = NULL
 *       x = strtok_r(NULL, "=", &sp);   // x = NULL
 *               // s = "abc\0-def\0"
 *		
 * @param s
 * @param delim
 * @param save_ptr
 * @return
 */
char *strtok_r (char *s, const char *delim, char **save_ptr)
{
  char *token;

  if (s == NULL)
    s = *save_ptr;

  /* Scan leading delimiters.  */
  s += strspn (s, delim);
  if (*s == '\0')
    {
      *save_ptr = s;
      return NULL;
    }

  /* Find the end of the token.  */
  token = s;
  s = strpbrk (token, delim);
  if (s == NULL)
    /* This token finishes the string.  */
    *save_ptr = __rawmemchr (token, '\0');
  else
    {
      /* Terminate the token and make *SAVE_PTR point past it.  */
      *s = '\0';
      *save_ptr = s + 1;
    }
  return token;
}

/** 
 * Set N bytes of S to 0.  
 *
 * @param s
 * @param len
 */
void	bzero (void *s, size_t len)
{
  long int dstp = (long int) s;
  const op_t zero = 0;

  if (len >= 8)
    {
      size_t xlen;

      /* There are at least some bytes to zero.  No need to test
         for LEN == 0 in this alignment loop.  */
      while (dstp % OPSIZ != 0)
        {
          ((byte *) dstp)[0] = 0;
          dstp += 1;
          len -= 1;
        }

      /* Write 8 op_t per iteration until less than 8 op_t remain.  */
      xlen = len / (OPSIZ * 8);
      while (xlen != 0)
        {
          ((op_t *) dstp)[0] = zero;
          ((op_t *) dstp)[1] = zero;
          ((op_t *) dstp)[2] = zero;
          ((op_t *) dstp)[3] = zero;
          ((op_t *) dstp)[4] = zero;
          ((op_t *) dstp)[5] = zero;
          ((op_t *) dstp)[6] = zero;
          ((op_t *) dstp)[7] = zero;
          dstp += 8 * OPSIZ;
          xlen -= 1;
        }
      len %= OPSIZ * 8;

      /* Write 1 op_t per iteration until less than op_t remain.  */
      xlen = len / OPSIZ;
      while (xlen != 0)
        {
          ((op_t *) dstp)[0] = zero;
          dstp += OPSIZ;
          xlen -= 1;
        }
      len %= OPSIZ;
    }

  /* Write the last few bytes.  */
  while (len != 0)
    {
      ((byte *) dstp)[0] = 0;
      dstp += 1;
      len -= 1;
    }
}
