#include <stdio.h>

#include "constants.h"
#include "config.h"
#include "types.h"
#include "externs.h"

#ifdef USG
#include <string.h>
#else
#include <strings.h>
#endif

#ifdef sun   /* correct SUN stupidity in the stdio.h file */
char *sprintf();
#endif

double chr_adj();

/* Comments vary...					-RAK-	*/
/* Comment one : Finished haggling				*/
prt_comment1()
{
  msg_flag = FALSE;
  switch(randint(14))
    {
    case 1: msg_print("Done!"); break;
    case 2: msg_print("Accepted!"); break;
    case 3: msg_print("Fine..."); break;
    case 4: msg_print("Agreed!"); break;
    case 5: msg_print("Okay..."); break;
    case 6: msg_print("Taken!"); break;
    case 7: msg_print("You drive a hard bargain, but taken..."); break;
    case 8: msg_print("You'll force me bankrupt, but it's a deal..."); break;
    case 9: msg_print("Sigh...  I'll take it..."); break;
    case 10: msg_print("My poor sick children may starve, but done!"); break;
    case 11: msg_print("Finally!  I accept..."); break;
    case 12: msg_print("Robbed again..."); break;
    case 13: msg_print("A pleasure to do business with you!"); break;
    case 14: msg_print("My spouse will skin me, but accepted."); break;
    }
}


/* %A1 is offer, %A2 is asking...		*/
prt_comment2(offer, asking, final)
int offer, asking, final;
{
  vtype comment;

  if (final > 0)
    switch(randint(3))
      {
      case 1:
	(void) strcpy(comment,"%A2 is my final offer; take it or leave it...");
	break;
      case 2:
	(void) strcpy(comment, "I'll give you no more than %A2.");
	break;
      case 3:
	(void) strcpy(comment, "My patience grows thin...  %A2 is final.");
	break;
      }
  else
    switch(randint(16))
      {
      case 1:
	(void) strcpy(comment,
		      "%A1 for such a fine item?  HAH!  No less than %A2.");
	break;
      case 2:
	(void) strcpy(comment, "%A1 is an insult!  Try %A2 gold pieces...");
	break;
      case 3:
	(void) strcpy(comment,
		      "%A1???  Thou wouldst rob my poor starving children?");
	break;
      case 4:
	(void) strcpy(comment, "Why, I'll take no less than %A2 gold pieces.");
	break;
      case 5:
	(void) strcpy(comment, "Ha!  No less than %A2 gold pieces.");
	break;
      case 6:
	(void) strcpy(comment,
		      "Thou knave!  No less than %A2 gold pieces.");
	break;
      case 7:
	(void) strcpy(comment, "%A1 is far too little; how about %A2?");
	break;
      case 8:
	(void) strcpy(comment, "I paid more than %A1 for it myself, try %A2.");
	break;
      case 9:
	(void) strcpy(comment,
		      "%A1?  Are you mad???  How about %A2 gold pieces?");
	break;
      case 10:
	(void) strcpy(comment,
		      "As scrap this would bring %A1.  Try %A2 in gold.");
	break;
      case 11:
	(void) strcpy(comment,
		      "May the fleas of 1000 orcs molest you.  I want %A2.");
	break;
      case 12:
	(void) strcpy(comment,
		      "My mother you can get for %A1, this costs %A2.");
	break;
      case 13:
	(void) strcpy(comment,
		      "May your chickens grow lips.  I want %A2 in gold!");
	break;
      case 14:
	(void) strcpy(comment,
		      "Sell this for such a pittance?  Give me %A2 gold.");
	break;
      case 15:
	(void) strcpy(comment,
		      "May the Balrog find you tasty!  %A2 gold pieces?");
	break;
      case 16:
	(void) strcpy(comment,"Your mother was a Troll!  %A2 or I'll tell...");
	break;
      }
  insert_num(comment, "%A1", offer, FALSE);
  insert_num(comment, "%A2", asking, FALSE);
  msg_print(comment);
}


