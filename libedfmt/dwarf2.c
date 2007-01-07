/*
** dwarf2.c for libedfmt (The Elf debug format library of ELFsh)
**
** Started Dec 26 2006 10:49:45 mxatone
**
*/

#include "libedfmt.h"

edfmtdw2info_t debug_info;
edfmtdw2sectlist_t dw2_sections;

hash_t abbrev_table;

/* References */
edfmtdw2fref_t *ref_global = NULL;
edfmtdw2fref_t *ref_cu = NULL;

hash_t ref_global_table;
hash_t ref_cu_table;

edfmtdw2cu_t *current_cu = NULL;

#define a_pos(name) (dw2_sections.name.data + dw2_sections.name.pos)
#define ac_pos(name) (char *) a_pos(name)

#define c_pos(val, name, type) val = *(type *) a_pos(name)
#define inc_pos(name, value) do { dw2_sections.name.pos += value; } while(0)
#define ci_pos(val, name, type) do { c_pos(val, name, type); inc_pos(name, sizeof(type)); } while(0)
#define c_read_1(val, name) do { c_pos(val, name, char); } while(0)
#define c_read_2(val, name) do { c_pos(val, name, short); } while(0)
#define c_read_4(val, name) do { c_pos(val, name, int); } while(0)
#define c_read_8(val, name) do { c_pos(val, name, long int); } while(0)
#define ci_read_1(val, name) do { c_read_1(val, name); inc_pos(name, 1); } while(0)
#define ci_read_2(val, name) do { c_read_2(val, name); inc_pos(name, 2); } while(0)
#define ci_read_4(val, name) do { c_read_4(val, name); inc_pos(name, 4); } while(0)
#define ci_read_8(val, name) do { c_read_8(val, name); inc_pos(name, 8); } while(0)

