# This makefile is for Aztec C 5.0 for the Amiga.
# It will not work with either an earlier version of Aztec C or with Lattice.


# BINDIR is the directory where the moria binary while be put
# LIBDIR is where the other files (score, news, hours) will be put
# LIBDIR must be the same directory defined in config.h
BINDIR = moria:
LIBDIR = moria:

# -bd = stack depth checking, -md = large data model, -so = optimized
CFLAGS = -so -md -mr -DAMIGA
# CFLAGS = -md -mr -DAMIGA

CC = cc
SRCS = main.c misc1.c misc2.c misc3.c misc4.c store1.c files.c io.c \
	create.c desc.c generate.c sets.c dungeon.c creature.c death.c \
	eat.c help.c magic.c potions.c prayer.c save.c staffs.c wands.c \
	scrolls.c spells.c wizard.c store2.c signals.c moria1.c moria2.c \
	moria3.c moria4.c monsters.c treasure.c variable.c rnd.c recall.c \
	amiga.c player.c tables.c timer.c

OBJS = main.o misc1.o misc2.o misc3.o misc4.o store1.o files.o io.o \
	create.o desc.o generate.o sets.o dungeon.o creature.o death.o \
	eat.o help.o magic.o potions.o prayer.o save.o staffs.o wands.o \
	scrolls.o spells.o wizard.o store2.o signals.o moria1.o moria2.o \
	moria3.o moria4.o monsters.o treasure.o variable.o rnd.o recall.o \
	amiga.o player.o tables.o timer.o

LIBFILES = hours news origcmds.hlp owizcmds.hlp roglcmds.hlp rwizcmds.hlp \
	scores version.hlp welcome.hlp

moria : $(OBJS)
	ln +q +ss -o moria $(OBJS) $(DETACH) -lcurses32 -lml -lcl

# you must define BINDIR and LIBDIR before installing
# assumes that BINDIR and LIBDIR exist
install:
	copy moria $(BINDIR)
	copy files/* $(LIBDIR)

clean:
	delete *.o
	delete moria

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
unix.o: constant.h config.h types.h externs.h
variable.o: constant.h types.h config.h
wands.o: constant.h types.h externs.h config.h
wizard.o: constant.h types.h externs.h config.h
