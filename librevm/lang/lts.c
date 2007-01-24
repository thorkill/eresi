/*
** objects.c for elfsh
**
** The implementation of the Lazy Abstract Type System (LATS)
**
** It is the first rewrite of the type-system, in order to
** give it a really structured shape with clear type conversion
** rules. Byte, Short, and Long types are now available and all
** values and ELF fields are now encoded on their exact size
** in bits. 
** 
** This has also lead to more flexibility in the shell since 
** more conversion are now availables and all (cross-)endianess 
** is now transparent to the end-user. All of that was necessary
** for the 32/64bits support anyway.
**
** See table.c for a hint on how to use the object constructors 
**     objects.c for the object constructors themself
**
** Started on  Tue Feb 08 12:21:12 2005 mayhem
**
*/
#include "revm.h"



/* Convert to string object */
int		vm_convert2str(revmobj_t *obj)
{
  u_char	val8;
  u_short	val16;
  u_int		val32;
  elfsh_Addr	val64;
  char		tmp[30];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (obj->type)
    {
    case REVM_TYPE_BYTE:
      val8 = (obj->immed ? obj->immed_val.half : obj->get_obj(obj->parent));
      snprintf(tmp, sizeof(tmp), "%hhd", val8);
      obj->immed_val.byte = 0;
      obj->immed_val.str = elfsh_strdup(tmp);
      obj->type = REVM_TYPE_STR;
      obj->immed = 1;
      obj->size = strlen(tmp);
      obj->sizelem = 0;
      break;
    case REVM_TYPE_SHORT:
      val16 = (obj->immed ? obj->immed_val.half : obj->get_obj(obj->parent));
      snprintf(tmp, sizeof(tmp), "%hd", val16);
      obj->immed_val.half = 0;
      obj->immed_val.str = elfsh_strdup(tmp);
      obj->type = REVM_TYPE_STR;
      obj->immed = 1;
      obj->size = strlen(tmp);
      obj->sizelem = 0;
      break;
    case REVM_TYPE_INT:
      val32 = (obj->immed ? obj->immed_val.word : obj->get_obj(obj->parent));
      snprintf(tmp, sizeof(tmp), "%d", val32);
      obj->immed_val.word = 0;
      obj->immed_val.str = elfsh_strdup(tmp);
      obj->type = REVM_TYPE_STR;
      obj->immed = 1;
      obj->size = strlen(tmp);
      obj->sizelem = 0;
      break;
    case REVM_TYPE_RAW:
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Raw -> String is not a valid "
			"conversion", -1);
    case REVM_TYPE_LONG:
      val64 = (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));
      snprintf(tmp, sizeof(tmp), XFMT, val64);
      obj->immed_val.ent = 0;
      obj->immed_val.str = elfsh_strdup(tmp);
      obj->type = REVM_TYPE_STR;
      obj->immed = 1;
      obj->size = strlen(tmp);
      obj->sizelem = 0;
    case REVM_TYPE_STR:
      break;
    default:
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Source type unknown", -1);
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Convert to 4 bytes object */
int		vm_convert2int(revmobj_t *obj)
{
  elfsh_Addr	val64;
  u_int		val32;
  u_short	val16;
  u_char	val8;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (obj->type)
    {
    case REVM_TYPE_BYTE:
      val8 = (u_char) (obj->immed ? obj->immed_val.byte : obj->get_obj(obj->parent));
      obj->immed_val.byte = 0;
      obj->immed_val.word = val8;
      obj->type = REVM_TYPE_INT;
      obj->immed = 1;
      obj->size = 4;
      obj->sizelem = 0;
      break;
    case REVM_TYPE_SHORT:
      val16 = (u_short) (obj->immed ? obj->immed_val.half : obj->get_obj(obj->parent));
      obj->immed_val.half = 0;
      obj->immed_val.word = val16;
      obj->type = REVM_TYPE_INT;
      obj->immed = 1;
      obj->size = 4;
      obj->sizelem = 0;
      break;
    case REVM_TYPE_STR:
    case REVM_TYPE_RAW:
      val32 = atoi((obj->immed ? obj->immed_val.str : obj->get_name(obj->root, obj->parent)));
      if (obj->immed && obj->immed_val.str)
	XFREE(obj->immed_val.str);
      obj->immed_val.str = 0;
      obj->immed_val.word = val32;
      obj->type = REVM_TYPE_INT;
      obj->immed = 1;
      obj->size = 4;
      obj->sizelem = 0;
      break;
    case REVM_TYPE_LONG:
      val64 = (elfsh_Addr) (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));
      obj->immed_val.ent = 0;
      obj->immed_val.word = (int) val64;
      obj->type = REVM_TYPE_INT;
      obj->immed = 1;
      obj->size = 4;
      obj->sizelem = 0;
      break;
    default:
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Source type unknown", -1);
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Convert to a long object */
int		vm_convert2long(revmobj_t *obj)
{
  elfsh_Addr	val64;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  switch (obj->type)
    {
    case REVM_TYPE_BYTE:
      val64 = (obj->immed ? obj->immed_val.byte : obj->get_obj(obj->parent));
      obj->immed_val.word = 0;
      obj->immed_val.ent = val64;
      obj->type = REVM_TYPE_LONG;
      obj->immed = 1;
      obj->size = sizeof(elfsh_Addr);
      obj->sizelem = 0;
      break;
    case REVM_TYPE_SHORT:
      val64 = (obj->immed ? obj->immed_val.half : obj->get_obj(obj->parent));
      obj->immed_val.half = 0;
      obj->immed_val.ent = val64;
      obj->type = REVM_TYPE_LONG;
      obj->immed = 1;
      obj->size = sizeof(elfsh_Addr);
      obj->sizelem = 0;
      break;
    case REVM_TYPE_STR:
    case REVM_TYPE_RAW:
      val64 = atol(obj->immed ? obj->immed_val.str : obj->get_name(obj->root, obj->parent));
      if (obj->immed && obj->immed_val.str)
	XFREE(obj->immed_val.str);
      obj->immed_val.str = 0;
      obj->immed_val.ent = val64;
      obj->type = REVM_TYPE_LONG;
      obj->immed = 1;
      obj->size = sizeof(elfsh_Addr);
      obj->sizelem = 0;
      break;
    case REVM_TYPE_INT:
      val64 = (obj->immed ? obj->immed_val.word : obj->get_obj(obj->parent));
      obj->immed_val.word = 0;
      obj->immed_val.ent = val64;
      obj->type = REVM_TYPE_LONG;
      obj->immed = 1;
      obj->size = sizeof(elfsh_Addr);
      obj->sizelem = 0;
      break;
    default:
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Source type unknown", -1);
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Convert to a raw data object */
int		vm_convert2raw(revmobj_t *obj)
{
  u_char	val8;
  u_short	val16;
  u_int		val32;
  elfsh_Addr	val64;
  char		*str;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  switch (obj->type)
    {
    case REVM_TYPE_BYTE:
      val8 = (obj->immed ? obj->immed_val.word : obj->get_obj(obj->parent));
      XALLOC(obj->immed_val.str, 2, -1);
      *obj->immed_val.str = val8;
      obj->type = REVM_TYPE_RAW;
      obj->immed = 1;
      obj->size = 1;
      obj->sizelem = 1;
      break;
    case REVM_TYPE_STR:
      str = (obj->immed ? obj->immed_val.str : obj->get_name(obj->root, obj->parent));
      XREALLOC(obj->immed_val.str, obj->immed_val.str, obj->size, -1);
      memcpy(obj->immed_val.str, str, obj->size);
      obj->type = REVM_TYPE_RAW;
      obj->immed = 1;
      //obj->size; No size change
      obj->sizelem = 0;
      break;
    case REVM_TYPE_SHORT:
      val16 = (obj->immed ? obj->immed_val.word : obj->get_obj(obj->parent));
      XALLOC(obj->immed_val.str, sizeof(val16) + 1, -1);
      memcpy(obj->immed_val.str, &val16, sizeof(val16));	// FIXME: Take care of endianess !
      obj->type = REVM_TYPE_RAW;
      obj->immed = 1;
      obj->size = 2;
      obj->sizelem = 0;
      break;
    case REVM_TYPE_INT:
      val32 = (obj->immed ? obj->immed_val.word : obj->get_obj(obj->parent));
      XALLOC(obj->immed_val.str, sizeof(val32) + 1, -1);
      memcpy(obj->immed_val.str, &val32, sizeof(val32));	// FIXME: Take care of endianess !
      obj->type = REVM_TYPE_RAW;
      obj->immed = 1;
      obj->size = 4;
      obj->sizelem = 0;
      break;
    case REVM_TYPE_LONG:
      val64 = (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));
      XALLOC(obj->immed_val.str, sizeof(val64) + 1, -1);
      memcpy(obj->immed_val.str, &val64, sizeof(val64)); // FIXME: Take care of endianess !
      obj->type = REVM_TYPE_RAW;
      obj->immed = 1;
      obj->size = sizeof(elfsh_Addr);
      obj->sizelem = 0;
      break;
    default:
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Source type unknown", -1);
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Convert to a raw data object */
int		vm_convert2byte(revmobj_t *obj)
{
  u_char	val8;
  u_short	val16;
  int		val32;
  elfsh_Addr	val64;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  switch (obj->type)
    {
    case REVM_TYPE_SHORT:
      val16 = (u_short) (obj->immed ? obj->immed_val.half : obj->get_obj(obj->parent));
      obj->immed_val.half = 0;
      obj->immed_val.byte = (u_char) val16;
      obj->type = REVM_TYPE_BYTE;
      obj->immed = 1;
      obj->size = 1;
      obj->sizelem = 0;
      break;
    case REVM_TYPE_RAW:
    case REVM_TYPE_STR:
      val8 = atoi((obj->immed ? obj->immed_val.str : obj->get_name(obj->root, obj->parent)));
      if (obj->immed && obj->immed_val.str)
	XFREE(obj->immed_val.str);
      obj->immed_val.str = 0;
      obj->immed_val.byte = val8;
      obj->type = REVM_TYPE_BYTE;
      obj->immed = 1;
      obj->size = 1;
      obj->sizelem = 0;
      break;
    case REVM_TYPE_INT:
      val32 = (u_int) (obj->immed ? obj->immed_val.word : obj->get_obj(obj->parent));
      obj->immed_val.word = 0;
      obj->immed_val.byte = (u_char) val32;
      obj->type = REVM_TYPE_BYTE;
      obj->immed = 1;
      obj->size = 1;
      obj->sizelem = 0;
      break;
    case REVM_TYPE_LONG:
      val64 = (elfsh_Addr) (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));
      obj->immed_val.ent = 0;
      obj->immed_val.byte = (u_char) val64;
      obj->type = REVM_TYPE_BYTE;
      obj->immed = 1;
      obj->size = 1;
      obj->sizelem = 0;
      break;
    default:
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Source type unknown", -1);
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Convert to a raw data object */
int		vm_convert2short(revmobj_t *obj)
{
  u_char	val8;
  u_short	val16;
  int		val32;
  elfsh_Addr	val64;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  switch (obj->type)
    {
    case REVM_TYPE_BYTE:
      val8 = (u_char) (obj->immed ? obj->immed_val.byte : obj->get_obj(obj->parent));;
      obj->immed_val.byte = 0;
      obj->immed_val.half = (u_short) val8;
      obj->type = REVM_TYPE_SHORT;
      obj->immed = 1;
      obj->size = 2;
      obj->sizelem = 0;
      break;
    case REVM_TYPE_RAW:
    case REVM_TYPE_STR:
      val16 = atoi((obj->immed ? obj->immed_val.str : obj->get_name(obj->root, obj->parent)));
      if (obj->immed && obj->immed_val.str)
	XFREE(obj->immed_val.str);
      obj->immed_val.str = 0;
      obj->immed_val.half = val16;
      obj->type = REVM_TYPE_SHORT;
      obj->immed = 1;
      obj->size = 2;
      obj->sizelem = 0;
      break;
    case REVM_TYPE_INT:
      val32 = (u_int) (obj->immed ? obj->immed_val.word : obj->get_obj(obj->parent));
      obj->immed_val.word = 0;
      obj->immed_val.half = (u_short) val32;
      obj->type = REVM_TYPE_SHORT;
      obj->immed = 1;
      obj->size = 2;
      obj->sizelem = 0;
      break;
    case REVM_TYPE_LONG:
      val64 = (elfsh_Addr) (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));
      obj->immed_val.ent = 0;
      obj->immed_val.half = (u_short) val64;
      obj->type = REVM_TYPE_SHORT;
      obj->immed = 1;
      obj->size = 2;
      obj->sizelem = 0;
      break;
    default:
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Source type unknown", -1);
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

