#ifndef INIT_DATA_H
#define INIT_DATA_H

extern int total_cnt;

typedef struct Data {
    char name[20];
    char price[20];
    int stock;
    int is_sold;
    int is_buying;
} Data;

void init_data(const char *filename, Data *data);
void save_data(const char *filename, Data *data);

#endif
