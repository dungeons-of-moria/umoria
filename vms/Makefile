# COPTS = /nowarning
COPTS =

OBJS1 = create.o creature.o death.o desc.o dungeon.o eat.o files.o generate.o help.o io.o magic.o main.o misc1.o misc2.o monsters.o moria1.o moria2.o
OBJS2 = potions.o prayer.o save.o scrolls.o sets.o signals.o spells.o staffs.o store1.o store2.o treasure1.o treasure2.o variables.o uexit.o wands.o wizard.o
OBJS3 = cbreak.o getch.o

moria : $(OBJS1) $(OBJS2) $(OBJS3)
	link moria/opt

cbreak.o : cbreak.c
	cc $(COPTS) cbreak.c /obj=cbreak.o

create.o : create.c constants.h types.h externs.h config.h
	cc $(COPTS) create.c /obj=create.o

creature.o : creature.c constants.h types.h externs.h config.h
	cc $(COPTS) creature.c /obj=creature.o

death.o : death.c constants.h types.h externs.h config.h
	cc $(COPTS) death.c /obj=death.o

desc.o : desc.c constants.h types.h externs.h config.h
	cc $(COPTS) desc.c /obj=desc.o

dungeon.o : dungeon.c constants.h types.h externs.h config.h
	cc $(COPTS) dungeon.c /obj=dungeon.o

eat.o : eat.c constants.h types.h externs.h config.h
	cc $(COPTS) eat.c /obj=eat.o

files.o : files.c constants.h types.h externs.h config.h
	cc $(COPTS) files.c /obj=files.o

generate.o : generate.c constants.h types.h externs.h config.h
	cc $(COPTS) generate.c /obj=generate.o

getch.o : getch.c
	cc $(COPTS) getch.c /obj=getch.o

help.o : help.c constants.h types.h externs.h config.h
	cc $(COPTS) help.c /obj=help.o

io.o : io.c constants.h types.h externs.h config.h
	cc $(COPTS) io.c /obj=io.o

magic.o : magic.c constants.h types.h externs.h config.h
	cc $(COPTS) magic.c /obj=magic.o

main.o : main.c constants.h types.h externs.h config.h
	cc $(COPTS) main.c /obj=main.o

misc1.o : misc1.c constants.h types.h externs.h config.h
	cc $(COPTS) misc1.c /obj=misc1.o

misc2.o : misc2.c constants.h types.h externs.h config.h
	cc $(COPTS) misc2.c /obj=misc2.o

monsters.o : monsters.c constants.h types.h config.h
	cc $(COPTS) monsters.c /obj=monsters.o

moria1.o : moria1.c constants.h types.h externs.h config.h
	cc $(COPTS) moria1.c /obj=moria1.o

moria2.o : moria2.c constants.h types.h externs.h config.h
	cc $(COPTS) moria2.c /obj=moria2.o

potions.o : potions.c constants.h types.h externs.h config.h
	cc $(COPTS) potions.c /obj=potions.o

prayer.o : prayer.c constants.h types.h externs.h config.h
	cc $(COPTS) prayer.c /obj=prayer.o

save.o : save.c constants.h types.h externs.h config.h
	cc $(COPTS) save.c /obj=save.o

scrolls.o : scrolls.c constants.h types.h externs.h config.h
	cc $(COPTS) scrolls.c /obj=scrolls.o

sets.o : sets.c constants.h config.h
	cc $(COPTS) sets.c /obj=sets.o

signals.o : signals.c constants.h types.h externs.h config.h
	cc $(COPTS) signals.c /obj=signals.o

spells.o : spells.c constants.h types.h externs.h config.h
	cc $(COPTS) spells.c /obj=spells.o

staffs.o : staffs.c constants.h types.h externs.h config.h
	cc $(COPTS) staffs.c /obj=staffs.o

store1.o : store1.c constants.h types.h externs.h config.h
	cc $(COPTS) store1.c /obj=store1.o

store2.o : store2.c constants.h types.h externs.h config.h
	cc $(COPTS) store2.c /obj=store2.o

treasure1.o : treasure1.c constants.h types.h config.h
	cc $(COPTS) treasure1.c /obj=treasure1.o

treasure2.o : treasure2.c constants.h types.h config.h
	cc $(COPTS) treasure2.c /obj=treasure2.o

variables.o : variables.c constants.h types.h config.h
	cc $(COPTS) variables.c /obj=variables.o

wands.o : wands.c constants.h types.h externs.h config.h
	cc $(COPTS) wands.c /obj=wands.o

wizard.o : wizard.c constants.h types.h externs.h config.h
	cc $(COPTS) wizard.c /obj=wizard.o

uexit.o : uexit.c
	cc $(COPTS) uexit.c /obj=uexit.o
