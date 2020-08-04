/*

Author: ChrisB
Description: Processes joystick events from Nintendo Switch Pro Controller
Date: Summer 2020

*/

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>


// source code headers
#include "config.h"
#include "cmd_utils.h"
#include "pro_con_errors.h"


int main(int argc, char* argv[]){
    char* js_dev_name = NULL;
    int js_fd = 0;

    cli_args_t* args = NULL;

    js_dev_name = argv[JOYSTICK_ARG];
    js_fd = open(js_dev_name,O_RDONLY);

    if(js_fd < 0){
        fprintf(stderr,"\nError(%d): %s -> %s.\n",
                BAD_FILE_PATH_ERROR,
                getErrorMessage(BAD_FILE_PATH_ERROR),
                js_dev_name
               );

        return BAD_FILE_PATH_ERROR;    
    }
    

    initRoutine();
    parseCLIArgs(argc,argv,args);
    startProConProcessing(js_fd,js_dev_name);

    freeArgsData(args);
    close(js_fd);

    return SUCCESSFUL_EXECUTION; 
}
