all : poker_s poker_c

poker_s : poker_s.c
	gcc -o poker_s poker_s.c -lcurses

poker_c : poker_c.c
	gcc -o poker_c poker_c.c -lcurses

clean:
	rm poker_s poker_c
