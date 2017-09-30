// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Store code, entering, command interpreter, buying, selling

#include "headers.h"
#include "externs.h"

// Save the store's last increment value.
static int16_t store_last_increment;

// Comments vary. -RAK-
// Comment one : Finished haggling
static void printSpeechFinishedHaggling() {
    printMessage(speech_sale_accepted[randomNumber(14) - 1]);
}

// %A1 is offer, %A2 is asking.
static void printSpeechSellingHaggle(int32_t offer, int32_t asking, int final) {
    vtype_t comment = {'\0'};

    if (final > 0) {
        (void) strcpy(comment, speech_selling_haggle_final[randomNumber(3) - 1]);
    } else {
        (void) strcpy(comment, speech_selling_haggle[randomNumber(16) - 1]);
    }

    insertNumberIntoString(comment, "%A1", offer, false);
    insertNumberIntoString(comment, "%A2", asking, false);
    printMessage(comment);
}

static void printSpeechBuyingHaggle(int32_t offer, int32_t asking, int final) {
    vtype_t comment = {'\0'};

    if (final > 0) {
        (void) strcpy(comment, speech_buying_haggle_final[randomNumber(3) - 1]);
    } else {
        (void) strcpy(comment, speech_buying_haggle[randomNumber(15) - 1]);
    }

    insertNumberIntoString(comment, "%A1", offer, false);
    insertNumberIntoString(comment, "%A2", asking, false);
    printMessage(comment);
}

// Kick 'da bum out. -RAK-
static void printSpeechGetOutOfMyStore() {
    int comment = randomNumber(5) - 1;
    printMessage(speech_insulted_haggling_done[comment]);
    printMessage(speech_get_out_of_my_store[comment]);
}

static void printSpeechTryAgain() {
    printMessage(speech_haggling_try_again[randomNumber(10) - 1]);
}

static void printSpeechSorry() {
    printMessage(speech_sorry[randomNumber(5) - 1]);
}

// Displays the set of commands -RAK-
static void displayStoreCommands() {
    putStringClearToEOL("You may:", 20, 0);
    putStringClearToEOL(" p) Purchase an item.           b) Browse store's inventory.", 21, 0);
    putStringClearToEOL(" s) Sell an item.               i/e/t/w/x) Inventory/Equipment Lists.", 22, 0);
    putStringClearToEOL("ESC) Exit from Building.        ^R) Redraw the screen.", 23, 0);
}

// Displays the set of commands -RAK-
static void displayStoreHaggleCommands(int haggle_type) {
    if (haggle_type == -1) {
        putStringClearToEOL("Specify an asking-price in gold pieces.", 21, 0);
    } else {
        putStringClearToEOL("Specify an offer in gold pieces.", 21, 0);
    }

    putStringClearToEOL("ESC) Quit Haggling.", 22, 0);
    eraseLine(23, 0); // clear last line
}

// Displays a store's inventory -RAK-
static void displayStoreInventory(int store_id, int item_pos_start) {
    Store_t *store = &stores[store_id];

    int item_pos_end = ((item_pos_start / 12) + 1) * 12;
    if (item_pos_end > store->store_id) {
        item_pos_end = store->store_id;
    }

    int item_identifier;

    for (item_identifier = (item_pos_start % 12); item_pos_start < item_pos_end; item_identifier++) {
        Inventory_t *item = &store->inventory[item_pos_start].item;

        // Save the current number of items
        int32_t current_item_count = item->items_count;

        if (item->sub_category_id >= ITEM_SINGLE_STACK_MIN && item->sub_category_id <= ITEM_SINGLE_STACK_MAX) {
            item->items_count = 1;
        }

        obj_desc_t description = {'\0'};
        itemDescription(description, item, true);

        // Restore the number of items
        item->items_count = (uint8_t) current_item_count;

        obj_desc_t msg = {'\0'};
        (void) sprintf(msg, "%c) %s", 'a' + item_identifier, description);
        putStringClearToEOL(msg, item_identifier + 5, 0);

        current_item_count = store->inventory[item_pos_start].cost;

        if (current_item_count <= 0) {
            int32_t value = -current_item_count;
            value = value * playerStatAdjustmentCharisma() / 100;
            if (value <= 0) {
                value = 1;
            }
            (void) sprintf(msg, "%9d", value);
        } else {
            (void) sprintf(msg, "%9d [Fixed]", current_item_count);
        }

        putStringClearToEOL(msg, item_identifier + 5, 59);
        item_pos_start++;
    }

    if (item_identifier < 12) {
        for (int i = 0; i < (11 - item_identifier + 1); i++) {
            // clear remaining lines
            eraseLine(i + item_identifier + 5, 0);
        }
    }

    if (store->store_id > 12) {
        putString("- cont. -", 17, 60);
    } else {
        eraseLine(17, 60);
    }
}

