# BINDIR is the directory where the moria binary while be put
# LIBDIR is where the other files (scoreboard, news, hours) will be put
#  LIBDIR must be the same directory defined in config.h
BINDIR = /usr/public/
LIBDIR = /usr/public/moriadir/

# if you use Gnu C, must add -fwritable-strings option to CFLAGS

CFLAGS = -O
CC = cc

SRCS = main.c misc1.c misc2.c store1.c files.c io.c create.c desc.c\
	generate.c sets.c dungeon.c creature.c death.c eat.c help.c magic.c\
	potions.c prayer.c save.c staffs.c wands.c scrolls.c spells.c\
	wizard.c store2.c signals.c moria1.c moria2.c monsters.c\
	treasure1.c treasure2.c variables.c

OBJS = main.o misc1.o misc2.o store1.o files.o io.o create.o desc.o\
	generate.o sets.o dungeon.o creature.o death.o eat.o help.o magic.o\
	potions.o prayer.o save.o staffs.o wands.o scrolls.o spells.o\
	wizard.o store2.o signals.o moria1.o moria2.o monsters.o\
  	treasure1.o treasure2.o variables.o

moria : $(OBJS)
	$(CC) -o moria $(CFLAGS) $(OBJS) -lm -lcurses -ltermcap

lintout : $(SRCS)
	lint $(SRCS) -lm -lcurses -ltermcap > lintout

lintout2 : $(SRCS)
	lint -bach $(SRCS) -lm -lcurses -ltermcap > lintout

TAGS : $(SRCS)
	ctags -x $(SRCS) > TAGS

# you must define BINDIR and LIBDIR before installing
install:
	cp moria $(BINDIR)
	cp Moria_hours $(LIBDIR)
	cp Moria_news $(LIBDIR)
	cp Highscores $(LIBDIR)
	chmod 4511 $(BINDIR)/moria
	chmod 644 $(LIBDIR)/Highscores
	chmod 444 $(LIBDIR)/Moria_news
	chmod 444 $(LIBDIR)/Moria_hours
	chmod 555 $(LIBDIR)

create.o: constants.h types.h externs.h config.h
creature.o: constants.h types.h externs.h config.h
death.o: constants.h types.h externs.h config.h
desc.o: constants.h types.h externs.h config.h
dungeon.o: constants.h types.h externs.h config.h
eat.o: constants.h types.h externs.h config.h
files.o: constants.h types.h externs.h config.h
generate.o: constants.h types.h externs.h config.h
help.o: constants.h types.h externs.h config.h
io.o: constants.h types.h externs.h config.h
magic.o: constants.h types.h externs.h config.h
main.o: constants.h types.h externs.h config.h
misc1.o: constants.h types.h externs.h config.h
misc2.o: constants.h types.h externs.h config.h
monsters.o: constants.h types.h config.h
moria1.o: constants.h types.h externs.h config.h
moria2.o: constants.h types.h externs.h config.h
potions.o: constants.h types.h externs.h config.h
prayer.o: constants.h types.h externs.h config.h
save.o: constants.h types.h externs.h config.h
scrolls.o: constants.h types.h externs.h config.h
sets.o: constants.h config.h
signals.o: constants.h types.h externs.h config.h
spells.o: constants.h types.h externs.h config.h
staffs.o: constants.h types.h externs.h config.h
store1.o: constants.h types.h externs.h config.h
store2.o: constants.h types.h externs.h config.h
treasure1.o: constants.h types.h config.h
treasure2.o: constants.h types.h config.h
variables.o: constants.h types.h config.h
wands.o: constants.h types.h externs.h config.h
wizard.o: constants.h types.h externs.h config.h
