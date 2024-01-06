#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "redis_dll.h"
//gcc -o DLL Double_linked_list.c redis_str.o

struct Node* createNode(char* key, char* value) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    if (newNode) {
        newNode->key = strdup(key);
        newNode->value = strdup(value);
        newNode->prev = NULL;
        newNode->next = NULL;
    }
    return newNode;
}

void insert_at_head(struct doubly_linked_list* ptr_to_DL_list, char* key, char* value) {
    struct Node* newNode = createNode(key, value);
    if (ptr_to_DL_list->head == NULL) {//當前的DL list是空的
        ptr_to_DL_list->head = newNode;
        ptr_to_DL_list->tail = newNode;
    } else {
        newNode->next = ptr_to_DL_list->head;
        ptr_to_DL_list->head->prev = newNode;
        ptr_to_DL_list->head = newNode;
    }
}

void insert_at_tail(struct doubly_linked_list* ptr_to_DL_list, char* key, char* value) {
    struct Node* newNode = createNode(key, value);
    if(ptr_to_DL_list->tail==NULL){//當前的DL list是空的
        ptr_to_DL_list->head=newNode;
        ptr_to_DL_list->tail=newNode;
    }
    else{
        newNode->prev=ptr_to_DL_list->tail;
        ptr_to_DL_list->tail->next=newNode;
        ptr_to_DL_list->tail=newNode;
    }
}
void delete_at_head(struct doubly_linked_list* ptr_to_DL_list){
    if(ptr_to_DL_list->head){//head要不是NULL，否則當前沒東西
        printf("key %s, value %s\n",ptr_to_DL_list->head->key,ptr_to_DL_list->head->value);
        if(ptr_to_DL_list->head==ptr_to_DL_list->tail){//當前Node只剩一個
            ptr_to_DL_list->head=NULL;
            ptr_to_DL_list->tail=NULL;
            //free(ptr_to_DL_list->head);
            //free(ptr_to_DL_list->tail);
        }
        else{//當前node至少還有兩個
            ptr_to_DL_list->head->next->prev=NULL;
            ptr_to_DL_list->head=ptr_to_DL_list->head->next;
        }
    }
}
void delete_at_tail(struct doubly_linked_list* ptr_to_DL_list){
    if(ptr_to_DL_list->head){//head要不是NULL，否則當前沒東西
        printf("key %s, value %s\n",ptr_to_DL_list->tail->key,ptr_to_DL_list->tail->value);
        if(ptr_to_DL_list->head==ptr_to_DL_list->tail){//當前Node只剩一個
            ptr_to_DL_list->head=NULL;
            ptr_to_DL_list->tail=NULL;
            //free(ptr_to_DL_list->head);
            //free(ptr_to_DL_list->tail);
        }
        else{//當前node至少還有兩個
            ptr_to_DL_list->tail->prev->next=NULL;
            ptr_to_DL_list->tail=ptr_to_DL_list->tail->prev;
        }
    }
}
void print_from_left(struct doubly_linked_list* ptr_to_DL_list) {
    struct Node* cur = ptr_to_DL_list->head;
    while (cur) {
        printf("Key: %s, Value: %s\n", cur->key, cur->value);
        cur = cur->next;
    }
}
int get_len(struct doubly_linked_list* ptr_to_DL_list){
    struct Node* cur=ptr_to_DL_list->head;
    int sum=0;
    while(cur){
        sum+=1;
        cur=cur->next;
    }
    return sum;
}
void handle_input_HW3(){
    struct doubly_linked_list* ptr_to_DLL = (struct doubly_linked_list*)malloc(sizeof(struct doubly_linked_list));
    ptr_to_DLL->head = NULL;
    ptr_to_DLL->tail = NULL;
    //--------------------------------------
    char buffer[1024];
    while(1){
        printf("Enter the operation\nRedis list Operation(LPUSH,LPOP,RPUSH,RPOP,LLEN,LRANGE)\n");
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            buffer[strcspn(buffer, "\n")] = '\0';// 去掉換行符
            char command[10];
            
            if (sscanf(buffer, "%s", command) == 1) {// 使用 sscanf 解析命令
                char* current_position = buffer; // 使用指標來追蹤位置
                if (strcmp(command,"LPUSH")==0||strcmp(command, "RPUSH") == 0) {
                    current_position+=strlen(command);
                    char key[20][100], value[20][100];
                    int pair_i=0;
                    while (sscanf(current_position, "%s %s", key[pair_i], value[pair_i]) == 2) {// 連續讀取 key-value pairs
                        current_position+=strlen(key[pair_i]) + strlen(value[pair_i]) + 2; //移動指標位置 考慮空格的長度
                        pair_i++;
                    }
                    if(strcmp(command, "LPUSH") == 0){
                        int i;
                        for(i=0;i<pair_i;i++){
                            insert_at_head(ptr_to_DLL,key[i],value[i]);
                        }
                        //print_from_left(ptr_to_DLL);
                    }
                    else if(strcmp(command, "RPUSH")==0){
                        int i;
                        for(i=0;i<pair_i;i++)insert_at_tail(ptr_to_DLL,key[i],value[i]);
                    }
                }
                else if(strcmp(command,"LLEN")==0||strcmp(command,"LPOP")==0||strcmp(command,"RPOP")==0||strcmp(command,"LRANGE")==0) {
                    if(strcmp(command,"LLEN")==0){
                        printf("LLEN: %d",get_len(ptr_to_DLL));
                    }
                    else if(strcmp(command,"LPOP")==0){
                        printf("LPOP: ");
                        delete_at_head(ptr_to_DLL);
                    }
                    else if(strcmp(command,"RPOP")==0){
                        printf("RPOP: ");
                        delete_at_tail(ptr_to_DLL);
                    }
                    else{
                        print_from_left(ptr_to_DLL);
                    }
                }
                else{
                    printf("invalid command\n");
                } 
            }
        }
        printf("\n");
    }
}