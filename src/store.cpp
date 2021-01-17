// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Store: entering, command interpreter, buying, selling

#include "headers.h"

// Save the store's last increment value.
static int16_t store_last_increment;

static bool storeNoNeedToBargain(Store_t const &store, int32_t min_price);
static void storeUpdateBargainingSkills(Store_t &store, int32_t price, int32_t min_price);

// Initializes the stores with owners -RAK-
void storeInitializeOwners() {
    int count = MAX_OWNERS / MAX_STORES;

    for (int store_id = 0; store_id < MAX_STORES; store_id++) {
        Store_t &store = stores[store_id];

        store.owner_id = (uint8_t)(MAX_STORES * (randomNumber(count) - 1) + store_id);
        store.insults_counter = 0;
        store.turns_left_before_closing = 0;
        store.unique_items_counter = 0;
        store.good_purchases = 0;
        store.bad_purchases = 0;

        for (auto &item : store.inventory) {
            inventoryItemCopyTo(config::dungeon::objects::OBJ_NOTHING, item.item);
            item.cost = 0;
        }
    }
}

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
    putStringClearToEOL("You may:", Coord_t{20, 0});
    putStringClearToEOL(" p) Purchase an item.           b) Browse store's inventory.", Coord_t{21, 0});
    putStringClearToEOL(" s) Sell an item.               i/e/t/w/x) Inventory/Equipment Lists.", Coord_t{22, 0});
    putStringClearToEOL("ESC) Exit from Building.        ^R) Redraw the screen.", Coord_t{23, 0});
}

// Displays the set of commands -RAK-
static void displayStoreHaggleCommands(int haggle_type) {
    if (haggle_type == -1) {
        putStringClearToEOL("Specify an asking-price in gold pieces.", Coord_t{21, 0});
    } else {
        putStringClearToEOL("Specify an offer in gold pieces.", Coord_t{21, 0});
    }

    putStringClearToEOL("ESC) Quit Haggling.", Coord_t{22, 0});
    eraseLine(Coord_t{23, 0}); // clear last line
}

