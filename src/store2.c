/* source/store2.c: store code, entering, command interpreter, buying, selling
 *
 * Copyright (C) 1989-2008 James E. Wilson, Robert A. Koeneke,
 *                         David J. Grabiner
 *
 * This file is part of Umoria.
 *
 * Umoria is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Umoria is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Umoria.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "standard_library.h"

#include "config.h"
#include "constant.h"
#include "types.h"

#include "externs.h"

static char *comment1[14] = {
    "Done!",
    "Accepted!",
    "Fine.",
    "Agreed!",
    "Ok.",
    "Taken!",
    "You drive a hard bargain, but taken.",
    "You'll force me bankrupt, but it's a deal.",
    "Sigh.  I'll take it.",
    "My poor sick children may starve, but done!",
    "Finally!  I accept.",
    "Robbed again.",
    "A pleasure to do business with you!",
    "My spouse will skin me, but accepted.",
};

static char *comment2a[3] = {
    "%A2 is my final offer; take it or leave it.",
    "I'll give you no more than %A2.",
    "My patience grows thin.  %A2 is final.",
};

static char *comment2b[16] = {
    "%A1 for such a fine item?  HA!  No less than %A2.",
    "%A1 is an insult!  Try %A2 gold pieces.",
    "%A1?!?  You would rob my poor starving children?",
    "Why, I'll take no less than %A2 gold pieces.",
    "Ha!  No less than %A2 gold pieces.",
    "Thou knave!  No less than %A2 gold pieces.",
    "%A1 is far too little, how about %A2?",
    "I paid more than %A1 for it myself, try %A2.",
    "%A1?  Are you mad?!?  How about %A2 gold pieces?",
    "As scrap this would bring %A1.  Try %A2 in gold.",
    "May the fleas of 1000 orcs molest you.  I want %A2.",
    "My mother you can get for %A1, this costs %A2.",
    "May your chickens grow lips.  I want %A2 in gold!",
    "Sell this for such a pittance?  Give me %A2 gold.",
    "May the Balrog find you tasty!  %A2 gold pieces?",
    "Your mother was a Troll!  %A2 or I'll tell.",
};

static char *comment3a[3] = {
    "I'll pay no more than %A1; take it or leave it.",
    "You'll get no more than %A1 from me.",
    "%A1 and that's final.",
};

static char *comment3b[15] = {
    "%A2 for that piece of junk?  No more than %A1.",
    "For %A2 I could own ten of those.  Try %A1.",
    "%A2?  NEVER!  %A1 is more like it.",
    "Let's be reasonable. How about %A1 gold pieces?",
    "%A1 gold for that junk, no more.",
    "%A1 gold pieces and be thankful for it!",
    "%A1 gold pieces and not a copper more.",
    "%A2 gold?  HA!  %A1 is more like it.",
    "Try about %A1 gold.",
    "I wouldn't pay %A2 for your children, try %A1.",
    "*CHOKE* For that!?  Let's say %A1.",
    "How about %A1?",
    "That looks war surplus!  Say %A1 gold.",
    "I'll buy it as scrap for %A1.",
    "%A2 is too much, let us say %A1 gold.",
};

static char *comment4a[5] = {
    "ENOUGH!  You have abused me once too often!",
    "THAT DOES IT!  You shall waste my time no more!",
    "This is getting nowhere.  I'm going home!",
    "BAH!  No more shall you insult me!",
    "Begone!  I have had enough abuse for one day.",
};

static char *comment4b[5] = {
    "Out of my place!", "out... Out... OUT!!!",
    "Come back tomorrow.", "Leave my place.  Begone!",
    "Come back when thou art richer.",
};

static char *comment5[10] = {
    "You will have to do better than that!",
    "That's an insult!",
    "Do you wish to do business or not?",
    "Hah!  Try again.",
    "Ridiculous!",
    "You've got to be kidding!",
    "You'd better be kidding!",
    "You try my patience.",
    "I don't hear you.",
    "Hmmm, nice weather we're having.",
};

static char *comment6[5] = {
    "I must have heard you wrong.", "What was that?",
    "I'm sorry, say that again.", "What did you say?",
    "Sorry, what was that again?",
};

/* Comments vary.          -RAK- */
/* Comment one : Finished haggling */
static void prt_comment1() {
    msg_print(comment1[randint(14) - 1]);
}

