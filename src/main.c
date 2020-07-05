/*

Author: ChrisB
Description: Processes joystick events from Nintendo Switch Pro Controller
Date: Summer 2020

*/

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>


// source code headers
#include "pro_con_errors.h"
#include "pro_con_utils.h"
#include "x11_map.h"


int main(int argc, char* argv[]){
    char* js_dev_name = NULL;
    int js_fd = 0;

    if(argc != CLI_ARGS_EXPECTED){
        fprintf(stderr,"\nError(%d): %s Only provided %d argument(s).\n",
                INPUT_ERROR,
                getErrorMessage(INPUT_ERROR),
                argc
               );

        return INPUT_ERROR;
    }

    js_dev_name = argv[JOYSTICK_DEV];
    js_fd = open(js_dev_name,O_RDONLY);

    if(js_fd < 0){
        fprintf(stderr,"\nError(%d): %s -> %s.\n",
                BAD_FILE_PATH_ERROR,
                getErrorMessage(BAD_FILE_PATH_ERROR),
                js_dev_name
               );

        return BAD_FILE_PATH_ERROR;    
    }
    
    //initialize error info
    loadErrorFile(ERROR_DEF_PATH);
    displayErrorList();

    //load key map
    loadKeyMap(KEY_MAP_PATH);

    //run test code
    processAllEvents(js_fd,js_dev_name);
    //testControllerInputs(js_fd,js_dev_name);

    close(js_fd);

    return SUCCESSFUL_EXECUTION; 
}