// Displays a store's inventory -RAK-
static void displayStoreInventory(Store_t &store, int item_pos_start) {
    int item_pos_end = ((item_pos_start / 12) + 1) * 12;
    if (item_pos_end > store.unique_items_counter) {
        item_pos_end = store.unique_items_counter;
    }

    int item_line_num;

    for (item_line_num = (item_pos_start % 12); item_pos_start < item_pos_end; item_line_num++) {
        Inventory_t &item = store.inventory[item_pos_start].item;

        // Save the current number of items
        int32_t current_item_count = item.items_count;

        if (inventoryItemSingleStackable(item)) {
            item.items_count = 1;
        }

        obj_desc_t description = {'\0'};
        itemDescription(description, item, true);

        // Restore the number of items
        item.items_count = (uint8_t) current_item_count;

        obj_desc_t msg = {'\0'};
        (void) sprintf(msg, "%c) %s", 'a' + item_line_num, description);
        putStringClearToEOL(msg, Coord_t{item_line_num + 5, 0});

        current_item_count = store.inventory[item_pos_start].cost;

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

        putStringClearToEOL(msg, Coord_t{item_line_num + 5, 59});
        item_pos_start++;
    }

    if (item_line_num < 12) {
        for (int i = 0; i < (11 - item_line_num + 1); i++) {
            // clear remaining lines
            eraseLine(Coord_t{i + item_line_num + 5, 0});
        }
    }

    if (store.unique_items_counter > 12) {
        putString("- cont. -", Coord_t{17, 60});
    } else {
        eraseLine(Coord_t{17, 60});
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
    putStringClearToEOL(msg, Coord_t{(item_id % 12) + 5, 59});
}

// Displays players gold -RAK-
static void displayPlayerRemainingGold() {
    vtype_t msg = {'\0'};
    (void) sprintf(msg, "Gold Remaining : %d", py.misc.au);
    putStringClearToEOL(msg, Coord_t{18, 17});
}

// Displays store -RAK-
static void displayStore(Store_t &store, const char *owner_name, int current_top_item_id) {
    clearScreen();
    putString(owner_name, Coord_t{3, 9});
    putString("Item", Coord_t{4, 3});
    putString("Asking Price", Coord_t{4, 60});
    displayPlayerRemainingGold();
    displayStoreCommands();
    displayStoreInventory(store, current_top_item_id);
}

// Get the ID of a store item and return it's value -RAK-
// Returns true if the item was found.
static bool storeGetItemId(int &item_id, const char *prompt, int item_pos_start, int item_pos_end) {
    item_id = -1;
    bool item_found = false;

    vtype_t msg = {'\0'};
    (void) sprintf(msg, "(Items %c-%c, ESC to exit) %s", item_pos_start + 'a', item_pos_end + 'a', prompt);

    char key_char;
    while (getCommand(msg, key_char)) {
        key_char -= 'a';
        if (key_char >= item_pos_start && key_char <= item_pos_end) {
            item_found = true;
            item_id = key_char;
            break;
        }
        terminalBellSound();
    }
    messageLineClear();

    return item_found;
}

// Increase the insult counter and get angry if too many -RAK-
static bool storeIncreaseInsults(int store_id) {
    Store_t &store = stores[store_id];

    store.insults_counter++;

    if (store.insults_counter <= store_owners[store.owner_id].max_insults) {
        return false;
    }

    // customer angered the store owner with too many insults!
    printSpeechGetOutOfMyStore();
    store.insults_counter = 0;
    store.bad_purchases++;
    store.turns_left_before_closing = dg.game_turn + 2500 + randomNumber(2500);

    return true;
}

// Decrease insults -RAK-
static void storeDecreaseInsults(int store_id) {
    if (stores[store_id].insults_counter != 0) {
        stores[store_id].insults_counter--;
    }
}

// Have insulted while haggling -RAK-
// Returns true if the store owner was angered.
static bool storeHaggleInsults(int store_id) {
    if (storeIncreaseInsults(store_id)) {
        return true;
    }

    printSpeechTryAgain();

    // keep insult separate from rest of haggle
    printMessage(CNIL);

    return false;
}

// Returns true if the customer made a valid offer
static bool storeGetHaggle(const char *prompt, int32_t &new_offer, int offer_count) {
    bool valid_offer = true;

    if (offer_count == 0) {
        store_last_increment = 0;
    }

    bool increment = false;
    int32_t adjustment = 0;

    auto prompt_len = (int) strlen(prompt);
    int start_len = prompt_len;

    char *p = nullptr;
    vtype_t msg = {'\0'};
    vtype_t last_offer_str = {'\0'};

    // Get a customers new offer
    while (valid_offer && adjustment == 0) {
        putStringClearToEOL(prompt, Coord_t{0, 0});

        if ((offer_count != 0) && store_last_increment != 0) {
            auto abs_store_last_increment = (int) std::abs((std::intmax_t) store_last_increment);

            (void) sprintf(last_offer_str, "[%c%d] ", (store_last_increment < 0) ? '-' : '+', abs_store_last_increment);
            putStringClearToEOL(last_offer_str, Coord_t{0, start_len});

            prompt_len = start_len + (int) strlen(last_offer_str);
        }

        if (!getStringInput(msg, Coord_t{0, prompt_len}, 40)) {
            // customer aborted, i.e. pressed escape
            valid_offer = false;
        }

        for (p = msg; *p == ' '; p++) {
            // fast forward to next space character
        }
        if (*p == '+' || *p == '-') {
            increment = true;
        }

        if ((offer_count != 0) && increment) {
            stringToNumber(msg, adjustment);

            // Don't accept a zero here.  Turn off increment if it was zero
            // because a zero will not exit.  This can be zero if the user
            // did not type a number after the +/- sign.
            if (adjustment == 0) {
                increment = false;
            } else {
                store_last_increment = (int16_t) adjustment;
            }
        } else if ((offer_count != 0) && *msg == '\0') {
            adjustment = store_last_increment;
            increment = true;
        } else {
            stringToNumber(msg, adjustment);
        }

        // don't allow incremental haggling, if player has not made an offer yet
        if (valid_offer && offer_count == 0 && increment) {
            printMessage("You haven't even made your first offer yet!");
            adjustment = 0;
            increment = false;
        }
    }

    if (valid_offer) {
        if (increment) {
            new_offer += adjustment;
        } else {
            new_offer = adjustment;
        }
    } else {
        messageLineClear();
    }

    return valid_offer;
}

// The status of the customer bid.
// Note: a received bid may still result in a rejected offer.
enum class BidState {
    Received = 0, // the big was received successfully
    Rejected,     // the bid was rejected, or cancelled by the customer
    Offended,     // customer tried to sell an undesirable item
    Insulted,     // the store owner was insulted too many times by the bid
};

static BidState storeReceiveOffer(int store_id, const char *prompt, int32_t &new_offer, int32_t last_offer, int offer_count, int factor) {
    BidState status = BidState::Received;

    bool done = false;
    while (!done) {
        if (storeGetHaggle(prompt, new_offer, offer_count)) {
            // customer submitted valid offer
            if (new_offer * factor >= last_offer * factor) {
                done = true;
            } else if (storeHaggleInsults(store_id)) {
                // customer angered the store owner!
                status = BidState::Insulted;
                done = true;
            } else {
                // new_offer rejected, reset new_offer so that incremental
                // haggling works correctly
                new_offer = last_offer;
            }
        } else {
            // customer aborted offer
            status = BidState::Rejected;
            done = true;
        }
    }

    return status;
}

static void storePurchaseCustomerAdjustment(int32_t &min_sell, int32_t &max_sell) {
    int charisma = playerStatAdjustmentCharisma();

    max_sell = max_sell * charisma / 100;
    if (max_sell <= 0) {
        max_sell = 1;
    }

    min_sell = min_sell * charisma / 100;
    if (min_sell <= 0) {
        min_sell = 1;
    }
}

// Haggling routine -RAK-
static BidState storePurchaseHaggle(int store_id, int32_t &price, Inventory_t const &item) {
    BidState status = BidState::Received;

    int32_t new_price = 0;

    Store_t const &store = stores[store_id];
    Owner_t const &owner = store_owners[store.owner_id];

    int32_t max_sell, min_sell;
    int32_t cost = storeItemSellPrice(store, min_sell, max_sell, item);

    storePurchaseCustomerAdjustment(min_sell, max_sell);

    // cast max_inflate to signed so that subtraction works correctly
    int32_t max_buy = cost * (200 - (int) owner.max_inflate) / 100;
    if (max_buy <= 0) {
        max_buy = 1;
    }

    displayStoreHaggleCommands(1);

    int32_t final_asking_price = min_sell;
    int32_t current_asking_price = max_sell;

    const char *comment = "Asking";
    bool accepted_without_haggle = false;
    int offers_count = 0; // this prevents incremental haggling on first try

    // go right to final price if player has bargained well
    if (storeNoNeedToBargain(stores[store_id], final_asking_price)) {
        printMessage("After a long bargaining session, you agree upon the price.");
        current_asking_price = min_sell;
        comment = "Final offer";
        accepted_without_haggle = true;

        // Set up automatic increment, so that a return will accept the final price.
        store_last_increment = (int16_t) min_sell;
        offers_count = 1;
    }

    int32_t min_offer = max_buy;
    int32_t last_offer = min_offer;
    int32_t new_offer = 0;

    int32_t min_per = owner.haggles_per;
    int32_t max_per = min_per * 3;

    int final_flag = 0;

    bool rejected = false;
    bool bidding_open;

    while (!rejected) {
        do {
            bidding_open = true;

            vtype_t msg = {'\0'};
            (void) sprintf(msg, "%s :  %d", comment, current_asking_price);
            putString(msg, Coord_t{1, 0});

            status = storeReceiveOffer(store_id, "What do you offer? ", new_offer, last_offer, offers_count, 1);

            if (status != BidState::Received) {
                rejected = true;
            } else {
                // review the received bid

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
                    rejected = true;
                    new_price = new_offer;
                } else {
                    bidding_open = false;
                }
            }
        } while (!rejected && bidding_open);

        if (!rejected) {
            int32_t adjustment = (new_offer - last_offer) * 100 / (current_asking_price - last_offer);

            if (adjustment < min_per) {
                rejected = storeHaggleInsults(store_id);
                if (rejected) {
                    status = BidState::Insulted;
                }
            } else if (adjustment > max_per) {
                adjustment = adjustment * 75 / 100;
                if (adjustment < max_per) {
                    adjustment = max_per;
                }
            }

            adjustment = ((current_asking_price - new_offer) * (adjustment + randomNumber(5) - 3) / 100) + 1;

            // don't let the price go up
            if (adjustment > 0) {
                current_asking_price -= adjustment;
            }

            if (current_asking_price < final_asking_price) {
                current_asking_price = final_asking_price;
                comment = "Final Offer";

                // Set the automatic haggle increment so that RET will give
                // a new_offer equal to the final_asking_price price.
                store_last_increment = (int16_t)(final_asking_price - new_offer);
                final_flag++;

                if (final_flag > 3) {
                    if (storeIncreaseInsults(store_id)) {
                        status = BidState::Insulted;
                    } else {
                        status = BidState::Rejected;
                    }
                    rejected = true;
                }
            } else if (new_offer >= current_asking_price) {
                rejected = true;
                new_price = new_offer;
            }

            if (!rejected) {
                last_offer = new_offer;
                offers_count++; // enable incremental haggling

                eraseLine(Coord_t{1, 0});
                vtype_t msg = {'\0'};
                (void) sprintf(msg, "Your last offer : %d", last_offer);
                putString(msg, Coord_t{1, 39});

                printSpeechSellingHaggle(last_offer, current_asking_price, final_flag);

                // If the current increment would take you over the store's
                // price, then decrease it to an exact match.
                if (current_asking_price - last_offer < store_last_increment) {
                    store_last_increment = (int16_t)(current_asking_price - last_offer);
                }
            }
        }
    }

    // update bargaining info
    if (status == BidState::Received && !accepted_without_haggle) {
        storeUpdateBargainingSkills(stores[store_id], new_price, final_asking_price);
    }

    price = new_price; // update callers price before returning

    return status;
}