static int	edfmt_dwarf2_updatefref(u_char global)
{
  edfmtdw2fref_t *ref, *tmp, *prev, *base;
  hash_t	*ht;
  char		ckey[DW2_CKEY_SIZE];
  edfmtdw2abbent_t *ent;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  ht = global ? &ref_global_table : &ref_cu_table;

  /* For each elements in the ref list */
  for (ref = (global ? ref_global : ref_cu); ref != NULL; ref = ref->next)
    {
      edfmt_dwarf2_ckey(ckey, DW2_CKEY_SIZE - 1, ref->offset);
      ent = (edfmtdw2abbent_t *) hash_get(ht, ckey);

      if (ent == NULL || ref->attr == NULL)
	continue;

      /* Set the correct reference */
      ref->attr->u.ref = ent;
    }

  /* Clean */
  base = global ? ref_global : ref_cu;

  if (base)
    {
      /* Only one element */
      if (base->next == NULL)
	{
	  XFREE(base);
	}
      else
	{
	  for (tmp = base, prev = NULL; tmp != NULL; prev = tmp, tmp = tmp->next)
	    {
	      if (prev != NULL)
		XFREE(prev);
	    }
	}

      if (global)
	ref_global = NULL;
      else
	ref_cu = NULL;
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Create a new reference that wait for resolution after compil unit or global parsing */
static int	edfmt_dwarf2_newfref(long int offset, edfmtdw2abbattr_t *attr, u_char global)
{
  edfmtdw2fref_t *tmp;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (attr == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Invalid parameters", -1);  

  XALLOC(tmp, sizeof(edfmtdw2fref_t), -1);

  tmp->offset = offset;
  tmp->attr = attr;

  if (global)
    {
      tmp->next = ref_global;
      ref_global = tmp;
    }
  else
    {
      tmp->next = ref_cu;
      ref_cu = tmp;
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  
}

/* Create a string from a key (used in hash table) */
char 		*edfmt_dwarf2_ckey(char *buf, u_int size, long int key)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (buf == NULL || size == 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", NULL);

  snprintf(buf, size, "%ld", key);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Create a string from a file + line (used in hash table) */
char 		*edfmt_dwarf2_cline(char *buf, u_int size, u_int line, char *file)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (buf == NULL || size == 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", NULL);

  snprintf(buf, size, "%s:%d", file, line);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Create a string from an addr (used in hash table) */
char 		*edfmt_dwarf2_caddr(char *buf, u_int size, elfsh_Addr addr)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (buf == NULL || size == 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", NULL);

  snprintf(buf, size, "%x", addr);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

static char *
dwarf_lne_name (u_char ext_opc)
{
  switch(ext_opc)
    {
    case DW_LNE_end_sequence:
      return "DW_LNE_end_sequence";
    case DW_LNE_set_address:
      return "DW_LNE_set_address";
    case DW_LNE_define_file:
      return "DW_LNE_define_file";
    default:
      return "DW_LNE_<UNKNOWN>";
    }
}

static char *
dwarf_lns_name (u_char opc)
{
  switch(opc)
    {
    case DW_LNS_extended_op:
      return "DW_LNS_extended_op";
    case DW_LNS_copy:
      return "DW_LNS_copy";
    case DW_LNS_advance_pc:
      return "DW_LNS_advance_pc";
    case DW_LNS_advance_line:
      return "DW_LNS_advance_line";
    case DW_LNS_set_file:
      return "DW_LNS_set_file";
    case DW_LNS_set_column:
      return "DW_LNS_set_column";
    case DW_LNS_negate_stmt:
      return "DW_LNS_negate_stmt";
    case DW_LNS_set_basic_block:
      return "DW_LNS_set_basic_block";
    case DW_LNS_const_add_pc:
      return "DW_LNS_const_add_pc";
    case DW_LNS_fixed_advance_pc:
      return "DW_LNS_fixed_advance_pc";
    case DW_LNS_set_prologue_end:
      return "DW_LNS_set_prologue_end";
    case DW_LNS_set_epilogue_begin:
      return "DW_LNS_set_epilogue_begin";
    case DW_LNS_set_isa:
      return "DW_LNS_set_isa";
    default:
      return "DW_LNS_<UNKOWN>";
    }
}

/* Convert a DIE tag into its string name.  */
static char *
dwarf_tag_name (u_char tag)
{
  switch (tag)
    {
    case DW_TAG_padding:
      return "DW_TAG_padding";
    case DW_TAG_array_type:
      return "DW_TAG_array_type";
    case DW_TAG_class_type:
      return "DW_TAG_class_type";
    case DW_TAG_entry_point:
      return "DW_TAG_entry_point";
    case DW_TAG_enumeration_type:
      return "DW_TAG_enumeration_type";
    case DW_TAG_formal_parameter:
      return "DW_TAG_formal_parameter";
    case DW_TAG_imported_declaration:
      return "DW_TAG_imported_declaration";
    case DW_TAG_label:
      return "DW_TAG_label";
    case DW_TAG_lexical_block:
      return "DW_TAG_lexical_block";
    case DW_TAG_member:
      return "DW_TAG_member";
    case DW_TAG_pointer_type:
      return "DW_TAG_pointer_type";
    case DW_TAG_reference_type:
      return "DW_TAG_reference_type";
    case DW_TAG_compile_unit:
      return "DW_TAG_compile_unit";
    case DW_TAG_string_type:
      return "DW_TAG_string_type";
    case DW_TAG_structure_type:
      return "DW_TAG_structure_type";
    case DW_TAG_subroutine_type:
      return "DW_TAG_subroutine_type";
    case DW_TAG_typedef:
      return "DW_TAG_typedef";
    case DW_TAG_union_type:
      return "DW_TAG_union_type";
    case DW_TAG_unspecified_parameters:
      return "DW_TAG_unspecified_parameters";
    case DW_TAG_variant:
      return "DW_TAG_variant";
    case DW_TAG_common_block:
      return "DW_TAG_common_block";
    case DW_TAG_common_inclusion:
      return "DW_TAG_common_inclusion";
    case DW_TAG_inheritance:
      return "DW_TAG_inheritance";
    case DW_TAG_inlined_subroutine:
      return "DW_TAG_inlined_subroutine";
    case DW_TAG_module:
      return "DW_TAG_module";
    case DW_TAG_ptr_to_member_type:
      return "DW_TAG_ptr_to_member_type";
    case DW_TAG_set_type:
      return "DW_TAG_set_type";
    case DW_TAG_subrange_type:
      return "DW_TAG_subrange_type";
    case DW_TAG_with_stmt:
      return "DW_TAG_with_stmt";
    case DW_TAG_access_declaration:
      return "DW_TAG_access_declaration";
    case DW_TAG_base_type:
      return "DW_TAG_base_type";
    case DW_TAG_catch_block:
      return "DW_TAG_catch_block";
    case DW_TAG_const_type:
      return "DW_TAG_const_type";
    case DW_TAG_constant:
      return "DW_TAG_constant";
    case DW_TAG_enumerator:
      return "DW_TAG_enumerator";
    case DW_TAG_file_type:
      return "DW_TAG_file_type";
    case DW_TAG_friend:
      return "DW_TAG_friend";
    case DW_TAG_namelist:
      return "DW_TAG_namelist";
    case DW_TAG_namelist_item:
      return "DW_TAG_namelist_item";
    case DW_TAG_packed_type:
      return "DW_TAG_packed_type";
    case DW_TAG_subprogram:
      return "DW_TAG_subprogram";
    case DW_TAG_template_type_param:
      return "DW_TAG_template_type_param";
    case DW_TAG_template_value_param:
      return "DW_TAG_template_value_param";
    case DW_TAG_thrown_type:
      return "DW_TAG_thrown_type";
    case DW_TAG_try_block:
      return "DW_TAG_try_block";
    case DW_TAG_variant_part:
      return "DW_TAG_variant_part";
    case DW_TAG_variable:
      return "DW_TAG_variable";
    case DW_TAG_volatile_type:
      return "DW_TAG_volatile_type";
    case DW_TAG_dwarf_procedure:
      return "DW_TAG_dwarf_procedure";
    case DW_TAG_restrict_type:
      return "DW_TAG_restrict_type";
    case DW_TAG_interface_type:
      return "DW_TAG_interface_type";
    case DW_TAG_namespace:
      return "DW_TAG_namespace";
    case DW_TAG_imported_module:
      return "DW_TAG_imported_module";
    case DW_TAG_unspecified_type:
      return "DW_TAG_unspecified_type";
    case DW_TAG_partial_unit:
      return "DW_TAG_partial_unit";
    case DW_TAG_imported_unit:
      return "DW_TAG_imported_unit";
    case DW_TAG_MIPS_loop:
      return "DW_TAG_MIPS_loop";
    case DW_TAG_format_label:
      return "DW_TAG_format_label";
    case DW_TAG_function_template:
      return "DW_TAG_function_template";
    case DW_TAG_class_template:
      return "DW_TAG_class_template";
    default:
      return "DW_TAG_<unknown>";
    }
}

/* Convert a DWARF attribute code into its string name.  */
static char *
dwarf_attr_name (u_char attr)
{
  switch (attr)
    {
    case DW_AT_sibling:
      return "DW_AT_sibling";
    case DW_AT_location:
      return "DW_AT_location";
    case DW_AT_name:
      return "DW_AT_name";
    case DW_AT_ordering:
      return "DW_AT_ordering";
    case DW_AT_subscr_data:
      return "DW_AT_subscr_data";
    case DW_AT_byte_size:
      return "DW_AT_byte_size";
    case DW_AT_bit_offset:
      return "DW_AT_bit_offset";
    case DW_AT_bit_size:
      return "DW_AT_bit_size";
    case DW_AT_element_list:
      return "DW_AT_element_list";
    case DW_AT_stmt_list:
      return "DW_AT_stmt_list";
    case DW_AT_low_pc:
      return "DW_AT_low_pc";
    case DW_AT_high_pc:
      return "DW_AT_high_pc";
    case DW_AT_language:
      return "DW_AT_language";
    case DW_AT_member:
      return "DW_AT_member";
    case DW_AT_discr:
      return "DW_AT_discr";
    case DW_AT_discr_value:
      return "DW_AT_discr_value";
    case DW_AT_visibility:
      return "DW_AT_visibility";
    case DW_AT_import:
      return "DW_AT_import";
    case DW_AT_string_length:
      return "DW_AT_string_length";
    case DW_AT_common_reference:
      return "DW_AT_common_reference";
    case DW_AT_comp_dir:
      return "DW_AT_comp_dir";
    case DW_AT_const_value:
      return "DW_AT_const_value";
    case DW_AT_containing_type:
      return "DW_AT_containing_type";
    case DW_AT_default_value:
      return "DW_AT_default_value";
    case DW_AT_inline:
      return "DW_AT_inline";
    case DW_AT_is_optional:
      return "DW_AT_is_optional";
    case DW_AT_lower_bound:
      return "DW_AT_lower_bound";
    case DW_AT_producer:
      return "DW_AT_producer";
    case DW_AT_prototyped:
      return "DW_AT_prototyped";
    case DW_AT_return_addr:
      return "DW_AT_return_addr";
    case DW_AT_start_scope:
      return "DW_AT_start_scope";
    case DW_AT_stride_size:
      return "DW_AT_stride_size";
    case DW_AT_upper_bound:
      return "DW_AT_upper_bound";
    case DW_AT_abstract_origin:
      return "DW_AT_abstract_origin";
    case DW_AT_accessibility:
      return "DW_AT_accessibility";
    case DW_AT_address_class:
      return "DW_AT_address_class";
    case DW_AT_artificial:
      return "DW_AT_artificial";
    case DW_AT_base_types:
      return "DW_AT_base_types";
    case DW_AT_calling_convention:
      return "DW_AT_calling_convention";
    case DW_AT_count:
      return "DW_AT_count";
    case DW_AT_data_member_location:
      return "DW_AT_data_member_location";
    case DW_AT_decl_column:
      return "DW_AT_decl_column";
    case DW_AT_decl_file:
      return "DW_AT_decl_file";
    case DW_AT_decl_line:
      return "DW_AT_decl_line";
    case DW_AT_declaration:
      return "DW_AT_declaration";
    case DW_AT_discr_list:
      return "DW_AT_discr_list";
    case DW_AT_encoding:
      return "DW_AT_encoding";
    case DW_AT_external:
      return "DW_AT_external";
    case DW_AT_frame_base:
      return "DW_AT_frame_base";
    case DW_AT_friend:
      return "DW_AT_friend";
    case DW_AT_identifier_case:
      return "DW_AT_identifier_case";
    case DW_AT_macro_info:
      return "DW_AT_macro_info";
    case DW_AT_namelist_items:
      return "DW_AT_namelist_items";
    case DW_AT_priority:
      return "DW_AT_priority";
    case DW_AT_segment:
      return "DW_AT_segment";
    case DW_AT_specification:
      return "DW_AT_specification";
    case DW_AT_static_link:
      return "DW_AT_static_link";
    case DW_AT_type:
      return "DW_AT_type";
    case DW_AT_use_location:
      return "DW_AT_use_location";
    case DW_AT_variable_parameter:
      return "DW_AT_variable_parameter";
    case DW_AT_virtuality:
      return "DW_AT_virtuality";
    case DW_AT_vtable_elem_location:
      return "DW_AT_vtable_elem_location";
    case DW_AT_allocated:
      return "DW_AT_allocated";
    case DW_AT_associated:
      return "DW_AT_associated";
    case DW_AT_data_location:
      return "DW_AT_data_location";
    case DW_AT_stride:
      return "DW_AT_stride";
    case DW_AT_entry_pc:
      return "DW_AT_entry_pc";
    case DW_AT_use_UTF8:
      return "DW_AT_use_UTF8";
    case DW_AT_extension:
      return "DW_AT_extension";
    case DW_AT_ranges:
      return "DW_AT_ranges";
    case DW_AT_trampoline:
      return "DW_AT_trampoline";
    case DW_AT_call_column:
      return "DW_AT_call_column";
    case DW_AT_call_file:
      return "DW_AT_call_file";
    case DW_AT_call_line:
      return "DW_AT_call_line";
#ifdef MIPS
    case DW_AT_MIPS_fde:
      return "DW_AT_MIPS_fde";
    case DW_AT_MIPS_loop_begin:
      return "DW_AT_MIPS_loop_begin";
    case DW_AT_MIPS_tail_loop_begin:
      return "DW_AT_MIPS_tail_loop_begin";
    case DW_AT_MIPS_epilog_begin:
      return "DW_AT_MIPS_epilog_begin";
    case DW_AT_MIPS_loop_unroll_factor:
      return "DW_AT_MIPS_loop_unroll_factor";
    case DW_AT_MIPS_software_pipeline_depth:
      return "DW_AT_MIPS_software_pipeline_depth";
#endif
    case DW_AT_MIPS_linkage_name:
      return "DW_AT_MIPS_linkage_name";

    case DW_AT_sf_names:
      return "DW_AT_sf_names";
    case DW_AT_src_info:
      return "DW_AT_src_info";
    case DW_AT_mac_info:
      return "DW_AT_mac_info";
    case DW_AT_src_coords:
      return "DW_AT_src_coords";
    case DW_AT_body_begin:
      return "DW_AT_body_begin";
    case DW_AT_body_end:
      return "DW_AT_body_end";
    case DW_AT_GNU_vector:
      return "DW_AT_GNU_vector";
    default:
      return "DW_AT_<unknown>";
    }
}

/* Convert a DWARF value form code into its string name.  */
static char *
dwarf_form_name (u_char form)
{
  switch (form)
    {
    case DW_FORM_addr:
      return "DW_FORM_addr";
    case DW_FORM_block2:
      return "DW_FORM_block2";
    case DW_FORM_block4:
      return "DW_FORM_block4";
    case DW_FORM_data2:
      return "DW_FORM_data2";
    case DW_FORM_data4:
      return "DW_FORM_data4";
    case DW_FORM_data8:
      return "DW_FORM_data8";
    case DW_FORM_string:
      return "DW_FORM_string";
    case DW_FORM_block:
      return "DW_FORM_block";
    case DW_FORM_block1:
      return "DW_FORM_block1";
    case DW_FORM_data1:
      return "DW_FORM_data1";
    case DW_FORM_flag:
      return "DW_FORM_flag";
    case DW_FORM_sdata:
      return "DW_FORM_sdata";
    case DW_FORM_strp:
      return "DW_FORM_strp";
    case DW_FORM_udata:
      return "DW_FORM_udata";
    case DW_FORM_ref_addr:
      return "DW_FORM_ref_addr";
    case DW_FORM_ref1:
      return "DW_FORM_ref1";
    case DW_FORM_ref2:
      return "DW_FORM_ref2";
    case DW_FORM_ref4:
      return "DW_FORM_ref4";
    case DW_FORM_ref8:
      return "DW_FORM_ref8";
    case DW_FORM_ref_udata:
      return "DW_FORM_ref_udata";
    case DW_FORM_indirect:
      return "DW_FORM_indirect";
    default:
      return "DW_FORM_<unknown>";
    }
}

/* Convert a DWARF stack opcode into its string name.  */
static char *
dwarf_stack_op_name (u_char op)
{
  switch (op)
    {
    case DW_OP_addr:
      return "DW_OP_addr";
    case DW_OP_deref:
      return "DW_OP_deref";
    case DW_OP_const1u:
      return "DW_OP_const1u";
    case DW_OP_const1s:
      return "DW_OP_const1s";
    case DW_OP_const2u:
      return "DW_OP_const2u";
    case DW_OP_const2s:
      return "DW_OP_const2s";
    case DW_OP_const4u:
      return "DW_OP_const4u";
    case DW_OP_const4s:
      return "DW_OP_const4s";
    case DW_OP_const8u:
      return "DW_OP_const8u";
    case DW_OP_const8s:
      return "DW_OP_const8s";
    case DW_OP_constu:
      return "DW_OP_constu";
    case DW_OP_consts:
      return "DW_OP_consts";
    case DW_OP_dup:
      return "DW_OP_dup";
    case DW_OP_drop:
      return "DW_OP_drop";
    case DW_OP_over:
      return "DW_OP_over";
    case DW_OP_pick:
      return "DW_OP_pick";
    case DW_OP_swap:
      return "DW_OP_swap";
    case DW_OP_rot:
      return "DW_OP_rot";
    case DW_OP_xderef:
      return "DW_OP_xderef";
    case DW_OP_abs:
      return "DW_OP_abs";
    case DW_OP_and:
      return "DW_OP_and";
    case DW_OP_div:
      return "DW_OP_div";
    case DW_OP_minus:
      return "DW_OP_minus";
    case DW_OP_mod:
      return "DW_OP_mod";
    case DW_OP_mul:
      return "DW_OP_mul";
    case DW_OP_neg:
      return "DW_OP_neg";
    case DW_OP_not:
      return "DW_OP_not";
    case DW_OP_or:
      return "DW_OP_or";
    case DW_OP_plus:
      return "DW_OP_plus";
    case DW_OP_plus_uconst:
      return "DW_OP_plus_uconst";
    case DW_OP_shl:
      return "DW_OP_shl";
    case DW_OP_shr:
      return "DW_OP_shr";
    case DW_OP_shra:
      return "DW_OP_shra";
    case DW_OP_xor:
      return "DW_OP_xor";
    case DW_OP_bra:
      return "DW_OP_bra";
    case DW_OP_eq:
      return "DW_OP_eq";
    case DW_OP_ge:
      return "DW_OP_ge";
    case DW_OP_gt:
      return "DW_OP_gt";
    case DW_OP_le:
      return "DW_OP_le";
    case DW_OP_lt:
      return "DW_OP_lt";
    case DW_OP_ne:
      return "DW_OP_ne";
    case DW_OP_skip:
      return "DW_OP_skip";
    case DW_OP_lit0:
      return "DW_OP_lit0";
    case DW_OP_lit1:
      return "DW_OP_lit1";
    case DW_OP_lit2:
      return "DW_OP_lit2";
    case DW_OP_lit3:
      return "DW_OP_lit3";
    case DW_OP_lit4:
      return "DW_OP_lit4";
    case DW_OP_lit5:
      return "DW_OP_lit5";
    case DW_OP_lit6:
      return "DW_OP_lit6";
    case DW_OP_lit7:
      return "DW_OP_lit7";
    case DW_OP_lit8:
      return "DW_OP_lit8";
    case DW_OP_lit9:
      return "DW_OP_lit9";
    case DW_OP_lit10:
      return "DW_OP_lit10";
    case DW_OP_lit11:
      return "DW_OP_lit11";
    case DW_OP_lit12:
      return "DW_OP_lit12";
    case DW_OP_lit13:
      return "DW_OP_lit13";
    case DW_OP_lit14:
      return "DW_OP_lit14";
    case DW_OP_lit15:
      return "DW_OP_lit15";
    case DW_OP_lit16:
      return "DW_OP_lit16";
    case DW_OP_lit17:
      return "DW_OP_lit17";
    case DW_OP_lit18:
      return "DW_OP_lit18";
    case DW_OP_lit19:
      return "DW_OP_lit19";
    case DW_OP_lit20:
      return "DW_OP_lit20";
    case DW_OP_lit21:
      return "DW_OP_lit21";
    case DW_OP_lit22:
      return "DW_OP_lit22";
    case DW_OP_lit23:
      return "DW_OP_lit23";
    case DW_OP_lit24:
      return "DW_OP_lit24";
    case DW_OP_lit25:
      return "DW_OP_lit25";
    case DW_OP_lit26:
      return "DW_OP_lit26";
    case DW_OP_lit27:
      return "DW_OP_lit27";
    case DW_OP_lit28:
      return "DW_OP_lit28";
    case DW_OP_lit29:
      return "DW_OP_lit29";
    case DW_OP_lit30:
      return "DW_OP_lit30";
    case DW_OP_lit31:
      return "DW_OP_lit31";
    case DW_OP_reg0:
      return "DW_OP_reg0";
    case DW_OP_reg1:
      return "DW_OP_reg1";
    case DW_OP_reg2:
      return "DW_OP_reg2";
    case DW_OP_reg3:
      return "DW_OP_reg3";
    case DW_OP_reg4:
      return "DW_OP_reg4";
    case DW_OP_reg5:
      return "DW_OP_reg5";
    case DW_OP_reg6:
      return "DW_OP_reg6";
    case DW_OP_reg7:
      return "DW_OP_reg7";
    case DW_OP_reg8:
      return "DW_OP_reg8";
    case DW_OP_reg9:
      return "DW_OP_reg9";
    case DW_OP_reg10:
      return "DW_OP_reg10";
    case DW_OP_reg11:
      return "DW_OP_reg11";
    case DW_OP_reg12:
      return "DW_OP_reg12";
    case DW_OP_reg13:
      return "DW_OP_reg13";
    case DW_OP_reg14:
      return "DW_OP_reg14";
    case DW_OP_reg15:
      return "DW_OP_reg15";
    case DW_OP_reg16:
      return "DW_OP_reg16";
    case DW_OP_reg17:
      return "DW_OP_reg17";
    case DW_OP_reg18:
      return "DW_OP_reg18";
    case DW_OP_reg19:
      return "DW_OP_reg19";
    case DW_OP_reg20:
      return "DW_OP_reg20";
    case DW_OP_reg21:
      return "DW_OP_reg21";
    case DW_OP_reg22:
      return "DW_OP_reg22";
    case DW_OP_reg23:
      return "DW_OP_reg23";
    case DW_OP_reg24:
      return "DW_OP_reg24";
    case DW_OP_reg25:
      return "DW_OP_reg25";
    case DW_OP_reg26:
      return "DW_OP_reg26";
    case DW_OP_reg27:
      return "DW_OP_reg27";
    case DW_OP_reg28:
      return "DW_OP_reg28";
    case DW_OP_reg29:
      return "DW_OP_reg29";
    case DW_OP_reg30:
      return "DW_OP_reg30";
    case DW_OP_reg31:
      return "DW_OP_reg31";
    case DW_OP_breg0:
      return "DW_OP_breg0";
    case DW_OP_breg1:
      return "DW_OP_breg1";
    case DW_OP_breg2:
      return "DW_OP_breg2";
    case DW_OP_breg3:
      return "DW_OP_breg3";
    case DW_OP_breg4:
      return "DW_OP_breg4";
    case DW_OP_breg5:
      return "DW_OP_breg5";
    case DW_OP_breg6:
      return "DW_OP_breg6";
    case DW_OP_breg7:
      return "DW_OP_breg7";
    case DW_OP_breg8:
      return "DW_OP_breg8";
    case DW_OP_breg9:
      return "DW_OP_breg9";
    case DW_OP_breg10:
      return "DW_OP_breg10";
    case DW_OP_breg11:
      return "DW_OP_breg11";
    case DW_OP_breg12:
      return "DW_OP_breg12";
    case DW_OP_breg13:
      return "DW_OP_breg13";
    case DW_OP_breg14:
      return "DW_OP_breg14";
    case DW_OP_breg15:
      return "DW_OP_breg15";
    case DW_OP_breg16:
      return "DW_OP_breg16";
    case DW_OP_breg17:
      return "DW_OP_breg17";
    case DW_OP_breg18:
      return "DW_OP_breg18";
    case DW_OP_breg19:
      return "DW_OP_breg19";
    case DW_OP_breg20:
      return "DW_OP_breg20";
    case DW_OP_breg21:
      return "DW_OP_breg21";
    case DW_OP_breg22:
      return "DW_OP_breg22";
    case DW_OP_breg23:
      return "DW_OP_breg23";
    case DW_OP_breg24:
      return "DW_OP_breg24";
    case DW_OP_breg25:
      return "DW_OP_breg25";
    case DW_OP_breg26:
      return "DW_OP_breg26";
    case DW_OP_breg27:
      return "DW_OP_breg27";
    case DW_OP_breg28:
      return "DW_OP_breg28";
    case DW_OP_breg29:
      return "DW_OP_breg29";
    case DW_OP_breg30:
      return "DW_OP_breg30";
    case DW_OP_breg31:
      return "DW_OP_breg31";
    case DW_OP_regx:
      return "DW_OP_regx";
    case DW_OP_fbreg:
      return "DW_OP_fbreg";
    case DW_OP_bregx:
      return "DW_OP_bregx";
    case DW_OP_piece:
      return "DW_OP_piece";
    case DW_OP_deref_size:
      return "DW_OP_deref_size";
    case DW_OP_xderef_size:
      return "DW_OP_xderef_size";
    case DW_OP_nop:
      return "DW_OP_nop";
      /* DWARF 3 extensions.  */
    case DW_OP_push_object_address:
      return "DW_OP_push_object_address";
    case DW_OP_call2:
      return "DW_OP_call2";
    case DW_OP_call4:
      return "DW_OP_call4";
    case DW_OP_call_ref:
      return "DW_OP_call_ref";
      /* GNU extensions.  */
    case DW_OP_GNU_push_tls_address:
      return "DW_OP_GNU_push_tls_address";
    default:
      return "OP_<unknown>";
    }
}

static char *
dwarf_bool_name (u_char mybool)
{
  if (mybool)
    return "TRUE";
  else
    return "FALSE";
}

/* Convert a DWARF type code into its string name.  */

static char *
dwarf_type_encoding_name (u_char enc)
{
  switch (enc)
    {
    case DW_ATE_address:
      return "DW_ATE_address";
    case DW_ATE_boolean:
      return "DW_ATE_boolean";
    case DW_ATE_complex_float:
      return "DW_ATE_complex_float";
    case DW_ATE_float:
      return "DW_ATE_float";
    case DW_ATE_signed:
      return "DW_ATE_signed";
    case DW_ATE_signed_char:
      return "DW_ATE_signed_char";
    case DW_ATE_unsigned:
      return "DW_ATE_unsigned";
    case DW_ATE_unsigned_char:
      return "DW_ATE_unsigned_char";
    case DW_ATE_imaginary_float:
      return "DW_ATE_imaginary_float";
    default:
      return "DW_ATE_<unknown>";
    }
}

/* Convert a DWARF call frame info operation to its string name. */
static char *
dwarf_cfi_name (unsigned cfi_opc)
{
  switch (cfi_opc)
    {
    case DW_CFA_advance_loc:
      return "DW_CFA_advance_loc";
    case DW_CFA_offset:
      return "DW_CFA_offset";
    case DW_CFA_restore:
      return "DW_CFA_restore";
    case DW_CFA_nop:
      return "DW_CFA_nop";
    case DW_CFA_set_loc:
      return "DW_CFA_set_loc";
    case DW_CFA_advance_loc1:
      return "DW_CFA_advance_loc1";
    case DW_CFA_advance_loc2:
      return "DW_CFA_advance_loc2";
    case DW_CFA_advance_loc4:
      return "DW_CFA_advance_loc4";
    case DW_CFA_offset_extended:
      return "DW_CFA_offset_extended";
    case DW_CFA_restore_extended:
      return "DW_CFA_restore_extended";
    case DW_CFA_undefined:
      return "DW_CFA_undefined";
    case DW_CFA_same_value:
      return "DW_CFA_same_value";
    case DW_CFA_register:
      return "DW_CFA_register";
    case DW_CFA_remember_state:
      return "DW_CFA_remember_state";
    case DW_CFA_restore_state:
      return "DW_CFA_restore_state";
    case DW_CFA_def_cfa:
      return "DW_CFA_def_cfa";
    case DW_CFA_def_cfa_register:
      return "DW_CFA_def_cfa_register";
    case DW_CFA_def_cfa_offset:
      return "DW_CFA_def_cfa_offset";

    /* DWARF 3 */
    case DW_CFA_def_cfa_expression:
      return "DW_CFA_def_cfa_expression";
    case DW_CFA_expression:
      return "DW_CFA_expression";
    case DW_CFA_offset_extended_sf:
      return "DW_CFA_offset_extended_sf";
    case DW_CFA_def_cfa_sf:
      return "DW_CFA_def_cfa_sf";
    case DW_CFA_def_cfa_offset_sf:
      return "DW_CFA_def_cfa_offset_sf";

      /* SGI/MIPS specific */
    case DW_CFA_MIPS_advance_loc8:
      return "DW_CFA_MIPS_advance_loc8";

    /* GNU extensions */
    case DW_CFA_GNU_window_save:
      return "DW_CFA_GNU_window_save";
    case DW_CFA_GNU_args_size:
      return "DW_CFA_GNU_args_size";
    case DW_CFA_GNU_negative_offset_extended:
      return "DW_CFA_GNU_negative_offset_extended";

    default:
      return "DW_CFA_<unknown>";
    }
}

/* Parse the DWARF-2 debug format */
int 		edfmt_dwarf2_parse(elfshobj_t *file, elfshsect_t *sect)
{
  edfmtdw2sect_t *pointers[7];
  char		*names[7];
  u_int		i;
  elfshsect_t 	*csect;
  char		**keys;
  u_int		index;
  u_int		keynbr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Full fill with 0 */
  memset(&dw2_sections, 0x00, sizeof(edfmtdw2sectlist_t));

  /* Init global reference table (for DW_FORM_ref entities) */
  hash_init(&ref_global_table, 50);

  /* We already have it ! */
  dw2_sections.info.data = sect->data;
  dw2_sections.info.size = sect->shdr->sh_size;

  /* Local table system to full fill every pointers correctly */
  pointers[0] = &dw2_sections.abbrev;
  pointers[1] = &dw2_sections.aranges;
  pointers[2] = &dw2_sections.frame;
  pointers[3] = &dw2_sections.line;
  pointers[4] = &dw2_sections.macinfo;
  pointers[5] = &dw2_sections.pubnames;
  pointers[6] = &dw2_sections.str;
  pointers[7] = NULL;

  names[0] = DWARF2_SECTION_ABBREV;
  names[1] = DWARF2_SECTION_ARANGES;
  names[2] = DWARF2_SECTION_FRAME;
  names[3] = DWARF2_SECTION_LINE;
  names[4] = DWARF2_SECTION_MACINFO;
  names[5] = DWARF2_SECTION_PUBNAMES;
  names[6] = DWARF2_SECTION_STR;
  names[7] = NULL;

  /* Use the table to fill every pointers and report a problem */
  for (i = 0; names[i] != NULL; i++)
    {
      csect = elfsh_get_section_by_name(file, names[i], NULL, NULL, NULL);

      if (!csect)
	{
	  printf("Section number %d didn't exist\n", i);
	  continue;
	}

      pointers[i]->data = csect->data;
      pointers[i]->size = csect->shdr->sh_size;
    }

  /* Start into the block entrie */
  edfmt_dwarf2_block_entrie();

  /* Clean references */
  edfmt_dwarf2_updatefref(1);

  keys = hash_get_keys(&ref_global_table, &keynbr);
  for (index = 0; index < keynbr; index++)
    XFREE(keys[index]);
  hash_free_keys((char *) keys);

  hash_destroy(&ref_global_table);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Central parse function that find every compilation unit */
int		edfmt_dwarf2_block_entrie()
{
  edfmtdw2cu_t	*tcu;
  u_int		last_pos;
  char		**keys;
  u_int		index;
  u_int		keynbr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  while (dw2_sections.info.pos < dw2_sections.info.size)
    {
      /* Init local reference compil unit table */
      hash_init(&ref_cu_table, 30);

      /* Alloc a new compil unit */
      XALLOC(tcu, sizeof(edfmtdw2cu_t), -1);
      
      /* Update current pointer */
      if (current_cu != NULL)
	{
	  current_cu->next = tcu;
	}
      else
	{
	  /* Init info structure */
	  debug_info.cu_list = tcu;
	  hash_init(&debug_info.file_line, 50);
	  hash_init(&debug_info.addr, 50);
	}

      current_cu = current_cu ? current_cu->next : tcu;

      /* Init local hashes */
      hash_init(&current_cu->file_line, 50);
      hash_init(&current_cu->addr, 50);

      /* Set compil unit start position */
      current_cu->start_pos = dw2_sections.info.pos;

      /* Read DWARF2 .debug_info header */
      ci_pos(current_cu->length, info, u_int);
      last_pos = dw2_sections.info.pos;
      ci_pos(current_cu->version, info, u_short);
      ci_pos(current_cu->offset, info, u_int);
      ci_pos(current_cu->addr_size, info, u_char);
      
      printf("COMPILATION UNIT:\nLength: %d\nVersion: %d\nOffset: %d\nAddr_size: %d\n",
	     current_cu->length, current_cu->version, current_cu->offset, current_cu->addr_size);
      
      /* Update abbrev position */
      dw2_sections.abbrev.pos = current_cu->offset;
     
      /* Loop on the current compil unit */
      edfmt_dwarf2_block_loop(last_pos + current_cu->length);

      /* We didn't finish at the exact correct offset ? */
      if (last_pos + current_cu->length != dw2_sections.info.pos)
	dw2_sections.info.pos = last_pos + current_cu->length;

      edfmt_dwarf2_updatefref(0);

      /* Clean */
      keys = hash_get_keys(&ref_cu_table, &keynbr);
      for (index = 0; index < keynbr; index++)
	XFREE(keys[index]);
      hash_free_keys((char *) keys);

      hash_destroy(&ref_cu_table);
    }
      
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

static u_long 	edfmt_read_uleb128(void *data, u_int *bread)
{
  u_long	sum = 0;
  u_int		read = 0;
  u_char 	c;
  int		s = 0;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  while (1)
    {
      c = *(u_char *) (data + read) & 0xFF; 
      read++;
      sum |= ((u_long)(c & 127) << s);

      if ((c & 128) == 0)
	break;

      s += 7;
    }

  if (bread)
    *bread = read;

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, sum);
}

static long 	edfmt_read_leb128(void *data, u_int *bread)
{
  long		sum = 0;
  u_int		read = 0;
  u_char 	c;
  int		s = 0;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  while (1)
    {
      c = *(u_char *) (data + read) & 0xFF; 
      read++;
      sum |= ((long)(c & 127) << s);

      if ((c & 128) == 0)
	break;

      s += 7;
    }

  if ((s < 8 * sizeof(sum)) && (c & 0x40))
    sum |= -(((long)1) << s);

  if (bread)
    *bread = read;

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, sum);
}

edfmtdw2abbent_t *edfmt_dwarf2_lookup_abbrev(u_int num_fetch)
{
  char		ckey[DW2_CKEY_SIZE];
  edfmtdw2abbent_t *ent;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  edfmt_dwarf2_ckey(ckey, DW2_CKEY_SIZE, num_fetch);
  ent = (edfmtdw2abbent_t *) hash_get(&abbrev_table, ckey);

  if (ent == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Abbrev number not found", NULL);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ent);
}

/* Parse .debug_line */
int		edfmt_dwarf2_line(u_long offset)
{
  u_int		i, bsize;
  edfmtdw2linehead_t header;
  u_long      	prev_pos;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Update position offset */
  dw2_sections.line.pos = offset;

  /* Parse the header */
  ci_read_4(header.total_length, line);
  header.end_pos = dw2_sections.line.pos + header.total_length;
  ci_read_2(header.version, line);
  ci_read_4(header.prologue_length, line);
  header.prologue_pos = dw2_sections.line.pos;
  ci_read_1(header.min_length_instr, line);
  ci_read_1(header.default_stmt, line);
  ci_read_1(header.line_base, line);
  ci_read_1(header.line_range, line);
  ci_read_1(header.opcode_base, line);
  header.std_opcode_length = ac_pos(line);
  inc_pos(line, header.opcode_base - 1);
  
  /* Dir first pass */
  prev_pos = dw2_sections.line.pos;
  for (i = 0; (ac_pos(line))[0] != '\0'; i++)
    inc_pos(line, strlen(ac_pos(line))+1);
  dw2_sections.line.pos = prev_pos;

  XALLOC(header.dirs, sizeof(char*)*i, -1);
  header.dirs_number = i;

  /* Dir second pass */
  for (i = 0; (ac_pos(line))[0] != '\0'; i++)
    {
      header.dirs[i] = ac_pos(line);
      inc_pos(line, strlen(ac_pos(line))+1);
    }
  inc_pos(line, 1);

  /* Filename first pass */
  prev_pos = dw2_sections.line.pos;
  for (i = 0; (ac_pos(line))[0] != '\0'; i++)
    {
      inc_pos(line, strlen(ac_pos(line))+1);
      edfmt_read_uleb128(a_pos(line), &bsize);
      inc_pos(line, bsize);
      edfmt_read_uleb128(a_pos(line), &bsize);
      inc_pos(line, bsize);
      edfmt_read_uleb128(a_pos(line), &bsize);
      inc_pos(line, bsize);
    }
  dw2_sections.line.pos = prev_pos;

  XALLOC(header.files_name, sizeof(char*)*i, -1);
  XALLOC(header.files_dindex, sizeof(u_int)*i, -1);
  XALLOC(header.files_time, sizeof(u_int)*i, -1);
  XALLOC(header.files_len, sizeof(u_int)*i, -1);
  header.files_number = i;

  /* Filename second pass */
  for (i = 0; (ac_pos(line))[0] != '\0'; i++)
    {
      header.files_name[i] = ac_pos(line);
      inc_pos(line, strlen(ac_pos(line))+1);
      header.files_dindex[i] = edfmt_read_uleb128(a_pos(line), &bsize);
      inc_pos(line, bsize);
      header.files_time[i] = edfmt_read_uleb128(a_pos(line), &bsize);
      inc_pos(line, bsize);
      header.files_len[i] = edfmt_read_uleb128(a_pos(line), &bsize);
      inc_pos(line, bsize);
    }
  inc_pos(line, 1);

  /* Check position validity */
  if (header.prologue_pos + header.prologue_length != dw2_sections.line.pos)
    dw2_sections.line.pos = header.prologue_pos + header.prologue_length;

  /* Save files & directories list */
  current_cu->dirs 	   = header.dirs;
  current_cu->files_name   = header.files_name;
  current_cu->files_dindex = header.files_dindex;
  current_cu->files_time   = header.files_time;
  current_cu->files_len    = header.files_len;

  /* Parse the rest */
  edfmt_dwarf2_line_data(&header);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		edfmt_dwarf2_line_rec(edfmtdw2cu_t *cu, u_int line, u_int column, elfsh_Addr addr, u_int fid)
{
  edfmtdw2line_t *pline;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(pline, sizeof(edfmtdw2line_t), -1);

  pline->addr = addr;
  pline->line = line;
  pline->column = column;
  pline->fileid = fid;
  pline->cu = cu;

  /* Create hash names */
  edfmt_dwarf2_cline(pline->cline, DW2_CLINE_SIZE - 1, line, cu->files_name[fid]);
  edfmt_dwarf2_caddr(pline->caddr, DW2_CADDR_SIZE - 1, addr);

  /* Global */
  hash_add(&debug_info.file_line, pline->cline, (void *) pline);
  hash_add(&debug_info.addr, pline->caddr, (void *) pline);

  /* Local */
  hash_add(&cu->file_line, pline->cline, (void *) pline);
  hash_add(&cu->addr, pline->caddr, (void *) pline);
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Parse data part of .debug_line format */
int		edfmt_dwarf2_line_data(edfmtdw2linehead_t *header)
{
  elfsh_Addr  	addr;
  u_int		file;
  u_int		line;
  u_int		column;
  u_int	      	is_stmt;
  u_int		basic_block;
  u_int		end_sequence;

  u_int		i, bsize;
  u_char       	opc, ext_opc;
  char		*read_addr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (header == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Invalid parameters", -1);

  /* Loop until end position */
  while (dw2_sections.line.pos < header->end_pos)
    {
      addr = 0;
      file = 1;
      line = 1;
      column = 0;
      is_stmt = header->default_stmt;
      basic_block = 0;
      end_sequence = 0;

      while (!end_sequence)
	{
	  ci_read_1(opc, line);

	  if (opc >= header->opcode_base)
	    {
	      opc -= header->opcode_base;
	      line += header->line_base + (opc % header->line_range);
	      addr += (opc / header->line_range) * header->min_length_instr;

	      edfmt_dwarf2_line_rec(current_cu, line, column, addr, file - 1);
	      basic_block = 1;
	    }
	  else
	    {
	      switch(opc)
		{
		case DW_LNS_extended_op:
		  edfmt_read_uleb128(a_pos(line), &bsize);
		  inc_pos(line, bsize);

		  ci_read_1(ext_opc, line);
		  switch(ext_opc)
		    {
		    case DW_LNE_end_sequence:
		      end_sequence = 1;
		      edfmt_dwarf2_line_rec(current_cu, line, column, addr, file - 1);
		      break;
		    case DW_LNE_set_address:
		      XALLOC(read_addr, current_cu->addr_size, -1);
		      
		      for (i = 0; i < current_cu->addr_size; i++)
			ci_pos(read_addr[i], line, u_char);

		      addr = *(elfsh_Addr *) read_addr;
		      
		      XFREE(read_addr);
		      break;
		    case DW_LNE_define_file:
		      header->files_number++;
		      /* Realloc pointers */
		      XREALLOC(header->files_name, header->files_name, 
			       sizeof(char*)*header->files_number, -1);
		      XREALLOC(header->files_dindex, header->files_dindex, 
			       sizeof(u_int)*header->files_number, -1);
		      XREALLOC(header->files_time, header->files_time, 
			       sizeof(u_int)*header->files_number, -1);
		      XREALLOC(header->files_len, header->files_len, 
			       sizeof(u_int)*header->files_number, -1);

		      /* Read information and fill new entrie */
		      header->files_name[header->files_number] = ac_pos(line);
		      inc_pos(line, strlen(ac_pos(line))+1);
		      header->files_dindex[header->files_number] = edfmt_read_uleb128(a_pos(line), &bsize);
		      inc_pos(line, bsize);
		      header->files_time[header->files_number] = edfmt_read_uleb128(a_pos(line), &bsize);
		      inc_pos(line, bsize);
		      header->files_len[header->files_number] = edfmt_read_uleb128(a_pos(line), &bsize);
		      inc_pos(line, bsize);
		      break;
		    }
		  break;
		case DW_LNS_copy:
		  basic_block = 0;
		  edfmt_dwarf2_line_rec(current_cu, line, column, addr, file - 1);
		  break;
		case DW_LNS_advance_pc:
		  addr += edfmt_read_uleb128(a_pos(line), &bsize) * header->min_length_instr;
		  inc_pos(line, bsize);
		  break;
		case DW_LNS_advance_line:
		  line += edfmt_read_leb128(a_pos(line), &bsize);
		  inc_pos(line, bsize);		  
		  break;
		case DW_LNS_set_file:
		  file = edfmt_read_uleb128(a_pos(line), &bsize);
		  inc_pos(line, bsize);
		  break;
		case DW_LNS_set_column:
		  column = edfmt_read_uleb128(a_pos(line), &bsize);
		  inc_pos(line, bsize);
		  break;
		case DW_LNS_negate_stmt:
		  is_stmt = !is_stmt;
		  break;
		case DW_LNS_set_basic_block:
		  basic_block = 1;
		  break;
		case DW_LNS_const_add_pc:
		  addr += ((255 - header->opcode_base) / header->line_range) 
		    * header->min_length_instr;
		  break;
		case DW_LNS_fixed_advance_pc:
		  ci_read_2(bsize, line);
		  addr += bsize;
		  break;
		default:
		  for (i = 0; i < header->std_opcode_length[opc]; i++)
		    {
		      edfmt_read_uleb128(a_pos(line), &bsize);
		      inc_pos(line, bsize);
		    }
		}
	    }
	}
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Follow .debug_info form using abbrev_table as structure reference */
int		edfmt_dwarf2_form(u_int endpos)
{  
  char		*ref_global_ckey;
  char	      	*ref_cu_ckey;
  void 		*ptr;
  u_int		num_fetch, i, bsize, level = 1;
  edfmtdw2abbent_t *ent;
  u_int		start_pos;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  do {
    ptr = a_pos(info);

    start_pos = dw2_sections.info.pos;
    
    /* Retrieve abbrev number */
    num_fetch = edfmt_read_uleb128(ptr, &bsize);
    inc_pos(info, bsize);
    ptr += bsize;

    /* Update current level */
    if (num_fetch == 0)
      {
	level--;
	continue;
      }
    
    ent = edfmt_dwarf2_lookup_abbrev(num_fetch);
    if (ent == NULL)
      continue;

    /* Update references */
    XALLOC(ref_global_ckey, DW2_CKEY_SIZE, -1);
    XALLOC(ref_cu_ckey, DW2_CKEY_SIZE, -1);

    edfmt_dwarf2_ckey(ref_global_ckey, DW2_CKEY_SIZE, start_pos);
    edfmt_dwarf2_ckey(ref_cu_ckey, DW2_CKEY_SIZE, start_pos - current_cu->start_pos);

    hash_add(&ref_global_table, ref_global_ckey, (void *) ent);
    hash_add(&ref_cu_table, ref_cu_ckey, (void *) ent);
    
    /* If we enter into a new child list, increase the level */
    if (ent->children != 0)
      level++;

    /* Resolv form value for each attribute */
    for (i = 0; ent->attr[i].attr; i++)
      {
	edfmt_dwarf2_form_value(ent->attr + i);

	switch(ent->attr[i].attr)
	  {
	  case DW_AT_stmt_list:
	    edfmt_dwarf2_line(ent->attr[i].u.udata);
	    break;
	  }
      }

  } while(level > 0 && dw2_sections.info.pos < endpos);
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Parse a form value */
int		edfmt_dwarf2_form_value(edfmtdw2abbattr_t *attr)
{
  u_int		i;
  void		*ptr;
  u_char 	*addr;
  u_int		bsize;
  u_int		data;
  u_long	ldata;
  long int	ddata;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  ptr = a_pos(info);

  switch (attr->form)
    {
    case DW_FORM_ref_addr:
      XALLOC(addr, current_cu->addr_size, -1);

      for (i = 0; i < current_cu->addr_size; i++)
	ci_pos(addr[i], info, u_char);

      edfmt_dwarf2_newfref(*(u_int *) addr, attr, 1);
      XFREE(addr);
      break;
    case DW_FORM_addr:
      XALLOC(attr->u.vbuf, current_cu->addr_size, -1);
      for (i = 0; i < current_cu->addr_size; i++)
	ci_pos(attr->u.vbuf[i], info, u_char);
      break;
    case DW_FORM_block1:
    case DW_FORM_block2:
    case DW_FORM_block4:
      switch (attr->form)
	{
	case DW_FORM_block1:
	  ci_read_1(attr->asize, info);
	  break;
	case DW_FORM_block2:
	  ci_read_2(attr->asize, info);
	  break;
	case DW_FORM_block4:
	  ci_read_4(attr->asize, info);
	  break;
	}

      XALLOC(attr->u.vbuf, attr->asize, -1);

      for (i = 0; i < attr->asize; i++)
	ci_pos(attr->u.vbuf[i], info, u_char);
      break;
    case DW_FORM_data1:
      ci_read_1(attr->u.udata, info);
      break;
    case DW_FORM_data2:
      ci_read_2(attr->u.udata, info);
      break;
    case DW_FORM_data4:
      ci_read_4(attr->u.udata, info);
      break;
    case DW_FORM_data8:
      ci_read_8(attr->u.udata, info);
      break;
    case DW_FORM_string:
      attr->u.str = (char *) ptr;
      inc_pos(info, strlen(attr->u.str)+1);
      break;
    case DW_FORM_block:
      attr->asize = edfmt_read_uleb128(ptr, &bsize);
      inc_pos(info, bsize);

      XALLOC(attr->u.vbuf, attr->asize, -1);

      for (i = 0; i < attr->asize; i++)
	ci_pos(attr->u.vbuf[i], info, u_char);
      break;
    case DW_FORM_flag:
      ci_read_1(attr->u.udata, info);
      break;
    case DW_FORM_strp:
      addr = (char *) dw2_sections.str.data;
      ci_read_4(data, info);
      attr->u.str = addr + data;
      break;
    case DW_FORM_udata:
      attr->u.udata = edfmt_read_uleb128(ptr, &bsize);
      inc_pos(info, bsize);
      break;
    case DW_FORM_sdata:
      attr->u.sdata = edfmt_read_leb128(ptr, &bsize);
      inc_pos(info, bsize);
      break;
    case DW_FORM_ref1:
      ci_read_1(data, info);
      edfmt_dwarf2_newfref(data, attr, 0);
      break;
    case DW_FORM_ref2:
      ci_read_2(data, info);
      edfmt_dwarf2_newfref(data, attr, 0);
      break;
    case DW_FORM_ref4:
      ci_read_4(data, info);
      edfmt_dwarf2_newfref(data, attr, 0);
      break;
    case DW_FORM_ref8:
      ci_read_8(ddata, info);
      edfmt_dwarf2_newfref(ddata, attr, 0);
      break;
    case DW_FORM_ref_udata:
      ldata = edfmt_read_uleb128(ptr, &bsize);
      inc_pos(info, bsize);

      edfmt_dwarf2_newfref(ldata, attr, 0);
      break;
    case DW_FORM_indirect:
      /* Read form from .debug_info */
      data = edfmt_read_uleb128(ptr, &bsize);
      inc_pos(info, bsize);
     
      /* Update attribute value and relaunch the function */
      attr->form = data;
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			 edfmt_dwarf2_form_value(attr));
      break;
    default:
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unknown form", -1);
      break;
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Parse the .debug_abbrev format */
int		edfmt_dwarf2_abbrev()
{
  u_int		num;
  u_int		bread;
  void 		*ptr;
  edfmtdw2abbent_t *ent = NULL;
  edfmtdw2abbent_t *tmp;
  edfmtdw2abbattr_t *attr = NULL;
  u_int		allocattr = 0;
  u_int		attri = 0;
  u_int		level = 1;
  const	u_int	allocstep = 12;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  do {
    ptr = a_pos(abbrev);

    /* Fetch the key number */
    num = edfmt_read_uleb128(ptr, &bread);
    inc_pos(abbrev, bread);
    ptr += bread;
    
    /* A level end */
    if (num == 0)
      {
	level--;
	/* It's the end ! */
	if (level == 0)
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	continue;
      }
    
    /* Save last element */
    tmp = ent;
    
    /* Alloc next elements */
    XALLOC(ent, sizeof(edfmtdw2abbent_t), -1);
    
    /* Fill main data */
    ent->key = num;
    ent->tag = edfmt_read_uleb128(ptr, &bread);
    inc_pos(abbrev, bread);
    ptr += bread;
    ci_read_1(ent->children, abbrev);
    ptr += 1;
    
    attri = allocattr = 0;
    attr = NULL;
    
    /* Update level */
    ent->level = level;
    
    if (ent->children != 0)
      level++;
    
    /* Update links between entities */
    if (tmp)
      {
	/* Direct child */
	if (ent->level > tmp->level)
	  {
	    ent->parent = tmp;
	  }
	/* Next sibling of last element parent */
	else if (ent->level < tmp->level)
	  {
	    if (tmp->parent != NULL)
	      {
		ent->parent = tmp->parent->parent;
		tmp->parent->sib = ent;
	      }
	  }
	/* Next sibling */
	else
	  {
	    tmp->sib = ent;
	    ent->parent = tmp->parent;
	  }
      }
    
    /* Parse attributes */
    do {
      /* First allocation */
      if (attr == NULL)
	{
	  allocattr = allocstep;
	  XALLOC(attr, sizeof(edfmtdw2abbattr_t)*allocattr, -1);
	}
      
      /* We fill all the array, so we realloc all */
      if (attri == allocattr)
	{
	  allocattr += allocstep;
	  XREALLOC(attr, attr, sizeof(edfmtdw2abbattr_t)*allocattr, -1);
	}

      /* Fill attribute structure */
      attr[attri].attr = edfmt_read_uleb128(ptr, &bread);
      inc_pos(abbrev, bread);
      ptr += bread;
      attr[attri].form = edfmt_read_uleb128(ptr, &bread);
      inc_pos(abbrev, bread);
      ptr += bread;
    } while (attr[attri++].attr);
      
    ent->attr = attr;
    
    /* Add into abbrev table */
    edfmt_dwarf2_ckey(ent->ckey, DW2_CKEY_SIZE - 1, ent->key);
    hash_add(&abbrev_table, ent->ckey, (void *) ent);
  } while (dw2_sections.abbrev.pos < dw2_sections.abbrev.size);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Manage parsing loops */
int		edfmt_dwarf2_block_loop(u_int endpos)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);   

  hash_init(&abbrev_table, 30);
  
  /* Parse abbrev table on the appropriate offset */
  edfmt_dwarf2_abbrev();

  /* Print dwarf */
  edfmt_dwarf2_form(endpos);

  hash_destroy(&abbrev_table);
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
