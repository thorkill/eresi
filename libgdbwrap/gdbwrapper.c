
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


static unsigned       gdbwrap_calc_checksum(const char * str)
{
  unsigned            i;
  uint8_t             sum;

  for (i = 0, sum = 0; i < strlen(str); i++)
    sum += str[i];

  return  sum;
}


char                 *gdbwrap_send_data(const int fd, char * query, char * ret)
{
  unsigned            checksum;
  /* has to be updated with gdb result */
  char                buf[50];
  int                 rval;
  
  checksum = gdbwrap_calc_checksum(query);
  /* Bad. Bof. */
  sprintf(buf, BEGIN_PACKET"%s"END_PACKET"%x", query, checksum);
  send(fd, buf, strlen(buf), 0x0);
  rval = recv(fd, ret, 150, 0 );
  if(ret[0] == '+')
    return ret;
  else
    return ERROR;
}

