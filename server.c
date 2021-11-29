/* timeserv.c - a socket-based time of day server
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define PORTNUM 10203	/* our time service phone number */
#define HOSTLEN 256
#define oops(msg)	{ perror(msg); exit(1); }
int newCard();
int main(int ac, char *av[])
{
	struct sockaddr_in saddr;	/* build our address here	*/
	struct hostent *hp;		/* this part of our		*/
	char hostname[HOSTLEN];		/* address			*/
	int sock_id, sock_fd;		/* line id, file descriptor	*/
	FILE *sock_fp;			/* use socket as steam		*/
	char *ctime();			/* convert secs to string	*/
	time_t thetime;			/* the time we report		*/
	
	int c1Num=0,c2Num=0;
	/*
	 * Step 1: ask kernel for a socket
	 */
	sock_id = socket( PF_INET, SOCK_STREAM, 0);
	if( sock_id == -1)
		oops( "socket" );

	/* 
	 * Step 2: bind address to socket. Address is host, port  
	 */
	bzero ( (void *) &saddr, sizeof( saddr) ); /* clear out struct	*/

	gethostname( hostname, HOSTLEN );	   /* where am I ?	*/
	hp = gethostbyname( hostname );		   /* get info about host */
						   /* fill in host part	*/
	bcopy( (void *) hp->h_addr, (void *) &saddr.sin_addr, hp->h_length);
	saddr.sin_port = htons(PORTNUM);	   /* fill in socket port */
	saddr.sin_family = AF_INET;		   /* fill in addr family */


	if( bind(sock_id, (struct sockaddr *) &saddr, sizeof(saddr)) != 0 )
		oops( "bind" );

	/* 
	 * Step 3: allow incoming calls with Qsize=1 on socket
	 */

	if ( listen (sock_id, 1) != 0 )
		oops( "listen" );

	/* 
	 * main loop: accept(), write(), close()
	 */

	while (1) {
		sock_fd = accept (sock_id, NULL, NULL); /* wait for call */
			printf("Wow! got a call!\n");
		if( sock_fd == -1 )
			oops( "accept" );	/* error getting calls */

		sock_fp = fdopen( sock_fd, "w");/* we'll write to the	*/
		if( sock_fp == NULL )		/* socket as a sream	*/
			oops ("fdopen" );	/* unless we can't	*/
		c1Num=newCard();
                c2Num=newCard();

		/* and convert to string */
		printf("접속되었습니다.\n게임을 시작하겠습니다.\n(카드나눠주기. 나눠주는 코드 쓰면  이 괄호는 삭제하기!)\n");
		fprintf( sock_fp, "접속되었습니다.\n게임을 시작하겠습니다.\n(카드나눠주기. 나눠주는 코드 쓰면 이 괄호는 삭제하기!)" );
		
		fprintf( sock_fp, "상대방의 카드는 %d입니다. 몇개의 칩을 배팅하실 건가요? 후후\n",c2Num);
		printf("상대방의 카드는 %d입니다. 상대가 배팅칩을 고르고 있습니다.\n",c1Num);
		
		//칩 입력받아서 처리하기
		//
		//

		fclose( sock_fp );		/* release connection	*/
	}
}
int newCard()
{
	int num=0;
	num=rand()%9+1;//num=1로 하면 잘 돌아가는데 random으로 하면 뒷부분이 안나옴 ㅠㅠ 왜지.. 여튼! rand 함수 쓰려면 헤더파일 정의해야하는데 그것 좀 해줘! 그리고 rand함수 정리좀 해줘!
	return num;
}
