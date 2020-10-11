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
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <sys/types.h>

//source code headers
#include "config.h"
#include "cmd_utils.h"
#include "pro_con_utils.h"
#include "pro_con_errors.h"
#include "x11_map.h"


void configureSettings(int settings_type, char* settings_name, char* settings_value){
    int setting = 0,
        value   = 0;

    switch(settings_type){
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


int connectToController(char* controller_path, int* js_fd, int* isTestJS){

    *js_fd = open(controller_path,O_RDONLY);

    if(*js_fd < 0){
        if ((*js_fd = open(TEST_JOYSTICK_PATH,O_RDONLY)) < 0 ){
            fprintf(stderr,"\nError(%d): %s -> %s.\n",
                    COULD_NOT_OPEN_JS_FILE,
                    getErrorMessage(COULD_NOT_OPEN_JS_FILE),
                    controller_path
                    );
            
            *isTestJS = -1;
            return BAD_FILE_PATH_ERROR;
        } else{
            fprintf(stdout,"\nInfo: Joystick could not be found @ \"%s\". Using test joystick @ \"%s\"\n", controller_path,TEST_JOYSTICK_PATH);    
            *isTestJS = 1;

        }   
    } else{
        *isTestJS = 0;
    }

    return SUCCESSFUL_EXECUTION;
}

void displayPossibleErrors(void){
    displayErrorList();
}

void displayAll(void){
    displaySoftwareVersion();
    displayErrorList();
    displayLoadedKeyMap();
    displayLoadedScriptMap();
}

void displaySettings(int settings_type){

    switch(settings_type){

        case CONTROLLER_MAP_SETTINGS:
            displayLoadedKeyMap();
            break;

        case SCRIPT_SETTINGS:
            displayLoadedScriptMap();
            break;
        default:
            fprintf(stderr,"\nNot a valid settings type (%d)\n",settings_type);
    }

}

void displaySoftwareVersion(void){
    fprintf(stdout,"Software Version: %d.%d.%d\n", SOFTWARE_MAJOR_VERSION
                                                 , SOFTWARE_MINOR_VERSION
                                                 , SOFTWARE_PATCH_VERSION
            );

}


int executeCommand(cli_args_t* args){
    int result        = 0,
        js_fd         = 0, 
        isTestJS      = 0,
        processLater  = 0,
        settings_code = 0; //boolean to control whether to start processing joystick events

    char local_buf[100] = {0};
    char* device_path = "";
    
    char* settings_name = NULL,
        * settings_val  = NULL;
   
    //loop through all args
    for(int arg_indx = 0; arg_indx < args->count;arg_indx++ ){
        if(arg_indx == 0){ printf("\nStarting command execution\n"); } 

        if( strcmp( (args->all_args+arg_indx)->option,EMPTY_OPTION) == COMMAND_MATCH ){
            
            if( strcmp( (args->all_args+arg_indx)->value, START_PROCESSING) == COMMAND_MATCH ){
                processLater = 1;
            } else{ //any other non-option argument is the joystick device file path
                device_path = (args->all_args+arg_indx)->value;
            }
        } else if(strcmp((args->all_args+arg_indx)->option, HELP_OPTION) == COMMAND_MATCH){
            showHelp();
            return SUCCESSFUL_EXECUTION;  //end execution of commands if help option is given                
        } else if ( strcmp( (args->all_args+arg_indx)->option,DISPLAY_OPTION) == COMMAND_MATCH){
            
            if( strcmp( (args->all_args+arg_indx)->value, DISPLAY_ALL) == COMMAND_MATCH){
                displayAll();    
            } else if ( strcmp( (args->all_args+arg_indx)->value, DISPLAY_ERRORS) == COMMAND_MATCH){
                displayErrorList();
            } else if ( strcmp( (args->all_args+arg_indx)->value, DISPLAY_MAP) == COMMAND_MATCH){ 
                displaySettings(CONTROLLER_MAP_SETTINGS); 
            } else if ( strcmp( (args->all_args+arg_indx)->value, DISPLAY_SCRIPTS) == COMMAND_MATCH){ 
                displaySettings(SCRIPT_SETTINGS); 
            } else if ( strcmp( (args->all_args+arg_indx)->value, DISPLAY_SW_VERS) == COMMAND_MATCH){ 
                displaySoftwareVersion(); 
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
                configureSettings(CONTROLLER_MAP_SETTINGS, settings_name, RUN_SCRIPT_CONSTANT_STR); 
                configureSettings(SCRIPT_SETTINGS, settings_name, settings_val);
            } else{
                fprintf(stderr,"\nInput is not in the right format: \"%s\"\n",(args->all_args+arg_indx)->value);
            }
        } else if( strcmp( (args->all_args+arg_indx)->option, DISPLAY_SW_VERS) == COMMAND_MATCH){
            displaySoftwareVersion();
        } else if( strcmp( (args->all_args+arg_indx)->option,SET_RESOURCE_DIR_OPTION) == COMMAND_MATCH ){
            //handled elsewhere
        } else{
            fprintf(stderr,"\nNot a supported command: \"-%s %s\"\n",
                    (args->all_args+arg_indx)->option,
                    (args->all_args+arg_indx)->value
                    );   
        }
    }

    if(processLater){ 
        result = connectToController(device_path,&js_fd, &isTestJS); 
        if(result < SUCCESSFUL_EXECUTION){
            fprintf(stderr,"\nError(%d): %s -> %s.\n", result, getErrorMessage(result), device_path);
            return result;
        }

        if(isTestJS != 1){ 
            startProConProcessing(js_fd,device_path); 
        } else{
            fprintf(stdout,"\nInfo: Real joystick is not available.\nEnding command execution.\n");
        }
    }
    
    return SUCCESSFUL_EXECUTION;
}


void freeArgsData(cli_args_t* args){
    free(args->all_args);
    args->all_args = NULL;

    free(args);
    args = NULL;
    
    return;
}


void getResourcePaths(cli_args_t* args, char** error_file_path, char** key_map_path, char** script_map_path){
    int isValidPath = 0;
    
    char* resource_path = NULL,
        * input_path    = NULL,
        * home_path     = NULL;

    char path_buf[100] = {0};

    DIR* resource_dir = NULL;

    *error_file_path = (char*) malloc(MAX_PATH_SIZE_SZ * sizeof(char));
    *key_map_path = (char*) malloc(MAX_PATH_SIZE_SZ * sizeof(char));
    *script_map_path = (char*) malloc(MAX_PATH_SIZE_SZ * sizeof(char));

    //loop through all args
    for(int arg_indx = 0; arg_indx < args->count;arg_indx++ ){

        if( strcmp( (args->all_args+arg_indx)->option,SET_RESOURCE_DIR_OPTION) == COMMAND_MATCH ){
            
            input_path = (args->all_args+arg_indx)->value;
            resource_dir = opendir(input_path);

            if(resource_dir){
                closedir(resource_dir);
                isValidPath = 1;
            } else if(errno == ENOENT){
                fprintf(stderr,"\nError: Resource directory does not exist.\n");
            } else{
                fprintf(stderr,"\nError: Could not open directory\n");
            }

        }

    }

    if(isValidPath){
        resource_path = input_path;
    } else{
       if((home_path = getenv("HOME")) == NULL){
           home_path = getpwuid(getuid())->pw_dir;
       }  
        sprintf(path_buf,"%s/%s", home_path, DEFAULT_RESOURCE_DIR);
        resource_path = path_buf;
    }

    sprintf(*error_file_path,"%s/%s", resource_path, ERROR_DEF_PATH);
    sprintf(*key_map_path,"%s/%s", resource_path, KEY_MAP_PATH);
    sprintf(*script_map_path,"%s/%s", resource_path, SCRIPT_MAP_PATH);

    fprintf(stdout,"\nInfo: Error definitions path = %s", *error_file_path);
    fprintf(stdout,"\nInfo: Key map path = %s", *key_map_path);
    fprintf(stdout,"\nInfo: Script map path = %s\n", *script_map_path);

    return;
}

void initRoutine(cli_args_t* args){

    char* error_file_path = NULL,
        * key_map_path = NULL,
        * script_map_path = NULL;
    
    getResourcePaths(args, &error_file_path, &key_map_path, &script_map_path);

    loadErrorFile(error_file_path);
    loadKeyMap(key_map_path);
    loadScriptMap(script_map_path);

    fprintf(stdout, "\nInfo: Initializing done.\n");
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
            
            if(strcmp((args->all_args+arg_indx)->option, HELP_OPTION) == COMMAND_MATCH){
                snprintf((args->all_args+arg_indx)->value, DEFAULT_CLI_STR_SZ, "Display help");
                continue; //help option doesn't take a value                
            }

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


void showHelp(void){
    fprintf(stdout,"\n\n------ App: %s ------\n",APP_NAME);
    fprintf(stdout,"\nDescription: %s\n",APP_DESCRIPTION);
    fprintf(stdout,"Synopsis: %s [-h] [joystick_file_path] [%s] [options] \n",APP_NAME,START_PROCESSING);
    fprintf(stdout,"\nOptions:");

    fprintf(stdout,"\n\'-h\'  Displays help information. Exits application.\n");
    
    fprintf(stdout,"\n\'joystick_file_path\'  Absolute path to joystick file to read events from.\n");
    fprintf(stdout,"\n\'%s\'  Starts processing and mapping of procon events.\n",START_PROCESSING);
    
    fprintf(stdout,"\n\'-%s type\'  Displays information based on provided type.\n",DISPLAY_OPTION);
    fprintf(stdout,"           Valid types: \'%s\', \'%s\', \'%s\', \'%s\', \'%s\'\n", DISPLAY_ALL, DISPLAY_ERRORS, DISPLAY_MAP, DISPLAY_SCRIPTS, DISPLAY_SW_VERS);

    fprintf(stdout,"\n\'-%s event_code%ssetting_value\'  Configures mapping of joystick code to keymap value.\n",CONFIGURE_MAP_SETTINGS, SETTING_DELIMITER);
    fprintf(stdout,"\n\'-%s event_code%s\"script\"\'  Configures mapping of joystick code to script to execute.\n",CONFIGURE_SCRIPT_SETTINGS, SETTING_DELIMITER);
    
    fprintf(stdout,"------------------------------\n");

    fprintf(stdout,"\nEnding command execution.\n");
    return;
}


void startProConProcessing(int joystick_fd,char* joystick_file_name){

    processAllEvents(joystick_fd,joystick_file_name);
}
