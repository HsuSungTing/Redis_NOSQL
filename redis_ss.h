#ifndef REDIS_SS_H
#define REDIS_SS_H

#define MAX_VALUES 100

typedef struct SS_Node {
    int data;
    char* member; // Node name
    struct SS_Node* next;
}SS_Node;

// Define a structure for the sorted set
typedef struct SortedSet {
    char name[50];
    SS_Node* head;
} SortedSet;

SortedSet* create_sorted_set(char* set_name);

// Function to create a new node
SS_Node* create_Node(int data, char* member);

// Function to merge two sorted linked lists
SS_Node* merge(SS_Node* list1, SS_Node* list2);

// Function to perform merge sort on the linked list
SS_Node* mergeSort(SS_Node* head) ;
void sortSet(SortedSet* set) ;

void insert_without_sorting(SortedSet* set, int data, char* member);

//如果使用 ZADD 插入了兩個成員相同但分數不同的情況，Redis 會根據成員來做更新，而不是相加分數。
void insertElement(SortedSet* set, int data, char* member);
// Function to display the elements in a sorted set
void displaySet(SortedSet* set, int start,int end) ;
// Function to delete nodes containing a specific string member from the set
void deleteMemberNodes(SortedSet* set, char* member) ;
SortedSet* find_union_sorted_set(SortedSet* set1,SortedSet* set2,char* union_name);
SortedSet* find_intersection_set(SortedSet* set1,SortedSet* set2,char* intersected_name);
SortedSet* find_set_by_name(char* set_name,SortedSet* keep_set_arr[],int keep_set_arr_size);
void ZRANK(SortedSet* set,char* member);
void ZCARD(SortedSet* set);
void ZRANGEBYSCORE(SortedSet* set,char *low_b,char* up_b);
void ZREMRANGEBYSCORE(SortedSet* set,char *low_b,char* up_b);
void ZCOUNT(SortedSet* set,int low_b,int up_b);
void handle_input_HW5();
#endif