/* File to include to use the wrapper. */

#include    "gdbwrapper-stddef.h"


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
} gdbwrap_t;


gdbwrap_t        *gdbwrap_init(int fd);
void             gdbwrap_close(gdbwrap_t *desc);
void             gdbwrap_hello(gdbwrap_t *desc);
void             gdbwrap_bye(gdbwrap_t *desc);
void             gdbwrap_reason_halted(gdbwrap_t *desc);
char             *gdbwrap_own_command(char *command, gdbwrap_t *desc);
void             gdbwrap_test(gdbwrap_t *desc);
gdbwrap_gdbreg32 *gdbwrap_readgenreg(gdbwrap_t *desc);
void             gdbwrap_continue(gdbwrap_t *desc);
char             *gdbwrap_memorycontent(gdbwrap_t *desc, la32 linaddr,
					uint8_t bytes);
void             gdbwrap_writereg(ureg32 regNum, la32 val, gdbwrap_t *desc);
void             gdbwrap_signal(int signal, gdbwrap_t *desc);
void             gdbwrap_stepi(gdbwrap_t *desc);
void             gdbwrap_vmwareinit(gdbwrap_t *desc);
void             gdbwrap_writereg(ureg32 regNum, la32 val, gdbwrap_t *desc);
