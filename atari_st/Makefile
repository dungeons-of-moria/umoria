# The MWC Makefile.

# BINDIR is the directory where the moria binary will be put
# LIBDIR is where the other files (scoreboard, news, hours) will be put
# LIBDIR must be the same directory defined in config.h
# this was clipped from the link command: -lcurses -ltermcap
BINDIR = /usr/public/
LIBDIR = /usr/public/moriadir/

CFLAGS = -A -V -VCSD
CC = cc

SRCS = main.c misc1.c misc2.c misc3.c misc4.c store1.c files.c io.c \
	create.c desc.c generate.c sets.c dungeon.c creature.c death.c \
	eat.c help.c magic.c potions.c prayer.c save.c staffs.c wands.c \
	scrolls.c spells.c wizard.c store2.c signals.c moria1.c moria2.c \
	moria3.c moria4.c monsters.c treasure.c variable.c rnd.c recall.c \
	atarist.c player.c tables.c

OBJS = main.o misc1.o misc2.o misc3.o misc4.o store1.o files.o io.o \
	create.o desc.o generate.o sets.o dungeon.o creature.o death.o \
	eat.o help.o magic.o potions.o prayer.o save.o staffs.o wands.o \
	scrolls.o spells.o wizard.o store2.o signals.o moria1.o moria2.o \
	moria3.o moria4.o monsters.o treasure.o variable.o rnd.o recall.o \
	atarist.o curses.o player.o tables.o

LIBFILES = moria_hours moria_help moria_orig_help moria_wiz_help \
	moria_orig_wiz_help moria_man

moria : $(OBJS)
	$(CC) -o moria $(CFLAGS) $(OBJS)

libfiles: $(LIBFILES)
	chmod 444 $(LIBFILES)
	cp $(LIBFILES) $(LIBDIR)

$(BINDIR):
	mkdir $(BINDIR)
	chmod 755 $(BINDIR)

$(LIBDIR):
	mkdir $(LIBDIR)
	chmod 711 $(LIBDIR)

$(LIBDIR)/moria_hours: moria_hours
	chmod 644 moria_hours
	cp moria_hours $(LIBDIR)

lintout : $(SRCS)
	lint $(SRCS) -lcurses -ltermcap > lintout

lintout2 : $(SRCS)
	lint -bach $(SRCS) -lcurses -ltermcap > lintout

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

clean:
	rm -r *.o
	rm -i moria

create.o: constant.h types.h externs.h config.h
creature.o: constant.h types.h externs.h config.h
death.o: constant.h types.h externs.h config.h
desc.o: constant.h types.h externs.h config.h
dungeon.o: constant.h types.h externs.h config.h
eat.o: constant.h types.h externs.h config.h
files.o: constant.h types.h externs.h config.h
generate.o: constant.h types.h externs.h config.h
help.o: constant.h types.h externs.h config.h
io.o: constant.h types.h externs.h config.h
magic.o: constant.h types.h externs.h config.h
main.o: constant.h types.h externs.h config.h
misc1.o: constant.h types.h externs.h config.h
misc2.o: constant.h types.h externs.h config.h
misc3.o: constant.h types.h externs.h config.h
misc4.o: constant.h types.h externs.h config.h
monsters.o: constant.h types.h config.h
moria1.o: constant.h types.h externs.h config.h
moria2.o: constant.h types.h externs.h config.h
moria3.o: constant.h types.h externs.h config.h
moria4.o: constant.h types.h externs.h config.h
player.o: constant.h types.h config.h
potions.o: constant.h types.h externs.h config.h
prayer.o: constant.h types.h externs.h config.h
recall.o: constant.h config.h types.h externs.h
rnd.o: constant.h types.h
save.o: constant.h types.h externs.h config.h
scrolls.o: constant.h types.h externs.h config.h
sets.o: constant.h config.h
signals.o: constant.h types.h externs.h config.h
spells.o: constant.h types.h externs.h config.h
staffs.o: constant.h types.h externs.h config.h
store1.o: constant.h types.h externs.h config.h
store2.o: constant.h types.h externs.h config.h
tables.o: constant.h types.h config.h
treasure.o: constant.h types.h config.h
atarist.o: constant.h config.h types.h externs.h
variable.o: constant.h types.h config.h
wands.o: constant.h types.h externs.h config.h
wizard.o: constant.h types.h externs.h config.h