/* %A1 is offer, %A2 is asking. */
static void prt_comment2(int32_t offer, int32_t asking, int final) {
    vtype comment;

    if (final > 0) {
        (void)strcpy(comment, comment2a[randint(3) - 1]);
    } else {
        (void)strcpy(comment, comment2b[randint(16) - 1]);
    }

    insert_lnum(comment, "%A1", offer, false);
    insert_lnum(comment, "%A2", asking, false);
    msg_print(comment);
}

static void prt_comment3(int32_t offer, int32_t asking, int final) {
    vtype comment;

    if (final > 0) {
        (void)strcpy(comment, comment3a[randint(3) - 1]);
    } else {
        (void)strcpy(comment, comment3b[randint(15) - 1]);
    }

    insert_lnum(comment, "%A1", offer, false);
    insert_lnum(comment, "%A2", asking, false);
    msg_print(comment);
}

/* Kick 'da bum out.          -RAK- */
static void prt_comment4() {
    int tmp = randint(5) - 1;
    msg_print(comment4a[tmp]);
    msg_print(comment4b[tmp]);
}

static void prt_comment5() {
    msg_print(comment5[randint(10) - 1]);
}

static void prt_comment6() {
    msg_print(comment6[randint(5) - 1]);
}

/* Displays the set of commands        -RAK- */
static void display_commands() {
    prt("You may:", 20, 0);
    prt(" p) Purchase an item.           b) Browse store's inventory.", 21, 0);
    prt(" s) Sell an item.               i/e/t/w/x) Inventory/Equipment Lists.", 22, 0);
    prt("ESC) Exit from Building.        ^R) Redraw the screen.", 23, 0);
}

/* Displays the set of commands        -RAK- */
static void haggle_commands(int typ) {
    if (typ == -1) {
        prt("Specify an asking-price in gold pieces.", 21, 0);
    } else {
        prt("Specify an offer in gold pieces.", 21, 0);
    }
    prt("ESC) Quit Haggling.", 22, 0);
    erase_line(23, 0); /* clear last line */
}

/* Displays a store's inventory        -RAK- */
static void display_inventory(int store_num, int start) {
    store_type *s_ptr = &store[store_num];

    int i = (start % 12);

    int stop = ((start / 12) + 1) * 12;
    if (stop > s_ptr->store_ctr) {
        stop = s_ptr->store_ctr;
    }

    while (start < stop) {
        inven_type *i_ptr = &s_ptr->store_inven[start].sitem;

        int32_t x = i_ptr->number;
        if ((i_ptr->subval >= ITEM_SINGLE_STACK_MIN) && (i_ptr->subval <= ITEM_SINGLE_STACK_MAX)) {
            i_ptr->number = 1;
        }

        bigvtype out_val1, out_val2;

        objdes(out_val1, i_ptr, true);
        i_ptr->number = x;
        (void)sprintf(out_val2, "%c) %s", 'a' + i, out_val1);
        prt(out_val2, i + 5, 0);
        x = s_ptr->store_inven[start].scost;
        if (x <= 0) {
            int32_t value = -x;
            value = value * chr_adj() / 100;
            if (value <= 0) {
                value = 1;
            }
            (void)sprintf(out_val2, "%9d", value);
        } else {
            (void)sprintf(out_val2, "%9d [Fixed]", x);
        }
        prt(out_val2, i + 5, 59);
        i++;
        start++;
    }
    if (i < 12) {
        for (int j = 0; j < (11 - i + 1); j++) {
            /* clear remaining lines */
            erase_line(j + i + 5, 0);
        }
    }
    if (s_ptr->store_ctr > 12) {
        put_buffer("- cont. -", 17, 60);
    } else {
        erase_line(17, 60);
    }
}

