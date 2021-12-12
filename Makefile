poker_s : poker_s.o
	gcc -o poker_s poker_s.o -lcurses

poker_s.o : poker_s.c
	gcc -c poker_s.c

poker_c : poker_c.o
	gcc -o poker_c poker_c.o -lcurses

poker_c.o : poker_c.c
	gcc -c poker_c.c
