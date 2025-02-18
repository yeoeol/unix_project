#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int total_cnt = 0;	// 물품의 총 개수

void init_data(const char *filename, Data *data) {	// 파일을 읽기 전용으로 열고, 데이터를 읽어서 구조체에 값을 채우는 함수
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("파일을 열 수 없습니다.");
        exit(1);
    }

    char line[256], *token;
    int idx;

    while (fgets(line, sizeof(line), file)) {
        if (total_cnt >= 30) {	// 물품의 총 개수는 30개로 제한
            fprintf(stderr, "데이터 초기화 중 오류 발생\n");
            break;
        }

        idx = 0;
        token = strtok(line, " ");	// 구분자를 기준으로 문자열을 나눔
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
    fclose(file);	// 파일 닫기
    printf("*** 데이터 초기화 성공 ***\n");
}

void save_data(const char *filename, Data *data) {	// 구조체 값들을 파일에 쓰기하는 함수
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("파일을 열 수 없습니다.");
        exit(1);
    }
    for (int i = 0; i < total_cnt; i++) {
        fprintf(file, "%s %s %d %d %d\n", data[i].name, data[i].price, data[i].stock, data[i].is_sold, data[i].is_buying);
    }
    fclose(file);
}
