/**
** @file objects.c
** @ingroup librevm_lang
** @brief The implementation of the Lazy Abstract Type System (LATS).
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
** Started on  Tue Feb 08 12:21:12 2005 jfv
**
**
** $Id: convert.c,v 1.8 2007-12-06 05:11:58 may Exp $
**
*/
#include "revm.h"


/** 
 * Convert to string object 
 */
int		revm_convert2str(revmobj_t *obj)
{
  unsigned char	val8;
  u_short	val16;
  unsigned int		val32;
  eresi_Addr	val64;
  char		tmp[30];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (obj->otype->type)
    {
    case ASPECT_TYPE_BYTE:
      val8 = (obj->immed ? obj->immed_val.half : obj->get_obj(obj->parent));
      snprintf(tmp, sizeof(tmp), "%02hhu", val8);
      obj->immed_val.byte = 0;
      obj->immed_val.str = strdup(tmp);
 
      obj->otype = aspect_type_get_by_id(ASPECT_TYPE_STR);
      obj->immed = 1;
      obj->size = strlen(tmp);
      obj->sizelem = 0;
      break;
    case ASPECT_TYPE_SHORT:
      val16 = (obj->immed ? obj->immed_val.half : obj->get_obj(obj->parent));
      snprintf(tmp, sizeof(tmp), "%04hu", val16);
      obj->immed_val.half = 0;
      obj->immed_val.str = strdup(tmp);
 
      obj->otype = aspect_type_get_by_id(ASPECT_TYPE_STR);
      obj->immed = 1;
      obj->size = strlen(tmp);
      obj->sizelem = 0;
      break;
    case ASPECT_TYPE_INT:
      val32 = (obj->immed ? obj->immed_val.word : obj->get_obj(obj->parent));
      snprintf(tmp, sizeof(tmp), "%08u", val32);
      obj->immed_val.word = 0;
      obj->immed_val.str = strdup(tmp);
 
      obj->otype = aspect_type_get_by_id(ASPECT_TYPE_STR);
      obj->immed = 1;
      obj->size = strlen(tmp);
      obj->sizelem = 0;
      break;
    case ASPECT_TYPE_RAW:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Raw -> String is not a valid "
			"conversion", -1);
    case ASPECT_TYPE_LONG:
    case ASPECT_TYPE_CADDR:
    case ASPECT_TYPE_DADDR:
      val64 = (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));
      snprintf(tmp, sizeof(tmp), AFMT, val64);
      obj->immed_val.ent = 0;
      obj->immed_val.str = strdup(tmp);
      obj->otype = aspect_type_get_by_id(ASPECT_TYPE_STR);
      obj->immed = 1;
      obj->size = strlen(tmp);
      obj->sizelem = 0;
    case ASPECT_TYPE_STR:
      break;
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Source type unknown", -1);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** 
 * Convert to 4 bytes object 
 */
