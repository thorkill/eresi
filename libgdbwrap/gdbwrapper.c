
/*  First try of implementation of a gdb wrapper. 
 *
 *  See gdb documentation, section D for more information on the
 *  remote serial protocol. To make it short, a packet looks like the following:
 *
 *  $packet-data#checksum  or  $sequence-id:packet-data#checksum.
 * 
 *  where the checksum is the sum of all the characters modulo 256.
 */


#include              "gdbwrapper.h"


char	              *gdbwrap_packet;
int		      gdbwrap_fd;
unsigned              gdbwrap_max_packet_size;


static unsigned       gdbwrap_calc_checksum(char * str)
{
  unsigned            i;
  uint8_t             sum;
  char                *packet_start;
  ptrdiff_t           packet_size;

  packet_start = strstr(str, GDBWRAP_BEGIN_PACKET);
  
  if (packet_start == NULL)
    {
      packet_start = str;
      packet_size  = strlen(str);
    }
  else
    {
      packet_start = GDBWRAP_PACKET_NO_BEGIN(packet_start);
      packet_size  = (strstr(packet_start, GDBWRAP_END_PACKET) - packet_start); 
    }
  
  for (i = 0, sum = 0; i < packet_size; i++)
    sum += packet_start[i];

  return  sum;
}


static unsigned      gdbwrap_atoh(const char * str, const unsigned size)
{
  unsigned           i;
  unsigned           hex;
  
  for (i = 0, hex = 0; i < size; i++)
    if (str[i] >= 'a' && str[i] <= 'f')
      hex += (str[i] - 0x57) << 4 * (size - i - 1);
    else if (str[i] >= '0' && str[i] <= '9')
      hex += (str[i] - 0x30) << 4 * (size - i - 1);
    else
      assert(0); /* will be replaced by false */

  return hex;
}


static unsigned      gdbwrap_atoh_checksum(const char * str)
{
  char               *checksum_start = strstr(str, GDBWRAP_END_PACKET) + 1;

  return gdbwrap_atoh(checksum_start, strlen(checksum_start));
}


static void          gdbwrap_make_message(char * query, char * buf)
{
  uint8_t            checksum       = gdbwrap_calc_checksum(query);
  unsigned           max_query_size = ( max_query_size -
					strlen(GDBWRAP_BEGIN_PACKET)
					- strlen(GDBWRAP_END_PACKET)
					- sizeof(checksum));
  
  if (strlen(query) < max_query_size)
    sprintf(buf, GDBWRAP_BEGIN_PACKET"%s"GDBWRAP_END_PACKET"%x",
	    query, checksum);
  else
    assert(0);
}


static void          gdbwrap_ack(const int fd)
{
  send(fd, GDBWRAP_COR_CHECKSUM, strlen(GDBWRAP_COR_CHECKSUM), 0x0);
}


static char          *gdbwrap_get_packet(char * buf, const int fd)
{
  int rval;

  rval = recv(fd, buf, 1, 0);
  assert(buf[0] == '+');
  rval = recv(fd, buf, gdbwrap_max_packet_size, 0);
  buf[rval] = '\0';
  
  if (rval > 0 && gdbwrap_atoh_checksum(buf) == gdbwrap_calc_checksum(buf))
    gdbwrap_ack(fd);

  return buf;
}


/* We can still send a pointer with a small buffer */
static char          *gdbwrap_send_data(char * query, char * buf, const int fd)
{
  unsigned           rval = 0;

  if (buf == NULL || !fd)
    {
      fprintf(stderr, "please, initialize the connection");
      return NULL;
    }
  
  gdbwrap_make_message(query, buf);
  send(fd, buf, strlen(buf), 0);
  gdbwrap_get_packet(buf, fd);
  printf("Query: %s - rval: %#x - buf: %s  \n", query, rval,
	 buf);
  fflush(stdout);

  return buf;
}


/**
 * Initialize a connection with the gdb server and allocate memory for
 * packets.
 *
 * @param fd file descriptor of the socket.   
 */
void                   gdbwrap_hello(int fd)
{
  char                 buf[600];
  char                 *tok_start;
  char                 *tok_end;
  
  gdbwrap_fd = fd;
  /* We set the max value to the size of buf, since we don't know it
     yet */
  gdbwrap_max_packet_size = sizeof(buf);
  tok_start = gdbwrap_send_data(GDBWRAP_QSUPPORTED, buf, fd);
  tok_start = strstr(tok_start, "PacketSize=");

  if (tok_start != NULL)
    {
      tok_start += strlen("PacketSize=");
      tok_end    = strstr(tok_start, ";");
      gdbwrap_max_packet_size = gdbwrap_atoh(tok_start, tok_end - tok_start);
    }
  
  if (gdbwrap_packet == NULL)
    gdbwrap_packet = malloc(gdbwrap_max_packet_size * sizeof(char));
}


/**
 * Send a disconnecting command to the server and free the packet..
 */
void                   gdbwrap_bye(void)
{
  gdbwrap_send_data(GDBWRAP_DISCONNECT, gdbwrap_packet, gdbwrap_fd);
  free(gdbwrap_packet);
  printf("\nThx for using gdbwrap :)");
}

