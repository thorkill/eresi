/*
** @file dwarf2-utils.c
** @ingroup libedfmt
** Started Dec 26 2006 10:49:45 mxatone
**
**
** $Id: dwarf2-utils.c,v 1.7 2007-03-07 16:45:35 thor Exp $
**
*/

#include "libedfmt.h"

/**
 * Read an unsigned leb128 number as describe on the dwarf2 documentation 
 * a little bit optimize
 * @param data pointer to a buffer where we'll read
 * @param bread store size readed
 * @return final value
 */
unsigned long 			edfmt_read_uleb128(void *data, unsigned int *bread)
{
  unsigned long		sum = 0;
  unsigned int			read = 0;
  unsigned char 		c;
  int			s = 0;

  NOPROFILER_IN();

  do {
    c = *(unsigned char *) (data + read) & 0xFF; 
    read++;
    sum |= ((unsigned long)(c & 127) << s);
    
    s += 7;
  } while ((c & 128) != 0);

  if (bread)
    *bread = read;

  NOPROFILER_ROUT(sum);
}

/**
 * Read a signed leb128 number as describe on the dwarf2 documentation 
 * a little bit optimize
 * @param data pointer to a buffer where we'll read
 * @param bread store size readed
 * @return final value
 */
long 			edfmt_read_leb128(void *data, unsigned int *bread)
{
  long			sum = 0;
  unsigned int			read = 0;
  unsigned char 		c;
  int			s = 0;

  NOPROFILER_IN();

  do {
    c = *(unsigned char *) (data + read) & 0xFF; 
    read++;
    sum |= ((long)(c & 127) << s);
    
    s += 7;
  } while ((c & 128) != 0);

  if ((s < 8 * sizeof(sum)) && (c & 0x40))
    sum |= -(((long)1) << s);

  if (bread)
    *bread = read;

  NOPROFILER_ROUT(sum);
}
