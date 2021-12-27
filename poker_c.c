#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <curses.h>

#define BUF_SIZE 1024

int client_fd, len, n, n2;
int mChip = 20;
int tChip = 1;
char oChip[BUF_SIZE];
char bChip[BUF_SIZE];
char totalChip[BUF_SIZE];
char mNumc[BUF_SIZE];
int oNum = 0;
int mNum = 0;
int minbet = 0;
char oNumc[BUF_SIZE];
int win = 1;
int giveup = 0;
char giveupsignal = -1;
char name[BUF_SIZE];
int same = 0;
int notfirst = 0;
void start();
int newCard();

int main(int argc, char* argv[])
{
    struct sockaddr_in client_addr;
    char* IP = argv[1];
    in_port_t PORT = atoi(argv[2]);

    if (argc != 3)
    {
        printf("Usege : ./filename [IP] [PORT] \n");
        exit(0);
    }


    client_fd = socket(PF_INET, SOCK_STREAM, 0);

    client_addr.sin_addr.s_addr = inet_addr(IP);
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(PORT);


    if (connect(client_fd, (struct sockaddr*)&client_addr, sizeof(client_addr)) == -1)
    {
        printf("Can't connect\n");
        close(client_fd);
        return -1;

    }

    start();
    int game = 1;

    printf("이름을 입력해주세요.\n");
    fgets(name, sizeof(name), stdin);
    int namelen = strlen(name);
    name[namelen - 1] = '\0';

    printf("상대가 선플레이어입니다.\n\n");

    while (game)
    {
        printf("====================================\n");

        sleep(2);
        printf("<상대방 카드>\n");
        oNum = newCard();

        printf("보유칩: { %d }\n", mChip--);
        sleep(1);
        printf("기본베팅은 1개입니다.\n");
        printf("보유칩: { %d }\n\n", mChip);
        giveup = 0;
        minbet = 0;
        notfirst = 0;
        if (!same)
            tChip = 2;
        else
            tChip += 2;
        if (win == 1)
        {
            while (1) {
                printf("상대의 베팅을 기다리는 중입니다..\n");

                read(client_fd, oChip, BUF_SIZE); //상대가 입력한 것 oChip저장
                if (!strcmp(oChip, "-1"))
                {
                    giveup = 2;
                    break;
                }
                if (!strcmp(oChip, "-2"))
                {
                    tChip += minbet;

                    break;
                }
                printf("상대는 %d개를 베팅했습니다.\n\n", atoi(oChip));
                minbet += atoi(oChip);
                tChip += atoi(oChip);
                printf("베팅할 칩 개수를 입력하세요.%d개보다 많이 입력하셔야 합니다. die는 0개를 입력하시면 됩니다.\n",minbet);
                fgets(bChip, sizeof(bChip), stdin);
                while (atoi(bChip)==0||atoi(bChip) < minbet || mChip < atoi(bChip) || mChip+atoi(bChip)>40)
                {
                    if (atoi(bChip) == 0)//die일경우
                    {
                        giveup = 1;
                        break;
                    }
                    else if (mChip + atoi(bChip) > 40)
                    {
                        printf("상대가 남은 칩보다 더 많이 배팅하셨습니다. 최대 %d개까지 배팅할 수 있습니다.",40-mChip);
                    }
                    else if (atoi(bChip) < minbet)//더 많이 입력해야함
                    {
                        printf("배팅하시려면 %d보다 많은 칩을 배팅하셔야합니다.\n", minbet);
                    }
                    else
                    {
                        printf("보유칩보다 베팅칩이 많습니다. 베팅칩을 다시 입력해주세요.\n");
                    }
                    printf("베팅할 칩 개수를 입력하세요.\n");
                    fgets(bChip, sizeof(bChip), stdin);

                }
                if (giveup)
                {
                    //포기했으니까 상대 승으로 처리해주소 칩도 다줘~
                    strcpy(bChip, "-1");
                    send(client_fd, bChip, sizeof(bChip), 0);
                    break;
                }
                else if (minbet==atoi(bChip))//&&notfirst)
                {

                    mChip -= atoi(bChip);
                    tChip += atoi(bChip);
                    strcpy(bChip, "-2");

                    send(client_fd, bChip, sizeof(bChip), 0);
                    break;
                }
                else
                {
                    printf("%s님은 %d개를 베팅했습니다.\n\n", name, atoi(bChip));
                    minbet -= atoi(bChip);
                    mChip -= atoi(bChip);
                    tChip += atoi(bChip);
                    send(client_fd, bChip, sizeof(bChip), 0);
                    sleep(2);
                }
                notfirst = 1;
            }
        }

        else
        {
            while (1) {
                printf("베팅할 칩 개수를 입력하세요.%d개보다 많이 입력하셔야 합니다. die는 0개를 입력하시면 됩니다.\n",minbet);
                fgets(bChip, sizeof(bChip), stdin);

                while (atoi(bChip) == 0 || atoi(bChip) < minbet || mChip < atoi(bChip) || mChip + atoi(bChip)>40)
                {
                    if (atoi(bChip) == 0)//die일경우
                    {
                        giveup = 1;
                        break;
                    }
                    else if (mChip + atoi(bChip) > 40)
                    {
                        printf("상대가 남은 칩보다 더 많이 배팅하셨습니다. 최대 %d개까지 배팅할 수 있습니다.", 40 - mChip);
                    }
                    else if (atoi(bChip) < minbet)//더 많이 입력해야함
                    {
                        printf("배팅하시려면 %d보다 많은 칩을 배팅하셔야합니다.\n", minbet);
                    }
                    else
                    {
                        printf("보유칩보다 베팅칩이 많습니다. 베팅칩을 다시 입력해주세요.\n");
                    }
                    printf("베팅할 칩 개수를 입력하세요.\n");
                    fgets(bChip, sizeof(bChip), stdin);

                }
                if (giveup)
                {
                    //포기했으니까 상대 승으로 처리해주소 칩도 다줘~
                    strcpy(bChip,"-1");
                    send(client_fd, bChip, sizeof(bChip), 0);
                    break;
                }
                else if (minbet==atoi(bChip))//&&notfirst)//끝장보자
                {
                    mChip -= atoi(bChip);
                    tChip += atoi(bChip);

                    strcpy(bChip, "-2");
                    
                    send(client_fd, bChip, sizeof(bChip), 0);
                    break;
                }
                else
                {
                    printf("%s님은 %d개를 베팅하셨습니다.\n\n", name, atoi(bChip));
                    minbet -= atoi(bChip);
                    mChip -= atoi(bChip);
                    tChip += atoi(bChip);
                    send(client_fd, bChip, sizeof(bChip), 0);
                }
                notfirst = 1;
                printf("상대의 베팅을 기다리는 중입니다..\n");
                read(client_fd, oChip, BUF_SIZE);
                if (!strcmp(oChip, "-1"))
                {
                    giveup = 2;
                    break;
                }
                else if (!strcmp(oChip, "-2"))
                {

                    tChip += minbet;
                    break;
                }
                printf("상대는 %d개를 베팅했습니다.\n\n", atoi(oChip));
                minbet += atoi(oChip);
                tChip += atoi(oChip);
                sleep(2);
            }
        }

        read(client_fd, mNumc, BUF_SIZE);
        mNum = atoi(mNumc);
        printf("%s님의 카드 숫자는 %d였습니다.\n\n", name, mNum);

        sprintf(oNumc, "%d", oNum);
        send(client_fd, oNumc, sizeof(oNumc), 0);
        sleep(3);

        oNum = atoi(oNumc);
        if (giveup==1)
        {
            //mChip = mChip - atoi(bChip);

            win = 1;

            printf("%s님이 기권으로 패배하셨습니다.\n", name);

            if (mChip <= 0)
            {
                printf("%s님의의 베팅칩이 모두 소진되었습니다.\n", name);
                printf("%s님의 패배로 게임을 종료합니다.\n", name);
                close(client_fd);
                game = 0;
            }
            if (game)
                printf("%s님이 선플레이어입니다.\n\n", name);
            same = 0;
        }
        else if (giveup == 2)
        {
            mChip += tChip;

            win = 2; //이기면 후공?

            printf("%s님이 승리하셨습니다.\n", name);

            if (40 - mChip > 0)
                printf("상대가 선플레이어입니다.\n\n");
            else
                printf("%s님의 승리로 게임을 종료합니다.\n", name);
            same = 0;
        }
        else
        {

            //승패 판단 후 칩에 반영
            if (mNum > oNum)
            {
                mChip += tChip;

                win = 2; //이기면 후공?

                printf("%s님이 승리하셨습니다.\n", name);

                if (40 - mChip > 0)
                    printf("상대가 선플레이어입니다.\n\n");
                else
                    printf("%s님의 승리로 게임을 종료합니다.\n", name);
                same = 0;
            }

            else if (mNum < oNum)
            {
                //mChip = mChip - atoi(bChip);

                win = 1;

                printf("%s님이 패배하셨습니다.\n", name);

                if (mChip <= 0)
                {
                    printf("%s님의의 베팅칩이 모두 소진되었습니다.\n", name);
                    printf("%s님의 패배로 게임을 종료합니다.\n", name);
                    close(client_fd);
                    game = 0;
                }
                if (game)
                    printf("%s님이 선플레이어입니다.\n\n", name);
                same = 0;
            }

            else //무승부
            {
                same = 1;
                //mChip += 1;
                printf("무승부입니다.\n");
                printf("베팅 순서는 유지됩니다.\n\n");
            }
            if (mChip >= 40) {
                game = 0;
                close(client_fd);
            }
        }
        


    }

    //thread_stop();


    close(client_fd);

    return 0;
}



void start() {
    initscr();
    clear();
    move(0, 0);
    addstr(" /* Let's play indianpoker game */");
    move(LINES - 1, 0);
    refresh();
    move(0, 0);
    addstr("I will tell you about game's rule ^^7\n\n");
    addstr("1. We'll distribute one card each.\n");
    addstr("2. You can see each other's cards, but you can't see your own cards.\n");
    addstr("3. If you think your card is bigger than your opponent, you can bet.\n");
    addstr("4. If you're scared, you can give up.\n");
    addstr("5. If you run out of chips, you lose.\n");
    addstr("6. The one with the big card wins.\n");
    addstr("8. There are 20 chips\n");
    addstr("\n Press 'q' to start game\n");
    sleep(2);
    getch();
    endwin();
    system("clear");
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


