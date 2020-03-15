// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Player's memory: monster descriptions

const char *recall_description_attack_type[25] = {
    "do something undefined",
    "attack",
    "weaken",
    "confuse",
    "terrify",
    "shoot flames",
    "shoot acid",
    "freeze",
    "shoot lightning",
    "corrode",
    "blind",
    "paralyse",
    "steal money",
    "steal things",
    "poison",
    "reduce dexterity",
    "reduce constitution",
    "drain intelligence",
    "drain wisdom",
    "lower experience",
    "call for help",
    "disenchant",
    "eat your food",
    "absorb light",
    "absorb charges",
};

const char *recall_description_attack_method[20] = {
    "make an undefined advance",
    "hit",
    "bite",
    "claw",
    "sting",
    "touch",
    "kick",
    "gaze",
    "breathe",
    "spit",
    "wail",
    "embrace",
    "crawl on you",
    "release spores",
    "beg",
    "slime you",
    "crush",
    "trample",
    "drool",
    "insult",
};

const char *recall_description_how_much[8] = {
    " not at all", " a bit", "", " quite", " very", " most", " highly", " extremely",
};

const char *recall_description_move[6] = {
    "move invisibly", "open doors", "pass through walls", "kill weaker creatures", "pick up objects", "breed explosively",
};

const char *recall_description_spell[15] = {
    "teleport short distances",
    "teleport long distances",
    "teleport its prey",
    "cause light wounds",
    "cause serious wounds",
    "paralyse its prey",
    "induce blindness",
    "confuse",
    "terrify",
    "summon a monster",
    "summon the undead",
    "slow its prey",
    "drain mana",
    "unknown 1",
    "unknown 2",
};

const char *recall_description_breath[5] = {
    "lightning", "poison gases", "acid", "frost", "fire",
};

const char *recall_description_weakness[6] = {
    "frost", "fire", "poison", "acid", "bright light", "rock remover",
};
