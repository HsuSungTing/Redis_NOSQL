#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "redis_json.h"

JsonNode* createStringNode(const char* key, const char* value) {
    JsonNode* node = (JsonNode*)malloc(sizeof(JsonNode));
    node->type = STRING;
    node->key = strdup(key);
    node->value.stringValue = strdup(value);
    node->valid_bool=1;
    return node;
}

JsonNode* createObjectNode(const char* key, JsonNode** objectValue) {
    JsonNode* node = (JsonNode*)malloc(sizeof(JsonNode));
    node->type = OBJECT;
    node->key = strdup(key);
    node->value.objectValue = objectValue;
    node->valid_bool=1;
    return node;
}

void printJsonTree(JsonNode* node, int level) {
    int obj_valid=0;
    if(node->valid_bool==1){
        for (int i = 0; i < level; i++) {
            printf("  ");
        }
        printf("%s: ", node->key);//key就直接印出來
    }

    switch (node->type) {
        case STRING:
            if(node->valid_bool==1){
                printf("\"%s\"\n", node->value.stringValue);
            }
            break;
        case OBJECT:
            for(int i = 0; node->value.objectValue[i] != NULL; i++){
                if(node->value.objectValue[i]->valid_bool==1)obj_valid=1;
            }
            if(obj_valid==1)printf("Object\n");
            for (int i = 0; node->value.objectValue[i] != NULL; i++) {
                printJsonTree(node->value.objectValue[i], level + 1);
            }
            break;
    }
}

void trimChars(char str[], char toRemove_head, char toRemove_tail) {
    size_t len = strlen(str);
    size_t start = 0;
    if (start < len && str[start] == toRemove_head) {
        ++start;
    }
    size_t end = len - 1;
    if (end > start && str[end - 1] == toRemove_tail) {
        --end;
    }

    char* ans = (char*)malloc(len - 1);  // 加2是为了包含 null 字符
    int i;
    for (i = 0; i < len-2; i++) {
        ans[i] = str[i + start];
    }
    ans[len-2] = '\0';
    strcpy(str, ans);
    free(ans);
}
int countBraces(const char *str, char tar) {
    int count = 0;
    while (*str != '\0') {
        if (*str == tar) {
            count++;
        }
        str++;
    }
    return count;
}

void delete_pair(JsonNode* node,char *key,int *found_bool){
    switch (node->type) {
        case STRING:
            if(strcmp(node->key,key)==0&&node->valid_bool==1){
                node->valid_bool=0;
                (*found_bool)=1;
            }
            break;
        case OBJECT:
            for (int i = 0; node->value.objectValue[i] != NULL; i++) {
                delete_pair(node->value.objectValue[i],key,found_bool);
            }
            break;
    }
}
void get_value_from_json(JsonNode* node,char *key,int *found_bool){
    switch (node->type) {
        case STRING:
            if(strcmp(node->key,key)==0&&node->valid_bool==1){
                printf("the value to %s is \"%s\"\n", node->key, node->value.stringValue);
                (*found_bool)=1;
            }
            break;
        case OBJECT:
            for (int i = 0; node->value.objectValue[i] != NULL; i++) {
                get_value_from_json(node->value.objectValue[i],key,found_bool);
            }
            break;
    }
}

