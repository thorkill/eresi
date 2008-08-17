
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
#include              "gdbwrapper-internals.h"


static void          gdbwrap_extract(char * buf, const char * begin,
                                     const char * end)
{
   
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


static uint8       gdbwrap_calc_checksum(const char * str)
{
  unsigned            i;
  uint8               sum;
  const char          *packet_start;
  ptrdiff             packet_size;

  packet_start = strstr(str, GDBWRAP_BEGIN_PACKET);
  
  if (packet_start == NULL)
    {
      packet_start = str;
      packet_size  = strlen(str);
    }
  else
    {
      GDBWRAP_PACKET_NO_BEGIN(packet_start, packet_start);
      packet_size  = (strstr(packet_start, GDBWRAP_END_PACKET) - packet_start); 
    }
  
  for (i = 0, sum = 0; i < packet_size; i++)
    sum += packet_start[i];

  return  sum;
}


static uint8         gdbwrap_atoh_checksum(const char * str)
{
  char               *checksum_start = strstr(str, GDBWRAP_END_PACKET) + 1;

  return gdbwrap_atoh(checksum_start, strlen(checksum_start));
}


static void          gdbwrap_make_message(const char * query, char * buf)
{
  uint8              checksum       = gdbwrap_calc_checksum(query);
  unsigned           max_query_size = (max_query_size -
                                       strlen(GDBWRAP_BEGIN_PACKET)
                                       - strlen(GDBWRAP_END_PACKET)
                                       - sizeof(checksum));
  if (strlen(query) < max_query_size)
     sprintf(buf, "%s%s%s%x", GDBWRAP_BEGIN_PACKET, query, GDBWRAP_END_PACKET,
             checksum);
  else
    assert(0);
}


/**
 * Populate the gdb registers with the values received in the
 * packet. A packet has the following form:
 *
 * $n:r;[n:r;]#checksum
 *
 * where n can be a number (the register), or "thread" and r is the
 * value of the thread/register.
 *
 * @param packet: the packet to parse.
 * @param reg   : the structure in which we want to write the registers.
 */
static void          gdbwrap_populate_reg(const char * packet,
                                          gdbwrap_gdbreg32 * reg)
{
   char              *locpacketbegin;
   char              *locpacketcolon;
/*    uint8             regnum; */
/*    char              *locpacketsemicolon; */
   printf("packet received: %s", packet);
   fflush(stdout);
   GDBWRAP_PACKET_NO_BEGIN(packet, locpacketbegin);
   memset(reg, 0, sizeof(gdbwrap_gdbreg32));
   
   while ((locpacketcolon = strstr(locpacketbegin, GDBWRAP_SEP_COLON)) != NULL)
      {
         /* If the number of char between a ":" or a ";" is greater
            than 2, then it's not a register. */

         printf(" The packet is: %s - %#lx ", locpacketcolon, locpacketcolon - locpacketbegin);
         if (locpacketcolon - locpacketbegin == 2)
            {
               LOG("YAHOUUUUUUUUUUU");
            } else
            {
               locpacketbegin = strstr(locpacketcolon, GDBWRAP_SEP_SEMICOLON) + 1;               
            }
      }

   
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
static char          *gdbwrap_send_data(const char * query, char * buf,
                                        int fd)
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
  printf("Query: %s - rval: %#x - buf: %s  \n", query, rval, buf);
  fflush(stdout);

  return buf;
}


/**
 * Initialize a connection with the gdb server and allocate memory for
 * packets.
 *
 * @param fd: file descriptor of the socket.   
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
      tok_end    = strstr(tok_start, GDBWRAP_SEP_SEMICOLON);
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


gdbwrap_gdbreg32       *gdbwrap_reason_halted(void)
{
   char                *received;
   
   received = gdbwrap_send_data(GDBWRAP_WHY_HALTED, gdbwrap_packet, gdbwrap_fd);

   LOG("At least we are here");
   /* Let's populate the gdb registers. We make the (true ?) supposition that */
   while (2)
      {
         gdbwrap_populate_reg(received, &gdbwrap_reg32);
      }
   gdbwrap_reg32.eip = 0;
   return &gdbwrap_reg32;
}


char                   *gdbwrap_own_command(const char * command)
{
   return gdbwrap_send_data(command, gdbwrap_packet, gdbwrap_fd);
}
