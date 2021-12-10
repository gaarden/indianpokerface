#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <time.h>
#include <curses.h>

#define BUF_LEN 1024

struct sockaddr_in server_addr,client_addr;
int server_fd,client_fd,n,n2;
char recv_data[BUF_LEN];

int mChip = 20; //my chip
char oChip[BUF_LEN]; //other chip->상대가 베팅한 칩 개수
char bChip[BUF_LEN]; //betting chip->내가 베팅한 칩 개수
char mNumc[BUF_LEN]; //my num->내 숫자
int oNum = 0,mNum=0; //other num->상대 숫자
char oNumc[BUF_LEN]; //char형으로 저장할 곳
int win = 1;

void start();
int newCard();

int main(int argc,char *argv[])
{
    char chat_data[BUF_LEN];
    char temp[20];
    int len;
 
    if(argc !=2)
    {
        printf("Usege ./filename [PORT] \n");
        exit(0);
    }
 
    if((server_fd = socket(AF_INET,SOCK_STREAM,0)) == -1)
    {
        printf("Server: can not Open Socket\n");
        exit(0);
    }
    
    memset(&server_addr,0x00,sizeof(server_addr));
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));
    
    if(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))< 0)
    {
        printf("Server: cat not bind local addrss\n");
        exit(0);
        
    }
    
    
    if(listen(server_fd,5) < 0)
    {
        printf("Server: cat not listen connnect.\n");
        exit(0);
    }
    
    memset(recv_data,0x00,sizeof(recv_data));
    len = sizeof(client_addr);
    printf("=====[PORT] : %d =====\n",atoi(argv[1]));
    printf("Server : wating connection request.\n");
 
    start();
    
    while(1)
    {
        client_fd = accept(server_fd,(struct sockaddr *)&client_addr,(socklen_t *)&len);
        
        if(client_fd < 0)
        {
            printf("Server: accept failed\n");
            exit(0);
        }
        
        inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,temp,sizeof(temp));
        printf("Server: %s client connect.\n",temp);
 
        printf("\nplayer2(%s)님이 들어오셨습니다.\n 게임을 시작합니다.\n\n",
                inet_ntoa(client_addr.sin_addr));
		printf("player2(%s)님이 선플레이어입니다.\n\n", inet_ntoa(client_addr.sin_addr));

        while(1)
        {
			sleep(2);
			printf("<상대방 카드>\n");
			oNum = newCard();

			printf("보유칩: { %d }\n", mChip--);
			sleep(1);
			printf("기본베팅은 1개입니다.\n");
			printf("보유칩: { %d }\n\n", mChip);
			
			if (win == 1)
			{
				printf("player2님의 베팅을 기다리는 중입니다..\n");
				read(client_fd, oChip, BUF_LEN); //상대가 입력한 것 oChip저장
				printf("player2님은 %d개를 베팅하셨습니다.\n\n", atoi(oChip));

				printf("베팅할 칩 개수를 입력하세요.\n");
				fgets(bChip, sizeof(bChip), stdin);
				printf("%d개를 베팅하셨습니다.\n\n", atoi(bChip));

				send(client_fd, bChip, sizeof(bChip), 0);
				sleep(2);
			}

			else
			{
				printf("베팅할 칩 개수를 입력하세요.\n");

				fgets(bChip, sizeof(bChip), stdin);
				printf("%d개를 베팅하셨습니다.\n\n", atoi(bChip));
			
				send(client_fd, bChip, sizeof(bChip), 0);

				printf("player2님의 베팅을 기다리는 중입니다..\n");
				read(client_fd, oChip, BUF_LEN);
				printf("player2님은 %d개를 베팅하셨습니다.\n\n", atoi(oChip));
				sleep(2);
			}

			
			sprintf(oNumc, "%d", oNum);
			send(client_fd, oNumc, sizeof(oNumc), 0);

			read(client_fd, mNumc, BUF_LEN);
			mNum = atoi(mNumc);
			printf("당신의 카드 숫자는 %d였습니다.\n\n", mNum);
			sleep(3);

			//승패 판단 후 칩에 반영
			if (mNum > oNum)
			{
				mChip = mChip + atoi(oChip) + 2;

				win = 1; //이기면 후공?

				printf("player1님이 승리하셨습니다.\n");
				printf("player2이 선플레이어입니다.\n\n");
			}

			else if (mNum < oNum)
			{
				mChip = mChip - atoi(bChip);

				win = 2;
				
				printf("player1님이 패배하셨습니다.\n");

				if (mChip <= 0)
				{
					printf("player1의 베팅칩이 모두 소진되었습니다.\n");
					printf("player1의 패배로 게임을 종료합니다.\n");
					close(client_fd);
					close(server_fd);
				}

				printf("player1가 선플레이어입니다.\n\n");
			}

			else //무승부
			{
				mChip += 1;
				
				printf("무승부입니다.\n");
				printf("베팅 순서는 유지됩니다.\n\n");
			}
        
        }
        
        close(client_fd);
        printf("Server: %s client closed.\n",temp);
	}
        
    
    close(server_fd);
    
    return 0;
}



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

