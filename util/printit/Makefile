CFLAGS= -c

all:    pr_items pr_monst

pr_items:    pr_items.c treasure.o
	cc pr_items.c treasure.o -o pr_items
	pr_items > items

pr_monst:    pr_monst.c monsters.o
	cc pr_monst.c monsters.o -o pr_monst
	pr_monst > monsters