prt_comment3(offer, asking, final)
     int offer, asking, final;
{
  vtype comment;

  if (final > 0)
    switch(randint(3))
      {
      case 1:
	(void) strcpy(comment,
		      "I'll pay no more than %A1; take it or leave it.");
	break;
      case 2:
	(void) strcpy(comment, "You'll get no more than %A1 from me...");
	break;
      case 3:
	(void) strcpy(comment, "%A1 and that's final.");
	break;
      }
  else
    switch(randint(15))
      {
      case 1:
	(void) strcpy(comment,"%A2 for that piece of junk?  No more than %A1");
	break;
      case 2:
	(void) strcpy(comment, "For %A2 I could own ten of those.  Try %A1.");
	break;
      case 3:
	(void) strcpy(comment, "%A2?  NEVER!  %A1 is more like it...");
	break;
      case 4:
	(void) strcpy(comment,
		      "Let's be reasonable... How about %A1 gold pieces?");
	break;
      case 5:
	(void) strcpy(comment, "%A1 gold for that junk, no more...");
	break;
      case 6:
	(void) strcpy(comment, "%A1 gold pieces and be thankful for it!");
	break;
      case 7:
	(void) strcpy(comment, "%A1 gold pieces and not a copper more...");
	break;
      case 8:
	(void) strcpy(comment, "%A2 gold?  HA!  %A1 is more like it...");
	break;
      case 9:
	(void) strcpy(comment, "Try about %A1 gold...");
	break;
      case 10:
	(void) strcpy(comment,
		      "I wouldn't pay %A2 for your children, try %A1.");
	break;
      case 11:
	(void) strcpy(comment, "*CHOKE* For that!?  Let's say %A1.");
	break;
      case 12:
	(void) strcpy(comment, "How about %A1?");
	break;
      case 13:
	(void) strcpy(comment, "That looks war surplus!  Say %A1 gold.");
	break;
      case 14:
	(void) strcpy(comment, "I'll buy it as scrap for %A1.");
	break;
      case 15:
	(void) strcpy(comment, "%A2 is too much; let us say %A1 gold.");
	break;
      }
  insert_num(comment, "%A1", offer, FALSE);
  insert_num(comment, "%A2", asking, FALSE);
  msg_print(comment);
}


/* Kick 'da bum out...					-RAK-	*/
prt_comment4()
{
  msg_flag = FALSE;
  switch(randint(5))
    {
    case 1:
      msg_print("ENOUGH!  Thou hast abused me once too often!");
      msg_print("Out of my place!");
      break;
    case 2:
      msg_print("THAT DOES IT!  You shall waste my time no more!");
      msg_print("out... Out... OUT!!!");
      break;
    case 3:
      msg_print("This is getting nowhere...  I'm going home!");
      msg_print("Come back tomorrow...");
      break;
    case 4:
      msg_print("BAH!  No more shall you insult me!");
      msg_print("Leave my place...  Begone!");
      break;
    case 5:
      msg_print("Begone!  I have had enough abuse for one day.");
      msg_print("Come back when thou art richer...");
      break;
    }
  /* make sure player sees last message, before he is kicked out of store */
  msg_print(" ");
  msg_flag = FALSE;
}


prt_comment5()
{
  switch(randint(10))
    {
    case 1: msg_print("You will have to do better than that!"); break;
    case 2: msg_print("That's an insult!"); break;
    case 3: msg_print("Do you wish to do business or not?"); break;
    case 4: msg_print("Hah!  Try again..."); break;
    case 5: msg_print("Ridiculous!"); break;
    case 6: msg_print("You've got to be kidding!"); break;
    case 7: msg_print("You'd better be kidding!!"); break;
    case 8: msg_print("You try my patience."); break;
    case 9: msg_print("I don't hear you."); break;
    case 10: msg_print("Hmmm, nice weather we're having..."); break;
    }
}


prt_comment6()
{
  switch(randint(5))
    {
    case 1: msg_print("I must have heard you wrong..."); break;
    case 2: msg_print("What was that?"); break;
    case 3: msg_print("I'm sorry, say that again..."); break;
    case 4: msg_print("What did you say?"); break;
    case 5: msg_print("Sorry, what was that again?"); break;
    }
}


/* Displays the set of commands				-RAK-	*/
display_commands()
{
  prt("You may:", 20, 0);
  prt(" p) Purchase an item.           b) Browse store's inventory.", 21, 0);
  prt(" s) Sell an item.               i) Inventory and Equipment Lists.",
      22, 0);
  prt("ESC) Exit from Building.       ^R) Redraw the screen.", 23, 0);
}


/* Displays the set of commands				-RAK-	*/
haggle_commands(typ)
int typ;
{
  if (typ == -1)
    prt("Specify an asking-price in gold pieces.", 21, 0);
  else
    prt("Specify an offer in gold pieces.", 21, 0);
  prt("ESC) Quit Haggling.", 22, 0);
  prt("", 23, 0);  /* clear last line */
}


