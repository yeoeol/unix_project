#include "data.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/un.h>

/* customer 기능 */
void send_string(int d, char *s);	// 문자열을 받아서 buf에 저장한 후 송신
void recv_string(int d);		// buf에 내용 수신
void sell(int d);			// 물품 판매를 위한 일련의 과정을 수행하는 함수
void get_file_content(const char *filename, Data *data);	// 파일의 내용을 구조체의 값으로 저장
void print_productList(int d);	// product 목록 출력
void choice_product(int d);		// product 선택
void buy(int d);			// 사용자가 구매한 물품의 개수를 재고에서 빼주고 데이터를 저장함
int again_choice_product(int d);	// 다른 물품을 구매할 것인지 질문하는 함수
int check_buy(int d);			// 해당 물품을 팔 수 있는지 확인하는 함수
void quit();				// SIGINT 시그널을 받았을 때 실행되는 handler

/* manager 기능 */
int manager_or_customer(int d);	// 현재 프로세스(사용자)가 manager인지 customer인지 확인하는 함수
int manage(int d);			
void print_manageList(int d);		// Manager가 수행할 수 있는 기능들을 출력하고 선택된 기능을 수행하는 함수를 호출함
void add_product(int d);		// 물품 정보를 입력받아서 구조체에 저장하고, 파일에도 저장하는 함수
void add_stock(int d);			// 원래 있던 물품들 중에 선택하여 재고를 추가하는 함수


char buf[BUFSIZ];
Data *item;		// 물품에 대한 정보를 저장할 구조체 타입 Data의 포인터 변수 선언
int num;		// 사용자가 선택한 물품의 인덱스
int shmid;	// 공유 메모리 식별자 변수
int amount;	// 사용자가 선택한 물품의 개수
int sd, nsd;	// 소켓 파일 기술자 변수

void send_string(int d, char* s) {
    strcpy(buf, s);
    if (send(d, buf, sizeof(buf), 0) == -1) {
        perror("send");
        exit(1);
    }
} 

void recv_string(int d) {
    if (recv(d, buf, sizeof(buf), 0) == -1) {
        perror("recv");
        exit(1);
    }
}

void sell(int d) {
    print_productList(d);    
    choice_product(d);
    int is_again = check_buy(d);
    item[num].is_buying = 0;	// 다른 사용자가 접근할 수 있게 해줌
    save_data(FILE_NAME, item);
    if (is_again == 1) {	// 다시 물품 판매를 위한 함수(sell) 실행
    	sell(d);
    }
    else {
        send_string(d, "와주셔서 감사합니다. 다음에 또 이용해주세요.");
        send_string(d, "q");	// q를 보냄으로써 클라이언트와의 연결 끊기
        close(d);
    }
}

void get_file_content(const char *filename, Data *data) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("파일을 열 수 없습니다.");
        exit(1);
    }

    char line[256], *token;
    int idx;
    total_cnt = 0;

    while (fgets(line, sizeof(line), file)) {
        if (total_cnt >= 30) {
            fprintf(stderr, "데이터 가져오는 중 오류 발생\n");
            break;
        }

        idx = 0;
        token = strtok(line, " ");
        while (token != NULL) {
            if (idx == 0) {
                strcpy(data[total_cnt].name, token);
            }
            else if (idx == 1) {
                strcpy(data[total_cnt].price, token);
            }
            else if (idx == 2) {
                data[total_cnt].stock = atoi(token);
            }
            else if (idx == 3) {
                data[total_cnt].is_sold = atoi(token);
            } 
            else if (idx == 4) {
                data[total_cnt].is_buying = 0;
            }
            idx++;
            token = strtok(NULL, " ");
        }
        total_cnt++;
    }
    fclose(file);
    printf("File: 파일 데이터 가져오기 성공\n");
}

void print_productList(int d) {
    get_file_content(FILE_NAME, item);
    for (int i = 0; i < total_cnt; i++) {
        sprintf(buf, "%d. %s, %s원", i+1, item[i].name, item[i].price);
        send_string(d, buf);
    }
}    

void choice_product(int d) {
    send_string(d, "\n무엇을 주문하시겠습니까? (번호를 입력하세요)");
    send_string(d, "i");
    recv_string(d);

    num = atoi(buf)-1;
    while (num < 0 || num > total_cnt) {	// 인덱스 범위가 음수이거나 물품의 총 개수보다 크다면 반복
        send_string(d, "번호를 다시 입력해주세요.");
        send_string(d, "i");
        recv_string(d);
        num = atoi(buf)-1;
    }
    while (item[num].is_buying == 1) {	// 이미 다른 사용자가 접근 중이라면 반복
        send_string(d, "=============== 이 물품은 현재 다른 사용자가 접근 중입니다. 잠시만 기다려주세요. ===============");
        sleep(2);
    }
    item[num].is_buying = 1;	// 자신이 접근하고 있음을 알리기 위해 1로 설정
    printf("log: %d 사용자가 %d번 %s을(를) 선택했습니다. %s 재고:%d\n", (int)getpid(), num+1, item[num].name, item[num].name, item[num].stock);
}