static void storeSellCustomerAdjustment(Owner_t const &owner, int32_t &cost, int32_t &min_buy, int32_t &max_buy, int32_t &max_sell) {
    cost = cost * (200 - playerStatAdjustmentCharisma()) / 100;
    cost = cost * (200 - race_gold_adjustments[owner.race][py.misc.race_id]) / 100;
    if (cost < 1) {
        cost = 1;
    }

    max_sell = cost * owner.max_inflate / 100;

    // cast max_inflate to signed so that subtraction works correctly
    max_buy = cost * (200 - (int) owner.max_inflate) / 100;
    min_buy = cost * (200 - owner.min_inflate) / 100;
    if (min_buy < 1) {
        min_buy = 1;
    }
    if (max_buy < 1) {
        max_buy = 1;
    }
    if (min_buy < max_buy) {
        min_buy = max_buy;
    }
}

// Haggling routine -RAK-
static BidState storeSellHaggle(int store_id, int32_t &price, Inventory_t const &item) {
    BidState status = BidState::Received;

    int32_t new_price = 0;

    Store_t const &store = stores[store_id];
    int32_t cost = storeItemValue(item);

    bool rejected = false;

    int32_t max_gold = 0;
    int32_t min_per = 0;
    int32_t max_per = 0;
    int32_t max_sell = 0;
    int32_t min_buy = 0;
    int32_t max_buy = 0;

    if (cost < 1) {
        status = BidState::Offended;
        rejected = true;
    } else {
        Owner_t const &owner = store_owners[store.owner_id];

        storeSellCustomerAdjustment(owner, cost, min_buy, max_buy, max_sell);

        min_per = owner.haggles_per;
        max_per = min_per * 3;
        max_gold = owner.max_cost;
    }

    int32_t final_asking_price = 0;
    int32_t current_asking_price = 0;

    int final_flag = 0;

    const char *comment = nullptr;
    bool accepted_without_haggle = false;

    if (!rejected) {
        displayStoreHaggleCommands(-1);

        int offer_count = 0; // this prevents incremental haggling on first try

        if (max_buy > max_gold) {
            final_flag = 1;
            comment = "Final Offer";

            // Disable the automatic haggle increment on RET.
            store_last_increment = 0;
            current_asking_price = max_gold;
            final_asking_price = max_gold;
            printMessage("I am sorry, but I have not the money to afford such a fine item.");
            accepted_without_haggle = true;
        } else {
            current_asking_price = max_buy;
            final_asking_price = min_buy;

            if (final_asking_price > max_gold) {
                final_asking_price = max_gold;
            }

            comment = "Offer";

            // go right to final price if player has bargained well
            if (storeNoNeedToBargain(stores[store_id], final_asking_price)) {
                printMessage("After a long bargaining session, you agree upon the price.");
                current_asking_price = final_asking_price;
                comment = "Final offer";
                accepted_without_haggle = true;

                // Set up automatic increment, so that a return
                // will accept the final price.
                store_last_increment = (int16_t) final_asking_price;
                offer_count = 1;
            }
        }

        int32_t min_offer = max_sell;
        int32_t last_offer = min_offer;
        int32_t new_offer = 0;

        if (current_asking_price < 1) {
            current_asking_price = 1;
        }

        bool bidding_open;

        do {
            do {
                bidding_open = true;

                vtype_t msg = {'\0'};
                (void) sprintf(msg, "%s :  %d", comment, current_asking_price);
                putString(msg, Coord_t{1, 0});

                status = storeReceiveOffer(store_id, "What price do you ask? ", new_offer, last_offer, offer_count, -1);

                if (status != BidState::Received) {
                    rejected = true;
                } else {
                    // review the received bid

                    if (new_offer < current_asking_price) {
                        printSpeechSorry();

                        // rejected, reset new_offer for incremental haggling
                        new_offer = last_offer;

                        // If the automatic increment is large enough to
                        // overflow, then the player must have made a mistake.
                        // Clear it because it is useless.
                        if (last_offer + store_last_increment < current_asking_price) {
                            store_last_increment = 0;
                        }
                    } else if (new_offer == current_asking_price) {
                        rejected = true;
                        new_price = new_offer;
                    } else {
                        bidding_open = false;
                    }
                }
            } while (!rejected && bidding_open);

            if (!rejected) {
                int32_t adjustment = (last_offer - new_offer) * 100 / (last_offer - current_asking_price);

                if (adjustment < min_per) {
                    rejected = storeHaggleInsults(store_id);
                    if (rejected) {
                        status = BidState::Insulted;
                    }
                } else if (adjustment > max_per) {
                    adjustment = adjustment * 75 / 100;
                    if (adjustment < max_per) {
                        adjustment = max_per;
                    }
                }

                adjustment = ((new_offer - current_asking_price) * (adjustment + randomNumber(5) - 3) / 100) + 1;

                // don't let the price go down
                if (adjustment > 0) {
                    current_asking_price += adjustment;
                }

                if (current_asking_price > final_asking_price) {
                    current_asking_price = final_asking_price;
                    comment = "Final Offer";

                    // Set the automatic haggle increment so that RET will give
                    // a new_offer equal to the final_asking_price price.
                    store_last_increment = (int16_t)(final_asking_price - new_offer);
                    final_flag++;

                    if (final_flag > 3) {
                        if (storeIncreaseInsults(store_id)) {
                            status = BidState::Insulted;
                        } else {
                            status = BidState::Rejected;
                        }
                        rejected = true;
                    }
                } else if (new_offer <= current_asking_price) {
                    rejected = true;
                    new_price = new_offer;
                }

                if (!rejected) {
                    last_offer = new_offer;
                    offer_count++; // enable incremental haggling

                    eraseLine(Coord_t{1, 0});
                    vtype_t msg = {'\0'};
                    (void) sprintf(msg, "Your last bid %d", last_offer);
                    putString(msg, Coord_t{1, 39});

                    printSpeechBuyingHaggle(current_asking_price, last_offer, final_flag);

                    // If the current decrement would take you under the store's
                    // price, then increase it to an exact match.
                    if (current_asking_price - last_offer > store_last_increment) {
                        store_last_increment = (int16_t)(current_asking_price - last_offer);
                    }
                }
            }
        } while (!rejected);
    }

    // update bargaining info
    if (status == BidState::Received && !accepted_without_haggle) {
        storeUpdateBargainingSkills(stores[store_id], new_price, final_asking_price);
    }

    price = new_price; // update callers price before returning

    return status;
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
// Returns true is the owner kicks out the customer
static bool storePurchaseAnItem(int store_id, int &current_top_item_id) {
    bool kick_customer = false; // don't kick them out of the store!

    Store_t &store = stores[store_id];

    if (store.unique_items_counter < 1) {
        printMessage("I am currently out of stock.");
        return false;
    }

    int item_id;
    int item_count = storeItemsToDisplay(store.unique_items_counter, current_top_item_id);
    if (!storeGetItemId(item_id, "Which item are you interested in? ", 0, item_count)) {
        return false;
    }

    // Get the item number to be bought

    item_id = item_id + current_top_item_id; // true item_id

    Inventory_t sell_item{};
    inventoryTakeOneItem(&sell_item, &store.inventory[item_id].item);

    if (!inventoryCanCarryItemCount(sell_item)) {
        putStringClearToEOL("You cannot carry that many different items.", Coord_t{0, 0});
        return false;
    }

    BidState status = BidState::Received;
    int32_t price;

    if (store.inventory[item_id].cost > 0) {
        price = store.inventory[item_id].cost;
    } else {
        status = storePurchaseHaggle(store_id, price, sell_item);
    }

    if (status == BidState::Insulted) {
        kick_customer = true;
    } else if (status == BidState::Received) {
        if (py.misc.au >= price) {
            printSpeechFinishedHaggling();
            storeDecreaseInsults(store_id);
            py.misc.au -= price;

            int new_item_id = inventoryCarryItem(sell_item);
            int saved_store_counter = store.unique_items_counter;

            storeDestroyItem(store_id, item_id, true);

            obj_desc_t description = {'\0'};
            itemDescription(description, py.inventory[new_item_id], true);

            obj_desc_t msg = {'\0'};
            (void) sprintf(msg, "You have %s (%c)", description, new_item_id + 'a');
            putStringClearToEOL(msg, Coord_t{0, 0});

            playerStrength();

            if (current_top_item_id >= store.unique_items_counter) {
                current_top_item_id = 0;
                displayStoreInventory(stores[store_id], current_top_item_id);
            } else {
                InventoryRecord_t &store_item = store.inventory[item_id];

                if (saved_store_counter == store.unique_items_counter) {
                    if (store_item.cost < 0) {
                        store_item.cost = price;
                        displaySingleCost(store_id, item_id);
                    }
                } else {
                    displayStoreInventory(stores[store_id], item_id);
                }
            }
            displayPlayerRemainingGold();
        } else {
            if (storeIncreaseInsults(store_id)) {
                kick_customer = true;
            } else {
                printSpeechFinishedHaggling();
                printMessage("Liar!  You have not the gold!");
            }
        }
    }

    // Less intuitive, but looks better here than in storePurchaseHaggle.
    displayStoreCommands();
    eraseLine(Coord_t{1, 0});

    return kick_customer;
}

// Functions to emulate the original Pascal sets
static bool setGeneralStoreItems(uint8_t item_id) {
    switch (item_id) {
        case TV_DIGGING:
        case TV_BOOTS:
        case TV_CLOAK:
        case TV_FOOD:
        case TV_FLASK:
        case TV_LIGHT:
        case TV_SPIKE:
            return true;
        default:
            return false;
    }
}

static bool setArmoryItems(uint8_t item_id) {
    switch (item_id) {
        case TV_BOOTS:
        case TV_GLOVES:
        case TV_HELM:
        case TV_SHIELD:
        case TV_HARD_ARMOR:
        case TV_SOFT_ARMOR:
            return true;
        default:
            return false;
    }
}

static bool setWeaponsmithItems(uint8_t item_id) {
    switch (item_id) {
        case TV_SLING_AMMO:
        case TV_BOLT:
        case TV_ARROW:
        case TV_BOW:
        case TV_HAFTED:
        case TV_POLEARM:
        case TV_SWORD:
            return true;
        default:
            return false;
    }
}

static bool setTempleItems(uint8_t item_id) {
    switch (item_id) {
        case TV_HAFTED:
        case TV_SCROLL1:
        case TV_SCROLL2:
        case TV_POTION1:
        case TV_POTION2:
        case TV_PRAYER_BOOK:
            return true;
        default:
            return false;
    }
}

static bool setAlchemistItems(uint8_t item_id) {
    switch (item_id) {
        case TV_SCROLL1:
        case TV_SCROLL2:
        case TV_POTION1:
        case TV_POTION2:
            return true;
        default:
            return false;
    }
}

static bool setMagicShopItems(uint8_t item_id) {
    switch (item_id) {
        case TV_AMULET:
        case TV_RING:
        case TV_STAFF:
        case TV_WAND:
        case TV_SCROLL1:
        case TV_SCROLL2:
        case TV_POTION1:
        case TV_POTION2:
        case TV_MAGIC_BOOK:
            return true;
        default:
            return false;
    }
}

// Each store will buy only certain items, based on TVAL
bool (*store_buy[MAX_STORES])(uint8_t) = {
    setGeneralStoreItems, setArmoryItems, setWeaponsmithItems, setTempleItems, setAlchemistItems, setMagicShopItems,
};

// Sell an item to the store -RAK-
// Returns true is the owner kicks out the customer
static bool storeSellAnItem(int store_id, int &current_top_item_id) {
    bool kick_customer = false; // don't kick them out of the store!

    int first_item = py.pack.unique_items;
    int last_item = -1;

    char mask[PlayerEquipment::Wield];

    for (int counter = 0; counter < py.pack.unique_items; counter++) {
        bool flag = (*store_buy[store_id])(py.inventory[counter].category_id);

        if (flag) {
            mask[counter] = 1;

            if (counter < first_item) {
                first_item = counter;
            }
            if (counter > last_item) {
                last_item = counter;
            }
        } else {
            mask[counter] = 0;
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
    inventoryTakeOneItem(&sold_item, &py.inventory[item_id]);

    obj_desc_t description = {'\0'};
    itemDescription(description, sold_item, true);

    obj_desc_t msg = {'\0'};
    (void) sprintf(msg, "Selling %s (%c)", description, item_id + 'a');
    printMessage(msg);

    if (!storeCheckPlayerItemsCount(stores[store_id], sold_item)) {
        printMessage("I have not the room in my store to keep it.");
        return false;
    }

    int32_t price;

    BidState status = storeSellHaggle(store_id, price, sold_item);

    if (status == BidState::Insulted) {
        kick_customer = true;
    } else if (status == BidState::Offended) {
        printMessage("How dare you!");
        printMessage("I will not buy that!");
        kick_customer = storeIncreaseInsults(store_id);
    } else if (status == BidState::Received) {
        // bid received, and accepted!

        printSpeechFinishedHaggling();
        storeDecreaseInsults(store_id);
        py.misc.au += price;

        // identify object in inventory to set objects_identified array
        itemIdentify(py.inventory[item_id], item_id);

        // retake sold_item so that it will be identified
        inventoryTakeOneItem(&sold_item, &py.inventory[item_id]);

        // call spellItemIdentifyAndRemoveRandomInscription for store item, so charges/pluses are known
        spellItemIdentifyAndRemoveRandomInscription(sold_item);
        inventoryDestroyItem(item_id);

        itemDescription(description, sold_item, true);
        (void) sprintf(msg, "You've sold %s", description);
        printMessage(msg);

        int item_pos_id;
        storeCarryItem(store_id, item_pos_id, sold_item);

        playerStrength();

        if (item_pos_id >= 0) {
            if (item_pos_id < 12) {
                if (current_top_item_id < 12) {
                    displayStoreInventory(stores[store_id], item_pos_id);
                } else {
                    current_top_item_id = 0;
                    displayStoreInventory(stores[store_id], current_top_item_id);
                }
            } else if (current_top_item_id > 11) {
                displayStoreInventory(stores[store_id], item_pos_id);
            } else {
                current_top_item_id = 12;
                displayStoreInventory(stores[store_id], current_top_item_id);
            }
        }
        displayPlayerRemainingGold();
    }

    // Less intuitive, but looks better here than in storeSellHaggle.
    eraseLine(Coord_t{1, 0});
    displayStoreCommands();

    return kick_customer;
}

// Entering a store -RAK-
void storeEnter(int store_id) {
    Store_t const &store = stores[store_id];

    if (store.turns_left_before_closing >= dg.game_turn) {
        printMessage("The doors are locked.");
        return;
    }

    int current_top_item_id = 0;
    displayStore(stores[store_id], store_owners[store.owner_id].name, current_top_item_id);

    bool exit_store = false;
    while (!exit_store) {
        moveCursor(Coord_t{20, 9});

        // clear the msg flag just like we do in dungeon.c
        message_ready_to_print = false;

        char command;
        if (getCommand("", command)) {
            int saved_chr;

            switch (command) {
                case 'b':
                    if (current_top_item_id == 0) {
                        if (store.unique_items_counter > 12) {
                            current_top_item_id = 12;
                            displayStoreInventory(stores[store_id], current_top_item_id);
                        } else {
                            printMessage("Entire inventory is shown.");
                        }
                    } else {
                        current_top_item_id = 0;
                        displayStoreInventory(stores[store_id], current_top_item_id);
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
                    saved_chr = py.stats.used[PlayerAttr::A_CHR];

                    do {
                        inventoryExecuteCommand(command);
                        command = game.doing_inventory_command;
                    } while (command != 0);

                    // redisplay store prices if charisma changes
                    if (saved_chr != py.stats.used[PlayerAttr::A_CHR]) {
                        displayStoreInventory(stores[store_id], current_top_item_id);
                    }

                    game.player_free_turn = false; // No free moves here. -CJS-
                    break;
                case 'p':
                    exit_store = storePurchaseAnItem(store_id, current_top_item_id);
                    break;
                case 's':
                    exit_store = storeSellAnItem(store_id, current_top_item_id);
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

// eliminate need to bargain if player has haggled well in the past -DJB-
static bool storeNoNeedToBargain(Store_t const &store, int32_t min_price) {
    if (store.good_purchases == SHRT_MAX) {
        return true;
    }

    int record = (store.good_purchases - 3 * store.bad_purchases - 5);

    return ((record > 0) && (record * record > min_price / 50));
}

// update the bargain info -DJB-
static void storeUpdateBargainingSkills(Store_t &store, int32_t price, int32_t min_price) {
    if (min_price < 10) {
        return;
    }

    if (price == min_price) {
        if (store.good_purchases < SHRT_MAX) {
            store.good_purchases++;
        }
    } else {
        if (store.bad_purchases < SHRT_MAX) {
            store.bad_purchases++;
        }
    }
}