/* Re-displays only a single cost      -RAK- */
static void display_cost(int store_num, int pos) {
    store_type *s_ptr = &store[store_num];

    int i = (pos % 12);

    vtype out_val;
    if (s_ptr->store_inven[pos].scost < 0) {
        int32_t j = -s_ptr->store_inven[pos].scost;
        j = j * chr_adj() / 100;
        (void)sprintf(out_val, "%d", j);
    } else {
        (void)sprintf(out_val, "%9d [Fixed]", s_ptr->store_inven[pos].scost);
    }
    prt(out_val, i + 5, 59);
}

/* Displays players gold          -RAK- */
static void store_prt_gold() {
    vtype out_val;
    (void)sprintf(out_val, "Gold Remaining : %d", py.misc.au);
    prt(out_val, 18, 17);
}

/* Displays store          -RAK- */
static void display_store(int store_num, int cur_top) {
    store_type *s_ptr = &store[store_num];

    clear_screen();
    put_buffer(owners[s_ptr->owner].owner_name, 3, 9);
    put_buffer("Item", 4, 3);
    put_buffer("Asking Price", 4, 60);
    store_prt_gold();
    display_commands();
    display_inventory(store_num, cur_top);
}

/* Get the ID of a store item and return it's value  -RAK- */
static bool get_store_item(int *com_val, char *pmt, int i, int j) {
    bool flag = false;

    *com_val = -1;

    vtype out_val;
    (void)sprintf(out_val, "(Items %c-%c, ESC to exit) %s", i + 'a', j + 'a', pmt);

    char command;
    while (get_com(out_val, &command)) {
        command -= 'a';
        if (command >= i && command <= j) {
            flag = true;
            *com_val = command;
            break;
        }
        bell();
    }
    erase_line(MSG_LINE, 0);

    return flag;
}

/* Increase the insult counter and get angry if too many -RAK- */
static bool increase_insults(int store_num) {
    bool increase = false;

    store_type *s_ptr = &store[store_num];
    s_ptr->insult_cur++;

    if (s_ptr->insult_cur > owners[s_ptr->owner].insult_max) {
        prt_comment4();
        s_ptr->insult_cur = 0;
        s_ptr->bad_buy++;
        s_ptr->store_open = turn + 2500 + randint(2500);
        increase = true;
    }

    return increase;
}

/* Decrease insults          -RAK- */
static void decrease_insults(int store_num) {
    store_type *s_ptr = &store[store_num];

    if (s_ptr->insult_cur != 0) {
        s_ptr->insult_cur--;
    }
}

/* Have insulted while haggling        -RAK- */
static bool haggle_insults(int store_num) {
    bool haggle = false;

    if (increase_insults(store_num)) {
        haggle = true;
    } else {
        prt_comment5();
        msg_print(CNIL); /* keep insult separate from rest of haggle */
    }

    return haggle;
}

