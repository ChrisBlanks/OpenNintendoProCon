/*

Author: ChrisB
Description: Support file for error handling.
Date: Summer 2020

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//source code headers
#define __PRO_CON_ERRORS_MAIN__
#include "pro_con_errors.h"


void  displayErrorList(void){
    if(GLOBAL_ERRORS.isInitialized != ERROR_TABLE_INITIALIZED){
        fprintf(stdout,"\nError File has not be loaded.\n");
        return;
    }  

    fprintf(stdout,"\nError List:\n\n");
    for(int indx = 0; indx < GLOBAL_ERRORS.max_index; indx++){
        fprintf(stdout,"Error #%d: %s\n",indx,GLOBAL_ERRORS.error_list[indx]);
    }  
    fprintf(stdout,"\nDone.\n");
    
    return;
}

void  loadErrorFile(char* error_file_path){
    int cur_index = 0;

    char* error_message = NULL,
        * newline_pos   = NULL,
        * path          = NULL;

    FILE* err_file = NULL;  

    path = (error_file_path == NULL) ? ERROR_DEF_PATH : error_file_path;

    err_file = fopen(path,"r");

    if(err_file == NULL){ 
        fprintf(stdout,"\nError: Could not load error file \"%s\".\n",path);
        exit(1); 
    }

    error_message = (char*) malloc(MAX_ERROR_MESSAGE_LEN * sizeof(char));

    while(fgets(error_message,MAX_ERROR_MESSAGE_LEN,err_file) != NULL ){
        if(strcmp(error_message,"\n")   == 0   || 
           strcmp(error_message,"\r\n") == 0   || 
           strcmp(error_message,"")     == 0   ||
           strcmp(error_message," ")    == 0 ){ 
               continue;
        }

        newline_pos = strchr(error_message,'\n');
        if(newline_pos != NULL){ *newline_pos = '\0'; } //remove newline

        snprintf(GLOBAL_ERRORS.error_list[cur_index],MAX_ERROR_MESSAGE_LEN,error_message);
        cur_index++; 
    }
    
    free(error_message);
    fclose(err_file);  

    fprintf(stdout,"\nLoaded %d errors.\n",cur_index);

    GLOBAL_ERRORS.max_index = cur_index;
    GLOBAL_ERRORS.isInitialized = ERROR_TABLE_INITIALIZED;  
    return;
}


char* getErrorMessage(int status){
    int error_index = status * -1;

    if(GLOBAL_ERRORS.isInitialized != ERROR_TABLE_INITIALIZED){
        fprintf(stdout,"\nError File has not be loaded.\nReturned NULL.\n");
        return NULL;
    }

    if(error_index > GLOBAL_ERRORS.max_index || error_index < 0){
        fprintf(stdout,"\nBad index(%d). Max index = %d\nReturned NULL.\n",error_index,GLOBAL_ERRORS.max_index);
        return NULL;
    }

    return GLOBAL_ERRORS.error_list[error_index]; 
}