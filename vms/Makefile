# COPTS = /nowarning
COPTS =

OBJS1 = create.obj creature.obj death.obj desc.obj dungeon.obj eat.obj \
	files.obj generate.obj getch.obj help.obj io.obj magic.obj main.obj \
	misc1.obj misc2.obj misc3.obj misc4.obj monsters.obj
OBJS2 = moria1.obj moria2.obj moria3.obj moria4.obj player.obj potions.obj \
	prayer.obj recall.obj rnd.obj save.obj scrolls.obj sets.obj \
	signals.obj spells.obj staffs.obj store1.obj store2.obj tables.obj
OBJS3 =  treasure.obj uexit.obj variable.obj wands.obj wizard.obj

moria : $(OBJS1) $(OBJS2) $(OBJS3)
	link moria/opt

create.obj : create.c constant.h types.h externs.h config.h
	cc $(COPTS) create.c /obj=create.obj

creature.obj : creature.c constant.h types.h externs.h config.h
	cc $(COPTS) creature.c /obj=creature.obj

death.obj : death.c constant.h types.h externs.h config.h
	cc $(COPTS) death.c /obj=death.obj

desc.obj : desc.c constant.h types.h externs.h config.h
	cc $(COPTS) desc.c /obj=desc.obj

dungeon.obj : dungeon.c constant.h types.h externs.h config.h
	cc $(COPTS) dungeon.c /obj=dungeon.obj

eat.obj : eat.c constant.h types.h externs.h config.h
	cc $(COPTS) eat.c /obj=eat.obj

files.obj : files.c constant.h types.h externs.h config.h
	cc $(COPTS) files.c /obj=files.obj

generate.obj : generate.c constant.h types.h externs.h config.h
	cc $(COPTS) generate.c /obj=generate.obj

getch.obj : getch.c
	cc $(COPTS) getch.c /obj=getch.obj

help.obj : help.c constant.h types.h externs.h config.h
	cc $(COPTS) help.c /obj=help.obj

io.obj : io.c constant.h types.h externs.h config.h
	cc $(COPTS) io.c /obj=io.obj

magic.obj : magic.c constant.h types.h externs.h config.h
	cc $(COPTS) magic.c /obj=magic.obj

main.obj : main.c constant.h types.h externs.h config.h
	cc $(COPTS) main.c /obj=main.obj

misc1.obj : misc1.c constant.h types.h externs.h config.h
	cc $(COPTS) misc1.c /obj=misc1.obj

misc2.obj : misc2.c constant.h types.h externs.h config.h
	cc $(COPTS) misc2.c /obj=misc2.obj

misc3.obj : misc3.c constant.h types.h externs.h config.h
	cc $(COPTS) misc3.c /obj=misc3.obj

misc4.obj : misc4.c constant.h types.h externs.h config.h
	cc $(COPTS) misc4.c /obj=misc4.obj

monsters.obj : monsters.c constant.h types.h config.h
	cc $(COPTS) monsters.c /obj=monsters.obj

moria1.obj : moria1.c constant.h types.h externs.h config.h
	cc $(COPTS) moria1.c /obj=moria1.obj

moria2.obj : moria2.c constant.h types.h externs.h config.h
	cc $(COPTS) moria2.c /obj=moria2.obj

moria3.obj : moria3.c constant.h types.h externs.h config.h
	cc $(COPTS) moria3.c /obj=moria3.obj

moria4.obj : moria4.c constant.h types.h externs.h config.h
	cc $(COPTS) moria4.c /obj=moria4.obj

player.obj : player.c constant.h types.h config.h
	cc $(COPTS) player.c /obj=player.obj

potions.obj : potions.c constant.h types.h externs.h config.h
	cc $(COPTS) potions.c /obj=potions.obj

prayer.obj : prayer.c constant.h types.h externs.h config.h
	cc $(COPTS) prayer.c /obj=prayer.obj

recall.obj : recall.c constant.h types.h externs.h config.h
	cc $(COPTS) recall.c /obj=recall.obj

rnd.obj : rnd.c constant.h types.h
	cc $(COPTS) rnd.c /obj=rnd.obj

save.obj : save.c constant.h types.h externs.h config.h
	cc $(COPTS) save.c /obj=save.obj

scrolls.obj : scrolls.c constant.h types.h externs.h config.h
	cc $(COPTS) scrolls.c /obj=scrolls.obj

sets.obj : sets.c constant.h config.h
	cc $(COPTS) sets.c /obj=sets.obj

signals.obj : signals.c constant.h types.h externs.h config.h
	cc $(COPTS) signals.c /obj=signals.obj

spells.obj : spells.c constant.h types.h externs.h config.h
	cc $(COPTS) spells.c /obj=spells.obj

staffs.obj : staffs.c constant.h types.h externs.h config.h
	cc $(COPTS) staffs.c /obj=staffs.obj

store1.obj : store1.c constant.h types.h externs.h config.h
	cc $(COPTS) store1.c /obj=store1.obj

store2.obj : store2.c constant.h types.h externs.h config.h
	cc $(COPTS) store2.c /obj=store2.obj

tables.obj : tables.c constant.h types.h config.h
	cc $(COPTS) tables.c /obj=tables.obj

treasure.obj : treasure.c constant.h types.h config.h
	cc $(COPTS) treasure.c /obj=treasure.obj

uexit.obj : uexit.c constant.h types.h config.h
	cc $(COPTS) uexit.c /obj=uexit.obj

variable.obj : variable.c constant.h types.h externs.h config.h
	cc $(COPTS) variable.c /obj=variable.obj

wands.obj : wands.c constant.h types.h externs.h config.h
	cc $(COPTS) wands.c /obj=wands.obj

wizard.obj : wizard.c constant.h types.h externs.h config.h
	cc $(COPTS) wizard.c /obj=wizard.obj


