#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include "locker.h"
#define DEFAULT_PROTOCOL 0
#define MAXLINE 100

struct locker *locker;
int small_locker_size, big_locker_size, total_locker_size, want_ID, canUse, setPW, password, correct_pw;
char using;

void *main_function(void *param);
void print_locker(struct locker *locker);

int main() {
  pthread_t thread;

  int listenfd, connfd, clientlen;
  char inmsg[MAXLINE], outmsg[MAXLINE];
  struct sockaddr_un serverUNIXaddr, clientUNIXaddr;

  signal(SIGCHLD, SIG_IGN);
  clientlen = sizeof(clientUNIXaddr);

  listenfd = socket(AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
  serverUNIXaddr.sun_family = AF_UNIX;
  strcpy(serverUNIXaddr.sun_path, "convert");
  unlink("convert");
  bind(listenfd, &serverUNIXaddr, sizeof(serverUNIXaddr));

  listen(listenfd, 5);

  //사물함 개수 설정
  printf("작은 사물함 개수를 설정하세요 :\n");
  scanf("%d", &small_locker_size);
  printf("큰 사물함 개수를 설정하세요: \n");
  scanf("%d", &big_locker_size);

  total_locker_size = small_locker_size + big_locker_size;
  locker = (struct locker*)malloc((total_locker_size+1)*sizeof(struct locker));

  printf("|------------------- 초기 사물함 정보 --------------------|\n"); // 초기 사물함 상태출력
  for(int i=1; i<=small_locker_size; i++) {
      locker[i].id = i;
      locker[i].used = 0; // 처음엔 이용중이 아님
      locker[i].space = 10; // 작은 사물함은 크기가 10
      strcpy(locker[i].PW, "0000");
      printf("|      locker ID : %d        |       Used :      No        |\n",i);
  }
  for(int i = small_locker_size+1; i<=total_locker_size; i++) {
      locker[i].id = i;
      locker[i].used = 0; // 처음엔 이용중이 아님
      locker[i].bigspace = 20; // 큰 사물함은 크기가 20
      strcpy(locker[i].PW, "0000");
      printf("|      Big locker ID : %d    |       Used :      No        |\n",i);
  }
  printf("|------------------- 초기 사물함 정보 --------------------|\n");

  while (1) { /* 소켓 연결 요청 수락 */
  clientlen = sizeof(clientUNIXaddr);
  connfd = accept(listenfd, &clientUNIXaddr, &clientlen);

  pthread_create(&thread, NULL, main_function, (void* )&connfd);

  }

  close(connfd);
  exit(0);
}

void *main_function(void *param) {
  int connfd = *((int *) param);
  int accept_message; //요청받은 명령어

  //locker 사이즈 client에 보냄
  write(connfd, &small_locker_size, sizeof(small_locker_size));
  write(connfd, &big_locker_size, sizeof(big_locker_size));

  for(int i=1; i<=small_locker_size; i++) { // small size 사용여부 보내기
    if(locker[i].used == 0) {
      using = '0'; // 현재 사용 여부
      write(connfd, &using, sizeof(using));
    }
    else {
      using = '1';
      write(connfd, &using, sizeof(using));
    }
  }

  for(int i = small_locker_size + 1; i <= total_locker_size; i++) { // big size locker 사용여부 보내기
    if(locker[i].used == 0) {
      using = '0';
      write(connfd, &using, sizeof(using));
    }
    else {
      using = '1';
      write(connfd, &using, sizeof(using));
    }
  }

  read(connfd, &accept_message, sizeof(accept_message));
  if(accept_message == 1) { // 사물함 선택모드(등록모드)
    read(connfd, &want_ID, sizeof(int));

    if(locker[want_ID].used == 0) { // 사물함 이용자가 없을 때
      canUse = 1;
      write(connfd, &canUse, sizeof(int));
      sleep(3);
      read(connfd, &setPW, sizeof(setPW));
      strcpy(locker[want_ID].PW, setPW);
      locker[want_ID].used = 1;
    }
    else { 
      canUse = 0;
      write(connfd, &canUse, sizeof(int));
    }
  }
  else if(accept_message == 2) { // 사물함 이용모드
    read(connfd, &want_ID, sizeof(int));
    if(locker[want_ID].used == 1) { // 해당 사물함 이용 가능할 때(이용자가 있을때)
      canUse = 1;
      write(connfd, &canUse, sizeof(int));
      while (1) {
        read(connfd, &password, sizeof(int));
        if(password == locker[want_ID].PW) {
          correct_pw = 1;
          write(connfd, &correct_pw, sizeof(correct_pw));
          break;
        }
        else {
          correct_pw = 0;
          write(connfd, &correct_pw, sizeof(correct_pw));
        }
      }
    }
    else {
      canUse = 0;
      write(connfd, &canUse, sizeof(int));
    }
  }


}

void print_locker(struct locker *locker) {

  printf("|------------------- 현재 사물함 정보 -------------------- |\n");
  //사물함 초기화 과정
  for(int i=1; i<=small_locker_size; i++) {

    if(locker[i].used == 0) {
      char yesno[] = "No ";
      strcpy(locker[i].yesorno, yesno);
      }  
    else {
      char yesno[] = "Yes";
      strcpy(locker[i].yesorno, yesno);
      }
  printf("|      locker ID : %d        |       Used :      %s        |\n",i, locker[i].yesorno);
  }

  for(int i=small_locker_size+1; i<=total_locker_size; i++) {

    if(locker[i].used == 0) {
        char yesno[] = "No ";
        strcpy(locker[i].yesorno, yesno);
        }  
    else {
        char yesno[] = "Yes";
        strcpy(locker[i].yesorno, yesno);
        }
    printf("|      Big locker ID : %d    |       Used :      %s        |\n",i, locker[i].yesorno);
  }
  printf("|------------------- 현재 사물함 정보 -------------------- |\n");

}