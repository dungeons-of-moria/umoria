/* config.h: configuration definitions

   Copyright (c) 1989 James E. Wilson

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

/* Person to bother if something goes wrong. */
/* Recompile files.c and misc2.c if these change. */
#define WIZARD	"Jim Wilson"
/* wizard password and wizard uid no longer used */


/* files used by moria, set these to valid pathnames */
#ifdef MSDOS

/* Files which can be in a varying place */
#define MORIA_SAV	moriasav
#define MORIA_TOP	moriatop
#define MORIA_TOP_NAME	"MORIA.SCR"
#define MORIA_SAV_NAME	"MORIA.SAV"
#define MORIA_CNF_NAME	"MORIA.CNF"
#define MORIA_HELP	"roglcmds.hlp"
#define MORIA_ORIG_HELP	"origcmds.hlp"
#define MORIA_WIZ_HELP	"rwizcmds.hlp"
#define MORIA_OWIZ_HELP	"owizcmds.hlp"
#define MORIA_WELCOME	"welcome.hlp"
#define MORIA_VER	"version.hlp"
#define MORIA_LOG	"moria.log"

#else
#ifdef MAC

/* These files are concatenated into the data fork of the app */
/* The names are retained to find the appropriate text */
#define MORIA_MOR	"news"
#define MORIA_HELP	"roglcmds.hlp"
#define MORIA_ORIG_HELP	"origcmds.hlp"
#define MORIA_WIZ_HELP	"rwizcmds.hlp"
#define MORIA_OWIZ_HELP	"owizcmds.hlp"
#define MORIA_WELCOME	"welcome.hlp"
#define MORIA_VER	"version.hlp"
/* Do not know what will happen with these yet */
#define MORIA_TOP	"High Scores"
#define MORIA_LOG	"Moria Log"
/* File types and creators for the Mac */
#define MORIA_FCREATOR	'MRIA'
#define SAVE_FTYPE	'SAVE'
#define INFO_FTYPE	'TEXT'
#define SCORE_FTYPE	'SCOR'
#define CONFIG_FTYPE	'CNFG'

#else

#ifdef VMS
#define MORIA_SAVE	"moria.sav"
#define MORIA_HOU	"moria:hours"
#define MORIA_MOR	"moria:news"
#define MORIA_TOP	"moria:scores"
#define MORIA_HELP	"moria:roglcmds.hlp"
#define MORIA_ORIG_HELP "moria:origcmds.hlp"
#define MORIA_WIZ_HELP  "moria:rwizcmds.hlp"
#define MORIA_OWIZ_HELP "moria:owizcmds.hlp"
#define MORIA_WELCOME	"moria:welcome.hlp"
#define MORIA_LOG	"moria:moria.log"
#define MORIA_VER	"moria:version.hlp"

#else
#if defined(GEMDOS)
/* Atari ST */
#define MORIA_SAV	"moria.sav"
#define MORIA_HOU	"files\\hours"
#define MORIA_MOR	"files\\news"
#define MORIA_TOP	"files\\scores"
#define MORIA_HELP	"files\\roglcmds.hlp"
#define MORIA_ORIG_HELP	"files\\origcmds.hlp"
#define MORIA_WIZ_HELP	"files\\rwizcmds.hlp"
#define MORIA_OWIZ_HELP	"files\\owizcmds.hlp"
#define MORIA_WELCOME	"files\\welcome.hlp"
#define MORIA_LOG	"files\\moria.log"
#define MORIA_VER	"files\\version.hlp"

#else

/* probably unix */
#define MORIA_SAV	"moria.save"
#define MORIA_HOU	"/tla_tmp_2/wilson/umoria/files/hours"
#define MORIA_MOR	"/tla_tmp_2/wilson/umoria/files/news"
#define MORIA_TOP	"/tla_tmp_2/wilson/umoria/files/scores"
#define MORIA_HELP	"/tla_tmp_2/wilson/umoria/files/roglcmds.hlp"
#define MORIA_ORIG_HELP	"/tla_tmp_2/wilson/umoria/files/origcmds.hlp"
#define MORIA_WIZ_HELP	"/tla_tmp_2/wilson/umoria/files/rwizcmds.hlp"
#define MORIA_OWIZ_HELP	"/tla_tmp_2/wilson/umoria/files/owizcmds.hlp"
#define MORIA_WELCOME	"/tla_tmp_2/wilson/umoria/files/welcome.hlp"
#define MORIA_LOG	"/tla_tmp_2/wilson/umoria/files/moria.log"
#define MORIA_VER	"/tla_tmp_2/wilson/umoria/files/version.hlp"

#endif
#endif
#endif
#endif

/* this sets the default user interface */
/* to use the original key bindings (keypad for movement) set ROGUE_LIKE
   to FALSE, to use the rogue-like key bindings (vi style movement)
   set ROGUE_LIKE to TRUE */
/* if you change this, you only need to recompile main.c */
#define ROGUE_LIKE FALSE


/* for the ANDREW distributed file system, define this to ensure that
   the program is secure with respect to the setuid code, this prohibits
   inferior shells, also does not relinquish setuid priviledges at the start,
   but instead calls the ANDREW library routines bePlayer(), beGames(),
   and Authenticate() */
/* #define SECURE */


/* no system definitions are needed for 4.3BSD, SUN OS, DG/UX */

/* if you are compiling on an ultrix/4.2BSD/Dynix/etc. version of UNIX,
   define this, not needed for SUNs */
/* #ifndef ultrix
#define ultrix
#endif */

/* if you are compiling on a SYS V version of UNIX, define this */
/* #define SYS_V */

/* if you are compiling on a SYS III version of UNIX, define this */
/* #define SYS_III */

/* if you are compiling on an ATARI ST with Mark Williams C, define this */
/* #define ATARIST_MWC */

/* if you are compiling on a Macintosh with MPW C 3.0, define this */
/* #define MAC */


/* system dependent defines follow, you should not need to change anything
   below */

/* substitute strchr for index on USG versions of UNIX */
#if defined(SYS_V) || defined(MSDOS) || defined(MAC)
#define index strchr
#endif

#ifdef SYS_III
char *index();
#endif

#if defined(SYS_III) || defined(SYS_V) || defined(MSDOS) || defined(MAC)
#ifndef USG
#define USG
#endif
#endif

#ifdef ATARIST_MWC
#define USG
#endif

/* Pyramid runs 4.2BSD-like UNIX version */
#if defined(Pyramid)
#define ultrix
#endif

#ifdef MSDOS
#define register      /* MSC 4.0 still has a problem with register bugs ... */
#endif

#ifdef MAC
#ifdef RSRC
#define MACRSRC		/* i.e., we're building the resources */
#else
#define MACGAME		/* i.e., we're building the game */
#endif
#endif

#ifdef MAC
/* Screen dimensions */
#define SCRN_ROWS	24
#define SCRN_COLS	80
#endif

#if vms
#define getch _getch
#define unlink delete
#define index strchr
#define lstat stat
#define exit uexit
#endif

#if defined(SYS_V) && defined(lint)
/* to prevent <string.h> from including <NLchar.h>, this prevents a bunch
   of lint errors. */
#define RTPC_NO_NLS
#endif

#ifdef SECURE
extern int PlayerUID;
#define getuid() PlayerUID
#define geteuid() PlayerUID
#endif
