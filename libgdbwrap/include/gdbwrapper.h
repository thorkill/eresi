/* File to include to use the wrapper. */

#include <sys/socket.h>
#include <netdb.h>
#include "revm.h"


typedef struct  gdbwrap_gdbreg32
{
  ureg32   eax;
  ureg32   ecx;
  ureg32   edx;
  ureg32   ebx;
  ureg32   esp;
  ureg32   ebp;
  ureg32   esi;
  ureg32   edi;
  ureg32   eip;
  ureg32   eflags;
  ureg32   cs;
  ureg32   ss;
  ureg32   ds;
  ureg32   es;
  ureg32   fs;
  ureg32   gs;
} gdbwrap_gdbreg32;


typedef struct gdbwrap_t
{
  char             *packet;
  int              fd;
  unsigned         max_packet_size;
  gdbwrap_gdbreg32 reg32;
  Bool             is_active;
  Bool             erroroccured;
  Bool             interrupted;
} gdbwrap_t;


typedef struct
{
  gdbwrap_t *gdbwrapptr;
} gdbwrapworld_t;

Bool             gdbwrap_erroroccured(gdbwrap_t *desc);
Bool             gdbwrap_cmdnotsup(gdbwrap_t *desc);
unsigned         gdbwrap_atoh(const char * str, unsigned size);
unsigned         gdbwrap_lastsignal(gdbwrap_t *desc);
Bool             gdbwrap_is_active(gdbwrap_t *desc);
gdbwrapworld_t   gdbwrap_current_set(gdbwrap_t *world);
gdbwrap_t        *gdbwrap_current_get(void);
gdbwrap_t        *gdbwrap_init(int fd);
void             gdbwrap_close(gdbwrap_t *desc);
void             gdbwrap_hello(gdbwrap_t *desc);
void             gdbwrap_bye(gdbwrap_t *desc);
void             gdbwrap_reason_halted(gdbwrap_t *desc);
char             *gdbwrap_own_command(gdbwrap_t *desc, char *command);
void             gdbwrap_test(gdbwrap_t *desc);
gdbwrap_gdbreg32 *gdbwrap_readgenreg(gdbwrap_t *desc);
void             gdbwrap_continue(gdbwrap_t *desc);
void             gdbwrap_setbp(gdbwrap_t *desc, la32 linaddr, void *datasaved);
void             gdbwrap_simplesetbp(gdbwrap_t *desc, la32 linaddr);
void             gdbwrap_delbp(gdbwrap_t *desc, la32 linaddr, void *datasaved);
void             gdbwrap_simpledelbp(gdbwrap_t *desc, la32 linaddr);
char             *gdbwrap_readmemory(gdbwrap_t *desc, la32 linaddr, unsigned bytes);
void             *gdbwrap_writememory(gdbwrap_t *desc, la32 linaddr, void *value,
				      unsigned bytes);
void             *gdbwrap_writememory2(gdbwrap_t *desc, la32 linaddr, void *value,
				       unsigned bytes);
void             gdbwrap_writereg(gdbwrap_t *desc, ureg32 regNum, la32 val);
char             *gdbwrap_shipallreg(gdbwrap_t *desc);
void             gdbwrap_ctrl_c(gdbwrap_t *desc);
void             gdbwrap_signal(gdbwrap_t *desc, int signal);
void             gdbwrap_stepi(gdbwrap_t *desc);
void             gdbwrap_writereg(gdbwrap_t *desc, ureg32 regNum, la32 val);
void             gdbwrap_writereg2(gdbwrap_t *desc, ureg32 regNum, la32 val);
char             *gdbwrap_remotecmd(gdbwrap_t *desc, char *cmd);


