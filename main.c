/* Moria Version 4.8	COPYRIGHT (c) Robert Alan Koeneke		*/
/*                                                                       */
/*       I lovingly dedicate this game to hackers and adventurers        */
/*       everywhere...                                                   */
/*                                                                       */
/*                                                                       */
/*       Designer and Programmer : Robert Alan Koeneke                   */
/*                                 University of Oklahoma                */
/*                                                                       */
/*       Assistant Programmers   : Jimmey Wayne Todd                     */
/*                                 University of Oklahoma                */
/*                                                                       */
/*                                 Gary D. McAdoo                        */
/*                                 University of Oklahoma                */
/*                                                                       */
/*       UNIX Port               : James E. Wilson                       */
/*                                 UC Berkeley                           */
/*                                 wilson@ernie.Berkeley.EDU             */
/*                                 ucbvax!ucbernie!wilson                */
/*                                                                       */
/*       Moria may be copied and modified freely as long as the above    */
/*       credits are retained.  No one who-so-ever may sell or market    */
/*       this software in any form without the expressed written consent */
/*       of the author Robert Alan Koeneke.                              */
/*                                                                       */

#include <curses.h>
#include <sys/types.h>

#include "constants.h"
#include "config.h"
#include "types.h"
#include "externs.h"

#ifdef USG
#include <string.h>
#else
#include <strings.h>
#endif

#if defined(ultrix) || defined(sun) || defined(USG)
int getuid();
int getgid();
#else
uid_t getuid();
uid_t getgid();
#endif

#if defined(ultrix) || defined(USG)
void perror();
void exit();
#endif

extern int key_bindings;

/* Initialize, restore, and get the ball rolling...	-RAK-	*/
main(argc, argv)
int argc;
char *argv[];
{
  /* call this routine to grab a file pointer to the highscore file *
  /* and prepare things to relinquish setuid privileges */
  init_scorefile();

  if (0 != setuid(getuid()))
    {
      perror("Gack!  Can't set permissions correctly!  Exiting!\n");
      exit(0);
    }
  if (0 != setgid(getgid()))
    {
      perror("Gack!  Can't set group id correctly!  Exiting!\n");
      exit(0);
    }

  /* use curses */
  init_curses();

  /* catch those nasty signals */
  /* must come after init_curses as some of the signal handlers use curses */
  init_signals();

  /* Build the secret wizard and god passwords	*/
  bpswd();

  /* check for user interface option */
  key_bindings = KEY_BINDINGS;
  if (argc >= 2)
    {
      if (!strcmp(argv[1], "-r"))
        {
          key_bindings = ROGUE_LIKE;
          argv[1][0] = '\0';
        }
      if (!strcmp(argv[1], "-o"))
	{
	  key_bindings = ORIGINAL;
	  argv[1][0] = '\0';
	}
      if (!strcmp(argv[1], "-s"))
	{
	  display_scores();
	  exit_game();
	}
    }

  /* Check operating hours 			*/
  /* If not wizard  No_Control_Y               */
  /* Check or create hours.dat, print message	*/
  /* if last arg is ^ then start as wizard, can not restore game also */
  if (argc >= 2)
    intro(argv[argc - 1]);
  else
    intro("");

  /* Some necessary initializations		*/
  /* all made into constants or initialized in variables.c */

  /* Grab a random seed from the clock		*/
  init_seeds();

  /* Sort the objects by level			*/
  sort_objects();

  /* Init monster and treasure levels for allocate */
  init_m_level();
  init_t_level();

  /* Init the store inventories			*/
  store_init();
  if (COST_ADJ != 1.00)  price_adjust();

  /* Generate a character, or retrieve old one...	*/
  if ((argc > 1) && strlen(argv[argc-1]))
    {     /* Retrieve character    */
      generate = get_char(argv[argc-1]);
      change_name();
      magic_init();
    }
  else
    {     /* Create character      */
      create_character();
      char_inven_init();
      if (class[py.misc.pclass].mspell)
	{         /* Magic realm   */
	  (void) learn_spell(&msg_flag);
	  gain_mana(int_adj());
	}
      else if (class[py.misc.pclass].pspell)
	{         /* Clerical realm*/
	  (void) learn_prayer();
	  gain_mana(wis_adj());
	}
      py.misc.cmana = (double)py.misc.mana;
      magic_init();
      generate = TRUE;
    }

  /* Begin the game				*/
  /*    This determines the maximum player level    */
  /* must be one less than real value so that prt_experience will work
     correctly, otherwise it is possible to reach level 41 */
  player_max_exp = player_exp[MAX_PLAYER_LEVEL-1] * py.misc.expfact - 1;
  clear_screen(0, 0);
  prt_stat_block();
  /* prevent ^c quit from entering score into scoreboard until this point */
  character_generated = 1;

  /* Loop till dead, or exit			*/
  while(!death) {
    if (generate)  generate_cave();         /* New level     */
    dungeon();                                  /* Dungeon logic */
    generate = TRUE;
  }
  upon_death();                         /* Character gets buried */
  /* should never reach here, but just in case */
  return (0);
}