/* Displays a store's inventory				-RAK-	*/
display_inventory(store_num, start)
int store_num, start;
{
  register store_type *s_ptr;
  register treasure_type *i_ptr;
  register int i, j, stop;
  vtype out_val1, out_val2;

  s_ptr = &store[store_num];
  i = (start % 12);
  stop = ((start / 12) + 1) * 12;
  if (stop > s_ptr->store_ctr)  stop = s_ptr->store_ctr;
  while (start < stop)
    {
      inventory[INVEN_MAX] = s_ptr->store_inven[start].sitem;
      i_ptr = &inventory[INVEN_MAX];
      if ((i_ptr->subval > 255) && (i_ptr->subval < 512))
	i_ptr->number = 1;
      objdes(out_val1, INVEN_MAX, TRUE);
      (void) sprintf(out_val2, "%c) %s", 97+i, out_val1);
      prt(out_val2, i+5, 0);
      if (s_ptr->store_inven[start].scost <= 0)
	{
	  j = abs(s_ptr->store_inven[start].scost);
	  j += (j * chr_adj());
	  if (j <= 0)
	    j = 1;
	  (void) sprintf(out_val2, "%9d", j);
	}
      else
	(void) sprintf(out_val2,"%9d [Fixed]",s_ptr->store_inven[start].scost);
      prt(out_val2, i+5, 59);
      i++;
      start++;
    }
  if (i < 12)
    for (j = 0; j < (11 - i + 1); j++)
      prt("", j+i+5, 0);  /* clear remaining lines */
  if (s_ptr->store_ctr > 12)
    prt("- cont. -", 17, 60);
  else
    prt("", 17, 60);  /* clear the line */
}


/* Re-displays only a single cost			-RAK-	*/
display_cost(store_num, pos)
int store_num, pos;
{
  register int i, j;
  vtype out_val;
  register store_type *s_ptr;

  s_ptr = &store[store_num];
  i = (pos % 12);
  if (s_ptr->store_inven[pos].scost < 0)
    {
      j = abs(s_ptr->store_inven[pos].scost);
      j += (j*chr_adj());
      (void) sprintf(out_val, "%d", j);
    }
  else
    (void) sprintf(out_val, "%9d [Fixed]", s_ptr->store_inven[pos].scost);
  prt(out_val, i+5, 59);
}


/* Displays players gold					-RAK-	*/
store_prt_gold()
{
  vtype out_val;

  (void) sprintf(out_val, "Gold Remaining : %d", py.misc.au);
  prt(out_val, 18, 17);
}


/* Displays store					-RAK-	*/
display_store(store_num, cur_top)
int store_num, cur_top;
{
  register store_type *s_ptr;

  s_ptr = &store[store_num];
  really_clear_screen();
  prt(owners[s_ptr->owner].owner_name, 3, 9);
  prt("   Item", 4, 0);
  prt("Asking Price", 4, 60);
  store_prt_gold();
  display_commands();
  display_inventory(store_num, cur_top);
}


/* Get the ID of a store item and return it's value	-RAK-	*/
int get_store_item(com_val, pmt, i, j)
int *com_val;
char *pmt;
register int i, j;
{
  char command;
  vtype out_val;
  register int flag;

  *com_val = -1;
  flag = TRUE;
  (void) sprintf(out_val, "(Items %c-%c, ESC to exit) %s", i+97, j+97, pmt);
  while (((*com_val < i) || (*com_val > j)) && (flag))
    {
      prt(out_val, 0, 0);
      inkey(&command);
      *com_val = (command);
      switch(*com_val)
	{
	case 0: case 27: flag = FALSE; break;
	default: *com_val = *com_val - 97; break;
	}
    }
  msg_flag = FALSE;
  erase_line(MSG_LINE, 0);
  return(flag);
}


/* Increase the insult counter and get pissed if too many -RAK-	*/
int increase_insults(store_num)
int store_num;
{
  register int increase;
  register store_type *s_ptr;

  increase = FALSE;
  s_ptr = &store[store_num];
  s_ptr->insult_cur++;
  if (s_ptr->insult_cur > owners[s_ptr->owner].insult_max)
    {
      prt_comment4();
      s_ptr->insult_cur = 0;
      s_ptr->store_open = turn + 2500 + randint(2500);
      increase = TRUE;
    }
  return(increase);
}


