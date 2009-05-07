#include <netdb.h>
#include "revm.h"
#include "libe2dbg.h"


/* Commands */
int             cmd_com1(void);
int             cmd_kedbgcont(void);
int             cmd_kedbg_dump_regs(void);
int             cmd_kedbgprintivt(void);

/* #include "gdbwrapper.h" */
int             gdbwrap_simpleconnect(char *host, int port);
eresi_Addr	*kedbg_getfp(void);
void            *kedbg_bt_ia32(void *frame);
void            *kedbg_getret_ia32(void *frame);
void            kedbg_setstep(void);
void            kedbg_resetstep(void);
int             kedbg_setbp(elfshobj_t *f, elfshbp_t *bp);
int             kedbg_delbp(elfshbp_t *bp);  
void            *kedbg_readmema(elfshobj_t *file, eresi_Addr addr,
				void *buf, unsigned size);
int             kedbg_writemem(elfshobj_t *file, eresi_Addr addr, void *data,
			       unsigned size);
void            *kedbg_readmem(elfshsect_t *base);
eresi_Addr      *kedbg_getpc_ia32(void);
void            kedbg_set_regvars_ia32(void);
void            kedbg_get_regvars_ia32(void);
void            kedbg_print_reg(void);
void            kedbg_sigint(int sig);
void            kedbg_continue(void);
void            kedbg_shipallreg(void);
int             kedbg_writereg(ureg32 regNum, la32 val);
Bool            kedbg_isrealmode(void);
eresi_Addr      kedbg_realmode_memalloc(u_int sizem, u_int prot);

eresi_Addr      *kedbg_getpc_ARM(void);
void            kedbg_print_ARMreg(void);
void            kedbg_set_regvars_ARM(void);
void            kedbg_get_regvars_ARM(void);

/* Miscellaneous functions. */
char            *kedbg_getstr(void *addr, char *buf, unsigned maxsize);


/* Linkmap related functions */
elfshlinkmap_t  *kedbg_linkmap_getaddr(void);