void insert_to_json(JsonNode* node,char *tar_key,char *tar_value,char *key_to_insert,char *value_to_insert,int *found_bool,JsonNode* pre_level_obj,int cur_level_i){
    switch (node->type) {
        case STRING:
            if(strcmp(node->key,tar_key)==0&&strcmp(node->value.stringValue,tar_value)==0&&node->valid_bool==1){
                (*found_bool)=1;
                int i_inserted=cur_level_i+1;
                JsonNode** obj_arr=(JsonNode**)malloc(100* sizeof(JsonNode*));
                        
                for(int j=0;j<i_inserted;j++){
                    obj_arr[j]=pre_level_obj->value.objectValue[j];
                }
                obj_arr[i_inserted]=createStringNode(key_to_insert,value_to_insert);
                        
                for(int j=cur_level_i+1;pre_level_obj->value.objectValue[j] != NULL;j++){
                    obj_arr[j+1]=pre_level_obj->value.objectValue[j];
                }               
                pre_level_obj->value.objectValue =obj_arr;
                //printJsonTree(pre_level_obj,0);
            }
            break;
        case OBJECT:
            for (int i = 0; node->value.objectValue[i] != NULL; i++) {
                insert_to_json(node->value.objectValue[i],tar_key,tar_value,key_to_insert,value_to_insert,found_bool,node,i);
            }
            break;
    }
}
JsonNode* partition(char *buffer){
    if(countBraces(buffer,'}')==0&&countBraces(buffer,'{')==0){
        //printf("finally: %s\n",buffer);
        char key[20], val[20];
        sscanf(buffer, "%[^:]:%s",key,val);
        return createStringNode(key,val);
    }
    else{
        //printf("before trimmed: %s\n",buffer);
        trimChars(buffer,'{','}');
        //printf("after trimmed: %s\n",buffer);
        JsonNode** Obj_level_0=(JsonNode**)malloc(100* sizeof(JsonNode*));
        //---------初始化temp-----------------
        char *temp = (char *)malloc(strlen(buffer) * sizeof(char));
        temp[0] = '\0'; 
        int obj_ct_level_0=0;
        int inside_brackets = 0;
        int count=0;
        int length = strlen(buffer);
        int start = 0;
        //-----------------------------------
        for (int i = 0; i <= length; i++) {
            if (buffer[i] == ',' || buffer[i] == '\0') {
                int tokenLength = i - start;
                char *token = (char *)malloc((tokenLength + 1) * sizeof(char));
                strncpy(token, &buffer[start], tokenLength);
                token[tokenLength] = '\0';
                //----------------------------------------------------
                if(strstr(token, "{")&&strstr(token, "}")){//,{A:B},...
                    //printf("case1 The token %s\n", token);
                    Obj_level_0[obj_ct_level_0]=partition(token);
                    obj_ct_level_0++;
                    count++;
                }
                else if (strstr(token, "{") != NULL) {
                    inside_brackets+=countBraces(token,'{');
                    strcat(temp, token);
                    strcat(temp, ",");
                    //printf("case2 The temp %s\n", temp);
                }
                else if (strstr(token, "}") != NULL&&inside_brackets>1) {
                    inside_brackets-=countBraces(token,'}');
                    strcat(temp, token);
                    if(inside_brackets==1){
                        strcat(temp, ",");
                    }
                    else if(inside_brackets==0){
                        //printf("case3 the temp %s\n", temp);
                        //---------------------
                        Obj_level_0[obj_ct_level_0]=partition(temp);
                        obj_ct_level_0++;
                        count++;
                        //---------------------
                        temp[0] = '\0';
                    }
                } 
                else if (strstr(token, "}") != NULL&&inside_brackets==1) {
                    inside_brackets--;
                    strcat(temp, token);
                    //printf("case6 the temp %s\n", temp);
                    Obj_level_0[obj_ct_level_0]=partition(temp);
                    obj_ct_level_0++;
                    count++;
                    temp[0] = '\0';
                } 
                else if (inside_brackets) {
                    strcat(temp, token);
                    strcat(temp, ",");
                    //printf("case4 The temp %s\n", temp);
                } 
                else {
                    //printf("case5 The token %s\n", token);
                    Obj_level_0[obj_ct_level_0]=partition(token);
                    obj_ct_level_0++;
                    count++;
                }
                //------------------------------------------------------
                start = i + 1;
            }
        }
        Obj_level_0[obj_ct_level_0] = NULL; // Mark the end of the object array
        JsonNode* node=createObjectNode("Object_key", Obj_level_0);
        return node;
    }
}
void removeSpaces(char *str) {
    int count = 0;
    for (int i = 0; str[i]; i++) {
        if (str[i] != ' ') {
            str[count++] = str[i];
        }
    }
    str[count] = '\0';
}