/* Decrease insults					-RAK-	*/
decrease_insults(store_num)
int store_num;
{
  register store_type *s_ptr;

  s_ptr = &store[store_num];
  s_ptr->insult_cur -= 2;
  if (s_ptr->insult_cur < 0)  s_ptr->insult_cur = 0;
}


/* Have insulted while haggling				-RAK-	*/
int haggle_insults(store_num)
int store_num;
{
  register int haggle;

  haggle = FALSE;
  if (increase_insults(store_num))
    haggle = TRUE;
  else
    prt_comment5();
  return(haggle);
}


int get_haggle(comment, num)
char *comment;
int *num;
{
  int i;
  vtype out_val;
  register int flag, clen;

  flag = TRUE;
  i = 0;
  clen = strlen(comment);
  do
    {
      msg_print(comment);
      msg_flag = FALSE;
      if (!get_string(out_val, 0, clen, 40))
	{
	  flag = FALSE;
	  erase_line(MSG_LINE, 0);
	}
      (void) sscanf(out_val, "%d", &i);
    }
  while ((i <= 0) && (flag));
  if (flag)  *num = i;
  return(flag);
}


int receive_offer(store_num, comment, new_offer, last_offer, factor)
int store_num;
char *comment;
int *new_offer;
int last_offer, factor;
{
  register int flag;
  register int receive;

  receive = 0;
  flag = FALSE;
  do
    {
      if (get_haggle(comment, new_offer))
	{
	  if (*new_offer*factor >= last_offer*factor)
	    flag = TRUE;
	  else if (haggle_insults(store_num))
	    {
	      receive = 2;
	      flag = TRUE;
	    }
	}
      else
	{
	  receive = 1;
	  flag = TRUE;
	}
    }
  while (!flag);
  return(receive);
}


/* Haggling routine					-RAK-	*/
int purchase_haggle(store_num, price, item)
int store_num;
int *price;
treasure_type item;
{
  int max_sell, min_sell, max_buy;
  int cost, cur_ask, final_ask, min_offer;
  int last_offer, new_offer, final_flag, x3;
  double x1, x2;
  double min_per, max_per;
  register int flag, loop_flag;
  vtype out_val, comment;
  int purchase;
  register store_type *s_ptr;
  register owner_type *o_ptr;

  flag = FALSE;
  purchase = 0;
  *price = 0;
  final_flag = 0;
  msg_flag = FALSE;
  s_ptr = &store[store_num];
  o_ptr = &owners[s_ptr->owner];
  cost = sell_price(store_num, &max_sell, &min_sell, item);
  max_sell = max_sell + (max_sell*chr_adj());
  if (max_sell <= 0)  max_sell = 1;
  min_sell = min_sell + (min_sell*chr_adj());
  if (min_sell <= 0)  min_sell = 1;
  max_buy  = (cost*(1-o_ptr->max_inflate));
  min_per  = o_ptr->haggle_per;
  max_per  = min_per*3.0;
  haggle_commands(1);
  cur_ask   = max_sell;
  final_ask = min_sell;
  min_offer = max_buy;
  last_offer = min_offer;
  (void) strcpy(comment, "Asking : ");
  do
    {
      do
	{
	  loop_flag = TRUE;
	  (void) sprintf(out_val, "%s %d", comment, cur_ask);
	  put_buffer(out_val, 1, 0);
	  switch(receive_offer(store_num, "What do you offer? ",
			     &new_offer, last_offer, 1))
	    {
	    case 1:
	      purchase = 1;
	      flag   = TRUE;
	      break;
	    case 2:
	      purchase = 2;
	      flag   = TRUE;
	      break;
	    default:
	      if (new_offer > cur_ask)
		{
		  prt_comment6();
		}
	      else if (new_offer == cur_ask)
		{
		  flag = TRUE;
		  *price = new_offer;
		}
	      else
		loop_flag = FALSE;
	    }
	}
      while (!flag && loop_flag);
      if (!flag)
	{
	  x1 = (double)(new_offer - last_offer)/(double)(cur_ask - last_offer);
	  if (x1 < min_per)
	    {
	      flag = haggle_insults(store_num);
	      if (flag)  purchase = 2;
	    }
	  else
	    {
	      if (x1 > max_per)
		{
		  x1 = x1*0.75;
		  if (x1 < max_per)  x1 = max_per;
		}
	    }
	  x2 = (x1 + (randint(5) - 3)/100.0);
	  x3 = ((cur_ask-new_offer)*x2) + 1;
	  /* don't let the price go up */
	  if (x3 < 0)
	    x3 = 0;
	  cur_ask -= x3;
	  if (cur_ask < final_ask)
	    {
	      cur_ask = final_ask;
	      (void) strcpy(comment, "Final Offer : ");
	      final_flag++;
	      if (final_flag > 3)
		{
		  if (increase_insults(store_num))
		    purchase = 2;
		  else
		    purchase = 1;
		  flag = TRUE;
		}
	    }
	  else if (new_offer >= cur_ask)
	    {
	      flag = TRUE;
	      *price = new_offer;
	    }
	  if (!flag)
	    {
	      last_offer = new_offer;
	      prt("", 1, 0);  /* clear the line */
	      (void) sprintf(out_val, "Your last offer : %d", last_offer);
	      put_buffer(out_val, 1, 39);
	      prt_comment2(last_offer, cur_ask, final_flag);
	    }
	}
    }
  while (!flag);
  prt("", 1, 0);  /* clear the line */
  display_commands();
  return(purchase);
}