static bool get_haggle(char *comment, int32_t *new_offer, int num_offer) {
    bool flag = true;
    bool increment = false;

    int clen = strlen(comment);
    int orig_clen = clen;

    if (num_offer == 0) {
        last_store_inc = 0;
    }

    char *p;
    int32_t i = 0;
    vtype out_val, default_offer;

    do {
        prt(comment, 0, 0);
        if (num_offer && last_store_inc != 0) {
            (void)sprintf(default_offer, "[%c%d] ", (last_store_inc < 0) ? '-' : '+', abs(last_store_inc));
            prt(default_offer, 0, orig_clen);
            clen = orig_clen + strlen(default_offer);
        }
        if (!get_string(out_val, 0, clen, 40)) {
            flag = false;
        }
        for (p = out_val; *p == ' '; p++) {
            ;
        }
        if (*p == '+' || *p == '-') {
            increment = true;
        }
        if (num_offer && increment) {
            i = atol(out_val);
            /* Don't accept a zero here.  Turn off increment if it was zero
               because a zero will not exit.  This can be zero if the user
               did not type a number after the +/- sign. */
            if (i == 0) {
                increment = false;
            } else {
                last_store_inc = i;
            }
        } else if (num_offer && *out_val == '\0') {
            i = last_store_inc;
            increment = true;
        } else {
            i = atol(out_val);
        }

        /* don't allow incremental haggling, if player has not made an offer yet */
        if (flag && num_offer == 0 && increment) {
            msg_print("You haven't even made your first offer yet!");
            i = 0;
            increment = false;
        }
    } while (flag && (i == 0));

    if (flag) {
        if (increment) {
            *new_offer += i;
        } else {
            *new_offer = i;
        }
    } else {
        erase_line(0, 0);
    }

    return flag;
}

static int receive_offer(int store_num, char *comment, int32_t *new_offer, int32_t last_offer, int num_offer, int factor) {
    int receive = 0;

    bool flag = false;
    do {
        if (get_haggle(comment, new_offer, num_offer)) {
            if (*new_offer * factor >= last_offer * factor) {
                flag = true;
            } else if (haggle_insults(store_num)) {
                receive = 2;
                flag = true;
            } else {
                /* new_offer rejected, reset new_offer so that incremental
                   haggling works correctly */
                *new_offer = last_offer;
            }
        } else {
            receive = 1;
            flag = true;
        }
    } while (!flag);

    return receive;
}

