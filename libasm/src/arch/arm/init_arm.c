/**
* @file libasm/src/arch/arm/init_arm.c
** @ingroup arm
*/
// TODO: file comments

#include <libasm.h>

/* TODO: this can become a vector of functions indexed by the first dimension */
/* Helper functions prototypes */
static void fill_dim_vector_type1(int inst, u_int *dim_vec);
static void fill_dim_vector_type2(int inst, u_int *dim_vec);
static void fill_dim_vector_type3(int inst, u_int *dim_vec);
static void fill_dim_vector_type4(int inst, u_int *dim_vec);

static void clear_operands(asm_instr *ins);

/* TODO: function comments */

/**
 * Fetching handler of the arm architecture.
 * @param ins
 * @param buf
 * @param len
 * @param proc
 */ 
int     asm_fetch_arm(asm_instr *ins, u_char *buf, u_int len, asm_processor *proc) 
{ 
  vector_t      *vec;
  u_int         dim[3];
  int           (*fetch)(asm_instr *, u_char *, u_int, asm_processor *); 
  
  int           converted;

  LIBASM_PROFILE_FIN();

#if __BYTE_ORDER == __LITTLE_ENDIAN

  memcpy(&converted, buf, 4);

#else
  u_char        *ptr;
  int           i;

  ptr = (u_char*) &converted;
  
  for (i = 0; i < 4; i++)
    *(ptr + i) = *(buf + 3 - i);

#endif

  ins->proc = proc;
  ins->len = 4;
  ins->ptr_instr = buf;
  ins->nb_op = 0;
  ins->type = ASM_TYPE_NONE;
  ins->flagsread = ASM_ARM_FLAG_NONE;
  ins->flagswritten = ASM_ARM_FLAG_NONE;
  clear_operands(ins);

  vec = aspect_vector_get(LIBASM_VECTOR_OPCODE_ARM);
  /* Undefined instruction is at (0,0,0) */
// XXX: define if we need ASM_ARM_ and ASM_ARM_BAD (ASM_ARM_ is being used for undefined instruction)
// The undefined instruction in the ARM arch generates an exception.
  dim[0] = (converted >> 26) & 0x00000003;
  dim[1] = 0;
  dim[2] = 0;

  switch (dim[0])
    {
    case 0x00:
      /* data processing, swap, multiply, status register manipulation */
      /* and extra load/store */
      fill_dim_vector_type1(converted,dim);
      break;
    case 0x01:
      /* load/store */
      fill_dim_vector_type2(converted,dim);
      break;
    case 0x02:
      /* load/store multiple and branch */
      fill_dim_vector_type3(converted,dim);
      break;
    case 0x03:
      /* coprocessor and exception */
      fill_dim_vector_type4(converted,dim);
      break;
    default:
      /* you shouldn't be here! */
      break;
    }
  
  fetch = aspect_vectors_select(vec, dim);

  LIBASM_PROFILE_FOUT(fetch(ins, (u_char*) &converted, len, proc));
}

/**
 * Initialize the arm processor.
 *
 */

int     asm_init_arm(asm_processor *proc)
{
  struct s_asm_proc_arm *inter;

  LIBASM_PROFILE_FIN();
  
  proc->instr_table = arm_instr_list;
  proc->resolve_immediate = asm_resolve_arm;
  proc->resolve_data = 0;
  proc->fetch = asm_fetch_arm;
  proc->display_handle = asm_arm_display_instr;
  proc->type = ASM_PROC_ARM;
  
  proc->internals = inter = malloc(sizeof (struct s_asm_proc_arm)); 

  inter->dataproc_table = arm_dataproc_table;
  inter->movpsr_table = arm_movpsr_table;
  inter->clz_table = arm_clz_table;
  inter->swi_table = arm_swi_table;
  inter->ldst_table = arm_ldst_table;
  inter->ldst_misc_table = arm_ldst_misc_table;
  inter->ldst_mult_table = arm_ldst_mult_table;
  inter->swap_table = arm_swap_table;
  inter->multiply_table = arm_multiply_table;
  inter->dsp_multiply_table = arm_dsp_multiply_table;
  inter->dsp_arith_table = arm_dsp_arith_table;
  inter->branch1_table = arm_branch1_table;
  inter->branch2_table = arm_branch2_table;
  inter->coproc_dataproc_table = arm_coproc_dataproc_table;
  inter->coproc_mov_table = arm_coproc_mov_table;
  inter->coproc_mov2_table = arm_coproc_mov2_table;
  inter->coproc_ldst_table = arm_coproc_ldst_table;

  asm_arch_register(proc, 0);

  LIBASM_PROFILE_FOUT(1);
}