// Re-displays only a single cost -RAK-
static void displaySingleCost(int store_id, int item_id) {
    int cost = stores[store_id].inventory[item_id].cost;

    vtype_t msg = {'\0'};
    if (cost < 0) {
        int32_t c = -cost;
        c = c * playerStatAdjustmentCharisma() / 100;
        (void) sprintf(msg, "%d", c);
    } else {
        (void) sprintf(msg, "%9d [Fixed]", cost);
    }
    putStringClearToEOL(msg, (item_id % 12) + 5, 59);
}

// Displays players gold -RAK-
static void displayPlayerRemainingGold() {
    vtype_t msg = {'\0'};
    (void) sprintf(msg, "Gold Remaining : %d", py.misc.au);
    putStringClearToEOL(msg, 18, 17);
}

// Displays store -RAK-
static void displayStore(int store_id, const char *owner_name, int current_top_item_id) {
    clearScreen();
    putString(owner_name, 3, 9);
    putString("Item", 4, 3);
    putString("Asking Price", 4, 60);
    displayPlayerRemainingGold();
    displayStoreCommands();
    displayStoreInventory(store_id, current_top_item_id);
}

// Get the ID of a store item and return it's value -RAK-
static bool storeGetItemID(int *item_id, const char *prompt, int item_pos_start, int item_pos_end) {
    *item_id = -1;

    vtype_t msg = {'\0'};
    (void) sprintf(msg, "(Items %c-%c, ESC to exit) %s", item_pos_start + 'a', item_pos_end + 'a', prompt);

    char command;
    bool item_found = false;

    while (getCommand(msg, &command)) {
        command -= 'a';
        if (command >= item_pos_start && command <= item_pos_end) {
            item_found = true;
            *item_id = command;
            break;
        }
        terminalBellSound();
    }
    eraseLine(MSG_LINE, 0);

    return item_found;
}

// Increase the insult counter and get angry if too many -RAK-
static bool storeIncreaseInsults(int store_id) {
    Store_t *store = &stores[store_id];

    store->insults_counter++;

    if (store->insults_counter > store_owners[store->owner].max_insults) {
        printSpeechGetOutOfMyStore();
        store->insults_counter = 0;
        store->bad_purchases++;
        store->turns_left_before_closing = current_game_turn + 2500 + randomNumber(2500);
        return true;
    }

    return false;
}

// Decrease insults -RAK-
static void storeDecreaseInsults(int store_id) {
    if (stores[store_id].insults_counter != 0) {
        stores[store_id].insults_counter--;
    }
}

// Have insulted while haggling -RAK-
static bool storeHaggleInsults(int store_id) {
    if (storeIncreaseInsults(store_id)) {
        return true;
    }

    printSpeechTryAgain();

    // keep insult separate from rest of haggle
    printMessage(CNIL);

    return false;
}

