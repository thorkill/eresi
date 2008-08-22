
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


/**
 * This function parses a string *strtoparse* starting at character
 * *begin* and ending at character *end*. The new parsed string is
 * saved in *strret*. If *begin* is not found in *strtoparse* then
 * *strret* is NULL. If *end* is not found in *strtoparse*, then
 * *strret* has the value from *begin* to the end of *strtoparse*. No
 * verification on strret is done.
 *
 * @param strtoparse: String to parse.
 * @param strret    : String to return without *begin* and *end*.
 * @param begin     : String where to start parsing. If NULL,
 *                    we start from the beginning.
 * @param end       : String where to end parsing. If NULL,
 *                    we copy the string from *begin* to then
 *                    end of *strtoparse* (ie a NULL char is found).
 *
 * @return          : A pointer on *end* + 1 or NULL if we've reached
 *                    the end.
 */
static char          *gdbwrap_extract_from_packet(const char *strtoparse,
						  char *strret,
						  const char *begin,
						  const char *end,
                                                  unsigned maxsize)
{
   const char         *loccharbegin;
   char               *loccharend;
   char               *retvalue;
   ptrdiff_t          strsize;
  
   assert(strtoparse != NULL);
   if (begin != NULL)
      {
         loccharbegin = strstr(strtoparse, begin);

         /* If begin is found, we point on the next char. */
         if (loccharbegin != NULL)
            loccharbegin+= strlen(begin);
      } else
      loccharbegin = strtoparse;
  
   if (loccharbegin != NULL)
      {
         if (end != NULL)
            {
               loccharend = strstr(loccharbegin, end);

               if (loccharend == NULL)
                  loccharend = strstr(loccharbegin, GDBWRAP_END_PACKET);
              
               if (loccharend != NULL)
                  {
                     strsize = loccharend - loccharbegin;
                    
                     if (strsize > maxsize)
                        strsize = maxsize;
                     strncpy(strret, loccharbegin, strsize);
                     /* Note that if we didn't copy anything, then we put
                        GDBWRAP_NULL_CHAR at strret[0]. */
                     strret[strsize] = GDBWRAP_NULL_CHAR;
                     retvalue = loccharend + 1;
                  }
               else
                  retvalue = NULL;
            } else
            {
               strncpy(strret, loccharbegin, maxsize);
               /* *end* is null, we don't want to point on *end* + 1. */
               retvalue = NULL;
            }
      }
   else
      retvalue = NULL;

   return retvalue;
}


