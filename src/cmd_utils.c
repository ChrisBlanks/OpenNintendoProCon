/*

Author: ChrisB
Description: Command related function definitions.
Date: Summer 2020

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//source code headers
#include "cmd_utils.h"
#include "pro_con_utils.h"
#include "pro_con_errors.h"
#include "x11_map.h"


void configureSettings(int settings_type, char* settings_name, char* settings_value){
    int setting = 0;
    int value = 0;

    switch(settings_type){
        case CONFIGURATION_SETTINGS:
            fprintf(stdout,"\nNot implemented yet.\n");
            break;

        case CONTROLLER_MAP_SETTINGS:
            
            setting = strtol(settings_name,NULL,10); // settings name should represent the button code 
            value = strtol(settings_value,NULL,10); //setting value should represent the desired keysym

            updateKeyMap(NULL, setting, (unsigned long) value);
            break;
        default:
            fprintf(stderr,"\nNot a valid settings type (%d)\n",settings_type);
    }
}

void connectToController(char* controller_path){

}

void displayPossibleErrors(void){
    displayErrorList();
}

void displayAll(void){
    displayErrorList();
    displayLoadedKeyMap();
}

void displaySettings(int settings_type){

    switch(settings_type){
        case CONFIGURATION_SETTINGS:
            fprintf(stdout,"\nNot implemented yet.\n");
            break;

        case CONTROLLER_MAP_SETTINGS:
            displayLoadedKeyMap();
            break;
        default:
            fprintf(stderr,"\nNot a valid settings type (%d)\n",settings_type);
    }

}


void freeArgsData(cli_args_t* args){
    free(args->all_args);
    args->all_args = NULL;

    free(args);
    args = NULL;
    
    return;
}

void initRoutine(void){
    loadErrorFile(NULL);
    loadKeyMap(NULL);

    displayAll();
}


void parseCLIArgs(int argc, char* argv[],cli_args_t* args){
    char* current_arg = NULL,
        * next_arg    = NULL,
        * char_pos    = NULL;

    if(args == NULL){ //initialize if empty
        args = (cli_args_t*) malloc(sizeof(cli_args_t));
        args->all_args = (arg_t*) malloc(DEFAULT_CLI_ARGS_SZ* sizeof(arg_t));
    }

    args->count = argc-1; //ignore 1st CLI arg (file name)

    for(int arg_indx=1,arg_num =1; arg_num <= args->count;arg_indx++,arg_num++){
        current_arg = argv[arg_num];
        char_pos = strchr(current_arg,'-');

        fprintf(stdout,"\nArg #%d: %s",arg_num, current_arg);

        if(char_pos != NULL && strlen(current_arg) == 2){ // check for one letter options
            snprintf((args->all_args+arg_indx)->option,DEFAULT_CLI_STR_SZ,"%s",char_pos+1);
            
            if(arg_num + 1 <= args->count){
                next_arg = argv[arg_num + 1];
                snprintf((args->all_args+arg_indx)->value,DEFAULT_CLI_STR_SZ,"%s",next_arg);
                arg_num++; //advance index since next arg is processed here
                fprintf(stdout,"\nArg #%d: %s",arg_num, next_arg);
            }

        } else{
            snprintf((args->all_args+arg_indx)->option,DEFAULT_CLI_STR_SZ,"None");
            snprintf((args->all_args+arg_indx)->value,DEFAULT_CLI_STR_SZ,"%s",current_arg);        
        }

        fprintf(stdout,"\nOption: %s\nValue: %s\n",(args->all_args+arg_indx)->option,(args->all_args+arg_indx)->value);
    }
    fprintf(stdout,"\n");
    
    args->isInitialized = 1;
    return;
}


void startProConProcessing(int joystick_fd,char* joystick_file_name){

    processAllEvents(joystick_fd,joystick_file_name);
}

void startTestMode(int mode){

}