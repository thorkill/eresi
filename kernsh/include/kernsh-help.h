/*
** kernsh-help.h for kernsh
**
** $Id: kernsh-help.h,v 1.5 2007-09-02 21:47:25 pouik Exp $
**
*/
#ifndef __KERNSH_HELP_H_
 #define __KERNSH_HELP_H_

/* Commands */
#define	HLP_OPENMEM		"open kernel's memory"
#define HLP_CLOSEMEM            "close kernel's memory"
#define HLP_KMODE		"switch mode : static, dynamic"
#define HLP_SCT		        "display the syscalltable"
#define HLP_IDT			"display the idt"
#define HLP_GDT			"display the gdt"
#define HLP_KALLOC		"alloc contiguous kernel memory"
#define HLP_KFREE		"free contiguous kernel memory"
#define HLP_KALLOCNC		"alloc non contiguous kernel memory"
#define HLP_KFREENC		"free non contiguous kernel memory"
#define HLP_KSYM		"search kernel symbol"
#define HLP_KMODULE		"handle kernel module"
#define HLP_AUTOTYPES		"autotypes"
#define HLP_KMD5		"Make md5 at a specific addr\n\n"\
				"kmd5 parameter\n\n"\
				"Parameter format :sa output, sa:rva output, sa%rva%size output, sa%size output\n"\
				"- sa : Symbol or addr\n"\
				"- rva : Byte offset form the beginning (optional)\n"\
				"size : Bytes number limit (optional)"\
				"- If size is 0 or not specified, we search the end of the function\n"\
				"- output : file output (optional)\n\n"
				
#define HLP_KCMD5		"Check md5\n\n"\
				"Parameter format : format, file\n"\
				"- format : addr:mode:size:off:md5\n"\
				"- file : filename\n\n"

#endif
