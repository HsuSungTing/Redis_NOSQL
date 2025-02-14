#include <stdio.h>
#include <ev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "redis_hash.h"

int hash_func(char* key) {
    unsigned long hash = 1117;
    int c;
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % MAXHASHTABLESIZE;
}


void insert_field(struct hash_TB_array_HW6* ptr_to_TB_array, char* key,char* field_name,char* value) {//包括插入和更新
    int index = hash_func(key);
    struct hash_node_HW6* cur_node=ptr_to_TB_array->TB_array[index];
   
    while(cur_node!=NULL){
        if(strcmp(cur_node->key,key)==0){//找到key的位置了
            //printf("1\n");
            struct field_node* cur_field_node=cur_node->field_node_head;
            //printf("2\n");
            while (cur_field_node!=NULL){
                if(strcmp(cur_field_node->field,field_name)==0){//把value覆蓋到已經建立好的filed就好
                    strcpy(cur_field_node->value, value);
                    return;
                }
                cur_field_node=cur_field_node->next_field_node;
            }
            //printf("11\n");
            struct field_node* ptr_to_new_field_node=(struct field_node*)malloc(sizeof(struct field_node));
            ptr_to_new_field_node->field =(char*)malloc(strlen(field_name)+1);
            //printf("22\n");
            strcpy(ptr_to_new_field_node->field, field_name);
            ptr_to_new_field_node->value =(char*)malloc(strlen(value)+1);
            //printf("33\n");
            strcpy(ptr_to_new_field_node->value, value);
            //printf("44\n");
            ptr_to_new_field_node->next_field_node=cur_node->field_node_head;
            cur_node->field_node_head=ptr_to_new_field_node;//把新的filed node接在這個hash jey node的head
            return;
        }
        cur_node=cur_node->ptr_to_next_hash_node;
    }
    //printf("1\n");
    struct hash_node_HW6* ptr_to_new_hash_node = (struct hash_node_HW6*)malloc(sizeof(struct hash_node_HW6));
    ptr_to_new_hash_node->key = (char*)malloc(strlen(key) + 1);
    strcpy(ptr_to_new_hash_node->key, key);
    //printf("2\n");
    struct field_node* ptr_to_new_field_node=(struct field_node*)malloc(sizeof(struct field_node));
    ptr_to_new_field_node->field =(char*)malloc(strlen(field_name)+1);
    strcpy(ptr_to_new_field_node->field, field_name);
    //printf("3\n");
    ptr_to_new_field_node->value =(char*)malloc(strlen(value)+1);
    strcpy(ptr_to_new_field_node->value, value);
    //printf("4\n");
    ptr_to_new_hash_node->field_node_head=ptr_to_new_field_node;//為了處理碰撞問題，Array存放的Pointer會指向Linked list的開頭，在往後接下去成文linked list
    ptr_to_new_hash_node->ptr_to_next_hash_node = ptr_to_TB_array->TB_array[index];
    ptr_to_TB_array->TB_array[index] = ptr_to_new_hash_node;
}


