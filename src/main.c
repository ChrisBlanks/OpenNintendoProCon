/*

Author: ChrisB
Description: Processes joystick events from Nintendo Switch Pro Controller
Date: Summer 2020

*/

#include <stdio.h>
#include <stdlib.h>

// source code headers
#include "config.h"
#include "cmd_utils.h"
#include "pro_con_errors.h"


int main(int argc, char* argv[]){

    cli_args_t* args = NULL;

    args = (cli_args_t*) malloc(sizeof(cli_args_t));
    args->all_args = (arg_t*) malloc(DEFAULT_CLI_ARGS_SZ* sizeof(arg_t));

    parseCLIArgs(argc,argv,args);
    initRoutine();
    executeCommand(args);
    
    freeArgsData(args);

    return SUCCESSFUL_EXECUTION; 
}
