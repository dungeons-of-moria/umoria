CFLAGS = -g

mc : creaturec.o symtab.o
	cc creaturec.o symtab.o -o mc -lm

creaturec.o : creaturec.y st.h generic.h
	yacc creaturec.y
	cc -c $(CFLAGS) y.tab.c
	rm y.tab.c
	mv y.tab.o creaturec.o

symtab.o : symtab.c st.h generic.h
	cc -c $(CFLAGS) symtab.c

lintout : symtab.c creaturec.y
	lint symtab.c creaturec.y > lintout
