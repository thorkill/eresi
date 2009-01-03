// TODO: file comments

#include <libasm.h>

/* TODO: this can become a vector of functions indexed by the first dimension */
/* Helper functions prototypes */
static void fill_dim_vector_type1(int inst, u_int *dim_vec);
static void fill_dim_vector_type2(int inst, u_int *dim_vec);
static void fill_dim_vector_type3(int inst, u_int *dim_vec);
static void fill_dim_vector_type4(int inst, u_int *dim_vec);

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

  /* TODO: check if endianess conversion is needed */
  /* TODO: it seems to have another way of verifying endianess */

  /*
#if __BYTE_ORDER == __LITTLE_ENDIAN
  u_char        *ptr;
  int           i;

  ptr = (u_char*) &converted;
  
  for (i = 0; i < 4; i++)
    *(ptr + i) = *(buf + 3 - i);

#else
  */
  memcpy(&converted, buf, 4);
  /*
#endif
  */

  LIBASM_PROFILE_FIN();

  ins->proc = proc;
  ins->len = 4;
  ins->ptr_instr = buf;
  ins->nb_op = 0;
  ins->type = ASM_TYPE_NONE;

  vec = aspect_vector_get(LIBASM_VECTOR_OPCODE_ARM);
  dim[0] = (converted & 0x0C000000) >> 26;
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
      /* you should't be here! */
      break;
    }
  
  fetch = aspect_vectors_select(vec, dim);

  LIBASM_PROFILE_FOUT(fetch(ins, (u_char*) &converted, len, proc));
}

/* TODO: update this function comment */

/**
 * Initialize the arm processor.
 *
 */

int     asm_init_arm(asm_processor *proc)
{
  /* TODO: check if it'll be needed to create an internal structure */
  /*  struct s_asm_proc_sparc	*inter; */

  LIBASM_PROFILE_FIN();
  
  proc->instr_table = arm_instr_list;
  /* proc->resolve_immediate = asm_resolve_arm; TODO: create this function */
  proc->resolve_data = 0;
  proc->fetch = asm_fetch_arm;
  proc->display_handle = asm_arm_display_instr;
  proc->type = ASM_PROC_ARM;
  
  /*
  proc->internals = inter = malloc(sizeof (struct s_asm_proc_sparc)); 
  
  inter->bcc_table = sparc_bcc_list;
  inter->brcc_table = sparc_brcc_list;
  inter->fbcc_table = sparc_fbcc_list;
  inter->shift_table = sparc_shift_list;
  inter->movcc_table = sparc_movcc_list;
  inter->movfcc_table = sparc_movfcc_list;
  inter->movr_table = sparc_movr_list;
  inter->fpop1_table = sparc_fpop1_list;
  inter->fmovcc_table = sparc_fmovcc_list;
  inter->fmovfcc_table = sparc_fmovfcc_list;
  inter->fmovr_table = sparc_fmovr_list;
  inter->fcmp_table = sparc_fcmp_list;
  inter->tcc_table = sparc_tcc_list; 
  inter->op2_table = sparc_op2_table;
  inter->op3_table = sparc_op3_table;
  */

  asm_arch_register(proc, 0);

  LIBASM_PROFILE_FOUT(1);
}

// TODO: function doc
/* Helper functions */
void    fill_dim_vector_type1(int inst, u_int *dim_vec)
{
  int   sec_nibble, six_nibble, sev_nibble;

  sec_nibble = MGETNIBBLE(inst,1);
  six_nibble = MGETNIBBLE(inst,5);
  sev_nibble = MGETNIBBLE(inst,6);

  if (sev_nibble == 0x03
      && (six_nibble & 0x0B) == 0x02)
    {
      /* MSR(1) */
      dim_vec[1] = 0x00;
      dim_vec[2] = 0x00;
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
            }
          else
            {
              /* swap: 2 instructions */
              dim_vec[1] = 0x02;
              dim_vec[2] = six_nibble >> 2;
            }
        }
      else
        {
          /* extra load/store: 6 instructions */
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
  int	eig_nibble;

  eig_nibble = MGETNIBBLE(inst,7);
  if (eig_nibble == 0x0F)
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
        /* TODO: undef */
      }
  else
    {
      /* divides into load and store: 8 instructions */

      dim_vec[1] = MGETBIT(inst,20);

      dim_vec[2] = MGETBIT(inst,22) << 1;
      if (!MGETBIT(inst,24) && MGETBIT(inst,21))
        {
          dim_vec[2] |= 0x1;
        }
    }
}

void    fill_dim_vector_type3(int inst, u_int *dim_vec)
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

void    fill_dim_vector_type4(int inst, u_int *dim_vec)
{
  int	eig_nibble, sev_nibble, six_nibble;

  eig_nibble = MGETNIBBLE(inst,7);
  sev_nibble = MGETNIBBLE(inst,6);
  six_nibble = MGETNIBBLE(inst,5);

  if (sev_nibble < 0x0E)
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
  else if (sev_nibble == 0x0F)
    {
      /* SWI */
      dim_vec[1] = 0x04;
      dim_vec[2] = 0x00;
    }
}
