
/*  First try of implementation of a gdb wrapper. 
 *
 *  See gdb documentation, section D for more information on the
 *  remote serial protocol. To make it short, a packet looks like the following:
 *
 *  $packet-data#checksum  or  $sequence-id:packet-data#checksum.
 * 
 *  where the checksum is the sum of all the characters modulo 256.
 */


#include "gdbwrapper.h"

static unsigned       gdbwrap_calc_checksum(char * str)
{
  unsigned            i;
  unsigned            sum;

  for (i = 0, sum = 0; i <= strlen(str); i++)
    {
      sum += str[i];
    }
  return  sum % 256;
}


static void           gdbwrap_send_data(const int * fd, const char * query)
{
  unsigned            checksum;
  char                buf[50];
  checksum = gdbwrap_calc_checksum(buf);
  sprintf(buf, BEGIN_PACKET"%s"END_PACKET"%x", query, checksum);
  send(*fd, buf, strlen(buf), 0x0);
}


void                  gdbwrap_query_supported(const int * fd)
{
  gdbwrap_send_data(fd, QSUPPORTED);
}
