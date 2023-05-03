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

int small_locker_size, big_locker_size, total_locker_size; // 서버로부터 받을 라커 사이즈들
int status, send_message, want_ID, canUse, wantPW, rePW, password, correct_pw;
char using;

int main() {
    int clientfd, result;
    char inmsg[MAXLINE], outmsg[MAXLINE];
    struct sockaddr_un serverUNIXaddr;
    
    clientfd = socket(AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    serverUNIXaddr.sun_family = AF_UNIX;
    strcpy(serverUNIXaddr.sun_path, "convert");

    do { /* 연결 요청 */
    result = connect(clientfd, &serverUNIXaddr, sizeof(serverUNIXaddr));
    if (result == -1) sleep(1);
    } while (result == -1);

    read(clientfd, &small_locker_size, sizeof(int));
    read(clientfd, &big_locker_size, sizeof(int));
    total_locker_size = small_locker_size + big_locker_size;

    for(int i=1; i<=small_locker_size; i++) { // small size locker의 이용여부 상태 등 출력
        read(clientfd, &using, 1);
        status = using - '0';
        if(status == 1) {
            printf("|      Locker ID : %d        |       Used :     Yes      |\n",i);
            printf("ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ\n");
        }
        else {
            printf("|      Locker ID : %d        |       Used :     No      |\n",i);
            printf("ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ\n");
        }
    }
    for(int i = small_locker_size+1; i <= total_locker_size; i++) { // big size locker 의 이용여부 상태 등 출력
        read(clientfd, &using, 1);
        status = using - '0';
        if(status == 1) {
            printf("|      Big Locker ID : %d    |       Used :     Yes     |\n",i);
            printf("ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ\n");
        }
        else {
            printf("|      Big Locker ID : %d    |       Used :     No      |\n",i);
            printf("ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ\n");
        }
    }
    while (1) {
        printf("1번은 사물함 선택, 2번은 사물함 이용입니다. ");
        scanf("%d", &send_message);
        write(clientfd, &send_message, sizeof(int));

        if(send_message == 1) { // 사물함 선택(등록)
            printf("\n이용을 원하는 사물함의 번호를 입력하세요 : ");
            scanf("%d", &want_ID);
            write(clientfd, &want_ID, sizeof(int));
            read(clientfd, &canUse, sizeof(int));

            if(canUse == 1) { //사용가능하다는 신호를 받음
                printf("%d번 사물함을 이용하려면 비밀번호를 설정하십시오. : ", want_ID);
                scanf("%d", &wantPW);

                while(1) { //비밀번호 재확인
                    printf("비밀번호 확인을위해 재입력 해주십시오. : ");
                    scanf("%d", &rePW);
                    if(wantPW == rePW) break;
                    sleep(1);
                    printf("설정하신 비밀번호와 일치하지 않습니다. ");
                }

                write(clientfd, &wantPW, sizeof(wantPW)); //최종적으로 승인된 비밀번호 저장
            }
            else {
                printf("해당 사물함은 이용중 입니다. 초기 화면으로 돌아갑니다. ");
            }
        }
        else if(send_message == 2) { //사물함 이용모드 (물건 넣고빼기)
            printf("\n이용을 원하는 사물함의 번호를 입력하세요. : ");
            scanf("%d", &want_ID);
            write(clientfd, &want_ID, sizeof(int));
            read(clientfd, &canUse, sizeof(int));
            if (canUse == 1) {
                printf("해당 사물함의 비밀번호를 입력하십시오 : ");
                while(1) { 
                    scanf("%d", &password);
                    write(clientfd, &password, sizeof(password));
                    read(clientfd, &correct_pw, sizeof(int));
                    if(correct_pw == 1) break;
                    printf("비밀번호가 틀립니다. 재입력해주세요: ");
                }
                
            }
            else {
                printf("해당 사물함은 등록되지 않았습니다. 초기화면으로 돌아갑니다. ");
            }
        }
    }
    close(clientfd);
    exit(0);
}