#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <limits.h>
#include "redis_ss.h"

SortedSet* create_sorted_set(char* set_name){
    SortedSet* sorted_set=(SortedSet*)malloc(sizeof(SortedSet));
    sorted_set->head=NULL;
    strcpy(sorted_set->name, set_name);
    return sorted_set;
}

// Function to create a new node
SS_Node* create_Node(int data, char* member) {
    SS_Node* newNode = (SS_Node*)malloc(sizeof(SS_Node));
    if (newNode == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    newNode->data = data;
    newNode->member = strdup(member);
    newNode->next = NULL;
    return newNode;
}

// Function to merge two sorted linked lists
SS_Node* merge(SS_Node* list1, SS_Node* list2) {
    if (list1 == NULL) {
        return list2;
    }
    if (list2 == NULL) {
        return list1;
    }
    SS_Node* result = NULL;
    if (list1->data <= list2->data) {
        result = list1;
        result->next = merge(list1->next, list2);
    } else {
        result = list2;
        result->next = merge(list1, list2->next);
    }

    return result;
}

// Function to perform merge sort on the linked list
SS_Node* mergeSort(SS_Node* head) {
    if (head == NULL || head->next == NULL) {
        return head;
    }
    SS_Node* middle = head;
    SS_Node* end = head->next;
    while (end != NULL && end->next != NULL) {
        middle = middle->next;
        end = end->next->next;
    }
    SS_Node* secondHalf = middle->next;
    middle->next = NULL;
    return merge(mergeSort(head), mergeSort(secondHalf));
}
void sortSet(SortedSet* set) {
    set->head = mergeSort(set->head);
}

void insert_without_sorting(SortedSet* set, int data, char* member){
    SS_Node* newNode = create_Node(data, member);
    newNode->next = set->head;
    set->head = newNode;
}

//如果使用 ZADD 插入了兩個成員相同但分數不同的情況，Redis 會根據成員來做更新，而不是相加分數。
void insertElement(SortedSet* set, int data, char* member) {
    SS_Node* head=set->head;
    int over_write_bool=0;
    while(head){
        if(strcmp(head->member,member)==0){
            //覆蓋掉之前的data
            head->data=data;
            over_write_bool=1;
        }
        head=head->next;
    }
    if(over_write_bool==0){
        SS_Node* newNode = create_Node(data, member);
        newNode->next = set->head;
        set->head = newNode;
        sortSet(set);
    }
}

// Function to display the elements in a sorted set
void displaySet(SortedSet* set, int start,int end) {
    
    printf("Sorted Set %s: ", set->name);
    SS_Node* current = set->head;
    int index=0;
    while (current != NULL) {
        if(index>=start&&(index<=end||end==-1)){
            printf("(%s: %d) ", current->member, current->data);
            index++;
        }
        current = current->next;
    }
    printf("\n");
}

// Function to delete nodes containing a specific string member from the set
void deleteMemberNodes(SortedSet* set, char* member) {
    SS_Node* current = set->head;
    SS_Node* prev = NULL;
    while (current != NULL) {
        if (strcmp(current->member, member) == 0) {
            if (prev == NULL) {
                SS_Node* temp = current;
                set->head = current->next;
                current = current->next;
                free(temp);
            } else {
                prev->next = current->next;
                free(current);
                current = prev->next;
            }
        } else {
            // Move to the next node
            prev = current;
            current = current->next;
        }
    }
}

SortedSet* find_union_sorted_set(SortedSet* set1,SortedSet* set2,char* union_name){
    SortedSet* union_set=create_sorted_set(union_name);
    SS_Node* set1_head=set1->head;
    SS_Node* set2_head=set2->head;
    while(set1_head){
        insert_without_sorting(union_set,set1_head->data,set1_head->member);
        set1_head=set1_head->next;
    }
    while(set2_head){
        set1_head=set1->head;
        int collision_bool=0;
        while(set1_head){
            if(strcmp(set1_head->member,set2_head->member)==0){
                deleteMemberNodes(union_set,set1_head->member);
                insert_without_sorting(union_set,set1_head->data+set2_head->data,set1_head->member);
                collision_bool=1;
            }
            set1_head=set1_head->next;
        }
        if(collision_bool==0){
            insert_without_sorting(union_set,set2_head->data,set2_head->member);
        }
        set2_head=set2_head->next;
    }
    sortSet(union_set);
    return union_set;
}

SortedSet* find_intersection_set(SortedSet* set1,SortedSet* set2,char* intersected_name){
    SortedSet* intersected_set=create_sorted_set(intersected_name);
    SS_Node* set1_head=set1->head;
    SS_Node* set2_head=set2->head;
    
    while(set2_head){
        set1_head=set1->head;
        while(set1_head){
            if(strcmp(set1_head->member,set2_head->member)==0){
                insert_without_sorting(intersected_set,set1_head->data+set2_head->data,set1_head->member);
            }
            set1_head=set1_head->next;
        }
        set2_head=set2_head->next;
    }
    sortSet(intersected_set);
    return intersected_set;
}
SortedSet* find_set_by_name(char* set_name,SortedSet* keep_set_arr[],int keep_set_arr_size){
    SortedSet* set=NULL;
    for(int i=0;i<keep_set_arr_size;i++){
        if(strcmp(set_name,keep_set_arr[i]->name)==0){
            set=keep_set_arr[i];
            break;
        }
    }
    return set;
}
void ZRANK(SortedSet* set,char* member){
    int rank=0;
    int found_bool=0;
    SS_Node* cur=set->head;
    while(cur){
        if(strcmp(cur->member,member)==0){
            found_bool=1;
            break;
        }
        rank++;
        cur=cur->next;
    }
    if(found_bool==0)printf("%s not found in %s\n",member,set->name);
    else printf("%d\n",rank);
}
void ZCARD(SortedSet* set){
    int ct_len=0;
    SS_Node* cur=set->head;
    while(cur){
        ct_len++;
        cur=cur->next;
    }
    printf("%d\n",ct_len);
}
void ZRANGEBYSCORE(SortedSet* set,char *low_b,char* up_b){
    int max_int = INT_MAX;
    int min_int = INT_MIN;
    if(strcmp(low_b,"-inf")==0)min_int = INT_MIN;
    else min_int = atoi(low_b);
    if(strcmp(up_b,"+inf")==0)max_int = INT_MAX;
    else max_int = atoi(up_b);
    
    SS_Node* cur=set->head;
    while(cur){
        if(cur->data>=min_int&&cur->data<=max_int){
            printf("(%s: %d) ", cur->member, cur->data);
        }
        cur=cur->next;
    }
    printf("\n");
}
void ZREMRANGEBYSCORE(SortedSet* set,char *low_b,char* up_b){
    int max_int = INT_MAX;
    int min_int = INT_MIN;
    if(strcmp(low_b,"-inf")==0)min_int = INT_MIN;
    else min_int = atoi(low_b);
    if(strcmp(up_b,"+inf")==0)max_int = INT_MAX;
    else max_int = atoi(up_b);
    char* member_to_remove[100];
    int member_to_remove_ct=0;
    SS_Node* cur=set->head;
    while(cur){
        if(cur->data>=min_int&&cur->data<=max_int){
            member_to_remove[member_to_remove_ct]=cur->member;
            member_to_remove_ct++;
        }
        cur=cur->next;
    }
    for(int i=0;i<member_to_remove_ct;i++){
        deleteMemberNodes(set, member_to_remove[i]);
    }
}
void ZCOUNT(SortedSet* set,int low_b,int up_b){
    SS_Node* cur = set->head;
    int index=0;
    while (cur != NULL) {
        if(cur->data>=low_b&&cur->data<=up_b){
            index++;
        }
        cur=cur->next;
    }
    printf("%d\n",index);
    
}//ZCOUNT key min max回傳min和max之間的數量

void handle_input_HW5(){
    char input[100]; // 假設最大輸入長度為100
    char command[20];
    char set_name[20];
    char values[MAX_VALUES][20]; // 假設每個value的最大長度為20
    
    SortedSet* keep_set_arr[100];
    int keep_set_arr_size=0;
    
    while(1){
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}//多加的，// 逐字讀取，直到清除缓冲區，讀一次就清除一次緩衝區
        
        printf("Please input the command (ZADD,ZCARD,ZCOUNT,ZINTERSTORE,ZUNIONSTORE,ZRANGE,ZRANGEBYSCORE,ZRANK,ZREM,ZREMRANGE,ZREMRGEBYSCORE)\n");
        fgets(input, sizeof(input), stdin);
        sscanf(input, "%s %s", command, set_name);
    
        int count = 0;//紀錄value的size
        input[strcspn(input, "\n")] = '\0';
        char *token = strtok(input, " "); // 使用空格切割字串
        token = strtok(NULL, " "); // 繼續切割
        while (token != NULL && count < MAX_VALUES) {
            token = strtok(NULL, " "); // 繼續切割
            if (token != NULL) {
                strcpy(values[count], token);
                count++;
            }
        }
        
        //--------------------------------------------
        if(strcmp(command,"ZADD")==0){//ZADD set_name score1 member1 score2 member2 ....
            SortedSet* set;
            int found_bool=0;
            for(int i=0;i<keep_set_arr_size;i++){
                if(strcmp(set_name,keep_set_arr[i]->name)==0){
                    set=find_set_by_name(set_name,keep_set_arr,keep_set_arr_size);
                    found_bool=1;
                    break;
                }
            }
            if(found_bool==0){//表示之前沒出現過，新開一個
                set=create_sorted_set(set_name);
                keep_set_arr[keep_set_arr_size]=set;
                keep_set_arr_size++;
            }
            for(int i=0;i<count;i+=2){
                insertElement(set, atoi(values[i]), values[i+1]);
                //printf("value[%d]: %d\n",i,atoi(values[i]));
                //printf("value[%d]: %s\n",i+1,values[i+1]);
            }
        }
        else if(strcmp(command,"ZREM")==0){
            SortedSet* set=find_set_by_name(set_name,keep_set_arr,keep_set_arr_size);
            if(set==NULL){//表示之前沒出現過，輸出warning
                printf("error, %s doesn't exist.\n",set_name);
            }
            else{
                for(int i=0;i<count;i++){
                    deleteMemberNodes(set, values[i]);
                }
            }
        }
        else if(strcmp(command,"ZRANGE")==0){
            SortedSet* set=find_set_by_name(set_name,keep_set_arr,keep_set_arr_size);
            if(set==NULL){//表示之前沒出現過，輸出warning
                printf("error, %s doesn't exist.\n",set_name);
            }
            else{
                displaySet(set, atoi(values[0]),atoi(values[1]));
            }
        }
        else if(strcmp(command,"ZRANK")==0){//ZRANK salary tom //顯示tom的排名
            SortedSet* set=find_set_by_name(set_name,keep_set_arr,keep_set_arr_size);
            if(set==NULL){//表示之前沒出現過，輸出warning
                printf("error, %s doesn't exist.\n",set_name);
            }
            else ZRANK(set,values[0]);
        }
        else if(strcmp(command,"ZCARD")==0){//ZCARD myzset //回傳當前myzset中的member數量
            SortedSet* set=find_set_by_name(set_name,keep_set_arr,keep_set_arr_size);
            if(set==NULL){//表示之前沒出現過，輸出warning
                printf("error, %s doesn't exist.\n",set_name);
            }
            else ZCARD(set);
        }
        else if(strcmp(command,"ZRANGEBYSCORE")==0){
            SortedSet* set=find_set_by_name(set_name,keep_set_arr,keep_set_arr_size);//ZRANGEBYSCORE salary -inf 5000 
            if(set==NULL){//表示之前沒出現過，輸出warning
                printf("error, %s doesn't exist.\n",set_name);
            }
            else ZRANGEBYSCORE(set,values[0],values[1]);
        }
        else if(strcmp(command,"ZREMRANGEBYSCORE")==0){
            SortedSet* set=find_set_by_name(set_name,keep_set_arr,keep_set_arr_size);//ZRANGEBYSCORE salary -inf 5000 
            if(set==NULL){//表示之前沒出現過，輸出warning
                printf("error, %s doesn't exist.\n",set_name);
            }
            else ZREMRANGEBYSCORE(set,values[0],values[1]);
        }
        else if(strcmp(command,"ZINTERSTORE")==0){
            SortedSet* set[100];
            if(atoi(values[0])==0){
                printf("error, invalid operation.\n");
            }
            else{
                int all_exist_bool=1;
                for(int i=0;i<atoi(values[0]);i++){
                    SortedSet* temp_set;
                    temp_set=find_set_by_name(values[i+1],keep_set_arr,keep_set_arr_size); 
                    if(set==NULL){//表示之前沒出現過，輸出warning
                        printf("%s doesn't exist.\n",values[i+1]);
                        all_exist_bool=0;
                    }
                }
                if(all_exist_bool==1){
                    SortedSet* set1=find_set_by_name(values[1],keep_set_arr,keep_set_arr_size); 
                    SortedSet* set2=find_set_by_name(values[2],keep_set_arr,keep_set_arr_size); 
                    SortedSet* merge_set=find_intersection_set(set1,set2,set_name);
                    for(int i=2;i<atoi(values[0]);i++){
                        set1=find_set_by_name(values[i+1],keep_set_arr,keep_set_arr_size); 
                        merge_set=find_intersection_set(set1,merge_set,set_name);
                    }
                    if(find_set_by_name(set_name,keep_set_arr,keep_set_arr_size)==NULL){
                        keep_set_arr[keep_set_arr_size]=merge_set;
                        keep_set_arr_size++;//之前如果沒出現過就開一格新的紀錄
                    }
                    else{
                        for(int i=0;i<keep_set_arr_size;i++){
                            if(strcmp(keep_set_arr[i]->name,set_name)==0){
                                keep_set_arr[i]=merge_set;
                                break;
                            }
                        }
                    }
                    //--------------------------------------------------------------------
                }
            }
        }
        else if(strcmp(command,"ZUNIONSTORE")==0){
            SortedSet* set[100];
            if(atoi(values[0])==0){
                printf("error, invalid operation.\n");
            }
            else{
                int all_exist_bool=1;
                for(int i=0;i<atoi(values[0]);i++){
                    SortedSet* temp_set;
                    temp_set=find_set_by_name(values[i+1],keep_set_arr,keep_set_arr_size); 
                    if(set==NULL){//表示之前沒出現過，輸出warning
                        printf("%s doesn't exist.\n",values[i+1]);
                        all_exist_bool=0;
                    }
                }
                if(all_exist_bool==1){
                    SortedSet* set1=find_set_by_name(values[1],keep_set_arr,keep_set_arr_size); 
                    SortedSet* set2=find_set_by_name(values[2],keep_set_arr,keep_set_arr_size); 
                    SortedSet* merge_set=find_union_sorted_set(set1,set2,set_name);//merge的部分如果之前有出現過要覆蓋
                    for(int i=2;i<atoi(values[0]);i++){
                        set1=find_set_by_name(values[i+1],keep_set_arr,keep_set_arr_size); 
                        merge_set=find_union_sorted_set(set1,merge_set,set_name);
                    }
                    //建構完成了新的交集sort
                    if(find_set_by_name(set_name,keep_set_arr,keep_set_arr_size)==NULL){
                        //printf("之前未出現過\n");
                        keep_set_arr[keep_set_arr_size]=merge_set;
                        keep_set_arr_size++;//之前如果沒出現過就開一格新的紀錄
                    }
                    else{
                        //printf("之前有出現過\n");
                        for(int i=0;i<keep_set_arr_size;i++){
                            if(strcmp(keep_set_arr[i]->name,set_name)==0){
                                keep_set_arr[i]=merge_set;
                                break;
                            }
                        }
                    }
                }
            }
        }
        else if(strcmp(command,"ZCOUNT")==0){
            SortedSet* set=find_set_by_name(set_name,keep_set_arr,keep_set_arr_size);//ZRANGEBYSCORE salary -inf 5000 
            if(set==NULL){//表示之前沒出現過，輸出warning
                printf("error, %s doesn't exist.\n",set_name);
            }
            else ZCOUNT(set,atoi(values[0]),atoi(values[1]));
        }
        else{
            printf("Invalid Command.\n");
        }
        printf("\n");
    }
}
