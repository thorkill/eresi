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
#include "elfsh.h"



/* Convert to string object */
int		vm_convert2str(elfshpath_t *obj)
{
  u_char	val8;
  u_short	val16;
  u_int		val32;
  elfsh_Addr	val64;
  char		tmp[30];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (obj->type)
    {
    case ELFSH_OBJBYTE:
      val8 = (obj->immed ? obj->immed_val.half : obj->get_obj(obj->parent));
      snprintf(tmp, sizeof(tmp), "%hhd", val8);
      obj->immed_val.byte = 0;
      obj->immed_val.str = strdup(tmp);
      obj->type = ELFSH_OBJSTR;
      obj->immed = 1;
      obj->size = strlen(tmp);
      obj->sizelem = 0;
      break;
    case ELFSH_OBJSHORT:
      val16 = (obj->immed ? obj->immed_val.half : obj->get_obj(obj->parent));
      snprintf(tmp, sizeof(tmp), "%hd", val16);
      obj->immed_val.half = 0;
      obj->immed_val.str = strdup(tmp);
      obj->type = ELFSH_OBJSTR;
      obj->immed = 1;
      obj->size = strlen(tmp);
      obj->sizelem = 0;
      break;
    case ELFSH_OBJINT:
      val32 = (obj->immed ? obj->immed_val.word : obj->get_obj(obj->parent));
      snprintf(tmp, sizeof(tmp), "%d", val32);
      obj->immed_val.word = 0;
      obj->immed_val.str = strdup(tmp);
      obj->type = ELFSH_OBJSTR;
      obj->immed = 1;
      obj->size = strlen(tmp);
      obj->sizelem = 0;
      break;
    case ELFSH_OBJRAW:
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Raw -> String is not a valid "
			"conversion", -1);
    case ELFSH_OBJLONG:
      val64 = (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));
      snprintf(tmp, sizeof(tmp), XFMT, val64);
      obj->immed_val.ent = 0;
      obj->immed_val.str = strdup(tmp);
      obj->type = ELFSH_OBJSTR;
      obj->immed = 1;
      obj->size = strlen(tmp);
      obj->sizelem = 0;
    case ELFSH_OBJSTR:
      break;
    default:
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Source type unknown", -1);
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Convert to 4 bytes object */
int		vm_convert2int(elfshpath_t *obj)
{
  elfsh_Addr	val64;
  u_int		val32;
  u_short	val16;
  u_char	val8;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (obj->type)
    {
    case ELFSH_OBJBYTE:
      val8 = (u_char) (obj->immed ? obj->immed_val.byte : obj->get_obj(obj->parent));
      obj->immed_val.byte = 0;
      obj->immed_val.word = val8;
      obj->type = ELFSH_OBJINT;
      obj->immed = 1;
      obj->size = 4;
      obj->sizelem = 0;
      break;
    case ELFSH_OBJSHORT:
      val16 = (u_short) (obj->immed ? obj->immed_val.half : obj->get_obj(obj->parent));
      obj->immed_val.half = 0;
      obj->immed_val.word = val16;
      obj->type = ELFSH_OBJINT;
      obj->immed = 1;
      obj->size = 4;
      obj->sizelem = 0;
      break;
    case ELFSH_OBJSTR:
    case ELFSH_OBJRAW:
      val32 = atoi((obj->immed ? obj->immed_val.str : obj->get_name(obj->root, obj->parent)));
      if (obj->immed && obj->immed_val.str)
	XFREE(obj->immed_val.str);
      obj->immed_val.str = 0;
      obj->immed_val.word = val32;
      obj->type = ELFSH_OBJINT;
      obj->immed = 1;
      obj->size = 4;
      obj->sizelem = 0;
      break;
    case ELFSH_OBJLONG:
      val64 = (elfsh_Addr) (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));
      obj->immed_val.ent = 0;
      obj->immed_val.word = (int) val64;
      obj->type = ELFSH_OBJINT;
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
int		vm_convert2long(elfshpath_t *obj)
{
  elfsh_Addr	val64;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  switch (obj->type)
    {
    case ELFSH_OBJBYTE:
      val64 = (obj->immed ? obj->immed_val.byte : obj->get_obj(obj->parent));
      obj->immed_val.word = 0;
      obj->immed_val.ent = val64;
      obj->type = ELFSH_OBJLONG;
      obj->immed = 1;
      obj->size = sizeof(elfsh_Addr);
      obj->sizelem = 0;
      break;
    case ELFSH_OBJSHORT:
      val64 = (obj->immed ? obj->immed_val.half : obj->get_obj(obj->parent));
      obj->immed_val.half = 0;
      obj->immed_val.ent = val64;
      obj->type = ELFSH_OBJLONG;
      obj->immed = 1;
      obj->size = sizeof(elfsh_Addr);
      obj->sizelem = 0;
      break;
    case ELFSH_OBJSTR:
    case ELFSH_OBJRAW:
      val64 = atol(obj->immed ? obj->immed_val.str : obj->get_name(obj->root, obj->parent));
      if (obj->immed && obj->immed_val.str)
	XFREE(obj->immed_val.str);
      obj->immed_val.str = 0;
      obj->immed_val.ent = val64;
      obj->type = ELFSH_OBJLONG;
      obj->immed = 1;
      obj->size = sizeof(elfsh_Addr);
      obj->sizelem = 0;
      break;
    case ELFSH_OBJINT:
      val64 = (obj->immed ? obj->immed_val.word : obj->get_obj(obj->parent));
      obj->immed_val.word = 0;
      obj->immed_val.ent = val64;
      obj->type = ELFSH_OBJLONG;
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
int		vm_convert2raw(elfshpath_t *obj)
{
  u_char	val8;
  u_short	val16;
  u_int		val32;
  elfsh_Addr	val64;
  char		*str;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  switch (obj->type)
    {
    case ELFSH_OBJBYTE:
      val8 = (obj->immed ? obj->immed_val.word : obj->get_obj(obj->parent));
      XALLOC(obj->immed_val.str, 2, -1);
      *obj->immed_val.str = val8;
      obj->type = ELFSH_OBJRAW;
      obj->immed = 1;
      obj->size = 1;
      obj->sizelem = 1;
      break;
    case ELFSH_OBJSTR:
      str = (obj->immed ? obj->immed_val.str : obj->get_name(obj->root, obj->parent));
      XREALLOC(obj->immed_val.str, obj->immed_val.str, obj->size, -1);
      memcpy(obj->immed_val.str, str, obj->size);
      obj->type = ELFSH_OBJRAW;
      obj->immed = 1;
      //obj->size; No size change
      obj->sizelem = 0;
      break;
    case ELFSH_OBJSHORT:
      val16 = (obj->immed ? obj->immed_val.word : obj->get_obj(obj->parent));
      XALLOC(obj->immed_val.str, sizeof(val16) + 1, -1);
      memcpy(obj->immed_val.str, &val16, sizeof(val16));	// FIXME: Take care of endianess !
      obj->type = ELFSH_OBJRAW;
      obj->immed = 1;
      obj->size = 2;
      obj->sizelem = 0;
      break;
    case ELFSH_OBJINT:
      val32 = (obj->immed ? obj->immed_val.word : obj->get_obj(obj->parent));
      XALLOC(obj->immed_val.str, sizeof(val32) + 1, -1);
      memcpy(obj->immed_val.str, &val32, sizeof(val32));	// FIXME: Take care of endianess !
      obj->type = ELFSH_OBJRAW;
      obj->immed = 1;
      obj->size = 4;
      obj->sizelem = 0;
      break;
    case ELFSH_OBJLONG:
      val64 = (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));
      XALLOC(obj->immed_val.str, sizeof(val64) + 1, -1);
      memcpy(obj->immed_val.str, &val64, sizeof(val64)); // FIXME: Take care of endianess !
      obj->type = ELFSH_OBJRAW;
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
int		vm_convert2byte(elfshpath_t *obj)
{
  u_char	val8;
  u_short	val16;
  int		val32;
  elfsh_Addr	val64;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  switch (obj->type)
    {
    case ELFSH_OBJSHORT:
      val16 = (u_short) (obj->immed ? obj->immed_val.half : obj->get_obj(obj->parent));
      obj->immed_val.half = 0;
      obj->immed_val.byte = (u_char) val16;
      obj->type = ELFSH_OBJBYTE;
      obj->immed = 1;
      obj->size = 1;
      obj->sizelem = 0;
      break;
    case ELFSH_OBJRAW:
    case ELFSH_OBJSTR:
      val8 = atoi((obj->immed ? obj->immed_val.str : obj->get_name(obj->root, obj->parent)));
      if (obj->immed && obj->immed_val.str)
	XFREE(obj->immed_val.str);
      obj->immed_val.str = 0;
      obj->immed_val.byte = val8;
      obj->type = ELFSH_OBJBYTE;
      obj->immed = 1;
      obj->size = 1;
      obj->sizelem = 0;
      break;
    case ELFSH_OBJINT:
      val32 = (u_int) (obj->immed ? obj->immed_val.word : obj->get_obj(obj->parent));
      obj->immed_val.word = 0;
      obj->immed_val.byte = (u_char) val32;
      obj->type = ELFSH_OBJBYTE;
      obj->immed = 1;
      obj->size = 1;
      obj->sizelem = 0;
      break;
    case ELFSH_OBJLONG:
      val64 = (elfsh_Addr) (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));
      obj->immed_val.ent = 0;
      obj->immed_val.byte = (u_char) val64;
      obj->type = ELFSH_OBJBYTE;
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
int		vm_convert2short(elfshpath_t *obj)
{
  u_char	val8;
  u_short	val16;
  int		val32;
  elfsh_Addr	val64;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  switch (obj->type)
    {
    case ELFSH_OBJBYTE:
      val8 = (u_char) (obj->immed ? obj->immed_val.byte : obj->get_obj(obj->parent));;
      obj->immed_val.byte = 0;
      obj->immed_val.half = (u_short) val8;
      obj->type = ELFSH_OBJSHORT;
      obj->immed = 1;
      obj->size = 2;
      obj->sizelem = 0;
      break;
    case ELFSH_OBJRAW:
    case ELFSH_OBJSTR:
      val16 = atoi((obj->immed ? obj->immed_val.str : obj->get_name(obj->root, obj->parent)));
      if (obj->immed && obj->immed_val.str)
	XFREE(obj->immed_val.str);
      obj->immed_val.str = 0;
      obj->immed_val.half = val16;
      obj->type = ELFSH_OBJSHORT;
      obj->immed = 1;
      obj->size = 2;
      obj->sizelem = 0;
      break;
    case ELFSH_OBJINT:
      val32 = (u_int) (obj->immed ? obj->immed_val.word : obj->get_obj(obj->parent));
      obj->immed_val.word = 0;
      obj->immed_val.half = (u_short) val32;
      obj->type = ELFSH_OBJSHORT;
      obj->immed = 1;
      obj->size = 2;
      obj->sizelem = 0;
      break;
    case ELFSH_OBJLONG:
      val64 = (elfsh_Addr) (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));
      obj->immed_val.ent = 0;
      obj->immed_val.half = (u_short) val64;
      obj->type = ELFSH_OBJSHORT;
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