static bool storeGetHaggle(const char *comment, int32_t *new_offer, int num_offer) {
    if (num_offer == 0) {
        store_last_increment = 0;
    }

    bool increment = false;

    auto comment_len = (int) strlen(comment);
    int save_comment_len = comment_len;

    char *p = nullptr;
    vtype_t msg = {'\0'};
    vtype_t default_offer = {'\0'};

    bool flag = true;
    int32_t offer_adjust = 0;

    while (flag && offer_adjust == 0) {
        putStringClearToEOL(comment, 0, 0);

        if ((num_offer != 0) && store_last_increment != 0) {
            auto abs_store_last_increment = (int) std::abs((std::intmax_t) store_last_increment);
            (void) sprintf(default_offer, "[%c%d] ", (store_last_increment < 0) ? '-' : '+', abs_store_last_increment);
            putStringClearToEOL(default_offer, 0, save_comment_len);
            comment_len = save_comment_len + (int) strlen(default_offer);
        }

        if (!getStringInput(msg, 0, comment_len, 40)) {
            flag = false;
        }

        for (p = msg; *p == ' '; p++) {
            // fast forward to next space character
        }

        if (*p == '+' || *p == '-') {
            increment = true;
        }

        if ((num_offer != 0) && increment) {
            stringToNumber(msg, &offer_adjust);

            // Don't accept a zero here.  Turn off increment if it was zero
            // because a zero will not exit.  This can be zero if the user
            // did not type a number after the +/- sign.
            if (offer_adjust == 0) {
                increment = false;
            } else {
                store_last_increment = (int16_t) offer_adjust;
            }
        } else if ((num_offer != 0) && *msg == '\0') {
            offer_adjust = store_last_increment;
            increment = true;
        } else {
            stringToNumber(msg, &offer_adjust);
        }

        // don't allow incremental haggling, if player has not made an offer yet
        if (flag && num_offer == 0 && increment) {
            printMessage("You haven't even made your first offer yet!");
            offer_adjust = 0;
            increment = false;
        }
    }

    if (flag) {
        if (increment) {
            *new_offer += offer_adjust;
        } else {
            *new_offer = offer_adjust;
        }
    } else {
        eraseLine(0, 0);
    }

    return flag;
}

static int storeReceiveOffer(int store_id, const char *comment, int32_t *new_offer, int32_t last_offer, int num_offer, int factor) {
    int receive = 0;
    bool success = false;

    while (!success) {
        if (storeGetHaggle(comment, new_offer, num_offer)) {
            if (*new_offer * factor >= last_offer * factor) {
                success = true;
            } else if (storeHaggleInsults(store_id)) {
                receive = 2;
                success = true;
            } else {
                // new_offer rejected, reset new_offer so that incremental
                // haggling works correctly
                *new_offer = last_offer;
            }
        } else {
            receive = 1;
            success = true;
        }
    }

    return receive;
}

