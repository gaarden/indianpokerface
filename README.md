# System Programming Team Project 8조

## 1. Indian Poker~~face~~ Project

### Topic

> 리눅스 환경에서 소켓 프로그래밍을 이용한 인디언 포커 게임
> 

### Game Rule

- 플레이어에게 카드를 한장씩 나누어 줍니다.
- 상대방의 카드는 볼 수 있지만 본인의 카드는 볼 수 없습니다.
- 칩은 20개씩 제공됩니다.
- 첫 라운드 선플레이어는 클라이언트입니다. 이후 해당 라운드 패자가 다음 라운드의 선플레이어 입니다.
- 상대의 카드를 보고 내 카드를 예측하여 칩을 베팅합니다.
- 기본 베팅은 매 라운드마다 칩 1개입니다.
- 더 큰 수의 카드를 가진 플레이어가 승리합니다.
- 한 플레이어의 보유칩이 0개가 될 때까지 게임을 반복합니다.
- 보유칩이 0개가 된 플레이어가 패배합니다.



## 2. Team

### Team Member

- 컴퓨터학부 20학번 김은정
- 컴퓨터학부 20학번 문정원
- 컴퓨터학부 20학번 엄지영
- 컴퓨터학부 20학번 정다혜

### Member's Role

**PM**: 김은정

**개발자**: 김은정, 문정원, 엄지영, 정다혜

**아키텍트**: 엄지영, 정다혜

**운영자**: 문정원



## 3. Program

### 프로그램 구조도
![Untitled](https://user-images.githubusercontent.com/94179998/145717695-7191953a-3bf2-4cfc-a9d0-e99f3a5cacd9.png)


### 프로그램 실행 방법

- Makefile을 이용하여 컴파일

- 서버
    - 코드 컴파일
    - cc -o poker_s poker_s.c -lcurses
    - ./poker_s <PORT 번호>
- 클라이언트
    - 코드 컴파일
    - cc -o poker_c poker_c.c -lcurses
    - ./poker_c <IP 주소> <PORT 번호>

- **테스트 서버**
    - IP 주소 : 192.168.56.101
    - PORT 번호 : 13876


### 프로그램 실행 화면

**서버**

1. 인디언 포커 게임 룰을 설명합니다.
    <img width="682" alt="스크린샷_2021-12-12_오후_8 11 58" src="https://user-images.githubusercontent.com/94179998/145717671-95f3fb0c-c59a-4ad3-95c0-170fdc32cb9f.png">
    
2. 이름을 입력한 후 클라이언트의 접속을 기다립니다.
    <img width="682" alt="스크린샷_2021-12-12_오후_9 08 06" src="https://user-images.githubusercontent.com/94179998/145717705-9cf544d3-8930-4b05-a66f-1bcb01e586a8.png">
    
3. 클라이언트가 접속한다면 게임을 실행합니다.
서버는 후플레이어이므로 클라이언트가 베팅한 후 베팅합니다.
    <img width="682" alt="스크린샷_2021-12-12_오후_9 09 53" src="https://user-images.githubusercontent.com/94179998/145717907-23883606-2110-45bb-9aee-0852431de58b.png">
    <img width="682" alt="스크린샷_2021-12-12_오후_9 16 27" src="https://user-images.githubusercontent.com/94179998/145718113-23afdcd0-c498-4833-aa85-ab317bb0e5a0.png">

    
4. 승자는 베팅 칩을 가져가고 패자는 다음 라운드 선플레이어가 됩니다.
5. 칩이 0개가 될 때까지 게임을 반복합니다.
    <img width="638" alt="스크린샷_2021-12-12_오후_9 17 13" src="https://user-images.githubusercontent.com/94179998/145717732-ec23a9db-6050-4c73-8dfd-92fe4a665cc9.png">




**클라이언트**

1. 인디언 포커 게임 룰을 설명합니다.
    <img width="682" alt="스크린샷_2021-12-12_오후_8 11 58" src="https://user-images.githubusercontent.com/94179998/145717766-f2689d21-f0b1-4a0b-a901-9cc70531a84f.png">
    
2. 이름을 입력합니다.

  <img width="682" alt="스크린샷_2021-12-12_오후_9 09 27" src="https://user-images.githubusercontent.com/94179998/145718207-53a5496f-46c3-4f46-91ef-b92b8df4ee08.png">


3. 게임을 실행합니다.
클라이언트가 선플레이어이므로 먼저 베팅합니다.
    <img width="682" alt="스크린샷_2021-12-12_오후_9 10 19" src="https://user-images.githubusercontent.com/94179998/145717792-7ddc8bd5-0116-4a32-ba13-1ba5d0e8abd6.png">

    <img width="682" alt="스크린샷_2021-12-12_오후_9 16 32" src="https://user-images.githubusercontent.com/94179998/145717797-f7439818-8f55-4601-a67f-991326b7757b.png">
  
4. 승자는 베팅 칩을 가져가고 패자는 다음 라운드 선플레이어가 됩니다.
5. 칩이 0개가 될 때까지 게임을 반복합니다.
    <img width="638" alt="스크린샷_2021-12-12_오후_9 16 57" src="https://user-images.githubusercontent.com/94179998/145717813-a6375623-1d49-42cb-8fe4-921228282b03.png">



