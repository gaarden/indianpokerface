#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
 
#define BUF_SIZE 1024
 
pthread_t thr;
int thr_id;
thr_exit = 1;
char recv_data[BUF_SIZE];
int client_fd,len,n,n2;
void *treturn;
 
void *thread_recv(void *arg);
 
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
             printf("\n[관리자]: %s\n",recv_data);
         }
    }
 
    pthread_exit((void*)0);
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
    
    while(1)
    {
        thread_start();
        fgets(chat_data,sizeof(chat_data),stdin);
        send(client_fd,chat_data,sizeof(chat_data),0);
    }
    
    thread_stop();
 
 
    close(client_fd);
 
    return 0;
}