eresi_Addr asm_dest_resolve_arm(eresi_Addr addr, u_int disp, u_char half)
{
  int off;

  /* Addr = PC + (SignExtend(disp) << 2) */
  off = (int) disp;
  if (off & 0x0800000)
    /* Sign extend */
    off |= 0xFF000000;
  off <<= 2;
  if (half)
    off |= 0x02;
  addr += off + 8; /* PC points to current instruction + 8 */

  return (addr);
}

// TODO: function doc
/* Helper functions */
void    fill_dim_vector_type1(int inst, u_int *dim_vec)
{
  int   sec_nibble, six_nibble, sev_nibble, eig_nibble;

  sec_nibble = MGETNIBBLE(inst,1);
  six_nibble = MGETNIBBLE(inst,5);
  sev_nibble = MGETNIBBLE(inst,6);
  eig_nibble = MGETNIBBLE(inst,7);

  if (eig_nibble == 0x0F
      || (sev_nibble == 0x03 && (six_nibble & 0x0B) == 0x00))
    {
      /* Undefined */
      /* UNPREDICTABLE prior to ARMv5 if eig_nibble == 0x0F */
      /* UNPREDICTABLE prior to ARMv4 otherwise */
      dim_vec[1] = 0x00;
      dim_vec[2] = 0x00;
    }
  else if (sev_nibble == 0x03 && (six_nibble & 0x0B) == 0x02)
    {
      /* MSR(1) */
      dim_vec[1] = 0x00;
      dim_vec[2] = 0x01;
    }
  else if ((sev_nibble & 0x0E) == 0x00
           && (sec_nibble & 0x09) == 0x09)
    {
      /* multiply, swap, extra load/store */
      if (sec_nibble == 0x09)
        {
          if (sev_nibble == 0x00)
            {
              /* multiply: 6 instructions */
              dim_vec[1] = 0x01;
              dim_vec[2] = six_nibble >> 1;
              /* if dim_vec[2] > 1, UNPREDICTABLE prior ARMv4 */
            }
          else
            {
              /* swap: 2 instructions */
              dim_vec[1] = 0x02;
              dim_vec[2] = six_nibble >> 2;
              // TODO: what if dim_vec[2] > 1?
            }
        }
      else
        {
          /* extra load/store: 6 instructions */
          /* UNPREDICTABLE prior to ARMv4 */
          /* LDRD/STRD present in E variant (>=ARMv5) excluding TExP */
          dim_vec[1] = 0x03;
          dim_vec[2] = (MGETBIT(six_nibble,0) << 2) | ((sec_nibble >> 1) & 0x03);
        }
    }
  else if (sev_nibble == 0x01
           && (six_nibble & 0x09) == 0x00
           && (sec_nibble & 0x09) != 0x09)
    {
      /* miscellaneous */
      if (MGETBIT(sec_nibble,3))
        {
          /* DSP multiply: 5 instructions */
          /* Present in E variant (>= ARMv5) */
          dim_vec[1] = 0x04;
          dim_vec[2] = (six_nibble >> 1);
          if (dim_vec[2] == 0x01)
            dim_vec[2] |= (MGETBIT(sec_nibble,1) << 2);
        }
      else
        {
          if ((sec_nibble >> 1) == 0x02)
            {
              /* DSP add/sub: 4 instructions */
              /* Present in E variant (>= ARMv5) */
              dim_vec[1] = 0x05;
              dim_vec[2] = six_nibble >> 1;
            }
          else
            {
              /* other: 6 instructions  */
              dim_vec[1] = 0x06;
              dim_vec[2] = six_nibble;
              if (sec_nibble == 0x00)
                dim_vec[2] &= 0x0B;
              dim_vec[2] += sec_nibble;
            }
        }
    }
  else
    {
      /* data processing: 16 instructions */
      dim_vec[1] = 0x07;
      dim_vec[2] = MGETNIBBLE((sev_nibble << 3) | (six_nibble >> 1),0);
    }
}

