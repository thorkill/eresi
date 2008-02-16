
#ifndef CONFIG_H
#define CONFIG_H

/* _________________________ OS definitions ____________________ */

/* FOR WINDOWS */
/* #define WIN32 */
/* #define KEYB_INTER 0x51 */

/* OR LINUX */
#ifndef LINUX
#define LINUX
#endif
#define KEYB_INTER 0x21

/* OR FREEBSD */
/* #define FREEBSD */
/* le reste viendra plus tard... */


/* ________________________ video definitions __________________ */

/* CONSOLE MODE */
#define VIDEO_AA

/* OR GRAPHIC MODE */
/* we want a graphic mode */
/*#define VIDEO_FB */
/* we want direct hardware writtng */
/* #define HW_FB */
/* we want ke2dbg to find automatically the address of the video hardware */
/* #define AUTO_HW_FB */

/* we set a default value... */
#define SCREEN_ADRESSE 0xF0000000


/* screen dimension (internal hardware dimensions) */
#define SCREEN_MAX_X 2048
#define SCREEN_MAX_Y 768

/* screen bytes per pixel (internal hardware dimensions) */
/* #define BYTE_PER_PIXEL_HALF */
/* #define BYTE_PER_PIXEL_ONE */
/* #define BYTE_PER_PIXEL_TWO */
/* #define BYTE_PER_PIXEL_THREE */
#define BYTE_PER_PIXEL_FOUR


/* ____________________________ custom _________________________ */

#define DONT_HOOK_KBD

#endif /* CONFIG_H */
