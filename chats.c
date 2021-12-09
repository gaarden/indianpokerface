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
 
pthread_t thr;
int thr_exit = 1;
int thr_id;
void *treturn;
struct sockaddr_in server_addr,client_addr;
int server_fd,client_fd,n,n2;
char recv_data[BUF_LEN];

int mChip = 20; //my chip
char bChip[BUF_LEN];

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
	printf("\nHello World!\n");
}

int newCard()
{
    int random = 0; // 정수형 변수 선언
    random = rand() % 9 + 1; // 난수 생성
    return random;
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
            printf("[접속자: %s(%d)] disconnect\n",inet_ntoa(client_addr.sin_addr)
                    ,ntohs(client_addr.sin_port));
            thread_stop();
            close(client_fd);
            
 
        }else if(n > 0)
        {
            recv_data[n] = '\0';	
            printf("\n[player2]: %s\n" ,recv_data);
        }
    }
 
        pthread_exit((void *)0);
 
}
 
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
            thread_start();

			printf("상대방 카드: { %d }\n", newCard());
			printf("보유칩: { %d }\n", mChip--);
			sleep(1);
			printf("기본베팅은 1개입니다.\n");
			printf("보유칩: { %d }\n", mChip);
			printf("베팅할 칩 개수를 입력하세요.\n");

			fgets(bChip, sizeof(bChip), stdin);
			printf("%d개를 베팅하셨습니다.\n\n", atoi(bChip));
			/*
			if((n2 = send(client_fd,chat_data,sizeof(chat_data),0)) == -1)
            {
                break;
            }
			*/
			send(client_fd, bChip, sizeof(bChip), 0);
        
        }
        
        thread_stop();
        close(client_fd);
        printf("Server: %s client closed.\n",temp);
	}
        
    
    close(server_fd);
    
    return 0;
}
