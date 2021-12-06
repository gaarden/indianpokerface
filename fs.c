#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h>

void start() {
	initscr();
	clear();
	move(0,0);
	addstr(" /* Let's play indianpoker game */");
	move(LINES-1, 0);
	refresh();
	move(0, 0);
	addstr("I will tell you about game's rule ^^7\n\n");
	addstr("1. We'll distribute one card each.\n");
	addstr("2. You can see each other's cards, but you can't see your own cards.\n");
	addstr("3. If you think your card is bigger than your opponent, you can bet.\n");
	addstr("4. If you're scared, you can give up.\n");
	addstr("5. If you run out of chips, you lose.\n");
	addstr("6. The one with the big card wins.\n");
	addstr("7. If your card is 10, but you die, the opponent wins\n");
	addstr("8. There are 30 chips\n");
	addstr("\n Press 'q' to start game\n");
	sleep(2);
	getch();
	endwin();
	system("clear");
	printf("\nHello World!\n");
}

main() {
	start();
}
