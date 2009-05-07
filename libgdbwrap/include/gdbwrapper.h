/* File to include to use the wrapper. */

#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include "libaspect.h"

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

typedef struct  gdbwrap_gdbARMreg32
{
  ureg32   r0;
  ureg32   r1;
  ureg32   r2;
  ureg32   r3;
  ureg32   r4;
  ureg32   r5;
  ureg32   r6;
  ureg32   r7;
  ureg32   r8;
  ureg32   r9;
  ureg32   r10; //SL reg
  ureg32   r11; //FP reg
  ureg32   r12;
  ureg32   r13_usr; //SP reg (XXX: add pointers with these names also?!)
  ureg32   r14_usr; //LR reg
  ureg32   r15; //PC reg
  ureg32   r8_fiq;
  ureg32   r9_fiq;
  ureg32   r10_fiq;
  ureg32   r11_fiq;
  ureg32   r12_fiq;
  ureg32   r13_fiq;
  ureg32   r14_fiq;
  ureg32   r13_irq;
  ureg32   r14_irq;
  ureg32   r13_svc;
  ureg32   r14_svc;
  ureg32   r13_abt;
  ureg32   r14_abt;
  ureg32   r13_und;
  ureg32   r14_und;
  ureg32   cpsr;
  ureg32   spsr_fiq;
  ureg32   spsr_irq;
  ureg32   spsr_svc;
  ureg32   spsr_abt;
  ureg32   spsr_und;
} gdbwrap_gdbARMreg32;


typedef struct gdbwrap_t
{
  char                 *packet;
  int                  fd;
  unsigned             max_packet_size;
  gdbwrap_gdbreg32     reg32;
  gdbwrap_gdbARMreg32  reg32_ARM;
  Bool                 is_active;
  Bool                 erroroccured;
  Bool                 interrupted;
  Bool                 pmode;
} gdbwrap_t;

typedef struct meminfo_t
{
  int              type;
  u_int            start;
  u_int            length;
  u_int            blocksize;
} meminfo_t;

typedef struct gdbmemap_t
{
  meminfo_t        ram;
  meminfo_t        rom;
  meminfo_t        flash;
} gdbmemap_t;


typedef struct
{
  gdbwrap_t   *gdbwrapptr;
  gdbmemap_t  *gdbmemapptr;
  Bool        is_no_ack_mode;
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
gdbwrap_gdbreg32 *gdbwrap_readgenreg(gdbwrap_t *desc);
void             gdbwrap_continue(gdbwrap_t *desc);
void             gdbwrap_setbp(gdbwrap_t *desc, la32 linaddr, void *datasaved);
void             gdbwrap_simplesetbp(gdbwrap_t *desc, la32 linaddr);
void             gdbwrap_delbp(gdbwrap_t *desc, la32 linaddr, void *datasaved);
void             gdbwrap_simpledelbp(gdbwrap_t *desc, la32 linaddr);
char             *gdbwrap_readmem(gdbwrap_t *desc, la32 linaddr, unsigned bytes);
void             gdbwrap_writemem(gdbwrap_t *desc, la32 linaddr, void *value,
				  unsigned bytes);
void             gdbwrap_writereg(gdbwrap_t *desc, ureg32 regnum, la32 val);
char             *gdbwrap_shipallreg(gdbwrap_t *desc);
void             gdbwrap_ctrl_c(gdbwrap_t *desc);
void             gdbwrap_signal(gdbwrap_t *desc, int signal);
void             gdbwrap_stepi(gdbwrap_t *desc);
char             *gdbwrap_remotecmd(gdbwrap_t *desc, char *cmd);
u_char           gdbwrap_lasterror(gdbwrap_t *desc);
gdbmemap_t       *gdbwrap_memorymap_get();
void             *gdbwrap_memorymap_read(gdbwrap_t *desc);
gdbwrap_gdbARMreg32     *gdbwrap_readgenARMreg(gdbwrap_t *desc);