void buy(int d) {	// 재고를 수정하고 파일에 저장하는 함수
    item[num].stock -= amount;
    if (item[num].stock == 0) {
    	item[num].is_sold = 1;
    }
    save_data(FILE_NAME, item);
    printf("log: %s %d개 판매 완료. 남은 재고: %d\n", item[num].name, amount, item[num].stock);
    sprintf(buf, "%s %d개 구매 완료되었습니다.\n", item[num].name, amount);
    send_string(d, buf);
}

int again_choice_product(int d) {	// 물품 구매 의사가 있는지 질의하는 함수
    send_string(d, "\n다른 물품을 구매하시겠습니까? (y/n)");
    send_string(d, "i");
    recv_string(d);
    
    if (strcmp(buf, "y") == 0) {
        return 1;
    }
    else if (strcmp(buf, "n") == 0) {
        return 0;
    }
}

int check_buy(int d) {		// 물품을 살 의향이 있는지 확인하는 함수
    int is_again, again_ask = 0;
    int is_sold_out = item[num].is_sold;
    if (is_sold_out == 1) {	// 재고가 품절 상태라면 실행
        send_string(d, "재고가 없습니다.");
        again_ask = 1;
    }
    else {
        sprintf(buf, "%s의 개당 가격은 %s입니다. 수량을 입력하세요.", item[num].name, item[num].price);
        send_string(d, buf);
        send_string(d, "i");
        recv_string(d);

        amount = atoi(buf);
        while (amount > item[num].stock) {	// 사용자가 입력한 물품의 개수가 현재 재고보다 많다면 다시 입력받음
            sprintf(buf, "요구량만큼의 재고가 없습니다. 개수를 다시 입력하세요. (재고:%d)", item[num].stock);
            send_string(d, buf);
            send_string(d, "i");
            recv_string(d);
            amount = atoi(buf);
        }
        sprintf(buf, "총 %d원입니다. 구매하시겠습니까? (y/n)", amount*atoi(item[num].price));
        send_string(d, buf);
        send_string(d, "i");
        recv_string(d);
        if (strcmp(buf, "n") == 0) {
            again_ask = 1;
        }
    }
    
    if (again_ask == 1) {
    	is_again = again_choice_product(d);	// 물품 구매 의사가 있는지 확인한 후 변수(is_again)에 결괏값 저장
    } else {
        buy(d);	// 물품 구매
    }
    return is_again;
}

void quit() {		// SIGINT 시그널을 받았을 때 수행할 handler
    save_data(FILE_NAME, item);
    sleep(1);	// 데이터가 안전하게 저장될 수 있도록 지연시간을 줌
    shmdt(item);
    shmctl(shmid, IPC_RMID, NULL);
    
    printf("log: 시스템 종료\n");
    send_string(nsd, "q");
    
    close(sd);
    sleep(1);
    kill(getppid(), SIGQUIT);

    exit(1);
}    

int manager_or_customer(int d) {	// manager라면 0, customer라면 1 반환
    send_string(d, "*** Manager라면 0, customer라면 1을 입력하세요. ***");
    send_string(d, "i");
    recv_string(d);
    if (strcmp(buf, "0") == 0) {
    	return 0;
    }
    else {
        return 1;
    }
}

int manage(int d) {
    print_manageList(d);
}

void print_manageList(int d) {	// 매니저 기능을 나열하고 호출하는 함수
    send_string(d, "** Manager 메뉴 **");
    send_string(d, "1. 재고 추가");
    send_string(d, "2. 물품 추가");
    send_string(d, "i");
    recv_string(d);
    
    if (strcmp(buf, "1") == 0) {
        printf("log: Manager %d: 재고 추가 중...\n", (int)getpid());
        add_stock(d);
    }
    else if (strcmp(buf, "2") == 0) {
        printf("log: Manager %d: 물품 추가 중...\n", (int)getpid());
        add_product(d);
    }
}   

