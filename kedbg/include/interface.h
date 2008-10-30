#include <netdb.h>
#include "revm.h"
#include "libe2dbg.h"
/* #include "gdbwrapper.h" */
int             gdbwrap_simpleconnect(char *host, int port);
void            kedbg_resetstep_ia32(void);
eresi_Addr	*kedbg_getfp_ia32(void);
void            *kedbg_bt_ia32(void *frame);
void            *kedbg_getret_ia32(void *frame);
void            *kedbg_readmema(elfshobj_t *file, eresi_Addr addr,
				void *buf, u_int size);
int             kedbg_writemem(elfshobj_t *file, eresi_Addr addr, void *data,
			       u_int size);
void            *kedbg_readmem(elfshsect_t *base);
eresi_Addr      *kedbg_getpc_ia32(void);
void            kedbg_setstep_ia32(void);
void            kedbg_set_regvars_ia32(void);
void            kedbg_get_regvars_ia32(void);
