# Obsolete, this is the 4.87 makefile
#
# Makefile for creating PC MORIA 4.871 using:
#		Microsoft C 4.0 & LINK 3.61
#		OPUS MAKE 5.1 by:
#			OPUS Software
#			1032 Irving St #439
#			San Francisco, CA 94122
#			ph. (415) 664-7901
.SUFFIXES:
.SUFFIXES: .obj .c

CFLAGS		= ${DEFS} -AL -W3 -Gt32
ANSI		= -DANSI	# define this if you want to support ANSI
				# as well as curses.
DEFS		= -DLINT_ARGS -DPC_CURSES ${ANSI}
DEST		= \games
LDFLAGS		= /stack:16384
LDMAP		=
!if '$(ANSI)'!=''
LIBS		= \lib\lcurses.lib \lib\ltermcap.lib
!else
LIBS		= \lib\lcurses.lib
!endif
LINKER		= link
OBJS		= create.obj creature.obj death.obj desc.obj dungeon.obj \
		  eat.obj files.obj generate.obj help.obj io.obj rnd.obj \
		  magic.obj main.obj misc1.obj misc2.obj monsters.obj \
		  moria1.obj moria2.obj ms_ansi.obj ms_misc.obj potions.obj \
		  prayer.obj save.obj scrolls.obj sets.obj signals.obj \
		  spells.obj staffs.obj store1.obj store2.obj treasur1.obj \
		  treasur2.obj variable.obj wands.obj wizard.obj
PROGRAM		= moria.exe
SRCS		= create.c creature.c death.c desc.c dungeon.c eat.c files.c \
		  generate.c help.c io.c rnd.c magic.c main.c misc1.c \
		  misc2.c monsters.c moria1.c moria2.c ms_ansi.c ms_misc.c \
		  potions.c prayer.c save.c scrolls.c sets.c signals.c \
		  spells.c staffs.c store1.c store2.c treasur1.c treasur2.c \
		  variable.c wands.c wizard.c

$(PROGRAM):	$(OBJS)
		@$(LINKER) $(OBJS), $(PROGRAM),, $(LIBS) $(LDFLAGS);

clean:;		@erase *.obj

depend:;	@mkmf -v -dL -c

install:	$(PROGRAM)
		@echo Installing $(PROGRAM) in $(DEST)
		exepack $(PROGRAM) $(DEST)\$(PROGRAM)
		exemod $(DEST)\$(PROGRAM) /max 1
		touch -t 0 $(DEST)\$(PROGRAM)

tags:		$(HDRS) $(SRCS); @ctags *.h *.c

### OPUS MKMF:	Do not remove this line!  Automatic dependencies follow.

create.obj: config.h constant.h externs.h types.h

creature.obj: config.h constant.h externs.h types.h

death.obj: config.h constant.h externs.h types.h

desc.obj: config.h constant.h externs.h types.h

dungeon.obj: config.h constant.h externs.h types.h

eat.obj: config.h constant.h externs.h types.h

files.obj: config.h constant.h externs.h types.h

generate.obj: config.h constant.h externs.h types.h

help.obj: config.h constant.h externs.h types.h

io.obj:	 config.h constant.h externs.h types.h

magic.obj: config.h constant.h externs.h types.h

main.obj: config.h constant.h externs.h types.h

misc1.obj: config.h constant.h externs.h types.h

misc2.obj: config.h constant.h externs.h types.h

monsters.obj: config.h constant.h types.h

moria1.obj: config.h constant.h externs.h types.h

moria2.obj: config.h constant.h externs.h types.h

potions.obj: config.h constant.h externs.h types.h

prayer.obj: config.h constant.h externs.h types.h

save.obj: config.h constant.h externs.h types.h

scrolls.obj: config.h constant.h externs.h types.h

sets.obj: config.h constant.h

signals.obj: config.h constant.h externs.h types.h

spells.obj: config.h constant.h externs.h types.h

staffs.obj: config.h constant.h externs.h types.h

store1.obj: config.h constant.h externs.h types.h

store2.obj: config.h constant.h externs.h types.h

treasur1.obj: config.h constant.h types.h

treasur2.obj: config.h constant.h types.h

variable.obj: config.h constant.h types.h

wands.obj: config.h constant.h externs.h types.h

wizard.obj: config.h constant.h externs.h types.h
