all : poker_s poker_c

all2 : p_s p_c

poker_s : poker_s.c
	gcc -o poker_s poker_s.c -lcurses

poker_c : poker_c.c
	gcc -o poker_c poker_c.c -lcurses

p_s : p_s.c
         gcc -o p_s p_s.c -lcurses

p_c : p_c.c
         gcc -o p_c p_c.c -lcurses

clean2:
         rm p_s p_c


clean:
	rm poker_s poker_c
