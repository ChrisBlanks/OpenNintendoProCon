/*

Author: ChrisB
Description: Support file for error handling.
Date: Summer 2020

*/

#include <stdio.h>
#include <stdlib.h>

//source code headers
#include "pro_con_errors.h"


char* getErrorMessage(int status){

    int line_num  = 0,
        err_code  = status * -1;

    char* error_message = (char*) malloc(MAX_ERROR_MESSAGE_LEN * sizeof(char));

    FILE* err_file = NULL;    

    err_file = fopen(ERROR_DEF_PATH,"r");

    if(err_file == NULL){ exit(1); }

    while(line_num <= err_code){
        fgets(error_message,MAX_ERROR_MESSAGE_LEN,err_file);
        line_num++; 
    }
    
    fclose(err_file);
    return error_message; 
}