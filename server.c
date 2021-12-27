#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/time.h>
#include <sys/select.h>
#include <time.h>
#include <curses.h>

#define BUF_SIZE 1024

int game = 1;

void error_handling(char * buf); // Error Handling 
int newCard(); // 카드 랜덤 배분

int serv_sock, clnt_sock;
struct sockaddr_in serv_adr, clnt_adr; // 서버를 위한 소켓 : Serv_adr, 클라이언트를 위핸 소켓 clnt_adr
    

fd_set reads, cpy_reads; // fd_set들 

int count_cli = 0; // 클라이언트 갯수체크
int counting=0;

socklen_t adr_sz; // socklenth 변수
int fd_max, str_len, fd_num, i;
char buf[BUF_SIZE];
char buf2[BUF_SIZE*10];

int main(int argc, char * argv[]){
    struct timeval timeout; // select를 사용하기 위한 timeval 구조체. Non-Blocking구현을 위해 각 FD들이 wait할 시간을 설정해줌
    
    if(argc!=2){
        printf("Usage: %s<port>\n",argv[0]);
        exit(1);
    }
    serv_sock = socket(PF_INET, SOCK_STREAM, 0); // server 소켓
    memset(&serv_adr, 0, sizeof(serv_adr)); // 소켓 초기화 

    // 서버 설정. IPv4, 접속가능 주소는 아무거나, 포트는 받아 오는 걸로. 
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    // 바인드 및 리슨     
    if(bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error!");
    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error");
    
    // fd_set을 초기화함
    FD_ZERO(&reads);
    
    // serv_sock의 번호에 해당하는 set을 초기화하여줌. 서버 소켓 번호 3 
    FD_SET(serv_sock, &reads);

    fd_max = serv_sock; // fd_set에서 0 : stdin , 1 : stdout, 2 : stderr , 3: 추가된 서버 소켓번호 , 그럼으로 for를 돌리기위한 fd_max는 서버 소켓 번호. 

    while(1){
        cpy_reads = reads;

        // 약 5초 정도로 설정
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        // 이곳에서 블락을 처리해줌 select()함수는 fd_set의 파일 디스크립터들 중에 입출력을 수행할 준비가 되거나 timeout변수에서 정해진 시간이 경과할 때까지만 블록.
        // select의 리턴값은 변화가 생긴 파일 디스크립터 번호가 됨. 
        if((fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout)) == -1)
            break;
        
        if(fd_num == 0)
            continue;

        // 지속적으로 for문을 돌려 각 파일디스크립터들이 서버이냐 클라이언트 이냐에 따라 다르게 수행함. 
        for(i = 0; i < fd_max+1; i++){

            // i 번째 fd가 설정이 되어있느냐 ?
            if(FD_ISSET(i, &cpy_reads)){

                // i 번째 스크립트가 서버 소켓의 fd이냐 
                if(i == serv_sock){
                    adr_sz = sizeof(clnt_adr);

                    // 지속적인 연결 수행함. 
                    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &adr_sz);
                    FD_SET(clnt_sock, &reads);

                    // 만약 새로 연결되었다면 fd_max를 늘려 새로운 소켓도 다음에 listen하도록 수행.
                    if(fd_max < clnt_sock)
                        fd_max = clnt_sock;
                    printf("connected client: %d\n", clnt_sock);
                    count_cli++;
                    counting++;

                    // 새로 연결이 수행되면 간단한 안내를 클라이언트에게 보냄 
                    // 중요한점 : 코딩의 용의성을 위해 한 소켓에 read or write를 수행할때 한번에 보내는 것을 매우 추천함. 여러번 나누어 보내다보면 
                    // 원하는 대로 수행이 안될 수도 있음. 예 ) 여러번 나누어 write를 수행하던 도중 클라이언트에서 블락을 해버림. 

                    sprintf(buf, "Server : Welcome~\nServer : The number of clients is %d now.",counting);
                    write(clnt_sock, buf, sizeof(buf));

                    // 현제 존재하는 클라이언트들에게 새로운 클라이언트가 들어옴을 알림.
                    for(int k = 4; k < 4+count_cli; k++)
                    {
                        if(k != clnt_sock){
                            sprintf(buf,"client %d has joined this game room\n",clnt_sock);
                            write(k, buf,sizeof(buf));
                        }
                    }
                // i 번째 fd가 클라이언트 관련 소켓이면
                }else {
                    // 정해준 시간만큼 읽음 대기. 
                    //str_len = read(i, buf, BUF_SIZE);

                    // 읽어 들였는데 0이다. 이는 클라이언트 소켓이 닫혔음을 의미함. 
                    if(str_len == 0){
                        for(int k = 4; k < 4+count_cli; k++)
                        {
                            // 각 클라이언트들에게 해당 클라이언트가 나갔음을 공지함.
                            if(k != i){
                               sprintf(buf,"client %d has left this game room\n",i);
                                write(k, buf,sizeof(buf));
                            }
                        }
                        // 해당번호의 fd를 비워줌. 
                        FD_CLR(i, &reads);
                        close(i);
                        printf("closed client: %d \n", i);
                        //count_cli--;
                        counting--;
                    }else{
			while (game){
			    int chip1 = 20;
			    int chip2 = 20;
			    
			    sprintf(buf2, "player1가 베팅할 칩 개수를 입력해주세요.\n");
			    write(4, buf2, strlen(buf2));

			    sprintf(buf2, "player1의 베팅을 기다리고 있습니다.\n");
			    write(5, buf2, BUF_SIZE);

			    read(4, buf, BUF_SIZE);
			    sprintf(buf2, "player1: %s\n", buf);
			    write(5, buf2, BUF_SIZE);

			    sprintf(buf2, "player2가 베팅할 칩 개수를 입력해주세요.\n");
			    write(5, buf2, BUF_SIZE);

			    sprintf(buf2, "player2의 베팅을 기다리고 있습니다.\n");
			    write(4, buf2, BUF_SIZE);

			    read(5, buf, BUF_SIZE);
			    sprintf(buf2, "player2: %s\n", buf);
			    write(4, buf2, BUF_SIZE);

			    if (chip1*chip2 == 0){ //chip1또는 chip2가 0이 되면 종료
				game = 0;
			    }
			}
                    }
                }
            }
        }
    }
    close(serv_sock);
    return 0;
}
void error_handling(char * buf){
    fputs(buf, stderr);
    fputc('\n', stderr);
    exit(1);
}

void printmsg(int num){
	int temp = strlen(buf2);
	//strncat(buf2, buf, str_len);
	write(num, buf2, temp);
}

int newCard()
{
    srand(time(NULL));
    int random = 0; // 정수형 변수 선언
    random = rand() % 9 + 1; // 난수 생성
    if (random == 1)
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
    else if (random == 2)
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
    else if (random == 3)
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
    else if (random == 4)
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
    else if (random == 5)
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
    else if (random == 6)
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

    else if (random == 7)
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

    else if (random == 8)
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
    else if (random == 9)
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
    else if (random == 10)
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