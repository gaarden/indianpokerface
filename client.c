#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/types.h>
#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char * argv[]){
    int sock;
    struct sockaddr_in serv_addr;
    char message[BUF_SIZE];
    int str_len;
    if(argc != 3){
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)
        error_handling("socket() error");
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));
    
    // 서버에 연결 성공했을 경우에 . 한번 소켓에서 보낸 입력을 받음. 
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error!");
    else{   
        while(1){
            int get = read(sock, message, BUF_SIZE);
            printf("%s\n", message);
            if(get != 0) break;
        }
        
    }

    // fork를 통해 두개의 프로세스에서 부모 프로세스 : 입력을 받아 입력받은 문자열을 서버 소켓에 write함, 자식 프로세스 서버에서 입력을 대기해 받으면 출력해줌. 
    pid_t pid = fork();
    while(1){
        
        if(pid != 0)
        {
            fgets(message, BUF_SIZE, stdin);

            if(!strcmp(message,"q\n") || !strcmp(message,"Q\n")){
                kill(pid,SIGINT);
                break;
            }
            write(sock, message, strlen(message));
        }
        
        if(pid == 0){
            str_len = read(sock, message, BUF_SIZE - 1);
            message[str_len] = 0;
            printf("%s", message);     
        }

    }
    close(sock);

    return 0;
}

void error_handling(char *message){
    fputs(message, stderr);
    fputc('\n',stderr);
    exit(1);
}