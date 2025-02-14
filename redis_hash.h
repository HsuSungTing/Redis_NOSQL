#ifndef REDIS_HASH_H
#define REDIS_HASH_H
#include <ev.h>

#define MAXHASHTABLESIZE 10000
#define MAX_FIELDS 20
//gcc -o test test.c -lev
struct field_node{
    char* key;
    char * field;
    char * value;
    struct field_node* next_field_node;
};
struct hash_node_HW6{
    char* key;
    struct field_node* field_node_head;
    struct hash_node_HW6* ptr_to_next_hash_node;
};


struct hash_TB_array_HW6 {
    struct hash_node_HW6* TB_array[MAXHASHTABLESIZE];//這邊存放的會是第一個hash node
};
//-------------------------
struct delayed_data {
    struct hash_TB_array_HW6 *myHashTable;
    char *key;
};

int hash_func(char* key) ;
void insert_field(struct hash_TB_array_HW6* ptr_to_TB_array, char* key,char* field_name,char* value);
void delete_field(struct hash_TB_array_HW6* ptr_to_TB_array, char* key,char* field_name);
void delete_key(struct hash_TB_array_HW6* ptr_to_TB_array, char* key);
void show_hash_pairs(struct hash_TB_array_HW6* ptr_to_TB_array);
char* get(struct hash_TB_array_HW6* ptr_to_TB_array, char* key,char* field_name);
void delayed_delete(EV_P_ ev_timer *w, int revents);
void stdin_cb(EV_P_ ev_io *w, int revents);
void handle_input_HW6();
#endif