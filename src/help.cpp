// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Identify a symbol

#include "headers.h"
#include "externs.h"

static const char *asciiCharacterDescription(char command) {
    // every printing ASCII character is listed here, in the
    // order in which they appear in the ASCII character set.
    switch (command) {
        case ' ':
            return "  - An open pit.";
        case '!':
            return "! - A potion.";
        case '"':
            return "\" - An amulet, periapt, or necklace.";
        case '#':
            return "# - A stone wall.";
        case '$':
            return "$ - Treasure.";
        case '%':
            if (!highlight_seams) {
                return "% - Not used.";
            }
            return "% - A magma or quartz vein.";
        case '&':
            return "& - Treasure chest.";
        case '\'':
            return "' - An open door.";
        case '(':
            return "( - Soft armor.";
        case ')':
            return ") - A shield.";
        case '*':
            return "* - Gems.";
        case '+':
            return "+ - A closed door.";
        case ',':
            return ", - Food or mushroom patch.";
        case '-':
            return "- - A wand";
        case '.':
            return ". - Floor.";
        case '/':
            return "/ - A pole weapon.";
//        case '0':
//            return "0 - Not used.";
        case '1':
            return "1 - Entrance to General Store.";
        case '2':
            return "2 - Entrance to Armory.";
        case '3':
            return "3 - Entrance to Weaponsmith.";
        case '4':
            return "4 - Entrance to Temple.";
        case '5':
            return "5 - Entrance to Alchemy shop.";
        case '6':
            return "6 - Entrance to Magic-Users store.";
//        case '7':
//            return "7 - Not used.";
//        case '8':
//            return "8 - Not used.";
//        case '9':
//            return "9 - Not used.";
        case ':':
            return ": - Rubble.";
        case ';':
            return "; - A loose rock.";
        case '<':
            return "< - An up staircase.";
        case '=':
            return "= - A ring.";
        case '>':
            return "> - A down staircase.";
        case '?':
            return "? - A scroll.";
        case '@':
            return py.misc.name;
        case 'A':
            return "A - Giant Ant Lion.";
        case 'B':
            return "B - The Balrog.";
        case 'C':
            return "C - Gelatinous Cube.";
        case 'D':
            return "D - An Ancient Dragon (Beware).";
        case 'E':
            return "E - Elemental.";
        case 'F':
            return "F - Giant Fly.";
        case 'G':
            return "G - Ghost.";
        case 'H':
            return "H - Hobgoblin.";
//        case 'I':
//            return "I - Invisible Stalker.";
        case 'J':
            return "J - Jelly.";
        case 'K':
            return "K - Killer Beetle.";
        case 'L':
            return "L - Lich.";
        case 'M':
            return "M - Mummy.";
//        case 'N':
//            return "N - Not used.";
        case 'O':
            return "O - Ooze.";
        case 'P':
            return "P - Giant humanoid.";
        case 'Q':
            return "Q - Quylthulg (Pulsing Flesh Mound).";
        case 'R':
            return "R - Reptile.";
        case 'S':
            return "S - Giant Scorpion.";
        case 'T':
            return "T - Troll.";
        case 'U':
            return "U - Umber Hulk.";
        case 'V':
            return "V - Vampire.";
        case 'W':
            return "W - Wight or Wraith.";
        case 'X':
            return "X - Xorn.";
        case 'Y':
            return "Y - Yeti.";
//        case 'Z':
//            return "Z - Not used.";
        case '[':
            return "[ - Hard armor.";
        case '\\':
            return "\\ - A hafted weapon.";
        case ']':
            return "] - Misc. armor.";
        case '^':
            return "^ - A trap.";
        case '_':
            return "_ - A staff.";
//        case '`':
//            return "` - Not used.";
        case 'a':
            return "a - Giant Ant.";
        case 'b':
            return "b - Giant Bat.";
        case 'c':
            return "c - Giant Centipede.";
        case 'd':
            return "d - Dragon.";
        case 'e':
            return "e - Floating Eye.";
        case 'f':
            return "f - Giant Frog.";
        case 'g':
            return "g - Golem.";
        case 'h':
            return "h - Harpy.";
        case 'i':
            return "i - Icky Thing.";
        case 'j':
            return "j - Jackal.";
        case 'k':
            return "k - Kobold.";
        case 'l':
            return "l - Giant Louse.";
        case 'm':
            return "m - Mold.";
        case 'n':
            return "n - Naga.";
        case 'o':
            return "o - Orc or Ogre.";
        case 'p':
            return "p - Person (Humanoid).";
        case 'q':
            return "q - Quasit.";
        case 'r':
            return "r - Rodent.";
        case 's':
            return "s - Skeleton.";
        case 't':
            return "t - Giant Tick.";
//        case 'u':
//            return "u - Not used.";
//        case 'v':
//            return "v - Not used.";
        case 'w':
            return "w - Worm or Worm Mass.";
//        case 'x':
//            return "x - Not used.";
        case 'y':
            return "y - Yeek.";
        case 'z':
            return "z - Zombie.";
        case '{':
            return "{ - Arrow, bolt, or bullet.";
        case '|':
            return "| - A sword or dagger.";
        case '}':
            return "} - Bow, crossbow, or sling.";
        case '~':
            return "~ - Miscellaneous item.";
        default:
            return "Not Used.";
    }
}

// Allow access to monster memory. -CJS-
static void printKnownMonsterMemories(char command) {
    int n = 0;
    char query;

    for (int i = MAX_CREATURES - 1; i >= 0; i--) {
        if (creatures_list[i].cchar == command && bool_roff_recall(i)) {
            if (n == 0) {
                putString("You recall those details? [y/n]", 0, 40);
                query = getKeyInput();

                if (query != 'y' && query != 'Y') {
                    break;
                }

                erase_line(0, 40);
                save_screen();
            }
            n++;

            query = (char) roff_recall(i);
            restore_screen();
            if (query == ESCAPE) {
                break;
            }
        }
    }
}

void displayWorldObjectDescription() {
    char command;
    if (!get_com("Enter character to be identified :", &command)) {
        return;
    }

    prt(asciiCharacterDescription(command), 0, 0);

    printKnownMonsterMemories(command);
}
