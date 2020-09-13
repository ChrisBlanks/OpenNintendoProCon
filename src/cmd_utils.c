/*

Author: ChrisB
Description: Command related function definitions.
Date: Summer 2020

*/


#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//source code headers
#include "cmd_utils.h"
#include "pro_con_utils.h"
#include "pro_con_errors.h"
#include "x11_map.h"


void configureSettings(int settings_type, char* settings_name, char* settings_value){
    int setting = 0,
        value   = 0;

    switch(settings_type){
        case CONFIGURATION_SETTINGS:
            fprintf(stdout,"\nNot implemented yet.\n");
            break;

        case CONTROLLER_MAP_SETTINGS:
            
            setting = strtol(settings_name,NULL,10); // settings name should represent the button code 
            value = strtol(settings_value,NULL,10); //setting value should represent the desired keysym

            updateKeyMap(NULL, setting, (unsigned long) value);
            break;

        case SCRIPT_SETTINGS:
            setting =  strtol(settings_name,NULL,10); // settings name should represent the button code 
            updateScriptMap(NULL, setting, settings_value);
            break;

        default:
            fprintf(stderr,"\nNot a valid settings type (%d)\n",settings_type);
    }
}


int connectToController(char* controller_path,int* js_fd){

    *js_fd = open(controller_path,O_RDONLY);

    if(*js_fd < 0){
        fprintf(stderr,"\nError(%d): %s -> %s.\n",
                BAD_FILE_PATH_ERROR,
                getErrorMessage(BAD_FILE_PATH_ERROR),
                controller_path
               );

        return BAD_FILE_PATH_ERROR;    
    }

    return SUCCESSFUL_EXECUTION;
}

void displayPossibleErrors(void){
    displayErrorList();
}

void displayAll(void){
    displayErrorList();
    displayLoadedKeyMap();
    displayLoadedScriptMap();
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


int executeCommand(cli_args_t* args){
    int result        = 0,
        js_fd         = 0,
        processLater  = 0,
        settings_code = 0; //boolean to control whether to start processing joystick events

    char local_buf[100] = {0};
    char* device_path = (args->all_args)->value;
    
    char* settings_name = NULL,
        * settings_val  = NULL;

    result = connectToController(device_path,&js_fd); //pass 1st arg value to read from specified device
    if(result < SUCCESSFUL_EXECUTION){
        fprintf(stderr,"\nError(%d): %s -> %s.\n", result, getErrorMessage(result), device_path);
        return result;
    }
   
    //loop through all args, besides device path
    for(int arg_indx = 1; arg_indx < args->count;arg_indx++ ){
        if(arg_indx == 1){ printf("\nStarting command execution\n"); } 

        if( strcmp( (args->all_args+arg_indx)->option,EMPTY_OPTION) == COMMAND_MATCH ){
            
            if( strcmp( (args->all_args+arg_indx)->value,START_PROCESSING) == COMMAND_MATCH ){
                processLater = 1;
            } else{ 
                continue; //To-Do: Execute other non-option commands here
            }
            
        } else if ( strcmp( (args->all_args+arg_indx)->option,DISPLAY_OPTION) == COMMAND_MATCH){
            
            if( strcmp( (args->all_args+arg_indx)->value, DISPLAY_ALL) == COMMAND_MATCH){
                displayAll();    
            } else if ( strcmp( (args->all_args+arg_indx)->value, DISPLAY_ERRORS) == COMMAND_MATCH){
                displayErrorList();
            } else if ( strcmp( (args->all_args+arg_indx)->value, DISPLAY_MAP) == COMMAND_MATCH){ 
                displaySettings(CONTROLLER_MAP_SETTINGS); 
            } else if ( strcmp( (args->all_args+arg_indx)->value, DISPLAY_CONFIG) == COMMAND_MATCH){ 
                displaySettings(CONFIGURATION_SETTINGS); 
            } else {
                fprintf(stderr,"\nNot a supported command: \"-%s %s\"\n",
                        (args->all_args+arg_indx)->option,
                        (args->all_args+arg_indx)->value
                        );    
            } 
            
        } else if ( strcmp( (args->all_args+arg_indx)->option,CONFIGURE_MAP_SETTINGS)  == COMMAND_MATCH){
            strcpy(local_buf,(args->all_args+arg_indx)->value);
            settings_name = strtok(local_buf,",");
            settings_val = strtok(NULL,",");

            if(settings_name != NULL && settings_val != NULL){
                configureSettings(CONTROLLER_MAP_SETTINGS,settings_name,settings_val);
            } else{
                fprintf(stderr,"\nInput is not in the right format: \"%s\"\n",(args->all_args+arg_indx)->value);
            }
        } else if( strcmp( (args->all_args+arg_indx)->option, CONFIGURE_SCRIPT_SETTINGS) == COMMAND_MATCH){
            strcpy(local_buf,(args->all_args+arg_indx)->value);
            settings_name = strtok(local_buf,",");
            settings_val = strtok(NULL,",");

            if(settings_name != NULL && settings_val != NULL){
                configureSettings(SCRIPT_SETTINGS,settings_name,settings_val);
            } else{
                fprintf(stderr,"\nInput is not in the right format: \"%s\"\n",(args->all_args+arg_indx)->value);
            }

        } else{
            fprintf(stderr,"\nNot a supported command: \"-%s %s\"\n",
                    (args->all_args+arg_indx)->option,
                    (args->all_args+arg_indx)->value
                    );   
        }
    }

    if(processLater){ startProConProcessing(js_fd,device_path); }
    
    return SUCCESSFUL_EXECUTION;
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
    loadScriptMap(NULL);
}


void parseCLIArgs(int argc, char* argv[],cli_args_t* args){
    char* current_arg = NULL,
        * next_arg    = NULL,
        * char_pos    = NULL;
    
    int arg_indx = 0,
        arg_num  = 0;

    if(args == NULL){ //initialize if empty
        args = (cli_args_t*) malloc(sizeof(cli_args_t));
        args->all_args = (arg_t*) malloc(DEFAULT_CLI_ARGS_SZ* sizeof(arg_t));
    }

    args->count = argc-1; //ignore 1st CLI arg (file name)

    for(arg_indx=0,arg_num =1; arg_num <= args->count;arg_indx++,arg_num++){
        current_arg = argv[arg_num];
        char_pos = strrchr(current_arg,'-'); 

        fprintf(stdout,"\nArg #%d: %s",arg_num, current_arg);

        if(char_pos != NULL && strlen(current_arg) >= 2){ //process single letter & multi-letter options
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
    
    args->count = arg_indx; //set to number of arg_t objects filled
    args->isInitialized = 1;
    return;
}


void startProConProcessing(int joystick_fd,char* joystick_file_name){

    processAllEvents(joystick_fd,joystick_file_name);
}

void startTestMode(int mode){

}