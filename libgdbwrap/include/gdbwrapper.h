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
} gdbwrap_t;

gdbwrap_t        *gdbwrap_init(int fd);
void             gdbwrap_close(gdbwrap_t *desc);
void             gdbwrap_hello(gdbwrap_t *desc);
void             gdbwrap_bye(gdbwrap_t *desc);
void             gdbwrap_reason_halted(gdbwrap_t *desc);
char             *gdbwrap_own_command(const char * command, gdbwrap_t *desc);
void             gdbwrap_test(gdbwrap_t *desc);