void    fill_dim_vector_type2(int inst, u_int *dim_vec)
{
  int	eig_nibble, sev_nibble, sec_nibble;

  eig_nibble = MGETNIBBLE(inst,7);
  sev_nibble = MGETNIBBLE(inst,6);
  sec_nibble = MGETNIBBLE(inst,1);

  if (eig_nibble == 0x0F)
    {
      /* UNPREDICTABLE prior to ARMv5 */
      if ((MGETNIBBLE(inst,6) & 0x0D) == 0x05
          && (MGETNIBBLE(inst,5) & 0x07) == 0x05
          && MGETNIBBLE(inst,3) == 0x0F)
        {
          /* PLD */
          dim_vec[1] = 0x02;
          dim_vec[2] = 0x00;
        }
      else
        {
          /* Undefined */
          dim_vec[0] = 0x00;
          dim_vec[1] = 0x00;
          dim_vec[2] = 0x00;
        }
    }
  else
    {
      if ((sev_nibble & 0x0E) == 0x06
          && (sec_nibble & 0x01) == 0x01)
        {
          /* Undefined (all versions) */
          dim_vec[0] = 0x00;
          dim_vec[1] = 0x00;
          dim_vec[2] = 0x00;
        }
      else
        {
          /* Load/store */
          /* divides into load and store: 8 instructions */
          dim_vec[1] = MGETBIT(inst,20);

          dim_vec[2] = MGETBIT(inst,22) << 1;
          if (!MGETBIT(inst,24) && MGETBIT(inst,21))
            {
              dim_vec[2] |= 0x1;
            }
        }
    }
}

void    fill_dim_vector_type3(int inst, u_int *dim_vec)
{
  int eig_nibble, sev_nibble;

  eig_nibble = MGETNIBBLE(inst,7);
  sev_nibble = MGETNIBBLE(inst,6);

  if (eig_nibble == 0x0F
      && (sev_nibble & 0x0E) == 0x08)
    {
      /* Undefined */
      /* UNPREDICTABLE prior to ARMv5 */
      dim_vec[0] = 0;
      dim_vec[1] = 0;
      dim_vec[2] = 0;
    }
  else
    {
      /* we could have just the second dimension, but i'll use the third to */
      /* divide into store/load multiple and branch instructions */
      dim_vec[1] = MGETBIT(inst,25);

      if (!dim_vec[1])
        {
          /* load/store multiple: 5 instructions */
          dim_vec[2] = MGETBIT(inst,22) << 2 | MGETBIT(inst,20) << 1;
          if (!dim_vec[1] && dim_vec[2] == 0x6)
            dim_vec[2] |= MGETBIT(inst,15);
        }
      else
        {
          /* branch: 3 instructions */
          if (MGETNIBBLE(inst,7) == 0x0F)
            /* BLX */
            dim_vec[2] = 0x02;
          else
            /* B, BL */
            dim_vec[2] = MGETBIT(inst,24);
        }
    }
}

void    fill_dim_vector_type4(int inst, u_int *dim_vec)
{
  int	eig_nibble, sev_nibble, six_nibble;

  eig_nibble = MGETNIBBLE(inst,7);
  sev_nibble = MGETNIBBLE(inst,6);
  six_nibble = MGETNIBBLE(inst,5);

  if (eig_nibble == 0x0F && sev_nibble == 0x0F)
    {
      /* Undefined */
      /* UNPREDICTABLE prior to ARMv5 */
      dim_vec[0] = 0;
      dim_vec[1] = 0;
      dim_vec[2] = 0;
    }
  else if (sev_nibble < 0x0E)
    {
      if (sev_nibble == 0x0C
          && (six_nibble & 0x0E) == 0x04)
        {
          /* MRRC & MCRR */
          dim_vec[1] = 0x00;
          dim_vec[2] = MGETBIT(six_nibble,0);
        }
      else
        {
          /* load/store coprocessor: 4 instructions */
          dim_vec[1] = 0x01;
          dim_vec[2] = MGETBIT(six_nibble,0);
          if (eig_nibble == 0x0F)
            dim_vec[2] += 0x02;
        }
    }
  else if (sev_nibble == 0x0E)
    {
      if (!MGETBIT(inst,4))
        {
          /* CDP, CDP2 */
          dim_vec[1] = 0x02;
          dim_vec[2] = 0x00;
          if (eig_nibble == 0x0F)
            dim_vec[2]++;
        }
      else
        {
          /* MCR, MCR2, MRC, MRC2 */
          dim_vec[1] = 0x03;
          dim_vec[2] = MGETBIT(sev_nibble,0);
          if (eig_nibble == 0x0F)
            dim_vec[2] += 0x02;
        }
    }
  else 
    {
      /* eig_nibble != 0x0F && sev_nibble == 0x0F */
      /* SWI */
      dim_vec[1] = 0x04;
      dim_vec[2] = 0x00;
    }
}

void	clear_operands(asm_instr *ins)
{
  u_int	i;

  memset(ins->op, 0, sizeof(ins->op));

  for (i = 0; i < 4; i++)
    {
      ins->op[i].destination = 0;
      ins->op[i].baser = ASM_ARM_REG_NUM;
      ins->op[i].indexr = ASM_ARM_REG_NUM;
      ins->op[i].shift_type = ASM_ARM_SHIFT_NUM;
      ins->op[i].regset = ASM_ARM_REGSET_NUM;
      ins->op[i].indexing = ASM_ARM_ADDRESSING_NUM;
    }
}
