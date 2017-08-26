// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Store owner's speech text

const char *speech_sale_accepted[14] = {
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

const char *speech_selling_haggle_final[3] = {
        "%A2 is my final offer; take it or leave it.",
        "I'll give you no more than %A2.",
        "My patience grows thin.  %A2 is final.",
};

const char *speech_selling_haggle[16] = {
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
        "May the fleas of 1000 Orcs molest you.  I want %A2.",
        "My mother you can get for %A1, this costs %A2.",
        "May your chickens grow lips.  I want %A2 in gold!",
        "Sell this for such a pittance?  Give me %A2 gold.",
        "May the Balrog find you tasty!  %A2 gold pieces?",
        "Your mother was a Troll!  %A2 or I'll tell.",
};

const char *speech_buying_haggle_final[3] = {
        "I'll pay no more than %A1; take it or leave it.",
        "You'll get no more than %A1 from me.",
        "%A1 and that's final.",
};

const char *speech_buying_haggle[15] = {
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

const char *speech_insulted_haggling_done[5] = {
        "ENOUGH!  You have abused me once too often!",
        "THAT DOES IT!  You shall waste my time no more!",
        "This is getting nowhere.  I'm going home!",
        "BAH!  No more shall you insult me!",
        "Begone!  I have had enough abuse for one day.",
};

const char *speech_get_out_of_my_store[5] = {
        "Out of my place!", "out... Out... OUT!!!",
        "Come back tomorrow.", "Leave my place.  Begone!",
        "Come back when thou art richer.",
};

const char *speech_haggling_try_again[10] = {
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

const char *speech_sorry[5] = {
        "I must have heard you wrong.", "What was that?",
        "I'm sorry, say that again.", "What did you say?",
        "Sorry, what was that again?",
};
