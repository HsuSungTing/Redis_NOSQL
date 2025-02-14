#ifndef REDIS_JSON_H
#define REDIS_JSON_H

typedef enum {
    STRING,
    OBJECT
} JsonNodeType;
typedef struct JsonNode {
    JsonNodeType type;
    char* key;
    union {
        char* stringValue;
        struct JsonNode** objectValue;
    } value;
    int valid_bool;
} JsonNode;
struct json_tree_node{
    char tree_name[200];
    JsonNode* tree_obj;
};

JsonNode* createStringNode(const char* key, const char* value) ;
JsonNode* createObjectNode(const char* key, JsonNode** objectValue) ;
void printJsonTree(JsonNode* node, int level);
void trimChars(char str[], char toRemove_head, char toRemove_tail);
int countBraces(const char *str, char tar);
void delete_pair(JsonNode* node,char *key,int *found_bool);
void get_value_from_json(JsonNode* node,char *key,int *found_bool);
void insert_to_json(JsonNode* node,char *tar_key,char *tar_value,char *key_to_insert,char *value_to_insert,int *found_bool,JsonNode* pre_level_obj,int cur_level_i);
JsonNode* partition(char *buffer);
void removeSpaces(char *str) ;
JsonNode* find_tree_object(struct json_tree_node tree_arr[],int tree_len,char* tree_name);
void handle_input_HW4();
#endif