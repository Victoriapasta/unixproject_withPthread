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
#define DEFAULT_PROTOCOL 0
#define MAXLINE 100

struct locker {
    int id; // 사물함 번호
    int used; // 사물함 이용여부
    int PW[10]; // 사물함 비밀번호
    char yesorno[10];
    int space; // 사물함 남은 공간
    int bigspace; // 큰 사물함 남은 공간
};