#include "config.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


char buf[BUFSIZ];

void send_string(int d, char* s) {	// 문자열을 받아서 buf에 저장한 후 송신
    strcpy(buf, s);
    if (send(d, buf, sizeof(buf), 0) == -1) {
        perror("send");
        exit(1);
    }
}

void recv_string(int d) {	// buf에 내용 수신
    if (recv(d, buf, sizeof(buf), 0) == -1) {	
        perror("recv");
        exit(1);
    }
}

int main() {
    int sd, len;
    struct sockaddr_un ser;

    if ((sd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {	// 소켓 파일 기술자 생성, 유닉스 도메인 소켓
        perror("socket");
        exit(1);
    }

    memset((char *)&ser, '\0', sizeof(ser));	// 구조체의 모든 값을 0으로 초기화
    ser.sun_family = AF_UNIX;
    strcpy(ser.sun_path, SOCKET_NAME);
    len = sizeof(ser.sun_family) + strlen(ser.sun_path);

    if (connect(sd, (struct sockaddr *)&ser, len) < 0) {	// 클라이언트가 서버에 접속 요청
        perror("bind");
        exit(1);
    }
    while (1) {
        recv_string(sd);
        if (strcmp("i", buf) == 0) {	// buf에 수신한 내용이 i일 경우 사용자에게 입력을 받음
            printf("-->> ");
            scanf("%s", buf);		// 입력 받기
            send_string(sd, buf); 	// buf 내용 송신
        } 
        else if (strcmp("q", buf) == 0 || strcmp("Q", buf) == 0) {	// buf에 수신한 내용이 q일 경우 while 루프에서 빠져나감
            break;
        }
        else {	// buf 내용 출력
            printf("%s\n", buf);
        }
    }
    
    close(sd);		// 소켓 파일 기술자 종료
}
