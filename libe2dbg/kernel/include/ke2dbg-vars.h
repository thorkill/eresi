/*
 * This file is part of the Rasta Ring 0 debug
 *
 * Copyright (C) 2004 Droids Corporation.
 *
 * $Id: ke2dbg-vars.h,v 1.1 2008-02-16 12:32:27 thor Exp $
 *
 */

#ifndef VARS_H
#define VARS_H

#include "ke2dbg-config.h"

#ifndef NULL
#define NULL 0
#endif


#define         KE2DBG_CONFIG            ".e2dbgrc"

/* We check whether an ASM style has been chosen */
#if defined(_MSC_VER) && defined(__GNUC__)
#error Your config.h is wrong : you cannot use MSC_VER *and* GNUC
#endif /* both MSC_VER and GNUC */
#if ! defined(_MSC_VER) && ! defined(__GNUC__)
#error Your config.h is wrong : you must choose either MSC_VER or GNUC
#endif /* neither MSC_VER nor GNUC */

/*
 * We check wether a video mode has been chosen. If VIDEO_FB has been
 * chosen, a value for SCREEN_ADRESSE needs to be set.
 */
#if defined VIDEO_FB && defined VIDEO_AA
#error Your config.h is wrong : you cannot use VIDEO_FB *and* VIDEO_AA
#endif /* both VIDEO_FB and VIDEO_AA */
#if ! defined VIDEO_FB && ! defined VIDEO_AA
#error Your config.h is wrong : you must choose either VIDEO_FB or VIDEO_AA
#endif /* neither VIDEO_FB nor VIDEO_AA */
#if defined VIDEO_FB && ! defined SCREEN_ADRESSE
#error Your config.h is wrong : if you want to use VIDEO_FB, you must define SCREEN_ADRESSE
#endif /* VIDEO_FB without SCREEN_ADRESSE */

/* We check that an OS has been chosen */

#if defined LINUX_26 && ! defined LINUX
#define LINUX
#warning I set LINUX macro because you have set LINUX_26
#endif /* LINUX_26 without LINUX */

#if defined FREEBSD || defined OPENBSD || defined NETBSD
#define XBSD
#endif

#if ! defined LINUX && ! defined XBSD && ! defined WIN32
#error Set your OS in config.h ! (and RTFM)
#endif /* no OS set */

#if (defined LINUX && (defined XBSD || defined WIN32)) || (defined XBSD && defined WIN32)
#error You must chose only *one* OS in config.h
#endif

#if (defined FREEBSD && (defined OPENBSD || defined NETBSD)) || (defined OPENBSD && defined NETBSD) 
#error You must chose only *one* OS in config.h
#endif

#if defined(__GNUC__)
#define _asm_ __asm__ __volatile__
#endif /* GNUC */

#ifdef LINUX

#define KERNEL_B 0xC0000000

#ifndef __KERNEL__
#define __KERNEL__
#endif /* __KERNEL__ */

#ifndef MODULE
#define MODULE
#endif /* MODULE */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>

#define Out_Debug_String(a) printk(a)
#define __declspec_naked __attribute__((cdecl))

#ifdef LINUX_26

#define fake_naked

#if defined(__GNUC__)
#define	RING_HOOO_SEGMENT "$0x7b"
//#define	RING_HOOO_SEGMENT "$0x60"
#elif defined(_MSC_VER)
#define	RING_HOOO_SEGMENT 0x7b
#endif

#define	RING_HOOO_SEGMENT_VALUE 0x7b
//#define	RING_HOOO_SEGMENT_VALUE 0x60

#else /* LINUX_24 */

#define fake_naked _asm_("\t" \
			 "add $0x08, %esp\n\t" \
			 "popl %ebp\n" \
);

#if defined(__GNUC__)
#define	RING_HOOO_SEGMENT "$0x18"
#elif defined(_MSC_VER)
#define	RING_HOOO_SEGMENT 0x18
#endif

#define	RING_HOOO_SEGMENT_VALUE 0x18

#endif /* LINUX_26 */

#endif /* LINUX */

#ifdef XBSD /* Generic stuffs for *BSD */

#if defined(__GNUC__)
#define	RING_HOOO_SEGMENT "$0x10"
#elif defined(_MSC_VER)
#define	RING_HOOO_SEGMENT 0x10
#endif

#define	RING_HOOO_SEGMENT_VALUE 0x10

#include <sys/param.h>
#include <sys/kernel.h>

#ifdef FREEBSD
#include <sys/libkern.h>
#include <sys/module.h>

#define KERNEL_B 0xC0000000
#endif /* FREEBSD */

#ifdef OPENBSD
#include <sys/systm.h>
#include <sys/exec.h>
#include <sys/conf.h>
#include <sys/lkm.h>

#define KERNEL_B 0xD0000000
#endif /* OPENBSD */

#ifdef NETBSD
#include <sys/systm.h>
#include <sys/exec.h>
#include <sys/conf.h>
#include <sys/lkm.h>

#define KERNEL_B 0xC0000000
#endif /* NETBSD */

#define __declspec_naked __attribute__((cdecl))
#define fake_naked _asm_("\t" \
			 "popl %ebp\n" \
);
#define Out_Debug_String(a) printf(a)
#endif /* XBSD */

#ifdef WIN32
#define KERNEL_B 0xC2690000

//#include <ntddk.h>

extern unsigned int DbgPrint(char* Format,...);
#define   Out_Debug_String(_X_)  \
                DbgPrint("CANCEL.SYS: ");\
                DbgPrint(_X_);







#if defined(__GNUC__)
#define RING_HOOO_SEGMENT	"$0x30"
#elif defined(_MSC_VER)
#define RING_HOOO_SEGMENT	0x30
#endif


#define RING_HOOO_SEGMENT_VALUE	0x30
/*
#include <basedef.h>
#include <vmm.h>
extern void  Out_Debug_String(char* psz);
*/
#define fake_naked
#define __declspec_naked __declspec(naked)
 
#endif /* WIN32 */


#define MAX_INT 255

#endif /* VARS_H */