// Haggling routine -RAK-
static int storePurchaseHaggle(int store_id, int32_t *price, Inventory_t *item) {
    bool did_not_haggle = false;

    *price = 0;
    int purchase = 0;
    int final_flag = 0;

    Store_t *store = &stores[store_id];
    Owner_t *owner = &store_owners[store->owner];

    int32_t max_sell, min_sell;
    int32_t cost = storeItemSellPrice(store_id, &min_sell, &max_sell, item);

    max_sell = max_sell * playerStatAdjustmentCharisma() / 100;
    if (max_sell <= 0) {
        max_sell = 1;
    }

    min_sell = min_sell * playerStatAdjustmentCharisma() / 100;
    if (min_sell <= 0) {
        min_sell = 1;
    }

    // cast max_inflate to signed so that subtraction works correctly
    int32_t max_buy = cost * (200 - (int) owner->max_inflate) / 100;
    if (max_buy <= 0) {
        max_buy = 1;
    }

    int32_t min_per = owner->haggles_per;
    int32_t max_per = min_per * 3;

    displayStoreHaggleCommands(1);

    int32_t current_asking_price = max_sell;
    int32_t final_asking_price = min_sell;
    int32_t min_offer = max_buy;
    int32_t last_offer = min_offer;
    int32_t new_offer = 0;
    int num_offer = 0; // this prevents incremental haggling on first try
    const char *comment = "Asking";

    // go right to final price if player has bargained well
    if (storeNoNeedToBargain(store_id, final_asking_price)) {
        printMessage("After a long bargaining session, you agree upon the price.");
        current_asking_price = min_sell;
        comment = "Final offer";
        did_not_haggle = true;

        // Set up automatic increment, so that a return will accept the final price.
        store_last_increment = (int16_t) min_sell;
        num_offer = 1;
    }

    vtype_t msg = {'\0'};
    bool flag = false;

    while (!flag) {
        bool loop_flag;
        do {
            loop_flag = true;

            (void) sprintf(msg, "%s :  %d", comment, current_asking_price);
            putString(msg, 1, 0);

            purchase = storeReceiveOffer(store_id, "What do you offer? ", &new_offer, last_offer, num_offer, 1);

            if (purchase != 0) {
                flag = true;
            } else {
                if (new_offer > current_asking_price) {
                    printSpeechSorry();
                    // rejected, reset new_offer for incremental haggling
                    new_offer = last_offer;

                    // If the automatic increment is large enough to overflow,
                    // then the player must have made a mistake.  Clear it
                    // because it is useless.
                    if (last_offer + store_last_increment > current_asking_price) {
                        store_last_increment = 0;
                    }
                } else if (new_offer == current_asking_price) {
                    flag = true;
                    *price = new_offer;
                } else {
                    loop_flag = false;
                }
            }
        } while (!flag && loop_flag);

        if (!flag) {
            int32_t x1 = (new_offer - last_offer) * 100 / (current_asking_price - last_offer);

            if (x1 < min_per) {
                flag = storeHaggleInsults(store_id);
                if (flag) {
                    purchase = 2;
                }
            } else if (x1 > max_per) {
                x1 = x1 * 75 / 100;
                if (x1 < max_per) {
                    x1 = max_per;
                }
            }

            int32_t x2 = x1 + randomNumber(5) - 3;
            int32_t x3 = ((current_asking_price - new_offer) * x2 / 100) + 1;

            // don't let the price go up
            if (x3 < 0) {
                x3 = 0;
            }

            current_asking_price -= x3;

            if (current_asking_price < final_asking_price) {
                current_asking_price = final_asking_price;
                comment = "Final Offer";

                // Set the automatic haggle increment so that RET will give
                // a new_offer equal to the final_asking_price price.
                store_last_increment = (int16_t) (final_asking_price - new_offer);
                final_flag++;

                if (final_flag > 3) {
                    if (storeIncreaseInsults(store_id)) {
                        purchase = 2;
                    } else {
                        purchase = 1;
                    }
                    flag = true;
                }
            } else if (new_offer >= current_asking_price) {
                flag = true;
                *price = new_offer;
            }
            if (!flag) {
                last_offer = new_offer;
                num_offer++; // enable incremental haggling
                eraseLine(1, 0);
                (void) sprintf(msg, "Your last offer : %d", last_offer);
                putString(msg, 1, 39);
                printSpeechSellingHaggle(last_offer, current_asking_price, final_flag);

                // If the current increment would take you over the store's
                // price, then decrease it to an exact match.
                if (current_asking_price - last_offer < store_last_increment) {
                    store_last_increment = (int16_t) (current_asking_price - last_offer);
                }
            }
        }
    }

    // update bargaining info
    if (purchase == 0 && !did_not_haggle) {
        storeUpdateBargainInfo(store_id, *price, final_asking_price);
    }

    return purchase;
}

