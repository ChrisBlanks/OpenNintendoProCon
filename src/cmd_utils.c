/*

Author: ChrisB
Description: Command related function definitions.
Date: Summer 2020

*/

#include <stdio.h>
#include <stdlib.h>

//source code headers
#include "cmd_utils.h"
#include "pro_con_utils.h"
#include "pro_con_errors.h"
#include "x11_map.h"


void configureSettings(int settings_type, char* settings_name, char* settings_value){
    int setting = 0;
    int setting_value = 0;

    switch(settings_type){
        case CONFIGURATION_SETTINGS:
            fprintf(stdout,"\nNot implemented yet.\n");
            break;

        case CONTROLLER_MAP_SETTINGS:
            setting = 0;
            setting_value = 0;

            updateKeyMap(NULL, setting, (unsigned long) setting_value);
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
    char* current_arg = NULL;

    if(args == NULL){ //initialize if empty
        args = (cli_args_t*) malloc(sizeof(cli_args_t));
        args->all_args = (arg_t*) malloc(DEFAULT_CLI_ARGS_SZ* sizeof(arg_t));
    }

    args->count = argc-1; //ignore 1st CLI arg (file name)

    for(int arg_indx=1; arg_indx <= args->count;arg_indx++){
        current_arg = argv[arg_indx];

        fprintf(stdout,"\nArg #%d: %s",arg_indx, current_arg);

        //assign after processing
        (args->all_args+arg_indx)->value =  "";
        (args->all_args+arg_indx)->option = "";
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