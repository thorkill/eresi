/*
** color.c for elfsh
**
** All functions about colors
**
** Started on Sept 11 2005 mxatone
*/
#include "libui.h"


/* Token system */
char 		tokens[COLOR_TOKENS][COLOR_TOKEN_LEN];
u_int 		curtok = 0;
u_int 		nocolor = 1;

/* Init color structure */
color_t 	*vm_colorblank()
{
  color_t 	*c;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(c, sizeof(color_t), NULL);

  c->fground = COLOR_NONE;
  c->bground = COLOR_NONE;
  c->bold = COLOR_NONE;
  c->underline = COLOR_NONE;

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (c));
}

color_t 	*vm_colortable(char *type, char *text)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Override color by warning color if we match the alert regex */
  if (world.state.vm_use_alert && 
      regexec(&world.state.vm_alert, text, 0, 0, 0) == NULL)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		       hash_get(&t_color_hash, "warnstring"));

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     hash_get(&t_color_hash, type)); 
}

/* Speed setup code*/
#define COLOR_SET_ELEMENT(_c, _v, _z) 					\
do {									\
if (_c) { 								\
snprintf(_v, 15, "%s%u", (!set ? S_STARTCOLOR : COLOR_SEPARE), _z); 	\
set = 1; } 								\
} while(0)

int		vm_colorpattern(color_t *t, char *text, char *pattern)
{
  char		bo[16], ul[16], fg[16], bg[16];
  u_short      	set = 0;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (t->bground == NULL && t->fground == NULL && 
      t->bold == NULL && t->underline == NULL)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);  

  /* Set every element */
  COLOR_SET_ELEMENT(t->bold, bo, COLOR_BOLD);
  COLOR_SET_ELEMENT(t->underline, ul, COLOR_UNDERLINE);
  COLOR_SET_ELEMENT(t->fground, fg, t->fground);
  COLOR_SET_ELEMENT(t->bground, bg, t->bground);

  snprintf(pattern, COLOR_TOKEN_LEN - 1,
	   "%%s%s%s%s%sm%%s%s%um%%s",
	   (t->bold != NULL ? bo : ""),
	   (t->underline != NULL ? ul : ""),
	   (t->fground != NULL ? fg : ""),
	   (t->bground != NULL ? bg : ""),
	   S_STARTCOLOR,
	   COLOR_NONE);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Return without color */
static char	*vm_colornothing(char *sp, void *object)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!strcmp(sp, "%s"))
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ((char*) object));

  if (strchr(sp, 's') == NULL)
    snprintf(tokens[curtok], COLOR_TOKEN_LEN - 1, sp, *(long *) object);
  else
    snprintf(tokens[curtok], COLOR_TOKEN_LEN - 1, sp, object);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, tokens[curtok++]);
}

/* Trim a string from blank char and copy it on to argument */
static int	trim(char *from, char *to, u_int size, char *start, char *end)
{
  u_int		len, istart, iend;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  if (size == 0 || size > COLOR_TOKEN_LEN || from == NULL || to == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", -1);

  len = strlen(from);

  /* Speed check */
  if (!IS_BLANK(from[0]) && !IS_BLANK(from[len]))
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);

  /* Before */
  for (istart = 0; istart < len && IS_BLANK(from[istart]); istart++);

  /* All blank, no modifications */
  if (istart == len)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);

  /* After */
  for (iend = len; iend > 0 && IS_BLANK(from[iend]); iend--);

  iend = len - iend;

  /* Copy the right char on to argument */
  strncpy(to, from + istart, len - istart - iend);
  to[len - istart - iend] = 0x00;

  if (start)
    {
      strncpy(start, from, istart);
      start[istart] = 0x00;
    }

  if (end)
    {
      strncpy(end, from + len - iend, iend);
      end[iend] = 0x00;
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Build color text */
char 		*vm_colorget(char *sp, char *type, void *object)
{
  color_t 	*t;
  char		pattern[COLOR_TOKEN_LEN];
  char		text[COLOR_TOKEN_LEN];
  char		trim_text[COLOR_TOKEN_LEN];
  char		white_s[COLOR_TOKEN_LEN];
  char		white_e[COLOR_TOKEN_LEN];
  char		*pText;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (curtok >= COLOR_TOKENS) 
    {
      printf("WARNING !!!! Token overflow (val:%u)\n", curtok);
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Token overflow", NULL);
    }

  //snprintf(text, COLOR_TOKEN_LEN - 1, sp, object);
  if (strchr(sp, 's') == NULL)
    snprintf(text, COLOR_TOKEN_LEN - 1, sp, *(long *) object);
  else
    snprintf(text, COLOR_TOKEN_LEN - 1, sp, object);

  /* Color isn't activated */
  if (!nocolor)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		       vm_colornothing(sp, object));

  t = vm_colortable(type, text);

  /* Color not found */
  if (t == NULL)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		       vm_colornothing(sp, object));

  /* Invalid pattern */
  if (vm_colorpattern(t, text, pattern) != 0)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		       vm_colornothing(sp, object));

  pText = text;

  memset(white_s, 0x00, COLOR_TOKEN_LEN);
  memset(white_e, 0x00, COLOR_TOKEN_LEN);

  /* Trim the string from blank char */
  if (!trim(text, trim_text, COLOR_TOKEN_LEN, white_s, white_e))
      pText = trim_text;

  snprintf(tokens[curtok], COLOR_TOKEN_LEN - 1, 
	   pattern, white_s, pText, white_e);

  //printf("curtok = %d\n", curtok);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, tokens[curtok++]);
}

