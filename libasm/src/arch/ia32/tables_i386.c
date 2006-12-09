/*
** private - do not distribute
** 
** Author  : <sk at devhell dot org>
** Started : Mon Jun 10 22:42:50 2002
** Updated : Tue Feb 25 00:22:06 2003
*/
#include <libasm.h>

int init_instr_table(asm_processor *proc) {
  
  proc->instr_table = malloc(sizeof(char *) * ASM_BAD + 1);
  
  proc->instr_table[ASM_ADC] = "adc";
  proc->instr_table[ASM_ADD] = "add";
  proc->instr_table[ASM_AND] = "and";
  proc->instr_table[ASM_AAM] = "aam";
  proc->instr_table[ASM_AAD] = "aad";
  proc->instr_table[ASM_AAA] = "aaa";
  proc->instr_table[ASM_AAS] = "aas";
  proc->instr_table[ASM_ARPL] = "arpl";
  
  proc->instr_table[ASM_BOUND] = "bound";
  
  proc->instr_table[ASM_CMPSB] = "cmpsb";
  proc->instr_table[ASM_CMPSD] = "cmpsd";
  proc->instr_table[ASM_CALL] = "call";
  proc->instr_table[ASM_CMP] = "cmp";
  proc->instr_table[ASM_CWTL] = "cwtl";
  proc->instr_table[ASM_CWTD] = "cwtd";
  proc->instr_table[ASM_CBTW] = "cbtw";
  proc->instr_table[ASM_CLTD] = "cltd";
  proc->instr_table[ASM_CPUID] = "cpuid";
  proc->instr_table[ASM_CLD] = "cld";
  proc->instr_table[ASM_CLI] = "cli";
  proc->instr_table[ASM_CLC] = "clc";

  proc->instr_table[ASM_DAA] = "daa";
  proc->instr_table[ASM_DAS] = "das";
  proc->instr_table[ASM_DIV] = "div";
  proc->instr_table[ASM_DEC] = "dec";
  
  proc->instr_table[ASM_ENTER] = "enter";
  
  proc->instr_table[ASM_FWAIT] = "fwait";
  
  proc->instr_table[ASM_HLT] = "hlt";

  proc->instr_table[ASM_INTO] = "into";
  proc->instr_table[ASM_IRET] = "iret";
  proc->instr_table[ASM_INT] = "int";
  proc->instr_table[ASM_INT3] = "int3";
  proc->instr_table[ASM_INT1] = "int1";
  proc->instr_table[ASM_IN] = "in";
  proc->instr_table[ASM_INC] = "inc";
  proc->instr_table[ASM_IMUL] = "imul";
  proc->instr_table[ASM_IDIV] = "idiv";

  proc->instr_table[ASM_BRANCH] = "jmp";
  proc->instr_table[ASM_BRANCH_U_LESS] = "jb";
  proc->instr_table[ASM_BRANCH_U_LESS_EQUAL] = "jbe";
  proc->instr_table[ASM_BRANCH_S_LESS] = "jl";
  proc->instr_table[ASM_BRANCH_S_LESS_EQUAL] = "jle";
  proc->instr_table[ASM_BRANCH_U_GREATER] = "ja";
  proc->instr_table[ASM_BRANCH_U_GREATER_EQUAL] = "jae";
  proc->instr_table[ASM_BRANCH_S_GREATER] = "jg";
  proc->instr_table[ASM_BRANCH_S_GREATER_EQUAL] = "jge";
  proc->instr_table[ASM_BRANCH_EQUAL] = "je";
  proc->instr_table[ASM_BRANCH_NOT_EQUAL] = "jne";
  proc->instr_table[ASM_BRANCH_PARITY] = "jp";
  proc->instr_table[ASM_BRANCH_NOT_PARITY] = "jnp";
  proc->instr_table[ASM_BRANCH_OVERFLOW] = "jo";
  proc->instr_table[ASM_BRANCH_NOT_OVERFLOW] = "jno";
  proc->instr_table[ASM_BRANCH_SIGNED] = "js";
  proc->instr_table[ASM_BRANCH_NOT_SIGNED] = "jns";
  proc->instr_table[ASM_BRANCH_CXZ] = "jecxz";

  proc->instr_table[ASM_LEA] = "lea";
  proc->instr_table[ASM_LOAD] = "ld";
  proc->instr_table[ASM_LOCK] = "lock";
  proc->instr_table[ASM_LOOP] = "loop";
  proc->instr_table[ASM_LOOPE] = "loope";
  proc->instr_table[ASM_LOOPNE] = "loopne";
  proc->instr_table[ASM_LEAVE] = "leave";
  proc->instr_table[ASM_LAHF] = "lahf";
  proc->instr_table[ASM_LODSB] = "lodsb";
  proc->instr_table[ASM_LODSD] = "lodsd";
  proc->instr_table[ASM_LES] = "les";
  proc->instr_table[ASM_LDS] = "lds";
  proc->instr_table[ASM_LBRANCH] = "ljmp";
  
  proc->instr_table[ASM_MOVSB] = "movsb";
  proc->instr_table[ASM_MOVSW] = "movsw";
  proc->instr_table[ASM_MOVSD] = "movsl";
  proc->instr_table[ASM_MOVSBL] = "movsbl";
  proc->instr_table[ASM_MOVSBW] = "movsbw";
  proc->instr_table[ASM_MOVSWL] = "movswl";
  proc->instr_table[ASM_MOVZWL] = "movzwl";
  proc->instr_table[ASM_MOVZBW] = "movzbw";
  proc->instr_table[ASM_MOVZBL] = "movzbl";
  proc->instr_table[ASM_MUL] = "mull";
  proc->instr_table[ASM_MOV] = "mov";

  proc->instr_table[ASM_NEG] = "neg";
  proc->instr_table[ASM_NOT] = "not";
  proc->instr_table[ASM_NOP] = "nop";

  proc->instr_table[ASM_OR] = "or";
  proc->instr_table[ASM_ORB] = "orb";
  proc->instr_table[ASM_OUT] = "out";
  proc->instr_table[ASM_OUTSB] = "outsb";
  
  proc->instr_table[ASM_PUSH] = "push";
  proc->instr_table[ASM_POP] = "pop";
  proc->instr_table[ASM_PUSHF] = "pushf";
  proc->instr_table[ASM_POPF] = "popf";
  proc->instr_table[ASM_PUSHA] = "pusha";
  proc->instr_table[ASM_POPA] = "popa";

  proc->instr_table[ASM_RET] = "ret";
  proc->instr_table[ASM_REPNZ] = "repnz";
  proc->instr_table[ASM_REPZ] = "repz";
  proc->instr_table[ASM_ROL] = "rol";
  proc->instr_table[ASM_ROR] = "ror";
  proc->instr_table[ASM_RCR] = "rcr";
  proc->instr_table[ASM_RETF] = "retf";

  proc->instr_table[ASM_STORE] = "st";
  proc->instr_table[ASM_STI] = "sti";
  proc->instr_table[ASM_SUB] = "sub";
  proc->instr_table[ASM_SBB] = "sbb";
  proc->instr_table[ASM_SCASB] = "scas";
  proc->instr_table[ASM_SCASD] = "scasd";
  proc->instr_table[ASM_STOSB] = "stosb";
  proc->instr_table[ASM_STOSD] = "stos";
  proc->instr_table[ASM_SHR] = "shr";
  proc->instr_table[ASM_SAHF] = "sahf";
  proc->instr_table[ASM_SHIFT] = "shift";
  proc->instr_table[ASM_SAR] = "sar";
  proc->instr_table[ASM_STD] = "std";
  proc->instr_table[ASM_SHL] = "shl";
  proc->instr_table[ASM_SHRD] = "shrd";
  proc->instr_table[ASM_STC] = "stc";

  proc->instr_table[ASM_TEST] = "test";

  proc->instr_table[ASM_XOR] = "xor";
  proc->instr_table[ASM_XCHG] = "xchg";
  proc->instr_table[ASM_XLATB] = "xlatb";
  
  /* i386 */
  
  proc->instr_table[ASM_SET_U_LESS] = "setb";
  proc->instr_table[ASM_CMPXCHG] = "cmpxchg";
  proc->instr_table[ASM_RDTSC] = "rdtsc";
  proc->instr_table[ASM_BTR] = "btr";
  proc->instr_table[ASM_SET_U_LESS_EQUAL] = "setbe";
  proc->instr_table[ASM_SET_S_LESS] = "setl";
  proc->instr_table[ASM_SET_S_LESS_EQUAL] = "setle";
  proc->instr_table[ASM_SET_U_GREATER] = "seta";
  proc->instr_table[ASM_SET_U_GREATER_EQUAL] = "setae";
  proc->instr_table[ASM_SET_S_GREATER] = "setg";
  proc->instr_table[ASM_SET_S_GREATER_EQUAL] = "setge";
  proc->instr_table[ASM_SET_SIGNED] = "sets";
  proc->instr_table[ASM_SET_NOT_SIGNED] = "setns";
  proc->instr_table[ASM_SET_PARITY] = "setp";
  proc->instr_table[ASM_SET_NOT_PARITY] = "setnp";
  proc->instr_table[ASM_SET_OVERFLOW] = "seto";
  proc->instr_table[ASM_SET_NOT_OVERFLOW] = "setno";
  proc->instr_table[ASM_SET_EQUAL] = "sete";
  proc->instr_table[ASM_SET_NOT_EQUAL] = "setne";  
  proc->instr_table[ASM_BT] = "bt";
  proc->instr_table[ASM_BTS] = "bts";
  proc->instr_table[ASM_SHLD] = "shld";
  proc->instr_table[ASM_CMC] = "cmc";
  proc->instr_table[ASM_BSR] = "bsr";
  proc->instr_table[ASM_CMOVNE] = "cmovne";
  proc->instr_table[ASM_CMOVA] = "cmova";
  proc->instr_table[ASM_CMOVAE] = "cmovae";
  proc->instr_table[ASM_CMOVE] = "cmove";
  proc->instr_table[ASM_CMOVO] = "cmovo";
  proc->instr_table[ASM_CMOVNO] = "cmovno";
  proc->instr_table[ASM_CMOVB] = "cmovb";
  proc->instr_table[ASM_CMOVBE] = "cmovbe";
  proc->instr_table[ASM_CMOVS] = "cmovs";
  proc->instr_table[ASM_CMOVNS] = "cmovns";
  proc->instr_table[ASM_CMOVP] = "cmovp";
  proc->instr_table[ASM_CMOVNP] = "cmovnp";
  proc->instr_table[ASM_CMOVL] = "cmovl";
  proc->instr_table[ASM_CMOVNL] = "cmovnl";
  proc->instr_table[ASM_CMOVLE] = "cmovle";
  proc->instr_table[ASM_CMOVNLE] = "cmovnle";
  proc->instr_table[ASM_LSS] = "lss";
  proc->instr_table[ASM_UD2A] = "ud2a";
  proc->instr_table[ASM_LGDT] = "lgdt";
  proc->instr_table[ASM_LIDT] = "lidt";
  proc->instr_table[ASM_SGDT] = "sgdt";
  proc->instr_table[ASM_SIDT] = "sidt";
  proc->instr_table[ASM_STR] = "str";
  proc->instr_table[ASM_LTR] = "ltr";
  proc->instr_table[ASM_LLDT] = "lldt";
  proc->instr_table[ASM_STMXCSR] = "stmxcsr";
  proc->instr_table[ASM_LDMXCSR] = "ldmxcsr";
  
  /* FPU */
  proc->instr_table[ASM_FILD] = "fild";
  proc->instr_table[ASM_FIMUL] = "fimul";
  proc->instr_table[ASM_FSTP] = "fstp";
  proc->instr_table[ASM_FCOM] = "fcom";
  proc->instr_table[ASM_FCOMP] = "fcomp";
  proc->instr_table[ASM_FCOMPP] = "fcompp";
  proc->instr_table[ASM_FLD] = "fldt";
  proc->instr_table[ASM_FLDZ] = "fldz";
  proc->instr_table[ASM_FADD] = "fadd";
  proc->instr_table[ASM_FIADD] = "fiadd";
  proc->instr_table[ASM_FABS] = "fabs";
  proc->instr_table[ASM_FADDP] = "faddp";
  proc->instr_table[ASM_FLD] = "fld";
  proc->instr_table[ASM_FCOM] = "fcom";
  proc->instr_table[ASM_FDIV] = "fdiv";
  proc->instr_table[ASM_FDIVR] = "fdivr";
  proc->instr_table[ASM_FDIVP] = "fdivp";
  proc->instr_table[ASM_FDIVRP] = "fdivrp";
  proc->instr_table[ASM_FXCH] = "fxch";
  proc->instr_table[ASM_FINIT] = "finit";
  proc->instr_table[ASM_FICOMP] = "ficomp";
  proc->instr_table[ASM_FIDIV] = "fidiv";
  proc->instr_table[ASM_FIDIVR] = "fidivr";
  proc->instr_table[ASM_FLD1] = "fld1";
  proc->instr_table[ASM_FLDCW] = "fldcw";
  proc->instr_table[ASM_FCHS] = "fchs";
  proc->instr_table[ASM_FST] = "fst";
  proc->instr_table[ASM_FISUB] = "fisub";
  proc->instr_table[ASM_FISUBR] = "fisubr";
  proc->instr_table[ASM_FIST] = "fist";
  proc->instr_table[ASM_FISTP] = "fistp";
  proc->instr_table[ASM_FMUL] = "fmul";
  proc->instr_table[ASM_FMULP] = "fmulp";
  proc->instr_table[ASM_FSUB] = "fsub";
  proc->instr_table[ASM_FSUBR] = "fsubr";
  proc->instr_table[ASM_FSUBP] = "fsubp";
  proc->instr_table[ASM_FSUBRP] = "fsubrp";
  proc->instr_table[ASM_FUCOM] = "fucom";
  proc->instr_table[ASM_FUCOMP] = "fucomp";
  proc->instr_table[ASM_FUCOMPP] = "fucompp";
  proc->instr_table[ASM_FNSTSW] = "fnstsw";
  proc->instr_table[ASM_FPREM] = "fprem";
  proc->instr_table[ASM_FSCALE] = "fscale";
  proc->instr_table[ASM_FSQRT] = "fsqrt";
  proc->instr_table[ASM_FNSTCW] = "fnstcw";
  proc->instr_table[ASM_FRNDINT] = "frndint";
  proc->instr_table[ASM_FPATAN] = "fpatan";
  proc->instr_table[ASM_BAD] = "(bad)";
  return (1);
}
