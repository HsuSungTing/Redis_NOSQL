#ifndef REDIS_STR_H
#define REDIS_STR_H

#define MAXSIZE 10000

struct hash_node {
    char* key;
    char* value;
    struct hash_node* ptr_to_next_hash_node;
};

struct hash_TB_array {
    struct hash_node* TB_array[MAXSIZE];
};

void insert(struct hash_TB_array* ptr_to_TB_array, char* key, char* value);
char* get_value(struct hash_TB_array* ptr_to_TB_array, char* key);
void updata(struct hash_TB_array* ptr_to_TB_array, char* key, char* value);
void delete(struct hash_TB_array* ptr_to_TB_array, char* key);
void show_all_pairs(struct hash_TB_array* ptr_to_TB_array);
void handle_input_HW1();

#endif
