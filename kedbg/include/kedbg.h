#include "revm.h"
#include "libe2dbg.h"
#include "gdbwrapper.h"
#include "interface.h"

#define  ERROR_CONNECTION  "problem when connecting... exiting\n"
#define  USAGE             "Usage: ./netgdb <server> <port> <file>\n"
#define  KEDBGNAME         "kedbg"
/* The commands are for internal debuging purpose. */
#define  COM1              "youpla"
#define  HELPCOM1          "Not much to say..."

int             cmd_linkmap(void);
int             e2dbg_linkmap_load(char *name);
void            cmd_com1(void);
void            cmd_kedbgcont(void);
