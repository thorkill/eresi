#include "libe2dbg.h"

/**
 * Debugger library initialisation
 * Only useful when debugger takes control by .ctors  
 */
void		e2dbg_init(void)
{
#if __DEBUG_E2DBG__
  write(2, " [D] Calling DLSYM_INIT from e2dbg_init() !\n", 45);
#endif

  aspect_called_ctors_inc();

  e2dbg_dlsym_init();

  if (aspect_called_ctors_finished())
    e2dbg_presence_reset();

#if __DEBUG_E2DBG__
  write(2, " [D] Finished e2dbg ctors \n", 27);
#endif
}