/* Haggling routine          -RAK- */
static int purchase_haggle(int store_num, int32_t *price, inven_type *item) {
    bool flag = false;
    bool didnt_haggle = false;

    *price = 0;
    int purchase = 0;
    int final_flag = 0;

    store_type *s_ptr = &store[store_num];
    owner_type *o_ptr = &owners[s_ptr->owner];

    int32_t max_sell, min_sell;
    int32_t cost = sell_price(store_num, &max_sell, &min_sell, item);

    max_sell = max_sell * chr_adj() / 100;
    if (max_sell <= 0) {
        max_sell = 1;
    }

    min_sell = min_sell * chr_adj() / 100;
    if (min_sell <= 0) {
        min_sell = 1;
    }

    /* cast max_inflate to signed so that subtraction works correctly */
    int32_t max_buy = cost * (200 - (int)o_ptr->max_inflate) / 100;
    if (max_buy <= 0) {
        max_buy = 1;
    }

    int32_t min_per = o_ptr->haggle_per;
    int32_t max_per = min_per * 3;

    haggle_commands(1);

    int32_t cur_ask = max_sell;
    int32_t final_ask = min_sell;
    int32_t min_offer = max_buy;
    int32_t last_offer = min_offer;
    int32_t new_offer = 0;
    int num_offer = 0; /* this prevents incremental haggling on first try */
    char *comment = "Asking";

    /* go right to final price if player has bargained well */
    if (noneedtobargain(store_num, final_ask)) {
        msg_print("After a long bargaining session, you agree upon the price.");
        cur_ask = min_sell;
        comment = "Final offer";
        didnt_haggle = true;

        /* Set up automatic increment, so that a return will accept the final
         * price. */
        last_store_inc = min_sell;
        num_offer = 1;
    }

    vtype out_val;

    do {
        bool loop_flag;
        do {
            loop_flag = true;

            (void)sprintf(out_val, "%s :  %d", comment, cur_ask);
            put_buffer(out_val, 1, 0);

            purchase = receive_offer(store_num, "What do you offer? ", &new_offer, last_offer, num_offer, 1);
            if (purchase != 0) {
                flag = true;
            } else {
                if (new_offer > cur_ask) {
                    prt_comment6();
                    /* rejected, reset new_offer for incremental haggling */
                    new_offer = last_offer;

                    /* If the automatic increment is large enough to overflow,
                       then the player must have made a mistake.  Clear it
                       because it is useless. */
                    if (last_offer + last_store_inc > cur_ask) {
                        last_store_inc = 0;
                    }
                } else if (new_offer == cur_ask) {
                    flag = true;
                    *price = new_offer;
                } else {
                    loop_flag = false;
                }
            }
        } while (!flag && loop_flag);

        if (!flag) {
            int32_t x1 = (new_offer - last_offer) * 100 / (cur_ask - last_offer);
            if (x1 < min_per) {
                flag = haggle_insults(store_num);
                if (flag) {
                    purchase = 2;
                }
            } else if (x1 > max_per) {
                x1 = x1 * 75 / 100;
                if (x1 < max_per) {
                    x1 = max_per;
                }
            }
            int32_t x2 = x1 + randint(5) - 3;
            int32_t x3 = ((cur_ask - new_offer) * x2 / 100) + 1;

            /* don't let the price go up */
            if (x3 < 0) {
                x3 = 0;
            }
            cur_ask -= x3;
            if (cur_ask < final_ask) {
                cur_ask = final_ask;
                comment = "Final Offer";

                /* Set the automatic haggle increment so that RET will give
                   a new_offer equal to the final_ask price. */
                last_store_inc = final_ask - new_offer;
                final_flag++;
                if (final_flag > 3) {
                    if (increase_insults(store_num)) {
                        purchase = 2;
                    } else {
                        purchase = 1;
                    }
                    flag = true;
                }
            } else if (new_offer >= cur_ask) {
                flag = true;
                *price = new_offer;
            }
            if (!flag) {
                last_offer = new_offer;
                num_offer++; /* enable incremental haggling */
                erase_line(1, 0);
                (void)sprintf(out_val, "Your last offer : %d", last_offer);
                put_buffer(out_val, 1, 39);
                prt_comment2(last_offer, cur_ask, final_flag);

                /* If the current increment would take you over the store's
                   price, then decrease it to an exact match. */
                if (cur_ask - last_offer < last_store_inc) {
                    last_store_inc = cur_ask - last_offer;
                }
            }
        }
    } while (!flag);

    /* update bargaining info */
    if ((purchase == 0) && (!didnt_haggle)) {
        updatebargain(store_num, *price, final_ask);
    }

    return purchase;
}

