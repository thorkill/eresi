/*
** pax.c for elfsh
** 
** Started on  Tue Feb 11 20:52:36 2003 mayhem
** Last update Tue Feb 18 06:17:27 2003 mayhem
*/
#include "libelfsh.h"


/* Change the ELF header flags field */
int		elfsh_set_paxflags(Elf32_Ehdr *hdr, Elf32_Word flags)
{
  u_short	*pax_flags;

  if (!hdr)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter", -1);
  pax_flags   = (u_short *) ((char *) hdr->e_ident + EI_PAX);
  *pax_flags = flags;
  return (0);
}


/* Return the ELF header flags field */
Elf32_Word	elfsh_get_paxflags(Elf32_Ehdr *hdr)
{
  u_short	*pax_flags;

  if (!hdr)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter", -1);
  pax_flags   = (u_short *) ((char *) hdr->e_ident + EI_PAX);
  return (*pax_flags);
}


/* Change the section header table file offset */
char		elfsh_set_pax_pageexec(Elf32_Ehdr *hdr, u_short off)
{
  u_short	*pax_flags;

  if (!hdr)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter", -1);
  pax_flags   = (u_short *) ((char *) hdr->e_ident + EI_PAX);
  if (off)
    *pax_flags |= ELFSH_PAX_PAGEEXEC;
  else
    *pax_flags &= (~ELFSH_PAX_PAGEEXEC);
  return (0);
}

/* Return the section header table file offset */
char		elfsh_get_pax_pageexec(Elf32_Ehdr *hdr)
{
  u_short	*pax_flags;

  if (!hdr)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter", -1);
  pax_flags = (u_short *) ((char *) hdr->e_ident + EI_PAX);
  return ((*pax_flags & ELFSH_PAX_PAGEEXEC) && 
	  !(*pax_flags & ELFSH_PAX_SEGMEXEC));
}



/* Change the section header table file offset */
char		elfsh_set_pax_emultramp(Elf32_Ehdr *hdr, u_short off)
{
  u_short	*pax_flags;

  if (!hdr)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter", -1);
  pax_flags = (u_short *) ((char *) hdr->e_ident + EI_PAX);
  if (off)
    *pax_flags |= ELFSH_PAX_EMULTRAMP;
  else
    *pax_flags &= (~ELFSH_PAX_EMULTRAMP);
  return (0);
}

/* Return the section header table file offset */
char		elfsh_get_pax_emultramp(Elf32_Ehdr *hdr)
{
  u_short	*pax_flags;

  if (!hdr)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter", -1);
  pax_flags   = (u_short *) ((char *) hdr->e_ident + EI_PAX);
  return ((*pax_flags & ELFSH_PAX_EMULTRAMP));
}

/* Change the section header table file offset */
char		elfsh_set_pax_mprotect(Elf32_Ehdr *hdr, u_short off)
{
  u_short	*pax_flags;

  if (!hdr)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter", -1);
  pax_flags   = (u_short *) ((char *) hdr->e_ident + EI_PAX);
  if (!off)
    *pax_flags |= ELFSH_PAX_MPROTECT;
  else
    *pax_flags &= (~ELFSH_PAX_MPROTECT);
  return (0);
}

/* Return the section header table file offset */
char		elfsh_get_pax_mprotect(Elf32_Ehdr *hdr)
{
  u_short	*pax_flags;

  if (!hdr)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter", -1);
  pax_flags   = (u_short *) ((char *) hdr->e_ident + EI_PAX);
  return (!(*pax_flags & ELFSH_PAX_MPROTECT));
}


/* Change the section header table file offset */
char		elfsh_set_pax_randmmap(Elf32_Ehdr *hdr, u_short off)
{
  u_short	*pax_flags;

  if (!hdr)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter", -1);
  pax_flags   = (u_short *) ((char *) hdr->e_ident + EI_PAX);
  if (!off)
    *pax_flags |= ELFSH_PAX_RANDMMAP;
  else
    *pax_flags &= (~ELFSH_PAX_RANDMMAP);
  return (0);
}

/* Return the section header table file offset */
char		elfsh_get_pax_randmmap(Elf32_Ehdr *hdr)
{
  u_short	*pax_flags;

  if (!hdr)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter", -1);
  pax_flags   = (u_short *) ((char *) hdr->e_ident + EI_PAX);
  return (!(*pax_flags & ELFSH_PAX_RANDMMAP));
}


/* Change the section header table file offset */
char		elfsh_set_pax_randexec(Elf32_Ehdr *hdr, u_short off)
{
  u_short	*pax_flags;

  if (!hdr)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter", -1);
  pax_flags   = (u_short *) ((char *) hdr->e_ident + EI_PAX);
  if (off)
    *pax_flags |= ELFSH_PAX_RANDEXEC;
  else
    *pax_flags &= (~ELFSH_PAX_RANDEXEC);
  return (0);
}

/* Return the section header table file offset */
char		elfsh_get_pax_randexec(Elf32_Ehdr *hdr)
{
  u_short	*pax_flags;

  if (!hdr)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter", -1);
  pax_flags   = (u_short *) ((char *) hdr->e_ident + EI_PAX);
  return ((*pax_flags & ELFSH_PAX_RANDEXEC));
}


/* Change the section header table file offset */
char		elfsh_set_pax_segmexec(Elf32_Ehdr *hdr, u_short off)
{
  u_short	*pax_flags;

  if (!hdr)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter", -1);
  pax_flags   = (u_short *) ((char *) hdr->e_ident + EI_PAX);
  if (!off)
    *pax_flags |= ELFSH_PAX_SEGMEXEC;
  else
    *pax_flags &= (~ELFSH_PAX_SEGMEXEC);
  return (0);
}

/* Return the section header table file offset */
char		elfsh_get_pax_segmexec(Elf32_Ehdr *hdr)
{
  u_short	*pax_flags;

  if (!hdr)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter", -1);
  pax_flags   = (u_short *) ((char *) hdr->e_ident + EI_PAX);
  return (!(*pax_flags & ELFSH_PAX_SEGMEXEC));
}
