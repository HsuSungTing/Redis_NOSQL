#ifndef REDIS_DLL_H
#define REDIS_DLL_H

//gcc -o DLL Double_linked_list.c redis_str.o
struct Node {
    char* key;
    char* value;
    struct Node* prev;
    struct Node* next;
};

struct doubly_linked_list{
    struct Node* head;
    struct Node* tail;
};

struct Node* createNode(char* key, char* value);
void insert_at_head(struct doubly_linked_list* ptr_to_DL_list, char* key, char* value);
void insert_at_tail(struct doubly_linked_list* ptr_to_DL_list, char* key, char* value);
void delete_at_head(struct doubly_linked_list* ptr_to_DL_list);
void delete_at_tail(struct doubly_linked_list* ptr_to_DL_list);
void print_from_left(struct doubly_linked_list* ptr_to_DL_list);
int get_len(struct doubly_linked_list* ptr_to_DL_list);
void handle_input_HW3();
#endif