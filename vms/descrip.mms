MORIA.EXE : MAIN.OBJ, CREATE.OBJ, CREATURE.OBJ, DEATH.OBJ, -
    DESC.OBJ, DUNGEON.OBJ, EAT.OBJ, FILES.OBJ, GENERATE.OBJ, -
    GETCH.OBJ, HELP.OBJ, IO.OBJ, MAGIC.OBJ, MAIN.OBJ, MISC1.OBJ, -
    MISC2.OBJ, MISC3.OBJ, MISC4.OBJ, MONSTERS.OBJ, MORIA1.OBJ, -
    MORIA2.OBJ, MORIA3.OBJ, MORIA4.OBJ, PLAYER.OBJ, -
    POTIONS.OBJ, PRAYER.OBJ, RECALL.OBJ, RND.OBJ, SAVE.OBJ, SCROLLS.OBJ, -
    SETS.OBJ, SIGNALS.OBJ, SPELLS.OBJ, STAFFS.OBJ, STORE1.OBJ, -
    STORE2.OBJ, TABLES.OBJ, TREASURE.OBJ, UEXIT.OBJ, -
    VARIABLE.OBJ, WANDS.OBJ, WIZARD.OBJ
    - $(link) $(linkflags) MORIA.OPT/opt

create.obj : constant.h, types.h, externs.h, config.h
creature.obj : constant.h, types.h, externs.h, config.h
death.obj : constant.h, types.h, externs.h, config.h
desc.obj : constant.h, types.h, externs.h, config.h
dungeon.obj : constant.h, types.h, externs.h, config.h
eat.obj : constant.h, types.h, externs.h, config.h
files.obj : constant.h, types.h, externs.h, config.h
generate.obj : constant.h, types.h, externs.h, config.h
help.obj : constant.h, types.h, externs.h, config.h
io.obj : constant.h, types.h, externs.h, config.h
magic.obj : constant.h, types.h, externs.h, config.h
main.obj : constant.h, types.h, externs.h, config.h
misc1.obj : constant.h, types.h, externs.h, config.h
misc2.obj : constant.h, types.h, externs.h, config.h
misc3.obj : constant.h, types.h, externs.h, config.h
misc4.obj : constant.h, types.h, externs.h, config.h
monsters.obj : constant.h, types.h, config.h
moria1.obj : constant.h, types.h, externs.h, config.h
moria2.obj : constant.h, types.h, externs.h, config.h
moria3.obj : constant.h, types.h, externs.h, config.h
moria4.obj : constant.h, types.h, externs.h, config.h
player.obj : constant.h, types.h, config.h
potions.obj : constant.h, types.h, externs.h, config.h
prayer.obj : constant.h, types.h, externs.h, config.h
recall.obj : constant.h, config.h, types.h, externs.h
rnd.obj : constant.h, types.h
save.obj : constant.h, types.h, externs.h, config.h
scrolls.obj : constant.h, types.h, externs.h, config.h
sets.obj : constant.h, config.h
signals.obj : constant.h, types.h, externs.h, config.h
spells.obj : constant.h, types.h, externs.h, config.h
staffs.obj : constant.h, types.h, externs.h, config.h
store1.obj : constant.h, types.h, externs.h, config.h
store2.obj : constant.h, types.h, externs.h, config.h
tables.obj : constant.h, types.h, config.h
treasure.obj : constant.h, types.h, config.h
variable.obj : constant.h, types.h, config.h
wands.obj : constant.h, types.h, externs.h, config.h
wizard.obj : constant.h, types.h, externs.h, config.h
