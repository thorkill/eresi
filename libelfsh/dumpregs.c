/*
** dumpregs.c for elfsh
** 
** Started on  Wed Jul 27 04:08:52 2005 ym
** 
*/
#include "libelfsh.h"


//static elfshia32regs_t regs;


struct		s_regs_ia32
{
  uint32_t	esp;
  uint32_t	eax;
  uint32_t	ebp;
  uint32_t	edi;
  uint32_t	esi;
  uint32_t	edx;
  uint32_t	ecx;
  uint32_t	ebx;
  uint32_t	es;	/* 16bits but we want to keep alignment */
  uint32_t	ds;	/* 16bits but we want to keep alignment */

}		ia32regs_t;





/* The breakpoint routine. Save registers and callback */
void	elfsh_genericbp_ia32()
{
  //ia32regs_t	breakpoint;
  
  /* Save registers value */
  /*
  __asm__ volatile (
		    "movl %%esp, 0(%esp)   \n"
		    "addl 48   , 0(%%esp)  \n"
		    "movl %%eax, 4(%%esp)  \n"
		    "movl %%ebp, 8(%%esp)  \n"
		    "movl %%edi, 12(%esp)  \n"
		    "movl %%esi, 16(%%esp) \n"
		    "movl %%edx, 20(%%esp) \n"
		    "movl %%ecx, 24(%%esp) \n"
		    "movl %%ebx, 28(%%esp) \n"
		    "movl %%es,  32(%%esp) \n"
		    "movl %%ds,  36(%%esp) \n"
		    );
  */
  /* Restore all registers */
  /*__asm__ volatile ("pop   %%ebx	\n"
		    "pop   %%ecx	\n"
		    "pop   %%edx	\n"
		    "pop   %%esi	\n"
		    "pop   %%edi	\n"
		    "pop   %%ebp	\n"
		    "pop   %%eax	\n"
		    "pop   %%ds		\n"
		    "pop   %%es		\n"
		    "pop   %%esp	\n"
		    );
  */

  /* XXX: need to trampoline on the intercepted function */
}