static la32          gdbwrap_little_endian(la32 addr)
{
   la32              addrlittle = 0;
   unsigned          i;

   for (i = 0; addr > 0; i++)
      {
         addrlittle += (LOBYTE(addr) << (8 * (sizeof(addr) - 1 - i)));
         addr >>= 8;
      }

   return addrlittle;
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


static uint8_t       gdbwrap_calc_checksum(const char * str, gdbwrap_t *desc)
{
  unsigned            i;
  uint8_t               sum;
  char                *result;
  
  result = gdbwrap_extract_from_packet(str, desc->packet, GDBWRAP_BEGIN_PACKET,
                                       GDBWRAP_END_PACKET,
				       desc->max_packet_size);
  /* If result == NULL, it's not a packet. */
  if (result == NULL)
     gdbwrap_extract_from_packet(str, desc->packet, NULL, NULL,
                                 desc->max_packet_size);

  for (i = 0, sum = 0; i < strlen(desc->packet); i++)
     sum += desc->packet[i];

  return  sum;
}


static void          gdbwrap_make_message(const char * query,
					  gdbwrap_t *desc)
{
  uint8_t              checksum       = gdbwrap_calc_checksum(query, desc);
  unsigned           max_query_size = (desc->max_packet_size -
                                       strlen(GDBWRAP_BEGIN_PACKET)
                                       - strlen(GDBWRAP_END_PACKET)
                                       - sizeof(checksum));
  if (strlen(query) < max_query_size)
    sprintf(desc->packet, "%s%s%s%x", GDBWRAP_BEGIN_PACKET, query,
	    GDBWRAP_END_PACKET, checksum);
  else
    assert(0);
}


/**
 * This function performes a run-length decoding and writes back to
 * *packet*, but no more than *maxsize* bytes.
 *
 * @param packet :  the encoded packet.
 * @param maxsize:  the maximal size of the decoded packet.
 */
static void          gdbwrap_run_length_decode(char *packet, unsigned maxsize)
{
  /* Protocol specifies to take the following value and substract 29
     and repeat by this number the previous character.  Note that the
     compression may be used multiple times in a packet. */
  char              *encoding   = packet;
  char              prevchar;
  uint8_t           numbertime;
  uint8_t           localmax    = strlen(packet);
  unsigned          iter;
  uint8_t           enlen;

  /* ----------------- NEED MORE TESTING. ----------------------- */

  while ((encoding = strstr(encoding + 1, GDBWRAP_START_ENCOD)) != NULL)
     {
       prevchar    = encoding[-1];
       numbertime  = encoding[1] - 29;
       printf(" encode: %s\n", encoding);
       fflush(stdout);
 
       /* In case it's used more than one, we have to keep an eye on
	  the overall size. */
       localmax += numbertime;  
       ASSERT(numbertime < 6 && localmax < maxsize);

       enlen = strlen(encoding);
       for (iter = 0; iter < enlen; iter++)
	 encoding[enlen - iter + numbertime - 1] = encoding[enlen - iter];

       memset(encoding, prevchar, numbertime + 1);
       printf("Locally modified: %s\n", encoding);
     }
  printf(" Modified string: %s\n", packet);
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
static void          gdbwrap_populate_reg(char *packet,
                                          gdbwrap_t *desc)
{
  const char        *nextpacket = packet;
  char              *nextupacket;
  char              packetsemicolon[100];
  char              packetcolon[100];

  gdbwrap_run_length_decode(packet, desc->max_packet_size);
/*   if (strstr(packet, "*") != NULL) */
/*     { */
/*       printf("Sorry, we don't support run-length encoding for now. "); */
/*       return; */
/*     } */

  while((nextpacket =
         gdbwrap_extract_from_packet( nextpacket,
                                      packetsemicolon,
                                      NULL,
                                      GDBWRAP_SEP_SEMICOLON,
                                      sizeof(packetsemicolon))) != NULL)
     {
        nextupacket = gdbwrap_extract_from_packet(packetsemicolon, packetcolon,
                                                  NULL, GDBWRAP_SEP_COLON,
                                                  sizeof(packetcolon));
        /* Depending on the host, we must convert to little
           endian. Since the registers are represented as a 1 byte
           value, strlen(packetcolon) exactly equals 2.*/
        if (strlen(packetcolon) == 2)
           {
              uint8_t regnumber = gdbwrap_atoh(packetcolon, 2);
              la32  regvalue  =
                 gdbwrap_little_endian(gdbwrap_atoh(nextupacket,
                                                    strlen(nextupacket)));

              *((ureg32 *)&desc->reg32.eax + regnumber) =  regvalue;
           }
     }
}


static void          gdbwrap_ack(const int fd)
{
  send(fd, GDBWRAP_COR_CHECKSUM, strlen(GDBWRAP_COR_CHECKSUM), 0x0);
}


static char          *gdbwrap_get_packet(gdbwrap_t *desc)
{
  int                rval;
  char               checksum[3];

  ASSERT(desc != NULL);
  
  rval = recv(desc->fd, desc->packet, 1, 0);
  assert(desc->packet[0] == '+');
  rval = recv(desc->fd, desc->packet, desc->max_packet_size, 0);
  desc->packet[rval] = GDBWRAP_NULL_CHAR;
  printf("Received in %s: %s\n", __PRETTY_FUNCTION__, desc->packet);
  fflush(stdout);
  gdbwrap_extract_from_packet(desc->packet, checksum, GDBWRAP_END_PACKET, NULL,
                              sizeof(checksum));
  printf(" the checksum is: %s\n ", checksum);
  fflush(stdout);
  
  if (rval > 0 &&
      gdbwrap_atoh(checksum, strlen(checksum)) ==
      gdbwrap_calc_checksum(desc->packet, desc))
    gdbwrap_ack(desc->fd);

  return desc->packet;
}


/* We can still send a pointer with a small buffer */
static char          *gdbwrap_send_data(const char *query, gdbwrap_t *desc)
{
  unsigned           rval = 0;

  ASSERT(desc != NULL && query != NULL);
  gdbwrap_make_message(query, desc);
  send(desc->fd, desc->packet, strlen(desc->packet), 0);
  gdbwrap_get_packet(desc);
  printf("Query: %s - rval: %#x - buf: %s  \n", query, rval, desc->packet);
  fflush(stdout);

  return desc->packet;
}


/**
 * Initialize the descriptor. We provide a default value of 600B for
 * the string that get the replies from server.
 *
 */
gdbwrap_t            *gdbwrap_init(int fd)
{
   gdbwrap_t         *desc = malloc(sizeof(gdbwrap_t));
      
   ASSERT(fd && desc != NULL);
   desc->packet            = malloc(desc->max_packet_size * (sizeof(char) + 1));
   desc->max_packet_size   = 600;
   desc->fd                = fd;
   ASSERT(desc->packet != NULL);

   LOG("Initialized");
   return desc;
}


void                gdbwrap_close(gdbwrap_t *desc)
{
   ASSERT(desc != NULL && desc->packet != NULL);
   free(desc->packet);
   free(desc);
}

/**
 * Initialize a connection with the gdb server and allocate more
 * memory for packets if necessary.
 *
 */
void                gdbwrap_hello(gdbwrap_t *desc)
{
   char              *received;
   char              *result;
  
   received = gdbwrap_send_data(GDBWRAP_QSUPPORTED, desc);
   result   = gdbwrap_extract_from_packet(received, desc->packet, "PacketSize=",
                                          GDBWRAP_SEP_SEMICOLON,
                                          desc->max_packet_size);
   /* If we receive the info, we update gdbwrap_max_packet_size-> */
   if (result != NULL)
      {
         desc->max_packet_size = gdbwrap_atoh(desc->packet, strlen(desc->packet));
         desc->packet = realloc(desc->packet,
                                desc->max_packet_size * (sizeof(char) + 1));
      }
  
}


/**
 * Send a disconnecting command to the server and free the packet..
 */
void                   gdbwrap_bye(gdbwrap_t *desc)
{
  assert(desc != NULL);
  gdbwrap_send_data(GDBWRAP_DISCONNECT, desc);
  free(desc->packet);
  printf("\nThx for using gdbwrap :)");
}


void                   gdbwrap_reason_halted(gdbwrap_t *desc)
{
   char                *received;

   received = gdbwrap_send_data(GDBWRAP_WHY_HALTED, desc);
   gdbwrap_populate_reg(received, desc);
}


void                   gdbwrap_test(gdbwrap_t *desc)
{
  
  char received[800];
  /* Possible with 64b */
  char *rec2 = "T0506:0*,;07:c085afedff7f0* ;10:f0eb8de5a07f0* ;";
  strncpy(received, "T05thread:00*\"01;05:0* 0000;04:0* 00;08:f0ff0f00;",
	  strlen("T05thread:00* 01;05:00000000;04:00000000;08:f0ff0f00;"));

   
   /* ASSERT(desc != NULL); */
   
   gdbwrap_run_length_decode(received, sizeof(received));
   gdbwrap_populate_reg(received, desc);
   gdbwrap_populate_reg(rec2, desc);

}

char                   *gdbwrap_own_command(const char *command,
					    gdbwrap_t *desc)
{
   return gdbwrap_send_data(command, desc);
}


