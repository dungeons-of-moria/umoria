
all:	calchits


calchits:	calchits.c
	$(CC) $(CFLAGS) $@.c $(LDFLAGS) -o $@

clean:
	-rm -f >&/dev/null *.o

dist:
	shar README Makefile calchits.c dragon.inf weapon.lst \
	     mkallwpn mkallwpn.lst highchar.in lowchar.in >calchit.shr


