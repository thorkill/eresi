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


void             gdbwrap_hello(int fd);
void             gdbwrap_bye(void);
gdbwrap_gdbreg32 *gdbwrap_reason_halted(void);
char             *gdbwrap_own_command(const char * command);
void             gdbwrap_test(void);

gdbwrap_gdbreg32 gdbwrap_reg32;