/* Reset token */
void 		vm_endline()
{
  curtok = 0;
}

/* Simple functions */
char *vm_colorinstr(char *text) 	{ return vm_colorget("%s", "instr"     , text); }

char *vm_colorstr(char *text) 		{ return vm_colorget("%s", "string"     , text); }
char *vm_colorfieldstr(char *text) 	{ return vm_colorget("%s", "fieldstring", text); }
char *vm_colortypestr(char *text) 	{ return vm_colorget("%s", "typestring" , text); }
char *vm_colorend(char *text) 		{ return vm_colorget("%s", "endstring"  , text); }
char *vm_colorwarn(char *text) 		{ return vm_colorget("%s", "warnstring ", text); }

/* Special functions */
char 		*vm_coloradv(char *type, char *pattern, char *text)
{
  char 		*p;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  p = vm_colorget(pattern, type, text); 

  strncpy(text, p, BUFSIZ);
  text[BUFSIZ-1] = 0;
  curtok--;

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (text));
}

/* Advanced functions */
char *vm_colorinstr_fmt(char *pattern, char *text)		{ return vm_colorget(pattern, "instr"      , text); }
char *vm_colorstr_fmt(char *pattern, char *text) 		{ return vm_colorget(pattern, "string"     , text); }
char *vm_colorfieldstr_fmt(char *pattern, char *text) 		{ return vm_colorget(pattern, "fieldstring", text); }
char *vm_colortypestr_fmt(char *pattern, char *text) 		{ return vm_colorget(pattern, "typestring" , text); }
char *vm_colorend_fmt(char *pattern, char *text) 		{ return vm_colorget(pattern, "endstring"  , text); }
char *vm_colorwarn_fmt(char *pattern, char *text) 		{ return vm_colorget(pattern, "warnstring" , text); }
char *vm_coloraddress(char *pattern, elfsh_Addr addr) 		{ return vm_colorget(pattern, "address"    , &addr); }
char *vm_colornumber(char *pattern, u_int numb) 		{ return vm_colorget(pattern, "number"     , &numb); }

/* Misc functions */

/* Return number of color chars (not total size) */
int		vm_color_count(char *string)
{
  int		count = 0;
  int		len;
  int		i;

  len = strlen(string);
  for (i = 0; i < len; i++)
    {
      if (string[i] == C_STARTCOLOR)
	{
	  count++;
	  while (i < len && string[i] != 'm')
	    i++;
	}
    }
  return (0);
}

/* Return total size of colors on a string */
int		vm_color_size(char *string)
{
  int		size = 0;
  int		len;
  int		i;

  len = strlen(string);
  for (i = 0; i < len; i++)
    {
      if (string[i] == C_STARTCOLOR)
	{
	  size++;
	  while (i < len && string[i] != 'm')
	    {
	      size++;
	      i++;
	    }
	}      
    }
  return (size);
}

