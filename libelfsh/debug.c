/**
 * @file debug.c
 * @ingroup libelfsh
 *
 * Started on  Thu Apr  3 21:36:55 2003 jfv
 *
 * $Id: debug.c,v 1.9 2008-02-16 12:32:27 thor Exp $
 *
 */
#include "libelfsh.h"

/**
 * Debug : print the section list 
 * @param obj
 * @param label
 * @return
 */
int		elfsh_print_sectlist(elfshobj_t *obj, char *label)
{
	elfshsect_t	*actual;
	unsigned int		index;
	unsigned char	*data;
	char		*sctname;

	PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

	printf(" [SCTLIST][%s]\n", label);
	for (index = 0, actual = obj->sectlist;
			actual != NULL;
			index++, actual = actual->next)
	{
		sctname = elfsh_get_section_name(obj, actual);
		if (sctname == NULL)
			sctname = "UNK";
		data = elfsh_get_raw(actual);
		if (data == (unsigned char *)NULL)
			data = (unsigned char *)"\xFF\xFF\xFF";
		printf(" [%03u:%03u] %-15s HDRNAM: %-15s BYTES[%02X %02X %02X] P(%8p) "
				"A(%8p) N(%8p) SCTIDX(%03u) HDRFOFF:%010u SZ:%010u VADDR:%08X \n",
				index, actual->index, 
				(actual->name != NULL ? actual->name : "UNK"),
				sctname,
				(unsigned int) data[0],
				(unsigned int) data[1],
				(unsigned int) data[2],
				actual->prev,
				actual,
				actual->next,
				actual->index,
				(unsigned int) actual->shdr->sh_offset,
				(unsigned int) actual->shdr->sh_size,
				(unsigned int) actual->shdr->sh_addr);
	}
	puts(" [EOL]\n");
	fflush(stdout);
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 *  Return architecture string related to e_machine 
 *  generated with
 *  cat /usr/include/elf.h | grep EM_ \
 *  awk -F\  \
 *  '{print "case "$3": return @"$2" : "$5 $6" "$7" "$8" "$9" "$20"@;" }'  \
 *  | sed s/@/\"/g | sed 's/\*\///g'
 *
 * @param machine Machine to get description string
 * @return A pointer to a static string to describe machine 
 */
char		*elfsh_get_machine_string(elfsh_Half machine)
{
  switch(machine)
    {
    case 0: return "EM_NONE : Nomachine";
    case 1: return "EM_M32 : AT&TWE 32100";
    case 2: return "EM_SPARC : SUNSPARC";
    case 3: return "EM_386 : Intel80386";
    case 4: return "EM_68K : Motorolam68k family";
    case 5: return "EM_88K : Motorolam88k family";
    case 7: return "EM_860 : Intel80860";
    case 8: return "EM_MIPS : MIPSR3000 big-endian";
    case 9: return "EM_S370 : IBMSystem/370";
    case 10: return "EM_MIPS_RS3_LE : MIPSR3000 little-endian";
    case 15: return "EM_PARISC : HPPA";
    case 17: return "EM_VPP500 : FujitsuVPP500";
    case 18: return "EM_SPARC32PLUS : Sun's\"v8plus\"";
    case 19: return "EM_960 : Intel80960";
    case 20: return "EM_PPC : PowerPC";
    case 21: return "EM_PPC64 : PowerPC64-bit";
    case 22: return "EM_S390 : IBMS390";
    case 36: return "EM_V800 : NECV800 series";
    case 37: return "EM_FR20 : FujitsuFR20";
    case 38: return "EM_RH32 : TRWRH-32";
    case 39: return "EM_RCE : MotorolaRCE";
    case 40: return "EM_ARM : ARM";
    case 41: return "EM_FAKE_ALPHA : DigitalAlpha";
    case 42: return "EM_SH : HitachiSH";
    case 43: return "EM_SPARCV9 : SPARCv9 64-bit";
    case 44: return "EM_TRICORE : SiemensTricore";
    case 45: return "EM_ARC : ArgonautRISC Core";
    case 46: return "EM_H8_300 : HitachiH8/300";
    case 47: return "EM_H8_300H : HitachiH8/300H";
    case 48: return "EM_H8S : HitachiH8S";
    case 49: return "EM_H8_500 : HitachiH8/500";
    case 50: return "EM_IA_64 : IntelMerced";
    case 51: return "EM_MIPS_X : StanfordMIPS-X";
    case 52: return "EM_COLDFIRE : MotorolaColdfire";
    case 53: return "EM_68HC12 : MotorolaM68HC12";
    case 54: return "EM_MMA : FujitsuMMA Multimedia Accelerator";
    case 55: return "EM_PCP : SiemensPCP";
    case 56: return "EM_NCPU : SonynCPU embeeded RISC";
    case 57: return "EM_NDR1 : DensoNDR1 microprocessor";
    case 58: return "EM_STARCORE : MotorolaStart*Core processor";
    case 59: return "EM_ME16 : ToyotaME16 processor";
    case 60: return "EM_ST100 : STMicroelectronicST100 processor";
    case 61: return "EM_TINYJ : AdvancedLogic Corp. Tinyj emb.fam";
    case 62: return "EM_X86_64 : AMDx86-64 architecture";
    case 63: return "EM_PDSP : SonyDSP Processor";
    case 66: return "EM_FX66 : SiemensFX66 microcontroller";
    case 67: return "EM_ST9PLUS : STMicroelectronicsST9+ 8/16 mc";
    case 68: return "EM_ST7 : STmicroelectronicsST7 8 bit mc";
    case 69: return "EM_68HC16 : MotorolaMC68HC16 microcontroller";
    case 70: return "EM_68HC11 : MotorolaMC68HC11 microcontroller";
    case 71: return "EM_68HC08 : MotorolaMC68HC08 microcontroller";
    case 72: return "EM_68HC05 : MotorolaMC68HC05 microcontroller";
    case 73: return "EM_SVX : SiliconGraphics SVx";
    case 74: return "EM_ST19 : STMicroelectronicsST19 8 bit mc";
    case 75: return "EM_VAX : DigitalVAX";
    case 76: return "EM_CRIS : AxisCommunications 32-bit embedded processor";
    case 77: return "EM_JAVELIN : InfineonTechnologies"
	       " 32-bit embedded processor";
    case 78: return "EM_FIREPATH : Element14 64-bit DSP Processor";
    case 79: return "EM_ZSP : LSILogic 16-bit DSP Processor";
    case 80: return "EM_MMIX : DonaldKnuth's educational"
	       "64-bit processor";
    case 81: return "EM_HUANY : HarvardUniversity machine-independent"
	       "object files";
    case 82: return "EM_PRISM : SiTeraPrism";
    case 83: return "EM_AVR : AtmelAVR 8-bit microcontroller";
    case 84: return "EM_FR30 : FujitsuFR30";
    case 85: return "EM_D10V : MitsubishiD10V";
    case 86: return "EM_D30V : MitsubishiD30V";
    case 87: return "EM_V850 : NECv850";
    case 88: return "EM_M32R : MitsubishiM32R";
    case 89: return "EM_MN10300 : MatsushitaMN10300";
    case 90: return "EM_MN10200 : MatsushitaMN10200";
    case 91: return "EM_PJ : picoJava";
    case 92: return "EM_OPENRISC : OpenRISC32-bit embedded processor";
    case 93: return "EM_ARC_A5 : ARCCores Tangent-A5";
    case 94: return "EM_XTENSA : TensilicaXtensa Architecture";
    default:
      return "unsupported ascii machine string";
    }
}
