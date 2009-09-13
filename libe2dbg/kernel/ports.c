/**
* @file libe2dbg/kernel/ports.c
 */
#include "ke2dbg.h"

/**
 * @param Port
 * @return
 */
unsigned char ke2dbg_inb (unsigned int Port)
{
  unsigned char Value;
#if defined(__GNUC__)
  __asm__("inb %w1, %0;\n\t"
	  : "=a" (Value)
	  : "d" (Port));
#endif
  return(Value);
}

/**
 * @param Port
 * @return
 */

unsigned short ke2dbg_inw (unsigned int Port)
{
  unsigned short Value;
#if defined(__GNUC__)
  __asm__("inw %w1, %0;\n\t"
	  : "=a" (Value)
	  : "d" (Port));
#endif
  return(Value);
}

/**
 * @param Port
 * @return
 */

unsigned int ke2dbg_inl (unsigned int Port)
{
  unsigned int Value;
#if defined(__GNUC__)
  __asm__("inl %w1, %0;\n\t"
	  : "=a" (Value)
	  : "d" (Port));
#endif
  return(Value);
}

/**
 * @param Value
 * @param Port
 * @return
 */
void ke2dbg_outb (unsigned char Value, unsigned int Port)
{
#if defined(__GNUC__)
  __asm__("outb %0, %w1\n\t"
	  : 
	  : "a" (Value),
	  "d" (Port));
#endif
}


/**
 * @param Value
 * @param Port
 * @return
 */
void ke2dbg_outw (unsigned short Value, unsigned int Port)
{
#if defined(__GNUC__)
  __asm__("outw %0, %w1\n\t"
	  : 
	  : "a" (Value),
	  "d" (Port));
#endif
}

/**
 * @param Value
 * @param Port
 * @return
 */
void ke2dbg_outl (unsigned int Value, unsigned int Port)
{
#if defined(__GNUC__)
  __asm__("outl %0, %w1\n\t"
	  : 
	  : "a" (Value),
	  "d" (Port));
#endif
}