void delete_field(struct hash_TB_array_HW6* ptr_to_TB_array, char* key,char* field_name) {//包括插入和更新
    int index = hash_func(key);
    struct hash_node_HW6* prev_node=NULL;
    struct hash_node_HW6* cur_node=ptr_to_TB_array->TB_array[index];
    int key_node_empty_bool=0;
    while(cur_node!=NULL){
        if(strcmp(cur_node->key,key)==0){//找到key的位置了
            struct field_node* cur_field_node=cur_node->field_node_head;//取出當前field_node的head
            struct field_node* prev_field_node=NULL;
            while (cur_field_node!=NULL){
                if(strcmp(cur_field_node->field,field_name)==0){//把這個value-filed node刪除就好
                    if(prev_field_node==NULL){//表示要刪除的node再第一個(head)
                        if(cur_field_node->next_field_node==NULL)key_node_empty_bool=1;
                        cur_node->field_node_head=cur_field_node->next_field_node;
                    }
                    else prev_field_node->next_field_node=cur_field_node->next_field_node;//要刪除的node在第二個或之後
                    if(key_node_empty_bool==0){
                        printf("%s : %s deleted successfully\n",key,field_name);
                        return ;
                    }//表示不用刪除整個key node
                }
                prev_field_node=cur_field_node;
                cur_field_node=cur_field_node->next_field_node;
            }
            //如果該hash node已經沒有其他field node，就刪除
            if(key_node_empty_bool==1){
                if(prev_node!=NULL){
                    prev_node->ptr_to_next_hash_node=cur_node->ptr_to_next_hash_node;
                }
                else ptr_to_TB_array->TB_array[index]=cur_node->ptr_to_next_hash_node;
                printf("%s : %s deleted successfully\n",key,field_name);
                return;
            }
        }
        prev_node=cur_node;
        cur_node=cur_node->ptr_to_next_hash_node;
    }
    printf("%s : %s not found\n",key,field_name);
    return;//刪除失敗
}


void delete_key(struct hash_TB_array_HW6* ptr_to_TB_array, char* key) {//包括插入和更新
    int index = hash_func(key);
    struct hash_node_HW6* prev_node=NULL;
    struct hash_node_HW6* cur_node=ptr_to_TB_array->TB_array[index];
    while(cur_node!=NULL){
        if(strcmp(cur_node->key,key)==0){//找到key的位置了
            if(prev_node!=NULL){
                prev_node->ptr_to_next_hash_node=cur_node->ptr_to_next_hash_node;
            }
            else ptr_to_TB_array->TB_array[index]=cur_node->ptr_to_next_hash_node;
            printf("%s is deleted successfully\n",key);
            return;
        }
        prev_node=cur_node;
        cur_node=cur_node->ptr_to_next_hash_node;
    }
    printf("%s not found\n",key);
    return ;
}


void show_hash_pairs(struct hash_TB_array_HW6* ptr_to_TB_array) {
    printf("All the key members:\n");
    int i;
    for (i = 0; i < MAXHASHTABLESIZE; i++) {
        struct hash_node_HW6* cur_node = ptr_to_TB_array->TB_array[i];
        while (cur_node != NULL){
            //printf("cur index: %d ",i);
            printf("key: %s\n", cur_node->key);
            struct field_node* cur_field_node=cur_node->field_node_head;
            while (cur_field_node!=NULL){
                printf("value: %s field: %s ", cur_field_node->value,cur_field_node->field);
                if(cur_field_node->next_field_node!=NULL)printf(", ");
                cur_field_node=cur_field_node->next_field_node;
            }
            printf("\n");
            cur_node = cur_node->ptr_to_next_hash_node;
        }
    }
}


char* get(struct hash_TB_array_HW6* ptr_to_TB_array, char* key,char* field_name){
    int index = hash_func(key);
    struct hash_node_HW6* cur_node=ptr_to_TB_array->TB_array[index];
    int find_key_bool=0;
    while(cur_node!=NULL){
        if(strcmp(cur_node->key,key)==0){//找到key的位置了
            find_key_bool=1;
            struct field_node* cur_field_node=cur_node->field_node_head;//取出當前field_node的head
           
            while (cur_field_node!=NULL){
                if(strcmp(cur_field_node->field,field_name)==0){//把這個value-filed node刪除就好
                    return cur_field_node->value;
                }
               
                cur_field_node=cur_field_node->next_field_node;
            }
           
        }
        if(find_key_bool==1) return NULL;
        cur_node=cur_node->ptr_to_next_hash_node;
    }
    //表示沒找到
    return NULL;
}
void delayed_delete(EV_P_ ev_timer *w, int revents) {
    struct delayed_data *data = (struct delayed_data *)w->data;
    delete_key(data->myHashTable, data->key); // 在延遲後執行 delete() 函數
    free(data->key);
    free(data);  
}


