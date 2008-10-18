#include <netdb.h>
#include "revm.h"
/* #include "gdbwrapper.h" */
int             gdbwrap_simpleconnect(char *host, int port);
int             cmd_linkmap(void);
int             e2dbg_linkmap_load(char *name);
void            cmd_com1(void);
void            cmd_kedbgcont(void);
