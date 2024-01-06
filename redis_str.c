#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "redis_str.h"

#define MAXSIZE 10000
//gcc -c redis_str.c -o redis_str.o

int hash(char* key) {
    unsigned long hash = 1117;
    int c;
   while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % MAXSIZE;
}

void insert(struct hash_TB_array* ptr_to_TB_array, char* key, char* value) {
    struct hash_node* ptr_to_new_hash_node = (struct hash_node*)malloc(sizeof(struct hash_node));

    ptr_to_new_hash_node->key = (char*)malloc(strlen(key) + 1);
    strcpy(ptr_to_new_hash_node->key, key);

    ptr_to_new_hash_node->value = (char*)malloc(strlen(value) + 1);
    strcpy(ptr_to_new_hash_node->value, value);

    int index = hash(key);
   // 為了處理碰撞問題，Array存放的Pointer會指向Linked list的開頭，在往後接下去成文linked list
    ptr_to_new_hash_node->ptr_to_next_hash_node = ptr_to_TB_array->TB_array[index];
    ptr_to_TB_array->TB_array[index] = ptr_to_new_hash_node;
}

char* get_value(struct hash_TB_array* ptr_to_TB_array,char* key){
    int index=hash(key);// 使用哈希函數計算鍵的索引位置
    struct hash_node* cur_node=ptr_to_TB_array->TB_array[index];// 取得該索引位置的鍵值對鏈表的第一個節點
    while(1){
        if(cur_node==NULL){// 如果鏈表為空，表示找不到指定的鍵值對
            break;
        }
        else{
            if(strcmp(cur_node->key,key)==0){// 找到了指定的鍵值對
                return cur_node->value;// 返回該鍵值對的值
            }
            else{// 如果當前節點不是要找的節點，繼續搜索下一個節點
                cur_node=cur_node->ptr_to_next_hash_node;
            }
        }
    }
    return NULL;
}
void updata(struct hash_TB_array* ptr_to_TB_array, char* key, char* value) {
    int index = hash(key);
    struct hash_node* cur_node = ptr_to_TB_array->TB_array[index];
    if (cur_node == NULL) {
        printf("The Key %s doesn't exist ", key);
        return;
    }
    while (1) {
        if (strcmp(cur_node->key, key) == 0) {
            free(cur_node->value); 
            cur_node->value = (char*)malloc(strlen(value) + 1);
            strcpy(cur_node->value, value);
            printf("Successfully updated\n");
            break;
        } else if (cur_node->ptr_to_next_hash_node == NULL) {
            break;
        } else {
            cur_node = cur_node->ptr_to_next_hash_node;
        }
    }
}

void delete(struct hash_TB_array* ptr_to_TB_array,char* key){
    int index=hash(key);
    struct hash_node* cur_node=ptr_to_TB_array->TB_array[index];
    struct hash_node* prev_node=NULL;
    while(1){
        if(cur_node==NULL){
            printf("We didn't find the value to %s \n",key);
            break;
        }
        else{
            if(strcmp(cur_node->key,key)==0){//找到了
                if(prev_node!=NULL){
                    prev_node->ptr_to_next_hash_node=cur_node->ptr_to_next_hash_node;
                    free(cur_node);
                }
                else{//要刪除的在linked list的開頭
                    // cur_node->ptr_to_next_hash_node=NULL;
                    ptr_to_TB_array->TB_array[index] = cur_node->ptr_to_next_hash_node;
                    cur_node->key=NULL;
                    cur_node->value=NULL;
                    cur_node=NULL;
                    free(cur_node);
                }
                printf("Successfully delete the value to %s \n",key);
                break;
            }
            else{
                prev_node=cur_node;
                cur_node=cur_node->ptr_to_next_hash_node;//沒有就往下一個找
            }
        }
    }
}

void show_all_pairs(struct hash_TB_array* ptr_to_TB_array) {
    printf("All the key-pairs\n");
    int i;
    for (i = 0; i < MAXSIZE; i++) {
        struct hash_node* cur_node = ptr_to_TB_array->TB_array[i];
        // printf("%s %s\n", cur_node->key, cur_node->value);
        while (cur_node != NULL) {
            printf("%s %s\n", cur_node->key, cur_node->value);
            cur_node = cur_node->ptr_to_next_hash_node;
        }
    }
}

void handle_input_HW1(){
    struct hash_TB_array* myHashTable = (struct hash_TB_array*)malloc(sizeof(struct hash_TB_array));
    int i;
    for(i=0;i<MAXSIZE;i++){
        myHashTable->TB_array[i]=NULL;
    }
    char buffer[1024];
    while(1){
        printf("Enter the operation\nRedis list Operation(LPUSH,LPOP,RPUSH,RPOP,LLEN,LRANGE)\nRedis String Operation(SET,GET,SHOW,DEL)\n");
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            buffer[strcspn(buffer, "\n")] = '\0';// 去掉換行符
            char command[10];
            
            if (sscanf(buffer, "%s", command) == 1) {// 使用 sscanf 解析命令
                char* current_position = buffer; // 使用指標來追蹤位置
                if(strcmp(command,"SET")==0||strcmp(command,"GET")==0||strcmp(command,"SHOW")==0||strcmp(command,"DEL")==0){
                    current_position+=strlen(command);
                    char key[100],value[100];
                    if(strcmp(command,"SET")==0){
                        sscanf(current_position, "%s %s", key, value);
                        //printf("the given key %s value %s",key, value);
                        insert(myHashTable,key,value);
                    }
                    else if(strcmp(command,"GET")==0){
                        sscanf(current_position, "%s", key);
                        //printf("the given key %s ",key);
                        printf("The value to %s is %s\n",key,get_value(myHashTable,key));
                    }
                    else if(strcmp(command,"DEL")==0){
                        sscanf(current_position, "%s", key);
                        delete(myHashTable,key);
                    }
                    else{
                        show_all_pairs(myHashTable);
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

