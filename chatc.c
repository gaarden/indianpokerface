#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <curses.h>

#define BUF_SIZE 1024
 
pthread_t thr;
int thr_id;
int thr_exit = 1;
char recv_data[BUF_SIZE];
int client_fd,len,n,n2;
void *treturn;
int mChip = 20;
char oChip[BUF_SIZE];
char bChip[BUF_SIZE];
char mNum[BUF_SIZE];
int oNum = 0;
char oNumc[BUF_SIZE];
int win = 1;

void *thread_recv(void *arg);

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
}

void thread_start()
{
        thr_exit = 0;
        thr_id = pthread_create(&thr,NULL,thread_recv,NULL);
 
}
 
void thread_stop()
{
    thr_exit = 1;
    thr_id = pthread_join(thr,&treturn);
}
 
void *thread_recv(void *arg)
{
    while(!thr_exit)
    {
         if((n = recv(client_fd,recv_data,sizeof(recv_data),0)) == -1)
         {
             printf("disconnect!!\n");
             return (int*)0;
         }
         else if(n > 0)
         {    
             recv_data[n] = '\0';
             printf("\n[player1]: %s\n",recv_data);
         }
    }
 
    pthread_exit((void*)0);
}

int newCard()
{
	srand(time(NULL));
    int random = 0; // 정수형 변수 선언
    random = rand() % 9 + 1; // 난수 생성
    return random;
}
 
int main(int argc,char *argv[])
{
    struct sockaddr_in client_addr;
    char *IP = argv[1];
    in_port_t PORT = atoi(argv[2]);
    char chat_data[BUF_SIZE];
    
    if(argc != 3)
    {
        printf("Usege : ./filename [IP] [PORT] \n");
        exit(0);
    }
 
    
    client_fd = socket(PF_INET,SOCK_STREAM,0);
    
    client_addr.sin_addr.s_addr = inet_addr(IP);
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(PORT);
    
            
    if(connect(client_fd,(struct sockaddr *)&client_addr,sizeof(client_addr))== -1)
    {
        printf("Can't connect\n");
        close(client_fd);
        return -1;
 
    }
    
	start();

    while(1)
    {
        thread_start();

		printf("상대방 카드: { %d }\n", newCard());
		printf("보유칩: { %d }\n", mChip--);
		sleep(1);
		printf("기본베팅은 1개입니다.\n");
		printf("보유칩: { %d }\n", mChip);
		
		if (win == 2)
			{
				printf("player1님의 베팅을 기다리는 중입니다..\n");
				read(client_fd, oChip, BUF_SIZE); //상대가 입력한 것 oChip저장
				printf("player1님은 %d개를 베팅하셨습니다.\n\n", atoi(oChip));

				printf("베팅할 칩 개수를 입력하세요.\n");
				fgets(bChip, sizeof(bChip), stdin);
				printf("%d개를 베팅하셨습니다.\n\n", atoi(bChip));

				send(client_fd, bChip, sizeof(bChip), 0);

				//승패 판단하고 칩 개수 반영
			}

			else
			{
				printf("베팅할 칩 개수를 입력하세요.\n");

				fgets(bChip, sizeof(bChip), stdin);
				printf("%d개를 베팅하셨습니다.\n\n", atoi(bChip));
			
				send(client_fd, bChip, sizeof(bChip), 0);
				
				printf("player1님의 베팅을 기다리는 중입니다..\n");
				read(client_fd, oChip, BUF_SIZE);
				printf("player1님은 %d개를 베팅하셨습니다.\n\n", atoi(oChip));

				//승패 판단하고 칩 개수 반영
			}

			read(client_fd, mNum, BUF_SIZE);
			printf("player2님의 카드 숫자는 %d였습니다.\n\n", atoi(mNum));

			printf("player1님의 카드 숫자는 무엇이었습니까?\n");
			fgets(oNumc, sizeof(oNumc), stdin);
			send(client_fd, oNumc, sizeof(oNumc), 0);
			
			oNum = atoi(oNumc);
			
			//승패 판단 후 칩에 반영
			if (atoi(mNum) > oNum)
			{
				mChip = mChip + atoi(oChip) + 2;

				win = 2; //이기면 후공?

				printf("player2님이 승리하셨습니다.\n");
				printf("player1이 선플레이어입니다.\n\n");
			}

			else if (atoi(mNum) < oNum)
			{
				mChip = mChip - atoi(bChip);

				win = 2;
				
				printf("player2님이 패배하셨습니다.\n");
				printf("player2가 선플레이어입니다.\n\n");
			}

			else //무승부
			{
				mChip += 1;
				
				printf("무승부입니다.\n");
				printf("베팅 순서는 유지됩니다.\n\n");
			}


    }
    
    thread_stop();
 
 
    close(client_fd);
 
    return 0;
}