/* Haggling routine          -RAK- */
static int sell_haggle(int store_num, int32_t *price, inven_type *item) {
    int32_t max_gold;
    int32_t min_per, max_per;
    int32_t max_sell, min_buy, max_buy;

    bool flag = false;
    bool didnt_haggle = false;

    *price = 0;
    int sell = 0;
    int final_flag = 0;

    store_type *s_ptr = &store[store_num];

    int32_t cost = item_value(item);
    if (cost < 1) {
        sell = 3;
        flag = true;
    } else {
        owner_type *o_ptr = &owners[s_ptr->owner];

        cost = cost * (200 - chr_adj()) / 100;
        cost = cost * (200 - rgold_adj[o_ptr->owner_race][py.misc.prace]) / 100;
        if (cost < 1) {
            cost = 1;
        }
        max_sell = cost * o_ptr->max_inflate / 100;

        /* cast max_inflate to signed so that subtraction works correctly */
        max_buy = cost * (200 - (int)o_ptr->max_inflate) / 100;
        min_buy = cost * (200 - o_ptr->min_inflate) / 100;
        if (min_buy < 1) {
            min_buy = 1;
        }
        if (max_buy < 1) {
            max_buy = 1;
        }
        if (min_buy < max_buy) {
            min_buy = max_buy;
        }
        min_per = o_ptr->haggle_per;
        max_per = min_per * 3;
        max_gold = o_ptr->max_cost;
    }

    int32_t cur_ask, final_ask;
    char *comment;

    if (!flag) {
        haggle_commands(-1);

        int num_offer = 0; /* this prevents incremental haggling on first try */

        if (max_buy > max_gold) {
            final_flag = 1;
            comment = "Final Offer";

            /* Disable the automatic haggle increment on RET. */
            last_store_inc = 0;
            cur_ask = max_gold;
            final_ask = max_gold;
            msg_print("I am sorry, but I have not the money to afford such a ""fine item.");
            didnt_haggle = true;
        } else {
            cur_ask = max_buy;
            final_ask = min_buy;
            if (final_ask > max_gold) {
                final_ask = max_gold;
            }
            comment = "Offer";

            /* go right to final price if player has bargained well */
            if (noneedtobargain(store_num, final_ask)) {
                msg_print("After a long bargaining session, you agree upon the ""price.");
                cur_ask = final_ask;
                comment = "Final offer";
                didnt_haggle = true;

                /* Set up automatic increment, so that a return will accept the
                   final price. */
                last_store_inc = final_ask;
                num_offer = 1;
            }
        }

        int32_t min_offer = max_sell;
        int32_t last_offer = min_offer;
        int32_t new_offer = 0;

        if (cur_ask < 1) {
            cur_ask = 1;
        }

        do {
            bool loop_flag;
            do {
                loop_flag = true;

                vtype out_val;
                (void)sprintf(out_val, "%s :  %d", comment, cur_ask);
                put_buffer(out_val, 1, 0);
                sell = receive_offer(store_num, "What price do you ask? ", &new_offer, last_offer, num_offer, -1);
                if (sell != 0) {
                    flag = true;
                } else {
                    if (new_offer < cur_ask) {
                        prt_comment6();

                        /* rejected, reset new_offer for incremental haggling */
                        new_offer = last_offer;

                        /* If the automatic increment is large enough to
                           overflow, then the player must have made a mistake.
                           Clear it because it is useless. */
                        if (last_offer + last_store_inc < cur_ask) {
                            last_store_inc = 0;
                        }
                    } else if (new_offer == cur_ask) {
                        flag = true;
                        *price = new_offer;
                    } else {
                        loop_flag = false;
                    }
                }
            } while (!flag && loop_flag);

            if (!flag) {
                int32_t x1 = (last_offer - new_offer) * 100 / (last_offer - cur_ask);

                if (x1 < min_per) {
                    flag = haggle_insults(store_num);
                    if (flag) {
                        sell = 2;
                    }
                } else if (x1 > max_per) {
                    x1 = x1 * 75 / 100;
                    if (x1 < max_per) {
                        x1 = max_per;
                    }
                }
                int32_t x2 = x1 + randint(5) - 3;
                int32_t x3 = ((new_offer - cur_ask) * x2 / 100) + 1;

                /* don't let the price go down */
                if (x3 < 0) {
                    x3 = 0;
                }

                cur_ask += x3;
                if (cur_ask > final_ask) {
                    cur_ask = final_ask;
                    comment = "Final Offer";

                    /* Set the automatic haggle increment so that RET will give
                       a new_offer equal to the final_ask price. */
                    last_store_inc = final_ask - new_offer;
                    final_flag++;
                    if (final_flag > 3) {
                        if (increase_insults(store_num)) {
                            sell = 2;
                        } else {
                            sell = 1;
                        }
                        flag = true;
                    }
                } else if (new_offer <= cur_ask) {
                    flag = true;
                    *price = new_offer;
                }

                if (!flag) {
                    last_offer = new_offer;
                    num_offer++; /* enable incremental haggling */
                    erase_line(1, 0);

                    vtype out_val;
                    (void)sprintf(out_val, "Your last bid %d", last_offer);
                    put_buffer(out_val, 1, 39);
                    prt_comment3(cur_ask, last_offer, final_flag);

                    /* If the current decrement would take you under the store's
                       price, then increase it to an exact match. */
                    if (cur_ask - last_offer > last_store_inc) {
                        last_store_inc = cur_ask - last_offer;
                    }
                }
            }
        } while (!flag);
    }

    /* update bargaining info */
    if ((sell == 0) && (!didnt_haggle)) {
        updatebargain(store_num, *price, final_ask);
    }

    return sell;
}

