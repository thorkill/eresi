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
char             *gdbwrap_own_command(char *command, gdbwrap_t *desc);
void             gdbwrap_test(gdbwrap_t *desc);
gdbwrap_gdbreg32 *gdbwrap_readgenreg(gdbwrap_t *desc);
void             gdbwrap_continue(gdbwrap_t *desc);
void             gdbwrap_setbp(la32 linaddr, void *datasaved, gdbwrap_t *desc);
void             gdbwrap_simplesetbp(la32 linaddr, gdbwrap_t *desc);
void             gdbwrap_delbp(la32 linaddr, void *datasaved, gdbwrap_t *desc);
void             gdbwrap_simpledelbp(la32 linaddr, gdbwrap_t *desc);
char             *gdbwrap_readmemory(la32 linaddr, unsigned bytes,
				     gdbwrap_t *desc);
void             *gdbwrap_writememory(la32 linaddr, void *value,
				      unsigned bytes, gdbwrap_t *desc);

void             *gdbwrap_writememory2(la32 linaddr, void *value,
				       unsigned bytes, gdbwrap_t *desc);
void             gdbwrap_writereg(ureg32 regNum, la32 val, gdbwrap_t *desc);
char             *gdbwrap_shipallreg(gdbwrap_t *desc);
void             gdbwrap_ctrl_c(gdbwrap_t *desc);
void             gdbwrap_signal(int signal, gdbwrap_t *desc);
void             gdbwrap_stepi(gdbwrap_t *desc);
void             gdbwrap_writereg(ureg32 regNum, la32 val, gdbwrap_t *desc);
void             gdbwrap_writereg2(ureg32 regNum, la32 val, gdbwrap_t *desc);
void             gdbwrap_vmwareinit(gdbwrap_t *desc);


