lc3: lc3.c lc3.h lc3ui.o
	cc -lncurses lc3.c lc3ui.o -o lc3
lc3ui.o: lc3ui.c lc3ui.h
	cc -c lc3ui.c
