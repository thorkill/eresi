
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
 * saved in *strret*. If *begin* is not found in *strtoparse* then the
 * function returns NULL. If *end* is not found in *strtoparse*, then
 * the function returns NULL..
 *
 * @param strtoparse: String to parse.
 * @param strret    : String to return without *begin* and *end*.
 * @param begin     : String where to start parsing. If NULL,
 *                    we start from the beginning.
 * @param end       : String where to end parsing. If NULL,
 *                    we copy the string from *begin* to then
 *                    end of *strtoparse* (ie a NULL char is found).
 * @param maxsize   : The maximum size to extract.
 * @return          : Returns a pointer on the beginning of the new string.
 */
static char          *gdbwrap_extract_from_packet(const char *strtoparse,
						  char *strret,
						  const char *begin,
						  const char *end,
                                                  unsigned maxsize)
{
  const char         *charbegin;
  const char         *charend;
  unsigned           strtorem;
  ptrdiff_t          strsize;

  ASSERT(strtoparse != NULL);

  if (begin == NULL)
    {
      charbegin = strtoparse;
      strtorem  = 0;
    }
  else
    {
      charbegin = strstr(strtoparse, begin);
      strtorem  = strlen(begin); 
      if (charbegin == NULL)
	return NULL;
    }
  
  if (end == NULL)
    charend = charbegin + strlen(charbegin);
  else
    {
      charend = strstr(charbegin, end);
      if (charend == NULL)
	return NULL;
    }

  strsize = charend - charbegin - strtorem;
  if (strsize > maxsize)
    strsize = maxsize;

  strncpy(strret, charbegin + strtorem, strsize);
  strret[strsize] = GDBWRAP_NULL_CHAR;

  return strret;
}


static la32          gdbwrap_little_endian(la32 addr)
{
  la32               addrlittle = 0;
  unsigned           i;

  for (i = 0; addr > 0; i++)
    {
      addrlittle += (LOBYTE(addr) << (8 * (sizeof(addr) - 1 - i)));
      addr >>= 8;
    }

  return addrlittle;
}


static unsigned      gdbwrap_atoh(const char * str, unsigned size)
{
  unsigned           i;
  unsigned           hex;

  for (i = 0, hex = 0; i < size; i++)
    if (str[i] >= 'a' && str[i] <= 'f')
      hex += (str[i] - 0x57) << 4 * (size - i - 1);
    else if (str[i] >= '0' && str[i] <= '9')
      hex += (str[i] - 0x30) << 4 * (size - i - 1);
    else
      ASSERT(FALSE); 

  return hex;
}


static uint8_t       gdbwrap_calc_checksum(const char * str, gdbwrap_t *desc)
{
  unsigned           i;
  uint8_t            sum;
  char               *result;

  
  result = gdbwrap_extract_from_packet(str, desc->packet, GDBWRAP_BEGIN_PACKET,
                                       GDBWRAP_END_PACKET,
				       desc->max_packet_size);
  /* If result == NULL, it's not a packet. */
  if (result == NULL)
    result = gdbwrap_extract_from_packet(str, desc->packet, NULL, NULL,
					 desc->max_packet_size);
    
  for (i = 0, sum = 0; i < strlen(result); i++)
    sum += result[i];

  return  sum;
}


static char          *gdbwrap_make_message(const char * query,
                                           gdbwrap_t *desc)
{
  uint8_t            checksum       = gdbwrap_calc_checksum(query, desc);
  unsigned           max_query_size = (desc->max_packet_size -
				       strlen(GDBWRAP_BEGIN_PACKET)
				       - strlen(GDBWRAP_END_PACKET)
				       - sizeof(checksum));

  /* Sometimes C sucks... Basic source and destination checking. We do
     not check the overlapping tho.*/
  if (strlen(query) < max_query_size && &*query != &*desc->packet)
    ASSERT(snprintf(desc->packet, desc->max_packet_size, "%s%s%s%.2x",
		    GDBWRAP_BEGIN_PACKET, query, GDBWRAP_END_PACKET, checksum) > 0);
  else
    ASSERT(FALSE);


  return desc->packet;
}


/**
 * This function performes a run-length decoding and writes back to
 * *dstpacket*, but no more than *maxsize* bytes. Actually, it blows
 * up if we try to write more packet :).
 *
 * @param srcpacket:  the encoded packet.
 * @param maxsize:  the maximal size of the decoded packet.
 */
