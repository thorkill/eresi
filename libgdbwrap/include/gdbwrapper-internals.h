/* This file is for the gdb wrapper internals. It is not meant to be
   included. */

#define     GDBWRAP_PACKET_NO_BEGIN(_tocmp, _ptr)               \
              assert(_tocmp);                                   \
              _ptr = (strstr(_tocmp, GDBWRAP_BEGIN_PACKET) + 1)
#define     LOG(_x)                                             \
              printf(_x);                                       \
              fflush(stdout);
const char  *GDBWRAP_BEGIN_PACKET        = "$";
const char  *GDBWRAP_END_PACKET          = "#";
const char  *GDBWRAP_QSUPPORTED          = "qSupported";
const char  *GDBWRAP_DISCONNECT          = "k";
const char  *GDBWRAP_ERROR               = "error";
const char  *GDBWRAP_COR_CHECKSUM        = "+";
const char  *GDBWRAP_BAD_CHECKSUM        = "-";
const char  *GDBWRAP_WHY_HALTED          = "?";
const char  *GDBWRAP_SEP_COLON           = ":";
const char  *GDBWRAP_SEP_SEMICOLON       = ";";
const char  *GDBWRAP_SEP_COMMA           = ",";
const char  GDBWRAP_NULL_CHAR            = '\0';

char	    *gdbwrap_packet;
int	    gdbwrap_fd;
unsigned    gdbwrap_max_packet_size;