/* Haggling routine					-RAK-	*/
int sell_haggle(store_num, price, item)
int store_num;
int *price;
treasure_type item;
{
  int max_sell, max_buy, min_buy;
  int cost, cur_ask, final_ask, min_offer;
  int last_offer, new_offer, final_flag, x3;
  int max_gold;
  double x1, x2;
  double min_per, max_per;
  register int flag, loop_flag;
  vtype comment, out_val;
  register store_type *s_ptr;
  register owner_type *o_ptr;
  int sell;

  flag = FALSE;
  sell = 0;
  *price = 0;
  final_flag = 0;
  msg_flag = FALSE;
  s_ptr = &store[store_num];
  cost = item_value(item);
  if (cost < 1)
    {
      sell = 3;
      flag = TRUE;
    }
  else
    {
      o_ptr = &owners[s_ptr->owner];
      cost += -(cost*chr_adj()) -
	       (cost*rgold_adj[o_ptr->owner_race][py.misc.prace]);
      if (cost < 1)  cost = 1;
      max_sell = (cost*(1+o_ptr->max_inflate));
      max_buy  = (cost*(1-o_ptr->max_inflate));
      min_buy  = (cost*(1-o_ptr->min_inflate));
      if (min_buy < 1) min_buy = 1;
      if (max_buy < 1) max_buy = 1;
      if (min_buy < max_buy)  min_buy = max_buy;
      min_per  = o_ptr->haggle_per;
      max_per  = min_per*3.0;
      max_gold = o_ptr->max_cost;
    }
  if (!flag)
    {
      haggle_commands(-1);
      if (max_buy > max_gold)
	{
	  final_flag= 1;
	  (void) strcpy(comment, "Final offer : ");
	  cur_ask   = max_gold;
	  final_ask = max_gold;
 msg_print("I am sorry, but I have not the money to afford such a fine item.");
	  /* make sure player see the message */
	  msg_print(" ");
	}
      else
	{
	  cur_ask   = max_buy;
	  final_ask = min_buy;
	  if (final_ask > max_gold)
	    final_ask = max_gold;
	  (void) strcpy(comment, "Offer : ");
	}
      min_offer = max_sell;
      last_offer = min_offer;
      if (cur_ask < 1)  cur_ask = 1;
      do
	{
	  do
	    {
	      loop_flag = TRUE;
	      (void) sprintf(out_val, "%s %d", comment, cur_ask);
	      put_buffer(out_val, 1, 0);
	      switch(receive_offer(store_num, "What price do you ask? ",
				 &new_offer, last_offer, -1))
		{
		case 1:
		  sell = 1;
		  flag   = TRUE;
		  break;
		case 2:
		  sell = 2;
		  flag   = TRUE;
		  break;
		default:
		  if (new_offer < cur_ask)
		    {
		      prt_comment6();
		    }
		  else if (new_offer == cur_ask)
		    {
		      flag = TRUE;
		      *price = new_offer;
		    }
		  else
		    loop_flag = FALSE;
	        }
	    }
	  while (!flag && loop_flag);
	  if (!flag)
	    {
	      msg_flag = FALSE;
	      x1 = (double)(last_offer - new_offer)/
		(double)(last_offer - cur_ask);
	      if (x1 < min_per)
		{
		  flag = haggle_insults(store_num);
		  if (flag)  sell = 2;
		}
	      else
		{
		  if (x1 > max_per)
		    {
		      x1 = x1 * 0.75;
		      if (x1 < max_per)  x1 = max_per;
		    }
		}
	      x2 = (x1 + (randint(5) - 3)/100.0);
	      x3 = ((new_offer-cur_ask)*x2) + 1;
	      /* don't let the price go down */
	      if (x3 < 0)
		x3 = 0;
	      cur_ask += x3;
	      if (cur_ask > final_ask)
		{
		  cur_ask = final_ask;
		  (void) strcpy(comment, "Final Offer : ");
		  final_flag++;
		  if (final_flag > 3)
		    {
		      if (increase_insults(store_num))
			sell = 2;
		      else
			sell = 1;
		      flag = TRUE;
		    }
		}
	      else if (new_offer <= cur_ask)
		{
		  flag = TRUE;
		  *price = new_offer;
		}
	      if (!flag)
		{
		  last_offer = new_offer;
		  prt("", 1, 0);  /* clear the line */
		  (void) sprintf(out_val, "Your last bid %d", last_offer);
		  put_buffer(out_val, 1, 39);
		  prt_comment3(cur_ask, last_offer, final_flag);
		}
	    }
	}
      while (!flag);
      prt("", 1, 0);  /* clear the line */
      display_commands();
    }
  return(sell);
}


