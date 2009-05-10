#include "libe2dbg.h"
#include "libasm-int.h"
#include "gdbwrapper.h"
#include "interface.h"

#define  ERROR_CONNECTION  "problem when connecting... exiting\n"
#define  USAGE             "Usage: ./netgdb <[server]:port> <file>\n"
#define  KEDBGNAME         "kedbg"
/* The commands are for internal debuging purpose. */
#define  COM1              "youpla"
#define  HELPCOM1          "Not much to say..."

#define  CMD_IVT           "ivt"
#define  CMD_HOOKIVT       "ivthook"

#define  HLP_IVT           "Displays the IVT of the remote VM"
#define  HLP_HOOKIVT       "Adds breakpoints to all addresses defined in the IVT"

/* State definitions */
#define KEDBG_USERLAND     0
#define KEDBG_VM           1
#define KEDBG_EMBEDDED     2

/* Various defined values */
#define  KERBASEADDR            0xc0000000
#define  BPCODE                 0xcc
#define  MEMINJECT              0x500
#define	 CR0STR		            "637230"
#define  MONITOREMUL            "r cr0"
#define  MONITORSTEP            "step"
#define  MONITORBREAK           "break"
#define  MONITORHALT            "halt"
#define  MONITORRESUME          "resume"
#define  MONITORRESETHALT       "reset halt"
#define  MONITORREG             "reg"
#define  CMD_MONITOR            "monitor"
#define  CMD_MONITOR_RESETHALT  "reset_halt"
#define  HLP_MONITOR            "Runs remote monitor commands on GDB servers."

/* Debug flags */
#define __DEBUG_KEDBG__	   FALSE

#if __DEBUG_KEDBG__
#define DEBUGMSG(_command)			\
do						\
{						\
  _command;					\
} while(0)
#else
#define DEBUGMSG(_command)
#endif

typedef struct
{
  /* Offset for the bp (see cmd_kedbgcont). */
  u_char offset;
  Bool   interrupted;
  u_char state;
  Bool   pmode;
  gdbmemap_t   *gdbmemap;
} kedbgworld_t;

extern          kedbgworld_t kedbgworld;
void            kedbg_register_command(void);
void            kedbg_register_vector(void);
int             cmd_linkmap(void);
int             e2dbg_linkmap_load(char *name);
int             cmd_com1(void);
int             cmd_kedbgcont(void);
int             cmd_kedbgstep(void);
int             cmd_kedbgquit(void);
int             cmd_kedbgdisasm(void);
int             cmd_kedbghookivt(void);
int             cmd_kedbgitrace(void);
int             cmd_kedbgproc(void);
int             cmd_kedbggraph(void);
int             cmd_kedbgmonitor(void);
