#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <limits.h>
#include "redis_dll.h"
#include "redis_str.h"
#include "redis_ss.h"
#include "redis_json.h"
#include "redis_hash.h"

int main() {
    while(1){
        int command;
        printf("Enter 1 to use Redis string, Enter 2 to use Redis list, Enter 3 to use Redis sorted set, Enter 4 to use Redis json, Enter 5 to use Redis hash\n");
        scanf("%d",&command);
        if(command==1)handle_input_HW1();//redis_str
        else if(command==2)handle_input_HW3();//redis_dll
        else if(command==3)handle_input_HW5();//redis_ss
        else if(command==4)handle_input_HW4();//redis_json
        else if(command==5)handle_input_HW6();//redis_hash
        else printf("invalid input\n");
    }
    return 0;
}