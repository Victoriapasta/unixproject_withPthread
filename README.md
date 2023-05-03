# unixproject_withPthread

## 라커를 관리하는 Client/Server Socket 프로젝트

> 지난번 완성했던 프로젝트에서 부족함을 느껴서 쓰레드를 이용해서 리빌딩했다.

### 수정점

- 파일 디스크립터를 이용하지 않고, 쓰레드를 이용했다.
- 여전히 다중 접속 기능을 유지중이다.
- 디테일 추가, 최적화 완료

### Compile
> gcc -o server server.c -lpthread

> gcc -o client client.c -lpthread

### execute
> ./server

> ./client
