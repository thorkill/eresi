/* This file is for the gdb wrapper internals. It is not meant to be
   included. */

#define     GDBWRAP_PACKET_NO_BEGIN(_tocmp, _ptr)               \
              assert(_tocmp);                                   \
              _ptr = (strstr(_tocmp, GDBWRAP_BEGIN_PACKET) + 1)
#define     LOG(_x)                                             \
              printf(_x);                                       \
              fflush(stdout);
#define     CONSTSTRDEC(_name, _value)  const char * const  _name = _value
#define     CONSTCHRDEC(_name, _value)  const char          _name = _value
CONSTSTRDEC(GDBWRAP_BEGIN_PACKET,  "$");
CONSTSTRDEC(GDBWRAP_END_PACKET,    "#");
CONSTSTRDEC(GDBWRAP_QSUPPORTED,    "qSupported");
CONSTSTRDEC(GDBWRAP_DISCONNECT,    "k");
CONSTSTRDEC(GDBWRAP_CONTINUE,      "Hc0");
CONSTSTRDEC(GDBWRAP_GENPURPREG,    "g");
CONSTSTRDEC(GDBWRAP_MEMCONTENT,    "m");
CONSTSTRDEC(GDBWRAP_ERROR,         "error");
CONSTSTRDEC(GDBWRAP_COR_CHECKSUM,  "+");
CONSTSTRDEC(GDBWRAP_BAD_CHECKSUM,  "-");
CONSTSTRDEC(GDBWRAP_WHY_HALTED,    "?");
CONSTSTRDEC(GDBWRAP_START_ENCOD,   "*");
CONSTSTRDEC(GDBWRAP_SEP_COLON,     ":");
CONSTSTRDEC(GDBWRAP_SEP_SEMICOLON, ";");
CONSTSTRDEC(GDBWRAP_SEP_COMMA,     ",");
CONSTCHRDEC(GDBWRAP_NULL_CHAR,     '\0');

