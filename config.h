/* Person to bother if something goes wrong */
/* recompile files.c and misc2.c if these change */
#define WIZARD	"Jim Wilson"
/* Wizard password */
#define PASSWD1 "Gandalf"
/* God password, for debugging */
#define PASSWD2 "Mithrandir"
/* Wizard/God user id */
#define UID 974

/* files used by moria, set these to valid pathnames */
/* if you change these, you only have to recompile death.c and files.c */
#define MORIA_HOU  "/usr/public/moriadir/Moria_hours"
#define MORIA_MOR  "/usr/public/moriadir/Moria_news"
#define MORIA_MAS  "/usr/public/moriadir/Character"
#define MORIA_TOP  "/usr/public/moriadir/Highscores"
#define MORIA_HLP  "/usr/public/moriadir/Helpfile"

/* this sets the default user interface, ONE of the two must be defined */
/* use the original key bindings, keypad for movement */
/* if you change this, you only need to recompile main.c */
#define KEY_BINDINGS ORIGINAL
/* this uses a rogue-like key bindings, vi style movement */
/* #define KEY_BINDINGS ROGUE_LIKE */

/* no system definitions are needed for 4.3BSD or SUN OS */

/* if you are compiling on an ultrix/4.2BSD/Dynix/etc. version of UNIX,
   define this, not needed for SUNs */
/* #define ultrix */

/* if you are compiling on a SYS V version of UNIX, define this */
/* #define SYS_V */

/* if you are compiling on a SYS III version of UNIX, define this */
/* #define SYS_III */

/* if the screen does not look right (it will be obvious) then you have a buggy
   version of curses, and you must define this */
/* this must be defined for Ultrix systems, and for most 4.2BSD systems
   it is not needed for Dynix */
/* only need to recompile io.c */
/* #define BUGGY_CURSES */

/* define this is your machine does not have signed characters:
   this is needed for IBM PC-RT, 3B1, 3B2, 3B20, and perhaps others */
/* #define NO_SIGNED_CHARS

/* this will slow down the game a bit, and therefore make it use up a little
   less cpu time, this enables sleep() code for the resting commands,
   and for character rerolling */
/* only need to recompile dungeon.c and create.c
/* #define SLOW */

/* system dependent defines follow, you should not need to change anything
   below */

/* substitute strchr for index on USG versions of UNIX */
#ifdef SYS_V
#define index strchr
#endif

#ifdef SYS_III
char *index();
#endif

#if defined(SYS_III) || defined(SYS_V)
#ifndef USG
#define USG
#endif
#endif
