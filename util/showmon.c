/* this used to be in files.c, this is not a working program */

/* move to cheat.c file */
/* Prints a listing of monsters				-RAK-	 */
print_monsters()
{
  register int i;
  int j, xpos, attype, adesc, adice, asides, count;
  register FILE *file1;
  vtype out_val, filename1;
  char *attstr;
  register creature_type *c_ptr;
  register char *string;

  prt("File name: ", 0, 0);
  if (get_string(filename1, 0, 11, 64))
    {
      if (strlen(filename1) == 0)
	(void) strcpy(filename1, "MORIAMON.DAT");
      if ((file1 = fopen(filename1, "w")) != NULL)
	{
	  prt("Writing Monster Dictionary...", 0, 0);
	  put_qio();
	  for (i = 0; i < MAX_CREATURES; i++)
	    {
	      c_ptr = &c_list[i];
	      /* Begin writing to file				       */
	      (void) fprintf(file1, "------------------------------------");
	      (void) fprintf(file1, "--------\n");
	      (void) fprintf(file1, "%3d  %-31s     (%c)\n", i, c_ptr->name,
			     c_ptr->cchar);
	      (void) fprintf(file1, "     Speed =%3d  Level     =%3d  Exp =%6u\n",
		      c_ptr->speed-10, c_ptr->level, c_ptr->mexp);
	      (void) fprintf(file1, "     AC    =%3d  Eye-sight =%3d  HD  =%6s\n",
		      c_ptr->ac, c_ptr->aaf, c_ptr->hd);
	      if (CM_WIN & c_ptr->cmove)
		(void) fprintf(file1, "     Creature is a ***Win Creature***\n");
	      if (CM_EATS_OTHER) & c_ptr->cmove)
		(void) fprintf(file1, "     Creature Eats/kills other creatures.\n");
	      if (CD_DRAGON & c_ptr->cdefense)
		(void) fprintf(file1, "     Creature is a dragon.\n");
	      if (CD_ANIMAL & c_ptr->cdefense)
		(void) fprintf(file1, "     Creature is an animal.\n");
	      if (CD_EVIL & c_ptr->cdefense)
		(void) fprintf(file1, "     Creature is evil.\n");
	      if (CD_UNDEAD & c_ptr->cdefense)
		(void) fprintf(file1, "     Creature is undead.\n");
	      if (CD_FROST & c_ptr->cdefense)
		(void) fprintf(file1, "     Creature harmed by cold.\n");
	      if (CD_FIRE & c_ptr->cdefense)
		(void) fprintf(file1, "     Creature harmed by fire.\n");
	      if (CD_POISON & c_ptr->cdefense)
		(void) fprintf(file1, "     Creature harmed by poison.\n");
	      if (CD_ACID & c_ptr->cdefense)
		(void) fprintf(file1, "     Creature harmed by acid.\n");
	      if (CD_LIGHT & c_ptr->cdefense)
		(void) fprintf(file1, "     Creature harmed by blue light.\n");
	      if (CD_STONE & c_ptr->cdefense)
		(void) fprintf(file1, "     Creature harmed by Stone-to-Mud.\n");
	      if (CD_NO_SLEEP & c_ptr->cdefense)
		(void) fprintf(file1, "     Creature cannot be charmed or slept.\n");
	      if (CD_INFRA & c_ptr->cdefense)
		(void) fprintf(file1, "     Creature seen with Infra-Vision.\n");
	      if (CD_MAX_HP & c_ptr->cdefense)
		(void) fprintf(file1, "     Creature has MAX hit points.\n");
	      if (CM_INVISIBLE & c_ptr->cmove)
		(void) fprintf(file1, "     Creature is invisible.\n");
	      if (CM_PICKS_UP & c_ptr->cmove)
		(void) fprintf(file1, "     Creature picks up objects.\n");
	      if (CM_MULTIPLY & c_ptr->cmove)
		(void) fprintf(file1, "     Creature multiplies.\n");
	      if (CM_CARRY_OBJ & c_ptr->cmove)
		(void) fprintf(file1, "     Carries object(s).\n");
	      if (CM_CARRY_GOLD & c_ptr->cmove)
		(void) fprintf(file1, "     Carries gold, gems, etc.\n");
	      if (CM_60_RANDOM & c_ptr->cmove)
		(void) fprintf(file1, "       Has object/gold 60%% of time.\n");
	      if (CM_90_RANDOM & c_ptr->cmove)
		(void) fprintf(file1, "       Has object/gold 90%% of time.\n");
	      if (CM_1D2_OBJ & c_ptr->cmove)
		(void) fprintf(file1, "       Has 1d2 object(s)/gold.\n");
	      if (CM_2D2_OBJ & c_ptr->cmove)
		(void) fprintf(file1, "       Has 2d2 object(s)/gold.\n");
	      if (CM_4D2_OBJ & c_ptr->cmove)
		(void) fprintf(file1, "       Has 4d2 object(s)/gold.\n");
	      /*
	       * Creature casts spells / Breathes Dragon
	       * breath.
	       */
	      if (c_ptr->spells != 0)
		{
		  (void) fprintf(file1, "   --Spells/Dragon Breath =\n");
		  (void) fprintf(file1, "       Casts spells 1 out of %d turns.\n",
			  (int)(CS_FREQ & c_ptr->spells));
		  if (CS_TEL_SHORT & c_ptr->spells)
		    (void) fprintf(file1, "       Can teleport short.\n");
		  if (CS_TEL_LONG & c_ptr->spells)
		    (void) fprintf(file1, "       Can teleport long.\n");
		  if (CS_TEL_TO & c_ptr->spells)
		    (void) fprintf(file1, "       Teleport player to itself.\n");
		  if (CS_LGHT_WND & c_ptr->spells)
		    (void) fprintf(file1, "       Cause light wounds.\n");
		  if (CS_SER_WND & c_ptr->spells)
		    (void) fprintf(file1, "       Cause serious wounds.\n");
		  if (CS_HOLD_PER & c_ptr->spells)
		    (void) fprintf(file1, "       Hold person.\n");
		  if (CS_BLIND & c_ptr->spells)
		    (void) fprintf(file1, "       Cause blindness.\n");
		  if (CS_CONFUSE & c_ptr->spells)
		    (void) fprintf(file1, "       Cause confusion.\n");
		  if (CS_FEAR & c_ptr->spells)
		    (void) fprintf(file1, "       Cause fear.\n");
		  if (CS_SUMMON_MON & c_ptr->spells)
		    (void) fprintf(file1, "       Summon a monster.\n");
		  if (CS_SUMMON_UND & c_ptr->spells)
		    (void) fprintf(file1, "       Summon an undead.\n");
		  if (CS_SLOW_PER & c_ptr->spells)
		    (void) fprintf(file1, "       Slow person.\n");
		  if (CS_DRAIN_MANA & c_ptr->spells)
		    (void) fprintf(file1, "       Drains mana for healing.\n");
		  if (0x00020000L & c_ptr->spells)
		    (void) fprintf(file1, "       **Unknown spell value**\n");
		  if (0x00040000L & c_ptr->spells)
		    (void) fprintf(file1, "       **Unknown spell value**\n");
		  if (CS_BR_LIGHT & c_ptr->spells)
		    (void) fprintf(file1, "       Breathes Lightning Dragon Breath.\n");
		  if (CS_BR_GAS & c_ptr->spells)
		    (void) fprintf(file1, "       Breathes Gas Dragon Breath.\n");
		  if (CS_BR_ACID & c_ptr->spells)
		    (void) fprintf(file1, "       Breathes Acid Dragon Breath.\n");
		  if (CS_BR_FROST & c_ptr->spells)
		    (void) fprintf(file1, "       Breathes Frost Dragon Breath.\n");
		  if (CS_BR_FIRE & c_ptr->spells)
		    (void) fprintf(file1, "       Breathes Fire Dragon Breath.\n");
		}
	      /* Movement for creature				       */
	      (void) fprintf(file1, "   --Movement =\n");
	      if ((CM_ALL_MV_FLAGS & c_ptr->cmove) == 0)
		(void) fprintf(file1, "       Do not move.\n");
	      if (CM_ATTACK_ONLY & c_ptr->cmove)
		(void) fprintf(file1, "       Move only to attack.\n");
	      if (CM_MOVE_NORMAL & c_ptr->cmove)
		(void) fprintf(file1, "       Move and attack normally.\n");
	      if (CM_20_RANDOM & c_ptr->cmove)
		(void) fprintf(file1, "       20%% random movement.\n");
	      if (CM_40_RANDOM & c_ptr->cmove)
		(void) fprintf(file1, "       40%% random movement.\n");
	      if (CM_75_RANDOM & c_ptr->cmove)
		(void) fprintf(file1, "       75%% random movement.\n");
	      if (CM_OPEN_DOOR & c_ptr->cmove)
		(void) fprintf(file1, "       Can open doors.\n");
	      if (CM_PHASE & c_ptr->cmove)
		(void) fprintf(file1, "       Can phase through walls.\n");

	      (void) fprintf(file1, "   --Creature attacks =\n");
	      attstr = c_ptr->damage;
	      count = 0;
	      while (*attstr != 0 && count < 4)
		{
		  attype = monster_attacks[*attstr].attack_type;
		  adesc = monster_attacks[*attstr].attack_desc;
		  adice = monster_attacks[*attstr].attack_dice;
		  asides = monster_attacks[*attstr].attack_sides;
		  attstr++;
		  count++;
		  out_val[0] = '\0';
		  switch (adesc)
		    {
		    case 0:
		      (void) strcpy(out_val, "       No hits for ");
		      break;
		    case 1:
		      (void) strcpy(out_val, "       Hits for ");
		      break;
		    case 2:
		      (void) strcpy(out_val, "       Bites for ");
		      break;
		    case 3:
		      (void) strcpy(out_val, "       Claws for ");
		      break;
		    case 4:
		      (void) strcpy(out_val, "       Stings for ");
		      break;
		    case 5:
		      (void) strcpy(out_val, "       Touches for ");
		      break;
		    case 6:
		      (void) strcpy(out_val, "       Kicks for ");
		      break;
		    case 7:
		      (void) strcpy(out_val, "       Gazes for ");
		      break;
		    case 8:
		      (void) strcpy(out_val, "       Breathes for ");
		      break;
		    case 9:
		      (void) strcpy(out_val, "       Spits for ");
		      break;
		    case 10:
		      (void) strcpy(out_val, "       Wails for ");
		      break;
		    case 11:
		      (void) strcpy(out_val, "       Embraces for ");
		      break;
		    case 12:
		      (void) strcpy(out_val, "       Crawls on you for ");
		      break;
		    case 13:
		      (void) strcpy(out_val, "       Shoots spores for ");
		      break;
		    case 14:
		      (void) strcpy(out_val, "       Begs for money for ");
		      break;
		    case 15:
		      (void) strcpy(out_val, "       Slimes you for ");
		      break;
		    case 16:
		      (void) strcpy(out_val, "       Crushes you for ");
		      break;
		    case 17:
		      (void) strcpy(out_val, "       Tramples you for ");
		      break;
		    case 18:
		      (void) strcpy(out_val, "       Drools on you for ");
		      break;
		    case 19:
		      (void) strcpy(out_val, "       Insults you for ");
		      break;
		    case 99:
		      (void) strcpy(out_val, "       Is repelled.");
		      break;
		    default:
		      (void) strcpy(out_val, "     **Unknown value** ");
		      break;
		    }
		  switch (attype)
		    {
		    case 0:
		      (void) strcat(out_val, "no damage.");
		      break;
		    case 1:
		      (void) strcat(out_val, "normal damage.");
		      break;
		    case 2:
		      (void) strcat(out_val, "lowering strength.");
		      break;
		    case 3:
		      (void) strcat(out_val, "confusion.");
		      break;
		    case 4:
		      (void) strcat(out_val, "fear.");
		      break;
		    case 5:
		      (void) strcat(out_val, "fire damage.");
		      break;
		    case 6:
		      (void) strcat(out_val, "acid damage.");
		      break;
		    case 7:
		      (void) strcat(out_val, "cold damage.");
		      break;
		    case 8:
		      (void) strcat(out_val, "lightning damage.");
		      break;
		    case 9:
		      (void) strcat(out_val, "corrosion damage.");
		      break;
		    case 10:
		      (void) strcat(out_val, "blindness.");
		      break;
		    case 11:
		      (void) strcat(out_val, "paralyzation.");
		      break;
		    case 12:
		      (void) strcat(out_val, "stealing money.");
		      break;
		    case 13:
		      (void) strcat(out_val, "stealing object.");
		      break;
		    case 14:
		      (void) strcat(out_val, "poison damage.");
		      break;
		    case 15:
		      (void) strcat(out_val, "lose dexterity.");
		      break;
		    case 16:
		      (void) strcat(out_val, "lose constitution.");
		      break;
		    case 17:
		      (void) strcat(out_val, "lose intelligence.");
		      break;
		    case 18:
		      (void) strcat(out_val, "lose wisdom.");
		      break;
		    case 19:
		      (void) strcat(out_val, "lose experience.");
		      break;
		    case 20:
		      (void) strcat(out_val, "aggravates monsters.");
		      break;
		    case 21:
		      (void) strcat(out_val, "disenchants objects.");
		      break;
		    case 22:
		      (void) strcat(out_val, "eating food.");
		      break;
		    case 23:
		      (void) strcat(out_val, "eating light source.");
		      break;
		    case 24:
		      (void) strcat(out_val, "absorbing charges.");
		      break;
		    case 99:
		      (void) strcat(out_val, "blank message.");
		      break;
		    default:
		      (void) strcat(out_val, "**Unknown value**");
		      break;
		    }
		  (void) fprintf(file1, "%s (%dd%d)\n", out_val, adice,asides);
		}
	      for (j = 0; j < 2; j++)
		(void) fprintf(file1, "\n");
	    }
	  /* End writing to file				   */
	  (void) fclose(file1);
	  prt("Completed.", 0, 0);
	}
    }
}