/* Buy an item from a store        -RAK- */
static bool store_purchase(int store_num, int *cur_top) {
    bool purchase = false;

    store_type *s_ptr = &store[store_num];

    /* i == number of objects shown on screen */
    int i;
    if (*cur_top == 12) {
        i = s_ptr->store_ctr - 1 - 12;
    } else if (s_ptr->store_ctr > 11) {
        i = 11;
    } else {
        i = s_ptr->store_ctr - 1;
    }

    int item_val;

    if (s_ptr->store_ctr < 1) {
        msg_print("I am currently out of stock.");
    } else if (get_store_item(&item_val, "Which item are you interested in? ", 0, i)) {
        /* Get the item number to be bought */

        item_val = item_val + *cur_top; /* true item_val */

        inven_type sell_obj;
        take_one_item(&sell_obj, &s_ptr->store_inven[item_val].sitem);

        if (inven_check_num(&sell_obj)) {
            int choice;
            int32_t price;

            if (s_ptr->store_inven[item_val].scost > 0) {
                price = s_ptr->store_inven[item_val].scost;
                choice = 0;
            } else {
                choice = purchase_haggle(store_num, &price, &sell_obj);
            }

            if (choice == 0) {
                if (py.misc.au >= price) {
                    prt_comment1();
                    decrease_insults(store_num);
                    py.misc.au -= price;

                    int item_new = inven_carry(&sell_obj);
                    i = s_ptr->store_ctr;
                    store_destroy(store_num, item_val, true);

                    bigvtype out_val, tmp_str;
                    objdes(tmp_str, &inventory[item_new], true);
                    (void)sprintf(out_val, "You have %s (%c)", tmp_str, item_new + 'a');
                    prt(out_val, 0, 0);

                    check_strength();
                    if (*cur_top >= s_ptr->store_ctr) {
                        *cur_top = 0;
                        display_inventory(store_num, *cur_top);
                    } else {
                        inven_record *r_ptr = &s_ptr->store_inven[item_val];

                        if (i == s_ptr->store_ctr) {
                            if (r_ptr->scost < 0) {
                                r_ptr->scost = price;
                                display_cost(store_num, item_val);
                            }
                        } else {
                            display_inventory(store_num, item_val);
                        }
                    }
                    store_prt_gold();
                } else {
                    if (increase_insults(store_num)) {
                        purchase = true;
                    } else {
                        prt_comment1();
                        msg_print("Liar!  You have not the gold!");
                    }
                }
            } else if (choice == 2) {
                purchase = true;
            }

            /* Less intuitive, but looks better here than in purchase_haggle. */
            display_commands();
            erase_line(1, 0);
        } else {
            prt("You cannot carry that many different items.", 0, 0);
        }
    }

    return purchase;
}