/* Buy an item from a store				-RAK-	*/
int store_purchase(store_num, cur_top)
int store_num;
int *cur_top;
{
  int i, item_val, price;
  int item_new, choice;
  int save_number;
  vtype out_val, tmp_str;
  register store_type *s_ptr;
  register treasure_type *i_ptr;
  register inven_record *r_ptr;
  int purchase;

  purchase = FALSE;
  s_ptr = &store[store_num];
  /* i == number of objects shown on screen	*/
  if (*cur_top == 12)
    i = s_ptr->store_ctr - 1 - 12;
  else if (s_ptr->store_ctr > 11)
    i = 11;
  else
    i = s_ptr->store_ctr - 1;
  if (s_ptr->store_ctr < 1)
    msg_print("I am currently out of stock.");
      /* Get the item number to be bought		*/
  else if (get_store_item(&item_val,
			  "Which item are you interested in? ", 0, i))
    {
      item_val = item_val + *cur_top;	/* TRUE item_val	*/
      inventory[INVEN_MAX] = s_ptr->store_inven[item_val].sitem;
      i_ptr = &inventory[INVEN_MAX];
      if ((i_ptr->subval > 255) && (i_ptr->subval < 512))
	{
	  save_number = i_ptr->number;
	  i_ptr->number = 1;
	}
      else
	save_number = 1;
      if (inven_check_weight())
	if (inven_check_num())
	  {
	    if (s_ptr->store_inven[item_val].scost > 0)
	      {
		price = s_ptr->store_inven[item_val].scost;
		choice = 0;
	      }
	    else
	      choice = purchase_haggle(store_num, &price,
				       inventory[INVEN_MAX]);
	    switch(choice)
	      {
	      case 0:
		if (py.misc.au >= price)
		  {
		    prt_comment1();
		    decrease_insults(store_num);
		    py.misc.au -= price;
		    store_destroy(store_num, item_val, TRUE);
		    inven_carry(&item_new);
		    objdes(tmp_str, item_new, TRUE);
		    (void) sprintf(out_val, "You have %s (%c)",
				   tmp_str, item_new+97);
		    msg_print(out_val);
		    if (*cur_top >= s_ptr->store_ctr)
		      {
			*cur_top = 0;
			display_inventory(store_num, *cur_top);
		      }
		    else
		      {
			r_ptr = &s_ptr->store_inven[item_val];
			if (save_number > 1)
			  {
			    if (r_ptr->scost < 0)
			      {
				r_ptr->scost = price;
				display_cost(store_num, item_val);
			      }
			  }
			else
			  display_inventory(store_num, item_val);
			store_prt_gold();
		      }
		  }
		else
		  {
		    if (increase_insults(store_num))
		      purchase = TRUE;
		    else
		      {
			prt_comment1();
			msg_print("Liar!  You have not the gold!");
		      }
		  }
		break;
	      case 2:
		purchase = TRUE;
		break;
	      default:
		break;
	      }
	    prt("", 1, 0);  /* clear the line */
	  }
	else
	  prt("You cannot carry that many different items.", 0, 0);
      else
	prt("You can not carry that much weight.", 0, 0);
    }
  return(purchase);
}