// Haggling routine -RAK-
static int storeSellHaggle(int store_id, int32_t *price, Inventory_t *item) {
    int32_t max_gold = 0;
    int32_t min_per = 0;
    int32_t max_per = 0;
    int32_t max_sell = 0;
    int32_t min_buy = 0;
    int32_t max_buy = 0;

    bool flag = false;
    bool did_not_haggle = false;

    *price = 0;
    int sell = 0;
    int final_flag = 0;

    Store_t *store = &stores[store_id];
    int32_t cost = storeItemValue(item);

    if (cost < 1) {
        sell = 3;
        flag = true;
    } else {
        Owner_t *owner = &store_owners[store->owner];

        cost = cost * (200 - playerStatAdjustmentCharisma()) / 100;
        cost = cost * (200 - race_gold_adjustments[owner->race][py.misc.race_id]) / 100;

        if (cost < 1) {
            cost = 1;
        }

        max_sell = cost * owner->max_inflate / 100;

        // cast max_inflate to signed so that subtraction works correctly
        max_buy = cost * (200 - (int) owner->max_inflate) / 100;
        min_buy = cost * (200 - owner->min_inflate) / 100;

        if (min_buy < 1) {
            min_buy = 1;
        }

        if (max_buy < 1) {
            max_buy = 1;
        }

        if (min_buy < max_buy) {
            min_buy = max_buy;
        }

        min_per = owner->haggles_per;
        max_per = min_per * 3;
        max_gold = owner->max_cost;
    }

    int32_t current_askin_price;
    int32_t final_asking_price = 0;
    const char *comment = nullptr;

    if (!flag) {
        displayStoreHaggleCommands(-1);

        int num_offer = 0; // this prevents incremental haggling on first try

        if (max_buy > max_gold) {
            final_flag = 1;
            comment = "Final Offer";

            // Disable the automatic haggle increment on RET.
            store_last_increment = 0;
            current_askin_price = max_gold;
            final_asking_price = max_gold;
            printMessage("I am sorry, but I have not the money to afford such a fine item.");
            did_not_haggle = true;
        } else {
            current_askin_price = max_buy;
            final_asking_price = min_buy;

            if (final_asking_price > max_gold) {
                final_asking_price = max_gold;
            }

            comment = "Offer";

            // go right to final price if player has bargained well
            if (storeNoNeedToBargain(store_id, final_asking_price)) {
                printMessage("After a long bargaining session, you agree upon the price.");
                current_askin_price = final_asking_price;
                comment = "Final offer";
                did_not_haggle = true;

                // Set up automatic increment, so that a return
                // will accept the final price.
                store_last_increment = (int16_t) final_asking_price;
                num_offer = 1;
            }
        }

        int32_t min_offer = max_sell;
        int32_t last_offer = min_offer;
        int32_t new_offer = 0;

        if (current_askin_price < 1) {
            current_askin_price = 1;
        }

        do {
            bool loop_flag;
            do {
                loop_flag = true;

                vtype_t msg = {'\0'};
                (void) sprintf(msg, "%s :  %d", comment, current_askin_price);
                putString(msg, 1, 0);

                sell = storeReceiveOffer(store_id, "What price do you ask? ", &new_offer, last_offer, num_offer, -1);

                if (sell != 0) {
                    flag = true;
                } else {
                    if (new_offer < current_askin_price) {
                        printSpeechSorry();

                        // rejected, reset new_offer for incremental haggling
                        new_offer = last_offer;

                        // If the automatic increment is large enough to
                        // overflow, then the player must have made a mistake.
                        // Clear it because it is useless.
                        if (last_offer + store_last_increment < current_askin_price) {
                            store_last_increment = 0;
                        }
                    } else if (new_offer == current_askin_price) {
                        flag = true;
                        *price = new_offer;
                    } else {
                        loop_flag = false;
                    }
                }
            } while (!flag && loop_flag);

            if (!flag) {
                int32_t x1 = (last_offer - new_offer) * 100 / (last_offer - current_askin_price);

                if (x1 < min_per) {
                    flag = storeHaggleInsults(store_id);
                    if (flag) {
                        sell = 2;
                    }
                } else if (x1 > max_per) {
                    x1 = x1 * 75 / 100;
                    if (x1 < max_per) {
                        x1 = max_per;
                    }
                }

                int32_t x2 = x1 + randomNumber(5) - 3;
                int32_t x3 = ((new_offer - current_askin_price) * x2 / 100) + 1;

                // don't let the price go down
                if (x3 < 0) {
                    x3 = 0;
                }

                current_askin_price += x3;

                if (current_askin_price > final_asking_price) {
                    current_askin_price = final_asking_price;
                    comment = "Final Offer";

                    // Set the automatic haggle increment so that RET will give
                    // a new_offer equal to the final_asking_price price.
                    store_last_increment = (int16_t) (final_asking_price - new_offer);
                    final_flag++;

                    if (final_flag > 3) {
                        if (storeIncreaseInsults(store_id)) {
                            sell = 2;
                        } else {
                            sell = 1;
                        }
                        flag = true;
                    }
                } else if (new_offer <= current_askin_price) {
                    flag = true;
                    *price = new_offer;
                }

                if (!flag) {
                    last_offer = new_offer;
                    num_offer++; // enable incremental haggling
                    eraseLine(1, 0);

                    vtype_t msg = {'\0'};
                    (void) sprintf(msg, "Your last bid %d", last_offer);
                    putString(msg, 1, 39);

                    printSpeechBuyingHaggle(current_askin_price, last_offer, final_flag);

                    // If the current decrement would take you under the store's
                    // price, then increase it to an exact match.
                    if (current_askin_price - last_offer > store_last_increment) {
                        store_last_increment = (int16_t) (current_askin_price - last_offer);
                    }
                }
            }
        } while (!flag);
    }

    // update bargaining info
    if (sell == 0 && !did_not_haggle) {
        storeUpdateBargainInfo(store_id, *price, final_asking_price);
    }

    return sell;
}

