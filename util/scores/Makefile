all: prscore delscore

# Use the player.c from ../../source
prscore: print.o player.o
	cc -o prscore print.o player.o

delscore: delete.o
	cc -o delscore delete.o

clean:
	rm delscore delete.o prscore print.o player.o