/* Sell an item to the store				-RAK-	*/
int store_sell(store_num, cur_top)
int store_num, cur_top;
{
  int item_val;
  int item_pos, price;
  int redraw;
  vtype out_val, tmp_str;
  register treasure_type *i_ptr;
  register int sell;

  sell = FALSE;
  redraw = FALSE;
  if (get_item(&item_val, "Which one? ", &redraw, 0, inven_ctr-1))
    {
      if (redraw)  display_store(store_num, cur_top);
      inventory[INVEN_MAX] = inventory[item_val];
      i_ptr = &inventory[INVEN_MAX];
      if ((i_ptr->subval > 255) && (i_ptr->subval < 512))
	i_ptr->number = 1;
      objdes(tmp_str, INVEN_MAX, TRUE);
      (void) sprintf(out_val, "Selling %s (%c)", tmp_str, item_val+97);
      msg_print(out_val);
      /* make sure player sees the message */
      msg_print(" ");
      if ((store_buy[store_num])(inventory[INVEN_MAX].tval))
	if (store_check_num(store_num))
	  switch(sell_haggle(store_num, &price, inventory[INVEN_MAX]))
	    {
	    case 0:
	      prt_comment1();
	      py.misc.au += price;
	      inven_destroy(item_val);
	      store_carry(store_num, &item_pos);
	      if (item_pos >= 0)
		if (item_pos < 12)
		  if (cur_top < 12)
		    display_inventory(store_num, item_pos);
		  else
		    display_inventory(store_num, cur_top);
		else if (cur_top > 11)
		  display_inventory(store_num, item_pos);
	      store_prt_gold();
	      break;
	    case 2:
	      sell = TRUE;
	      break;
	    case 3:
	      msg_print("How dare you!");
	      msg_print("I will not buy that!");
	      sell = increase_insults(store_num);
	      break;
	    default:
	      break;
	    }
	else
	  prt("I have not the room in my store to keep it...", 0, 0);
      else
	prt("I do not buy such items.", 0, 0);
    }
  else if (redraw)
    display_store(store_num, cur_top);
  return(sell);
}


/* Entering a store					-RAK-	*/
enter_store(store_num)
int store_num;
{
  int com_val, cur_top;
  char command;
  register int exit_flag;
  register store_type *s_ptr;

  s_ptr = &store[store_num];
  if (s_ptr->store_open < turn)
    {
      exit_flag = FALSE;
      cur_top = 0;
      display_store(store_num, cur_top);
      do
	{
	  if (get_com("", &command))
	    {
	      msg_flag = FALSE;
	      com_val = (command);
	      switch(com_val)
		{
		case 18:
		  display_store(store_num, cur_top);
		  break;
		case 98:
		  if (cur_top == 0)
		    if (s_ptr->store_ctr > 12)
		      {
			cur_top = 12;
			display_inventory(store_num, cur_top);
		      }
		    else
		      prt("Entire inventory is shown.", 0, 0);
		  else
		    {
		      cur_top = 0;
		      display_inventory(store_num, cur_top);
		    }
		  break;
		case 101:      /* Equipment List	*/
		  if (inven_command('e', 0, 0))
		    display_store(store_num, cur_top);
		  break;
		case 105:      /* Inventory		*/
		  if (inven_command('i', 0, 0))
		    display_store(store_num, cur_top);
		  break;
		case 116:      /* Take off		*/
		  if (inven_command('t', 0, 0))
		    display_store(store_num, cur_top);
		  break;
		case 119:	/* Wear			*/
		  if (inven_command('w', 0, 0))
		    display_store(store_num, cur_top);
		  break;
		case 120:     /* Switch weapon		*/
		  if (inven_command('x', 0, 0))
		    display_store(store_num, cur_top);
		  break;
		case 112:
		  exit_flag = store_purchase(store_num, &cur_top);
		  break;
		case 115:
		  exit_flag = store_sell(store_num, cur_top);
		  break;
		default:
		  prt("Invalid Command.", 0, 0);
		  break;
		}
	    }
	  else
	    exit_flag = TRUE;
	}
      while (!exit_flag);
      draw_cave();
    }
  else
    msg_print("The doors are locked.");
}