void add_product(int d) {	// 물품 추가
    send_string(d, "추가할 물품의 이름을 입력하세요.");
    send_string(d, "i");
    recv_string(d);
    strcpy(item[total_cnt].name, buf);
    
    send_string(d, "추가할 물품의 가격을 입력하세요.");
    send_string(d, "i");
    recv_string(d);
    strcpy(item[total_cnt].price, buf);
    
    send_string(d, "추가할 물품의 재고를 입력하세요.");
    send_string(d, "i");
    recv_string(d);
    item[total_cnt].stock = atoi(buf);
    if (item[total_cnt].stock == 0) {
        item[total_cnt].is_sold = 1;
    }
    else {
        item[total_cnt].is_sold = 0;
    }
    item[total_cnt].is_buying = 0;
    
    sprintf(buf, "추가된 물품 -> 이름: %s, 가격: %s, 재고: %d", item[total_cnt].name, item[total_cnt].price, item[total_cnt].stock);
    send_string(d, buf);
    total_cnt++;
    save_data(FILE_NAME, item);
    
    printf("=============== 물품 추가 완료 ===============\n");
    send_string(d, "=============== 물품 추가 완료 ===============");
    send_string(d, "q");
    close(d);
}

void add_stock(int d) {	// 재고 추가
    int n;
    
    print_productList(d);
    send_string(d, "i");
    recv_string(d);
    
    num = atoi(buf)-1;
    while (num < 0 || num > total_cnt) {
        send_string(d, "번호를 다시 입력하세요.");
        send_string(d, "i");
        recv_string(d);
        num = atoi(buf)-1;
    }
    sprintf(buf, "%s의 현재 재고: %d", item[num].name, item[num].stock);
    send_string(d, buf);
    send_string(d, "추가할 재고의 개수를 입력하세요.");
    send_string(d, "i");
    recv_string(d);
    
    n = atoi(buf);
    item[num].stock += n;
    if (item[num].stock > 0) {
        item[num].is_sold = 0;
    }
    save_data(FILE_NAME, item);
    
    sprintf(buf, "추가한 후, %s의 재고: %d", item[num].name, item[num].stock);
    send_string(d, buf);
    
    printf("=============== 재고 추가 완료 ===============\n");
    send_string(d, "=============== 재고 추가 완료 ===============");
    send_string(d, "q");
    close(d);
}

int main() {
    struct sockaddr_un ser, cli;
    int len, clen;
    
    key_t key = ftok("shmfile", 100);	// 키 생성
    void (* hand)(int);
    hand = signal(SIGINT, quit);	// SIGINT 시그널 핸들러 지정
    if (hand == SIG_ERR) {
        perror("signal");
        exit(1);
    }

    shmid = shmget(key, sizeof(Data) * MAX_SIZE, IPC_CREAT | 0644);	// 공유 메모리 식별자 생성
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }
    item = shmat(shmid, NULL, 0);	// 공유 메모리 연결
    total_cnt = 0;	// 전체 물품 개수
    init_data(FILE_NAME, item); 	// 데이터 초기화

    if ((sd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {	// 소켓 파일 기술자 생성, 유닉스 도메인 소켓
        perror("socket");
        exit(1);
    }

    unlink(SOCKET_NAME);	// 이미 파일이 존재한다면 지우기
    memset((char *)&ser, 0, sizeof(struct sockaddr_un));	// 구조체의 모든 값을 0으로 초기화
    ser.sun_family = AF_UNIX;
    strcpy(ser.sun_path, SOCKET_NAME);
    len = sizeof(ser.sun_family) + strlen(ser.sun_path);

    if (bind(sd, (struct sockaddr *)&ser, len)) {	// 지정된 소켓 기술자와 결합
        perror("bind");
        exit(1);
    }

    if (listen(sd, 5) < 0) {	// 클라이언트의 연결 요청 대기
        perror("listen");
        exit(1);
    }

    printf("☕ 후니커피 Server Log Start:\n");
    while (1) {
        if ((nsd = accept(sd, (struct sockaddr *)&cli, &clen)) == -1) {	// 클라이언트의 연결 요청 수락
            perror("accept");
            exit(1);
        }
        switch (fork()) {	// 실제 로직은 자식 프로세스를 생성해서 처리함
            case -1:    /* fork failed */
                perror("fork");
                exit(1);
            case 0:     /* child process */
                item = shmat(shmid, NULL, 0);
                send_string(nsd, "☕ 어서오세요. 후니커피입니다. ☕");
                int moc = manager_or_customer(nsd);
                if (moc == 0) {	// 사용자가 0을 입력했다면 Manager 기능을 사용
                    printf("*** Manager 기능 사용 중 ***\n");
                    manage(nsd);
                }
                else {			// 사용자가 1을 입력했다면 Customer 기능을 사용
                    printf("*** Customer 기능 사용 중 ***\n");
                    sell(nsd);
                }
                shmdt(item);	// 공유 메모리 연결 해제
                break;
        }
    }
    close(nsd);	// 소켓 파일 기술자 종료
    close(sd);		// 소켓 파일 기술자 종료
}