JsonNode* find_tree_object(struct json_tree_node tree_arr[],int tree_len,char* tree_name){
    for(int i=0;i<tree_len;i++){
        if(strcmp(tree_name,tree_arr[i].tree_name)==0){
            return tree_arr[i].tree_obj;
        }
    }    
    return NULL;
}

void handle_input_HW4(){
    char input[1000];
    char command[100];  // 假設命令的最大長度為100個字元
    char json_name[100];
    char value[1000];
    
    struct json_tree_node tree_arr[100];
    int tree_len=0;
    int first_time=0;
    while(1){
        
        int c;
        while (first_time==0&&(c = getchar()) != '\n' && c != EOF) {}//多加的，// 逐字讀取，直到清除缓冲區，讀一次就清除一次緩衝區
        first_time++;
        
        printf("Please input the command for Redis Json(create,find,delete,insert,show)\n");
        
        fgets(input, sizeof(input), stdin);
        
        sscanf(input,"%s %s %[^\n]s", command,json_name, value);
        
        if(strcmp(command,"create")==0){
            JsonNode* tree_object=NULL;
            int tar_index=-1;
            for(int i=0;i<tree_len;i++){
                if(strcmp(json_name,tree_arr[i].tree_name)==0){
                    tree_object=tree_arr[i].tree_obj;
                    tar_index=i;
                }
            }
            if(tree_object!=NULL){
                removeSpaces(value);
                tree_arr[tar_index].tree_obj=partition(value);
                //printJsonTree(tree_arr[tar_index].tree_obj, 0);
            }
            else{
                strcpy(tree_arr[tree_len].tree_name,json_name);
                removeSpaces(value);
                tree_arr[tree_len].tree_obj=partition(value);
                //printJsonTree(tree_arr[tree_len].tree_obj, 0);
                tree_len++;
            }
        }
        else if(strcmp(command,"find")==0){
            JsonNode* tree_object=find_tree_object(tree_arr,tree_len,json_name);
            removeSpaces(value);
            if(tree_object!=NULL){
                int found_bool=0;
                get_value_from_json(tree_object,value,&found_bool);
                if(found_bool==0)printf("no value corresponds to %s\n",value);
            }
            else printf("%s doesn't exist\n",json_name);
        }
        else if(strcmp(command,"delete")==0){
            JsonNode* tree_object=find_tree_object(tree_arr,tree_len,json_name);
            removeSpaces(value);
            if(tree_object!=NULL){
                int found_bool=0;
                delete_pair(tree_object,value,&found_bool);
                //printJsonTree(tree_object, 0);
                if(found_bool==0)printf("key:%s doesn't exist\n",value);
                else printf("deleted successfully\n");
            }
            else printf("%s doesn't exist\n",json_name);
        }
        else if(strcmp(command,"insert")==0){
            JsonNode* tree_object=find_tree_object(tree_arr,tree_len,json_name);
            if(tree_object!=NULL){
                char key1[20], value1[20], key2[20], value2[20];
                if (sscanf(value, "%19[^:]:%19s %19[^:]:%19s", key1, value1, key2, value2) == 4) {
                    removeSpaces(key1);
                    removeSpaces(value1);
                    removeSpaces(key2);
                    removeSpaces(value2);
                    //printf("Key1: %s\nValue1: %s\nKey2: %s\nValue2: %s\n", key1, value1, key2, value2);
                    int found_bool=0;
                    insert_to_json(tree_object,key1,value1,key2,value2,&found_bool,tree_object,0);
                    if(found_bool==0)printf("insertion failed, %s:%s doesn't exist\n",key1,value1);
                    else printf("inserted successfully\n");
                } 
                else printf("Wrong input format\n");
            }
            else printf("%s doesn't exist\n",json_name);
        }
        else if(strcmp(command,"show")==0){
            JsonNode* tree_object=find_tree_object(tree_arr,tree_len,json_name);
            removeSpaces(value);
            if(tree_object!=NULL){
                printf("%s:\n",json_name);
                printJsonTree(tree_object, 0);
            }
            else printf("%s doesn't exist\n",json_name);
        }
        else printf("Invalid command\n");
        
        printf("\n");
    }
}