// Get the number of store items to display on the screen
static int storeItemsToDisplay(int store_counter, int current_top_item_id) {
    if (current_top_item_id == 12) {
        return store_counter - 1 - 12;
    }

    if (store_counter > 11) {
        return 11;
    }

    return store_counter - 1;
}

// Buy an item from a store -RAK-
static bool storePurchaseAnItem(int store_id, int *current_top_item_id) {
    Store_t *store = &stores[store_id];

    if (store->store_id < 1) {
        printMessage("I am currently out of stock.");
        return false;
    }

    int item_id;
    int item_count = storeItemsToDisplay(store->store_id, *current_top_item_id);
    if (!storeGetItemID(&item_id, "Which item are you interested in? ", 0, item_count)) {
        return false;
    }

    // Get the item number to be bought

    item_id = item_id + *current_top_item_id; // true item_id

    Inventory_t sell_item{};
    inventoryTakeOneItem(&sell_item, &store->inventory[item_id].item);

    if (!inventoryCanCarryItemCount(&sell_item)) {
        putStringClearToEOL("You cannot carry that many different items.", 0, 0);
        return false;
    }

    int choice = 0;
    int32_t price;
    bool purchased = false;

    if (store->inventory[item_id].cost > 0) {
        price = store->inventory[item_id].cost;
    } else {
        choice = storePurchaseHaggle(store_id, &price, &sell_item);
    }

    if (choice == 0) {
        if (py.misc.au >= price) {
            printSpeechFinishedHaggling();
            storeDecreaseInsults(store_id);
            py.misc.au -= price;

            int new_item_id = inventoryCarryItem(&sell_item);
            int saved_store_counter = store->store_id;

            storeDestroy(store_id, item_id, true);

            obj_desc_t description = {'\0'};
            itemDescription(description, &inventory[new_item_id], true);

            obj_desc_t msg = {'\0'};
            (void) sprintf(msg, "You have %s (%c)", description, new_item_id + 'a');
            putStringClearToEOL(msg, 0, 0);

            playerStrength();

            if (*current_top_item_id >= store->store_id) {
                *current_top_item_id = 0;
                displayStoreInventory(store_id, *current_top_item_id);
            } else {
                InventoryRecord_t *store_item = &store->inventory[item_id];

                if (saved_store_counter == store->store_id) {
                    if (store_item->cost < 0) {
                        store_item->cost = price;
                        displaySingleCost(store_id, item_id);
                    }
                } else {
                    displayStoreInventory(store_id, item_id);
                }
            }
            displayPlayerRemainingGold();
        } else {
            if (storeIncreaseInsults(store_id)) {
                purchased = true;
            } else {
                printSpeechFinishedHaggling();
                printMessage("Liar!  You have not the gold!");
            }
        }
    } else if (choice == 2) {
        purchased = true;
    }

    // Less intuitive, but looks better here than in storePurchaseHaggle.
    displayStoreCommands();
    eraseLine(1, 0);

    return purchased;
}

// Each store will buy only certain items, based on TVAL
bool (*store_buy[MAX_STORES])(int) = {
    setGeneralStoreItems,
    setArmoryItems,
    setWeaponsmithItems,
    setTempleItems,
    setAlchemistItems,
    setMagicShopItems,
};