int		revm_convert2int(revmobj_t *obj)
{
  eresi_Addr	val64;
  unsigned int		val32;
  u_short	val16;
  unsigned char	val8;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (obj->otype->type)
    {
    case ASPECT_TYPE_BYTE:
      val8 = (unsigned char) (obj->immed ? obj->immed_val.byte : 
		       obj->get_obj(obj->parent));
      obj->immed_val.byte = 0;
      obj->immed_val.word = val8;
      obj->otype = aspect_type_get_by_id(ASPECT_TYPE_INT);
      obj->immed = 1;
      obj->size = 4;
      obj->sizelem = 0;
      break;
    case ASPECT_TYPE_SHORT:
      val16 = (u_short) (obj->immed ? obj->immed_val.half : 
			 obj->get_obj(obj->parent));
      obj->immed_val.half = 0;
      obj->immed_val.word = val16;
      obj->otype = aspect_type_get_by_id(ASPECT_TYPE_INT);
      obj->immed = 1;
      obj->size = 4;
      obj->sizelem = 0;
      break;
    case ASPECT_TYPE_STR:
    case ASPECT_TYPE_RAW:
      val32 = atoi((obj->immed ? obj->immed_val.str : 
		    obj->get_name(obj->root, obj->parent)));
      if (obj->immed && obj->immed_val.str)
	XFREE(__FILE__, __FUNCTION__, __LINE__,obj->immed_val.str);
      obj->immed_val.str = 0;
      obj->immed_val.word = val32;
      obj->otype = aspect_type_get_by_id(ASPECT_TYPE_INT);
      obj->immed = 1;
      obj->size = 4;
      obj->sizelem = 0;
      break;
    case ASPECT_TYPE_LONG:
    case ASPECT_TYPE_CADDR:
    case ASPECT_TYPE_DADDR:
      val64 = (eresi_Addr) (obj->immed ? obj->immed_val.ent : 
			    obj->get_obj(obj->parent));
      obj->immed_val.ent = 0;
      obj->immed_val.word = (int) val64;
      obj->otype = aspect_type_get_by_id(ASPECT_TYPE_INT);
      obj->immed = 1;
      obj->size = 4;
      obj->sizelem = 0;
      break;
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Source type unknown", -1);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * Convert to a long object 
 */
int		revm_convert2addr(revmobj_t *obj, unsigned int type)
{
  eresi_Addr	val64;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  switch (obj->otype->type)
    {
    case ASPECT_TYPE_BYTE:
      val64 = (obj->immed ? obj->immed_val.byte : 
	       obj->get_obj(obj->parent));
      obj->immed_val.word = 0;
      obj->immed_val.ent = val64;
      obj->otype = aspect_type_get_by_id(type);
      obj->immed = 1;
      obj->size = sizeof(eresi_Addr);
      obj->sizelem = 0;
      break;
    case ASPECT_TYPE_SHORT:
      val64 = (obj->immed ? obj->immed_val.half : 
	       obj->get_obj(obj->parent));
      obj->immed_val.half = 0;
      obj->immed_val.ent = val64;
      obj->otype = aspect_type_get_by_id(type);
      obj->immed = 1;
      obj->size = sizeof(eresi_Addr);
      obj->sizelem = 0;
      break;
    case ASPECT_TYPE_STR:
    case ASPECT_TYPE_RAW:
      val64 = atol(obj->immed ? obj->immed_val.str : 
		   obj->get_name(obj->root, obj->parent));
      if (obj->immed && obj->immed_val.str)
	XFREE(__FILE__, __FUNCTION__, __LINE__,obj->immed_val.str);
      obj->immed_val.str = 0;
      obj->immed_val.ent = val64;
      obj->otype = aspect_type_get_by_id(type);
      obj->immed = 1;
      obj->size = sizeof(eresi_Addr);
      obj->sizelem = 0;
      break;
    case ASPECT_TYPE_INT:
      val64 = (obj->immed ? obj->immed_val.word : obj->get_obj(obj->parent));
      obj->immed_val.word = 0;
      obj->immed_val.ent = val64;
      obj->otype = aspect_type_get_by_id(type);
      obj->immed = 1;
      obj->size = sizeof(eresi_Addr);
      obj->sizelem = 0;
      break;
    case ASPECT_TYPE_CADDR:
    case ASPECT_TYPE_DADDR:
    case ASPECT_TYPE_LONG:
      obj->otype = aspect_type_get_by_id(type);
      break;
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Source type unknown", -1);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * Small handlers for same sized types 
 */
int		revm_convert2caddr(revmobj_t *obj)
{
  return (revm_convert2addr(obj, ASPECT_TYPE_CADDR));
}

int		revm_convert2daddr(revmobj_t *obj)
{
  return (revm_convert2addr(obj, ASPECT_TYPE_DADDR));
}

int		revm_convert2long(revmobj_t *obj)
{
  return (revm_convert2addr(obj, ASPECT_TYPE_LONG));
}




/** 
 * Convert to a raw data object 
 */
int		revm_convert2raw(revmobj_t *obj)
{
  unsigned char	val8;
  u_short	val16;
  unsigned int		val32;
  eresi_Addr	val64;
  char		*str;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  switch (obj->otype->type)
    {
    case ASPECT_TYPE_BYTE:
      val8 = (obj->immed ? obj->immed_val.word : obj->get_obj(obj->parent));
      XALLOC(__FILE__, __FUNCTION__, __LINE__,obj->immed_val.str, 2, -1);
      *obj->immed_val.str = val8;
      obj->otype = aspect_type_get_by_id(ASPECT_TYPE_RAW);
      obj->immed = 1;
      obj->size = 1;
      obj->sizelem = 1;
      break;
    case ASPECT_TYPE_STR:
      str = (obj->immed ? obj->immed_val.str : 
	     obj->get_name(obj->root, obj->parent));
      XREALLOC(__FILE__, __FUNCTION__, __LINE__,obj->immed_val.str, obj->immed_val.str, obj->size, -1);
      memcpy(obj->immed_val.str, str, obj->size);
      obj->otype = aspect_type_get_by_id(ASPECT_TYPE_RAW);
      obj->immed = 1;
      //obj->size; No size change
      obj->sizelem = 0;
      break;
    case ASPECT_TYPE_SHORT:
      val16 = (obj->immed ? obj->immed_val.word : obj->get_obj(obj->parent));
      XALLOC(__FILE__, __FUNCTION__, __LINE__,obj->immed_val.str, sizeof(val16) + 1, -1);
      memcpy(obj->immed_val.str, &val16, sizeof(val16));	// FIXME: Take care of endianess !
      obj->otype = aspect_type_get_by_id(ASPECT_TYPE_RAW);
      obj->immed = 1;
      obj->size = 2;
      obj->sizelem = 0;
      break;
    case ASPECT_TYPE_INT:
      val32 = (obj->immed ? obj->immed_val.word : obj->get_obj(obj->parent));
      XALLOC(__FILE__, __FUNCTION__, __LINE__,obj->immed_val.str, sizeof(val32) + 1, -1);
      memcpy(obj->immed_val.str, &val32, sizeof(val32));	// FIXME: Take care of endianess !
      obj->otype = aspect_type_get_by_id(ASPECT_TYPE_RAW);
      obj->immed = 1;
      obj->size = 4;
      obj->sizelem = 0;
      break;
    case ASPECT_TYPE_LONG:
    case ASPECT_TYPE_CADDR:
    case ASPECT_TYPE_DADDR:
      val64 = (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));
      XALLOC(__FILE__, __FUNCTION__, __LINE__,obj->immed_val.str, sizeof(val64) + 1, -1);
      memcpy(obj->immed_val.str, &val64, sizeof(val64)); // FIXME: Take care of endianess !
      obj->otype = aspect_type_get_by_id(ASPECT_TYPE_RAW);
      obj->immed = 1;
      obj->size = sizeof(eresi_Addr);
      obj->sizelem = 0;
      break;
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Source type unknown", -1);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** 
 * Convert to a raw data object 
 */
int		revm_convert2byte(revmobj_t *obj)
{
  unsigned char	val8;
  u_short	val16;
  int		val32;
  eresi_Addr	val64;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  switch (obj->otype->type)
    {
    case ASPECT_TYPE_SHORT:
      val16 = (u_short) (obj->immed ? obj->immed_val.half : 
			 obj->get_obj(obj->parent));
      obj->immed_val.half = 0;
      obj->immed_val.byte = (unsigned char) val16;
      obj->otype = aspect_type_get_by_id(ASPECT_TYPE_BYTE);
      obj->immed = 1;
      obj->size = 1;
      obj->sizelem = 0;
      break;
    case ASPECT_TYPE_RAW:
    case ASPECT_TYPE_STR:
      val8 = *(obj->immed ? obj->immed_val.str : obj->get_name(obj->root, obj->parent));
      if (obj->immed && obj->immed_val.str)
	XFREE(__FILE__, __FUNCTION__, __LINE__,obj->immed_val.str);
      obj->immed_val.str = 0;
      obj->immed_val.byte = val8;
      obj->otype = aspect_type_get_by_id(ASPECT_TYPE_BYTE);
      obj->immed = 1;
      obj->size = 1;
      obj->sizelem = 0;
      break;
    case ASPECT_TYPE_INT:
      val32 = (unsigned int) (obj->immed ? obj->immed_val.word : 
		       obj->get_obj(obj->parent));
      obj->immed_val.word = 0;
      obj->immed_val.byte = (unsigned char) val32;
      obj->otype = aspect_type_get_by_id(ASPECT_TYPE_BYTE);
      obj->immed = 1;
      obj->size = 1;
      obj->sizelem = 0;
      break;
    case ASPECT_TYPE_LONG:
    case ASPECT_TYPE_CADDR:
    case ASPECT_TYPE_DADDR:
      val64 = (eresi_Addr) (obj->immed ? obj->immed_val.ent : 
			    obj->get_obj(obj->parent));
      obj->immed_val.ent = 0;
      obj->immed_val.byte = (unsigned char) val64;
      obj->otype = aspect_type_get_by_id(ASPECT_TYPE_BYTE);
      obj->immed = 1;
      obj->size = 1;
      obj->sizelem = 0;
      break;
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Source type unknown", -1);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** 
 * Convert to a raw data object 
*/
int		revm_convert2short(revmobj_t *obj)
{
  unsigned char	val8;
  u_short	val16;
  int		val32;
  eresi_Addr	val64;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  switch (obj->otype->type)
    {
    case ASPECT_TYPE_BYTE:
      val8 = (unsigned char) (obj->immed ? obj->immed_val.byte : 
		       obj->get_obj(obj->parent));;
      obj->immed_val.byte = 0;
      obj->immed_val.half = (u_short) val8;
      obj->otype = aspect_type_get_by_id(ASPECT_TYPE_SHORT);
      obj->immed = 1;
      obj->size = 2;
      obj->sizelem = 0;
      break;
    case ASPECT_TYPE_RAW:
    case ASPECT_TYPE_STR:
      val16 = atoi((obj->immed ? obj->immed_val.str : 
		    obj->get_name(obj->root, obj->parent)));
      if (obj->immed && obj->immed_val.str)
	XFREE(__FILE__, __FUNCTION__, __LINE__,obj->immed_val.str);
      obj->immed_val.str = 0;
      obj->immed_val.half = val16;
      obj->otype = aspect_type_get_by_id(ASPECT_TYPE_SHORT);
      obj->immed = 1;
      obj->size = 2;
      obj->sizelem = 0;
      break;
    case ASPECT_TYPE_INT:
      val32 = (unsigned int) (obj->immed ? obj->immed_val.word : 
		       obj->get_obj(obj->parent));
      obj->immed_val.word = 0;
      obj->immed_val.half = (u_short) val32;
      obj->otype = aspect_type_get_by_id(ASPECT_TYPE_SHORT);
      obj->immed = 1;
      obj->size = 2;
      obj->sizelem = 0;
      break;
    case ASPECT_TYPE_LONG:
    case ASPECT_TYPE_CADDR:
    case ASPECT_TYPE_DADDR:
      val64 = (eresi_Addr) (obj->immed ? obj->immed_val.ent : 
			    obj->get_obj(obj->parent));
      obj->immed_val.ent = 0;
      obj->immed_val.half = (u_short) val64;
      obj->otype = aspect_type_get_by_id(ASPECT_TYPE_SHORT);
      obj->immed = 1;
      obj->size = 2;
      obj->sizelem = 0;
      break;
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Source type unknown", -1);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