int newCard()
{
	srand(time(NULL)+1);
    int random = 0; // 정수형 변수 선언
    random = rand() % 9 + 1; // 난수 생성
 if(random==1)
    {
            printf("***********\n");
            printf("*         *\n");
            printf("*    *    *\n");
            printf("*  * *    *\n");
            printf("*    *    *\n");
            printf("*    *    *\n");
            printf("*    *    *\n");
            printf("*    *    *\n");
            printf("*  *****  *\n");
            printf("*         *\n");
            printf("***********\n");
    }
    else if(random==2)
            {
            printf("***********\n");
            printf("*         *\n");
            printf("*   ***   *\n");
            printf("*  *   *  *\n");
            printf("*     *   *\n");
            printf("*    *    *\n");
            printf("*   *     *\n");
            printf("*  *      *\n");
            printf("*  *****  *\n");
            printf("*         *\n");
            printf("***********\n");
    }
else if(random==3)
            {
            printf("***********\n");
            printf("*         *\n");
            printf("*   ***   *\n");
            printf("*  *   *  *\n");
            printf("*      *  *\n");
            printf("*    **   *\n");
            printf("*      *  *\n");
            printf("*  *   *  *\n");
            printf("*   ***   *\n");
            printf("*         *\n");
            printf("***********\n");
            }
        else if(random==4)
            {
            printf("***********\n");
            printf("*         *\n");
            printf("*     *   *\n");
            printf("*    **   *\n");
            printf("*   * *   *\n");
            printf("*  *  *   *\n");
            printf("* ******* *\n");
            printf("*     *   *\n");
            printf("*     *   *\n");
            printf("*         *\n");
            printf("***********\n");
            }
        else if(random==5)
            {
            printf("***********\n");
            printf("*         *\n");
            printf("*  *****  *\n");
            printf("*  *      *\n");
            printf("*  ****   *\n");
            printf("*      *  *\n");
            printf("*      *  *\n");
            printf("*  *   *  *\n");
            printf("*   ***   *\n");
            printf("*         *\n");
            printf("***********\n");
    }
else if(random==6)
            {
            printf("***********\n");
            printf("*         *\n");
            printf("*   ***   *\n");
            printf("*  *   *  *\n");
            printf("*  *      *\n");
            printf("*  ****   *\n");
            printf("*  *   *  *\n");
            printf("*  *   *  *\n");
            printf("*   ***   *\n");
            printf("*         *\n");
            printf("***********\n");
    }
else if(random==7)
            {
            printf("***********\n");
            printf("*         *\n");
            printf("*  *****  *\n");
            printf("*  *   *  *\n");
            printf("*  *   *  *\n");
            printf("*      *  *\n");
            printf("*      *  *\n");
            printf("*      *  *\n");
            printf("*      *  *\n");
            printf("*         *\n");
            printf("***********\n");
    }
else if(random==8)
            {
            printf("***********\n");
            printf("*         *\n");
            printf("*   ***   *\n");
            printf("*  *   *  *\n");
            printf("*  *   *  *\n");
            printf("*   ***   *\n");
            printf("*  *   *  *\n");
            printf("*  *   *  *\n");
            printf("*   ***   *\n");
            printf("*         *\n");
            printf("***********\n");
    }
    else if(random==9)
            {
            printf("***********\n");
            printf("*         *\n");
            printf("*   ***   *\n");
            printf("*  *   *  *\n");
            printf("*  *   *  *\n");
            printf("*   ****  *\n");
            printf("*      *  *\n");
            printf("*  *   *  *\n");
            printf("*   ***   *\n");
            printf("*         *\n");
            printf("***********\n");
    }
    else if(random==10)
            {
            printf("***********\n");
            printf("*         *\n");
            printf("* *   **  *\n");
            printf("* *  *  * *\n");
            printf("* *  *  * *\n");
            printf("* *  *  * *\n");
            printf("* *  *  * *\n");
            printf("* *  *  * *\n");
            printf("* *   **  *\n");
            printf("*         *\n");
            printf("***********\n");
    }


    return random;
}