void stdin_cb(EV_P_ ev_io *w, int revents) {
    struct hash_TB_array_HW6 *myHashTable = (struct hash_TB_array_HW6 *)w->data;
    char input[1000];
    char command[20];
    char key[20];
    char fields[MAX_FIELDS][20];
    char values[MAX_FIELDS][20];
    int pair_ct = 0;
    printf("please input the command(HSET, HGET, HDEL,EXPIRE, SHOW): ");
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%s %s", command, key);
    int count = 0;
    input[strcspn(input, "\n")] = '\0';
    char *token = strtok(input, " ");


    token = strtok(NULL, " ");
    while (token != NULL && count < MAX_FIELDS) {
        token = strtok(NULL, " ");
        //printf("cur count %d\n", count);
        //printf("token: %s\n", token);
        if (token != NULL) {
            if (count % 2 == 0)
                strcpy(fields[count / 2], token);
            else if (count % 2 == 1)
                strcpy(values[count / 2], token);
            count++;
        }
    }


    pair_ct = (count + 1) / 2;
    if (strcmp(command, "HSET") == 0) {
        for (int i = 0; i < pair_ct; i++) {
            insert_field(myHashTable, key, fields[i], values[i]);
        }
    }
    else if (strcmp(command, "HDEL") == 0) {
        delete_field(myHashTable, key,fields[0]);
    }
    else if (strcmp(command, "HGET") == 0) {
        char *ans=get(myHashTable, key,fields[0]);
        if(ans==NULL)printf("the value to  %s : %s not found\n",key,fields[0]);
        else printf("the value to  %s : %s is %s\n",key,fields[0],ans);
    }
    else if(strcmp(command,"EXPIRE")==0){
        //printf("1\n");
        struct delayed_data *expire_data = (struct delayed_data *)malloc(sizeof(struct delayed_data));
        //printf("2\n");
        expire_data->myHashTable=(struct hash_TB_array_HW6 *)malloc(sizeof(struct hash_TB_array_HW6));
        expire_data->myHashTable=myHashTable;
        expire_data->key=(char *)malloc(sizeof(char));
        strcpy(expire_data->key ,key);
        //printf("3\n");
        int delay_second = atoi(fields[0]); // 將字串轉換為整數
        //printf("4\n");
        ev_timer *delay_timer = (ev_timer *)malloc(sizeof(ev_timer));
        ev_timer_init(delay_timer, delayed_delete, delay_second, 0.0); // 5 秒延遲
        //printf("5\n");
        delay_timer->data =expire_data;
        ev_timer_start(loop, delay_timer);
    }
    else if (strcmp(command, "SHOW") == 0) {
        show_hash_pairs(myHashTable);
    }


    else {
        printf("Unknown command\n");
    }
    // printf("please input the command(HSET, HGET, HDEL,EXPIRE, SHOW): ");
    if (!(revents & EV_ERROR) && ev_is_active(w) && !ev_is_pending(w)) {
        // 如果沒有任何待處理的事件，再輸出提示
        printf("please input the command(HSET, HGET, HDEL,EXPIRE, SHOW): ");
        fflush(stdout); // 強制刷新 stdout，確保提示立即顯示
    }
}
void handle_input_HW6(){
    struct ev_loop *loop = EV_DEFAULT;
    ev_io stdin_watcher;
    struct hash_TB_array_HW6 *myHashTable = (struct hash_TB_array_HW6 *)malloc(sizeof(struct hash_TB_array_HW6));

    int i;
    for (i = 0; i < MAXHASHTABLESIZE; i++) {
        myHashTable->TB_array[i] = NULL;
    }

    // 開始監聽標準輸入，並傳遞哈希表給回調函數
    ev_io_init(&stdin_watcher, stdin_cb, 0, EV_READ);
    stdin_watcher.data = myHashTable; // 傳遞哈希表
    ev_io_start(loop, &stdin_watcher);

    // 開始 libev 主循環
    ev_run(loop, 0);
}