static char          *gdbwrap_run_length_decode(char *dstpacket, const char *srcpacket,
						unsigned maxsize)
{
  /* Protocol specifies to take the following value and substract 29
     and repeat by this number the previous character.  Note that the
     compression may be used multiple times in a packet. */
  char               *encodestr;
  char               valuetocopy;
  uint8_t            numberoftimes;
  unsigned           iter;
  unsigned           strlenc;
  unsigned           locmaxsize = 0;
  
  ASSERT(dstpacket != NULL && srcpacket != NULL &&
	 strncmp(srcpacket, GDBWRAP_START_ENCOD, 1));
  if (&*srcpacket != &*dstpacket)
    strncpy(dstpacket, srcpacket, maxsize);
  encodestr   = strstr(dstpacket, GDBWRAP_START_ENCOD);
  while (encodestr != NULL)
    {
      valuetocopy    = encodestr[-1];
      numberoftimes  = encodestr[1] - 29;
      strlenc        = strlen(encodestr);
      locmaxsize    += strlenc + numberoftimes - 2;
      ASSERT(locmaxsize < maxsize);
      /* We move the string to the right, then set the bytes. We
	 substract 2, because we have <number>*<char> where * and
	 <char> are filled with the value of <number> (ie 2 chars). */
      for (iter = 0; iter < strlenc; iter++)
	encodestr[strlenc + numberoftimes - iter - 2] = encodestr[strlenc - iter];
      memset(encodestr, valuetocopy, numberoftimes);
      encodestr = strstr(encodestr + 1, GDBWRAP_START_ENCOD);
    }
  
  return dstpacket;
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
static void         gdbwrap_populate_reg(char *packet,
					 gdbwrap_t *desc)
{
  const char        *nextpacket;
  char              *nextupacket;
  char              packetsemicolon[50];
  char              packetcolon[50];
  unsigned          packetoffset = 0;

  while ((nextpacket =
	  gdbwrap_extract_from_packet(packet + packetoffset,
				      packetsemicolon,
				      NULL,
				      GDBWRAP_SEP_SEMICOLON,
				      sizeof(packetsemicolon))) != NULL)
    {
      nextupacket = gdbwrap_extract_from_packet(nextpacket, packetcolon, NULL,
						GDBWRAP_SEP_COLON,
						sizeof(packetcolon));
      ASSERT(nextupacket != NULL);
      if (strlen(nextupacket) == 2)
	{
	  uint8_t regnumber = gdbwrap_atoh(nextupacket, strlen(nextupacket));
	  ureg32  regvalue;
	  
	  nextupacket = gdbwrap_extract_from_packet(nextpacket, packetcolon,
						    GDBWRAP_SEP_COLON, NULL,
						    sizeof(packetcolon));
	  ASSERT(nextupacket != NULL);
	  regvalue = gdbwrap_atoh(nextupacket, strlen(nextupacket));
	  regvalue = gdbwrap_little_endian(regvalue);
					   
	  *(&desc->reg32.eax + regnumber) =  regvalue;
	}
      /* We add 1 in order not to take the right limit. In the worst
	 case, we should get the NULL char. */
      packetoffset += strlen(nextpacket) + 1;
    }
}


static char          *gdbwrap_get_packet(gdbwrap_t *desc)
{
  int                rval;
  char               checksum[3];

  ASSERT(desc != NULL);
  
  rval = recv(desc->fd, desc->packet, 1, 0);
  /* The result of the previous recv must be a "+". */
  ASSERT(rval != -1  && !strncmp(desc->packet, GDBWRAP_COR_CHECKSUM, 1));
  rval = recv(desc->fd, desc->packet, desc->max_packet_size, 0);
  desc->packet[rval] = GDBWRAP_NULL_CHAR;
  gdbwrap_extract_from_packet(desc->packet, checksum, GDBWRAP_END_PACKET, NULL,
                              sizeof(checksum));
  /* If no error, we ack the packet. */
  if (rval != -1 &&
      gdbwrap_atoh(checksum, strlen(checksum)) ==
      gdbwrap_calc_checksum(desc->packet, desc))
    send(desc->fd, GDBWRAP_COR_CHECKSUM, strlen(GDBWRAP_COR_CHECKSUM), 0x0);
  else
    ASSERT(FALSE);

  return gdbwrap_run_length_decode(desc->packet, desc->packet,
				   desc->max_packet_size);
}


static char          *gdbwrap_send_data(const char *query, gdbwrap_t *desc)
{
  unsigned           rval = 0;
  char               *mes;

  ASSERT(desc != NULL && query != NULL);
  mes  = gdbwrap_make_message(query, desc);
  rval = send(desc->fd, mes, strlen(mes), 0);
  ASSERT(rval != -1);
  mes  = gdbwrap_get_packet(desc);

  return mes;
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
  desc->max_packet_size   = 600;
  desc->packet            = malloc((desc->max_packet_size + 1) * sizeof(char));
  desc->fd                = fd;
  ASSERT(desc->packet != NULL);

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
  unsigned          previousmax;
  
  received = gdbwrap_send_data(GDBWRAP_QSUPPORTED, desc);
  result   = gdbwrap_extract_from_packet(received, desc->packet, "PacketSize=",
					 GDBWRAP_SEP_SEMICOLON,
					 desc->max_packet_size);
  /* If we receive the info, we update gdbwrap_max_packet_size. */
  if (result != NULL)
    {
      char *reallocptr;
      
      previousmax = desc->max_packet_size;
      desc->max_packet_size = gdbwrap_atoh(desc->packet, strlen(desc->packet));
      reallocptr = realloc(desc->packet, desc->max_packet_size + 1);
      if (realloc != NULL)
	desc->packet = reallocptr;
      else
	{
	  fprintf(stderr, "space was not updated.\n");
	  desc->max_packet_size = previousmax;
	}
    }
  /* We set the last bit to a NULL char to avoid getting out of the
     weeds with a (unlikely) bad strlen. */
  desc->packet[desc->max_packet_size] = GDBWRAP_NULL_CHAR;
}


/**
 * Send a "disconnect" command to the server and free the packet.
 */
void                gdbwrap_bye(gdbwrap_t *desc)
{
  assert(desc != NULL);
  gdbwrap_send_data(GDBWRAP_DISCONNECT, desc);
  printf("\nThx for using gdbwrap :)\n");
}


void                gdbwrap_reason_halted(gdbwrap_t *desc)
{
  char              *received;

  received = gdbwrap_send_data(GDBWRAP_WHY_HALTED, desc);
  gdbwrap_populate_reg(received, desc);
}


/**
 * Great, the gdb protocol has absolutely no consistency, thus we
 * cannot reuse the gdbwrap_populate_reg. We receive a poorly
 * documented bulk message when sending the "g" query.
 */
gdbwrap_gdbreg32     *gdbwrap_readgenreg(gdbwrap_t *desc)
{
  char               *rec;
  unsigned           i;
  ureg32             regvalue;
  
  rec = gdbwrap_send_data(GDBWRAP_GENPURPREG, desc);
  for (i = 0; i < strlen(rec) / sizeof(ureg32); i++)
    {
      /* The value(8) must be replaced here. */
       regvalue = gdbwrap_atoh(rec, 8);
       regvalue = gdbwrap_little_endian(regvalue);
       *(&desc->reg32.eax + i) = regvalue;
       rec += 8;
    }

  return &desc->reg32;
}


void                 gdbwrap_continue(gdbwrap_t *desc)
{
  char               *rec;

  rec = gdbwrap_send_data(GDBWRAP_CONTINUETHREAD0, desc);
  if (!strncmp(rec, GDBWRAP_ERROR, 1))
    rec = gdbwrap_send_data(GDBWRAP_CONTINUE, desc);
}


/* TODO: error handling. E00, etc */
char                 *gdbwrap_memorycontent(gdbwrap_t *desc, la32 linaddr,
					    uint8_t bytes)
{
  char               *rec;
  char               packet[50];
  
  snprintf(packet, sizeof(packet), "%s%x%s%x", GDBWRAP_MEMCONTENT,
	   linaddr, GDBWRAP_SEP_COMMA, bytes);
  rec = gdbwrap_send_data(packet, desc);

  return rec;
}


char                 *gdbwrap_own_command(const char *command,
					  gdbwrap_t *desc)
{
  return gdbwrap_send_data(command, desc);
}


#if 0
/* Set a memory breakpoint en go there. It assumes a continue. To see
   how it's gonna work when merging with the rest.*/
void                 *gdbwrap_memorybp(gdbwrap_t *desc)
{
  
}
#endif

/*  Typical sequence (does it suck ?):

 *  $qSupported#37+$#00++$Hc-1#09+$E00#a5+$qC#b4+$QC1#c5+$?#3f
 *  +$T05thread:00000001;05:00000000;04:00000000;08:f0ff0f00;#0f+$mffff0,8#99
 *  +$e906e70000000000#80+$mffff0,7#98+$e906e700000000#20+
 */
void                 gdbwrap_vmwareinit(gdbwrap_t *desc)
{
  char               *rec;

  gdbwrap_hello(desc);
  gdbwrap_reason_halted(desc);
  rec = gdbwrap_memorycontent(desc, desc->reg32.eip, QWORDB);
}


void                 gdbwrap_test(gdbwrap_t *desc)
{
  gdbwrap_memorycontent(desc,0xb7fc99a0 , QWORDB);
}