// Sell an item to the store -RAK-
static bool storeSellAnItem(int store_id, int *current_top_item_id) {
    int first_item = inventory_count;
    int last_item = -1;

    char mask[EQUIPMENT_WIELD];

    for (int counter = 0; counter < inventory_count; counter++) {
        int flag = (*store_buy[store_id])(inventory[counter].category_id);

        mask[counter] = (char) flag;
        if (flag) {
            if (counter < first_item) {
                first_item = counter;
            }
            if (counter > last_item) {
                last_item = counter;
            }
        }
    }

    if (last_item == -1) {
        printMessage("You have nothing to sell to this store!");
        return false;
    }

    int item_id;
    if (!inventoryGetInputForItemId(item_id, "Which one? ", first_item, last_item, mask, "I do not buy such items.")) {
        return false;
    }

    Inventory_t sold_item{};
    inventoryTakeOneItem(&sold_item, &inventory[item_id]);

    obj_desc_t description = {'\0'};
    itemDescription(description, &sold_item, true);

    obj_desc_t msg = {'\0'};
    (void) sprintf(msg, "Selling %s (%c)", description, item_id + 'a');
    printMessage(msg);

    if (!storeCheckPlayerItemsCount(store_id, &sold_item)) {
        printMessage("I have not the room in my store to keep it.");
        return false;
    }

    int32_t price;
    bool sold = false;

    int choice = storeSellHaggle(store_id, &price, &sold_item);

    if (choice == 0) {
        printSpeechFinishedHaggling();
        storeDecreaseInsults(store_id);
        py.misc.au += price;

        // identify object in inventory to set objects_identified array
        itemIdentify(&item_id);

        // retake sold_item so that it will be identified
        inventoryTakeOneItem(&sold_item, &inventory[item_id]);

        // call spellItemIdentifyAndRemoveRandomInscription for store item, so charges/pluses are known
        spellItemIdentifyAndRemoveRandomInscription(&sold_item);
        inventoryDestroyItem(item_id);

        itemDescription(description, &sold_item, true);
        (void) sprintf(msg, "You've sold %s", description);
        printMessage(msg);

        int item_pos;
        storeCarry(store_id, &item_pos, &sold_item);

        playerStrength();

        if (item_pos >= 0) {
            if (item_pos < 12) {
                if (*current_top_item_id < 12) {
                    displayStoreInventory(store_id, item_pos);
                } else {
                    *current_top_item_id = 0;
                    displayStoreInventory(store_id, *current_top_item_id);
                }
            } else if (*current_top_item_id > 11) {
                displayStoreInventory(store_id, item_pos);
            } else {
                *current_top_item_id = 12;
                displayStoreInventory(store_id, *current_top_item_id);
            }
        }
        displayPlayerRemainingGold();
    } else if (choice == 2) {
        sold = true;
    } else if (choice == 3) {
        printMessage("How dare you!");
        printMessage("I will not buy that!");
        sold = storeIncreaseInsults(store_id);
    }

    // Less intuitive, but looks better here than in storeSellHaggle.
    eraseLine(1, 0);
    displayStoreCommands();

    return sold;
}

// Entering a store -RAK-
void storeEnter(int store_id) {
    Store_t *store = &stores[store_id];

    if (store->turns_left_before_closing >= current_game_turn) {
        printMessage("The doors are locked.");
        return;
    }

    int current_top_item_id = 0;
    displayStore(store_id, store_owners[store->owner].name, current_top_item_id);

    bool exit_store = false;
    while (!exit_store) {
        moveCursor(20, 9);

        // clear the msg flag just like we do in dungeon.c
        message_ready_to_print = false;

        char command;
        if (getCommand(CNIL, &command)) {
            int saved_chr;

            switch (command) {
                case 'b':
                    if (current_top_item_id == 0) {
                        if (store->store_id > 12) {
                            current_top_item_id = 12;
                            displayStoreInventory(store_id, current_top_item_id);
                        } else {
                            printMessage("Entire inventory is shown.");
                        }
                    } else {
                        current_top_item_id = 0;
                        displayStoreInventory(store_id, current_top_item_id);
                    }
                    break;
                case 'E':
                case 'e': // Equipment List
                case 'I':
                case 'i': // Inventory
                case 'T':
                case 't': // Take off
                case 'W':
                case 'w': // Wear
                case 'X':
                case 'x': // Switch weapon
                    saved_chr = py.stats.used[A_CHR];

                    do {
                        inventoryExecuteCommand(command);
                        command = doing_inventory_command;
                    } while (command != 0);

                    // redisplay store prices if charisma changes
                    if (saved_chr != py.stats.used[A_CHR]) {
                        displayStoreInventory(store_id, current_top_item_id);
                    }

                    player_free_turn = false; // No free moves here. -CJS-
                    break;
                case 'p':
                    exit_store = storePurchaseAnItem(store_id, &current_top_item_id);
                    break;
                case 's':
                    exit_store = storeSellAnItem(store_id, &current_top_item_id);
                    break;
                default:
                    terminalBellSound();
                    break;
            }
        } else {
            exit_store = true;
        }
    }

    // Can't save and restore the screen because inventoryExecuteCommand() does that.
    drawCavePanel();
}