/* Sell an item to the store        -RAK- */
static bool store_sell(int store_num, int *cur_top) {
    bool sell = false;

    int first_item = inven_ctr;
    int last_item = -1;

    char mask[INVEN_WIELD];

    for (int counter = 0; counter < inven_ctr; counter++) {
        int flag = (*store_buy[store_num])(inventory[counter].tval);

        mask[counter] = flag;
        if (flag) {
            if (counter < first_item) {
                first_item = counter;
            }
            if (counter > last_item) {
                last_item = counter;
            }
        } /* end of if (flag) */
    } /* end of for (counter) */

    int item_val;

    if (last_item == -1) {
        msg_print("You have nothing to sell to this store!");
    } else if (get_item(&item_val, "Which one? ", first_item, last_item, mask, "I do not buy such items.")) {
        inven_type sold_obj;
        bigvtype out_val, tmp_str;

        take_one_item(&sold_obj, &inventory[item_val]);
        objdes(tmp_str, &sold_obj, true);

        (void)sprintf(out_val, "Selling %s (%c)", tmp_str, item_val + 'a');
        msg_print(out_val);

        if (store_check_num(&sold_obj, store_num)) {
            int32_t price;

            int choice = sell_haggle(store_num, &price, &sold_obj);
            if (choice == 0) {
                prt_comment1();
                decrease_insults(store_num);
                py.misc.au += price;

                /* identify object in inventory to set object_ident */
                identify(&item_val);

                /* retake sold_obj so that it will be identified */
                take_one_item(&sold_obj, &inventory[item_val]);

                /* call known2 for store item, so charges/pluses are known */
                known2(&sold_obj);
                inven_destroy(item_val);
                objdes(tmp_str, &sold_obj, true);
                (void)sprintf(out_val, "You've sold %s", tmp_str);
                msg_print(out_val);

                int item_pos;
                store_carry(store_num, &item_pos, &sold_obj);

                check_strength();

                if (item_pos >= 0) {
                    if (item_pos < 12) {
                        if (*cur_top < 12) {
                            display_inventory(store_num, item_pos);
                        } else {
                            *cur_top = 0;
                            display_inventory(store_num, *cur_top);
                        }
                    } else if (*cur_top > 11) {
                        display_inventory(store_num, item_pos);
                    } else {
                        *cur_top = 12;
                        display_inventory(store_num, *cur_top);
                    }
                }
                store_prt_gold();
            } else if (choice == 2) {
                sell = true;
            } else if (choice == 3) {
                msg_print("How dare you!");
                msg_print("I will not buy that!");
                sell = increase_insults(store_num);
            }

            /* Less intuitive, but looks better here than in sell_haggle. */
            erase_line(1, 0);
            display_commands();
        } else {
            msg_print("I have not the room in my store to keep it.");
        }
    }
    return sell;
}

/* Entering a store          -RAK- */
void enter_store(int store_num) {
    store_type *s_ptr = &store[store_num];

    if (s_ptr->store_open < turn) {
        bool exit_flag = false;
        int cur_top = 0;
        display_store(store_num, cur_top);

        do {
            move_cursor(20, 9);

            /* clear the msg flag just like we do in dungeon.c */
            msg_flag = false;

            char command;
            if (get_com(CNIL, &command)) {
                int tmp_chr;

                switch (command) {
                case 'b':
                    if (cur_top == 0) {
                        if (s_ptr->store_ctr > 12) {
                            cur_top = 12;
                            display_inventory(store_num, cur_top);
                        } else {
                            msg_print("Entire inventory is shown.");
                        }
                    } else {
                        cur_top = 0;
                        display_inventory(store_num, cur_top);
                    }
                    break;
                case 'E': case 'e': /* Equipment List */
                case 'I': case 'i': /* Inventory */
                case 'T': case 't': /* Take off */
                case 'W': case 'w': /* Wear */
                case 'X': case 'x': /* Switch weapon */
                    tmp_chr = py.stats.use_stat[A_CHR];

                    do {
                        inven_command(command);
                        command = doing_inven;
                    } while (command);

                    /* redisplay store prices if charisma changes */
                    if (tmp_chr != py.stats.use_stat[A_CHR]) {
                        display_inventory(store_num, cur_top);
                    }
                    free_turn_flag = false; /* No free moves here. -CJS- */
                    break;
                case 'p':
                    exit_flag = store_purchase(store_num, &cur_top);
                    break;
                case 's':
                    exit_flag = store_sell(store_num, &cur_top);
                    break;
                default:
                    bell();
                    break;
                }
            } else {
                exit_flag = true;
            }
        } while (!exit_flag);

        /* Can't save and restore the screen because inven_command does that. */
        draw_cave();
    } else {
        msg_print("The doors are locked.");
    }
}
