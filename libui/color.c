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

int		vm_colorpattern(color_t *t, char *text, char *pattern)
{
  char		bo[16], ul[16], fg[16], bg[16];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (t->bground == NULL && t->fground == NULL && 
      t->bold == NULL && t->underline == NULL)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);  

  if (t->bold != NULL)
    snprintf(bo, 15, "\e[%u", COLOR_BOLD);

  if (t->underline != NULL)
    snprintf(ul, 15, "%s%u", 
	     (t->bold == NULL ? "\e[" : COLOR_SEPARE), 
	     COLOR_UNDERLINE);    
    
  if (t->fground != NULL)
    snprintf(fg, 15, "%s%u", 
	     (t->bold == NULL && t->underline == NULL ? "\e[" : COLOR_SEPARE), 
	     t->fground); 

  if (t->bground != NULL)
    snprintf(bg, 15, "%s%u", 
	     (t->bold == NULL && t->underline == NULL && 
	      t->fground == NULL ? "\e[" : COLOR_SEPARE), 
	     t->bground); 

  snprintf(pattern, COLOR_TOKEN_LEN - 1,
	   "%%s%s%s%s%sm%%s\e[%um%%s", 
	   (t->bold != NULL ? bo : ""), 
	   (t->underline != NULL ? ul : ""),
	   (t->fground != NULL ? fg : ""), 
	   (t->bground != NULL ? bg : ""), 
	   COLOR_NONE);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0); 
}

/* Build color text */
char 		*vm_colorget(char *sp, char *type, void *object)
{
  color_t 	*t;
  char		pattern[COLOR_TOKEN_LEN];
  char		text[COLOR_TOKEN_LEN], *pt;
  char		white_s[COLOR_TOKEN_LEN], white_e[COLOR_TOKEN_LEN];
  int 		i, num = 0;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (curtok >= COLOR_TOKENS) {
    printf("WARNING !!!! Token overflow (val:%u)\n", curtok);
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "", NULL);
  }


  //snprintf(text, COLOR_TOKEN_LEN - 1, sp, object);
  if (strchr(sp, 's') == NULL)
    snprintf(text, COLOR_TOKEN_LEN - 1, sp, *(long *) object);
  else
    snprintf(text, COLOR_TOKEN_LEN - 1, sp, object);
  

  if (!nocolor)
    {
      if (!strcmp(sp, "%s"))
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ((char*) object));

      strncpy(tokens[curtok], text, COLOR_TOKEN_LEN);
      tokens[curtok][COLOR_TOKEN_LEN-1] = 0;

      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, tokens[curtok++]);
    }

  t = vm_colortable(type, text);
  if (t == NULL)
    {
      if (!strcmp(sp, "%s"))
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ((char*) object));

      strncpy(tokens[curtok], text, COLOR_TOKEN_LEN);
      tokens[curtok][COLOR_TOKEN_LEN-1] = 0;
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, tokens[curtok++]);
    }

  if (vm_colorpattern(t, text, pattern) != 0)
    {
      if (!strcmp(sp, "%s"))
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ((char*) object));

      strncpy(tokens[curtok], text, COLOR_TOKEN_LEN);
      tokens[curtok][COLOR_TOKEN_LEN-1] = 0;
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, tokens[curtok++]);
    }

  for(i=0; IS_BLANK(text[i]); i++)
    white_s[i] = text[i];
  white_s[i] = 0;

  if (i > 0 && i < strlen(text))
    {
      pt = text + i;
      strcpy(text, pt);
      text[strlen(text)] = 0;
    }
  else if (i == strlen(text))
    white_s[0] = 0;

  for(i=(strlen(text)-1), num=0; IS_BLANK(text[i]); i--, num++)
    white_e[num] = text[i];
  white_e[num] = 0;
  
  if (num > 0)
    text[strlen(text)-num] = 0;

  snprintf(tokens[curtok], COLOR_TOKEN_LEN - 1, 
	   pattern, white_s, text, white_e);

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



