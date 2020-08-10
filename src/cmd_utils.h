/*

Author: ChrisB
Description: Defines, declarations, function prototypes, etc. for running commands.
Date: Summer 2020

*/

#ifndef __CMD__
#define __CMD__

//defines
#define DEFAULT_CLI_ARGS_SZ 20
#define DEFAULT_CLI_STR_SZ 30

/// command defines
#define COMMAND_MATCH    0
#define EMPTY_OPTION     "None"
#define START_PROCESSING "start"

#define DISPLAY_OPTION    "d"
#define DISPLAY_ALL       "all"
#define DISPLAY_ERRORS     "errors"

#define CONFIGURE_SETTINGS "c"

// enum definitions 
enum pro_con_settings_type {
    CONFIGURATION_SETTINGS,
    CONTROLLER_MAP_SETTINGS
};

enum testing_modes {
    PROCESS_ALL_INPUTS,
    WRITE_A_TO_Z
};

//struct definitions

typedef struct arg_t {
    char value[DEFAULT_CLI_STR_SZ];
    char option[DEFAULT_CLI_STR_SZ]; 
} arg_t;

typedef struct cli_args_t {
    int count;
    int isInitialized;
    arg_t* all_args;
} cli_args_t;

//function prototypes

void configureSettings(int settings_type, char* settings_name, char* settings_value);
int  connectToController(char* controller_path,int* js_fd);
void displayAll(void);
void displayPossibleErrors(void);
void displaySettings(int settings_type);
int executeCommand(cli_args_t* args);
void freeArgsData(cli_args_t* args);
void initRoutine(void);
void parseCLIArgs(int argc, char* argv[],cli_args_t* args);
void startProConProcessing(int joystick_fd,char* joystick_file_name);
void startTestMode(int mode);



#endif //ifndef __CMD__
