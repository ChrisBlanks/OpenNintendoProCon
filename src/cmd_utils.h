/*

Author: ChrisB
Description: Defines, declarations, function prototypes, etc. for running commands.
Date: Summer 2020

*/

#ifndef __CMD__
#define __CMD__

//defines
#define DEFAULT_CLI_ARGS_SZ 20
#define DEFAULT_CLI_STR_SZ 100
#define MAX_PATH_SIZE_SZ 300

//// CLI command defines
#define COMMAND_MATCH    0
#define EMPTY_OPTION     "None"

/// Command syntax: /exe/path /path/to/Joystick start
#define START_PROCESSING "start"


// Command syntax: /exe/path -HELP_OPTION 
#define HELP_OPTION "h"

/// Command syntax: /exe/path -SET_RESOURCE_DIR_OPTION /path/to/resource/files
#define SET_RESOURCE_DIR_OPTION "r"

//DISPLAY_OPTION value
#define DISPLAY_OPTION    "d"

//DISPLAY_TYPEs
/// Command syntax: /exe/path -DISPLAY_OPTION_VALUE DISPLAY_TYPE

#define DISPLAY_ALL       "all"
#define DISPLAY_ERRORS    "errors"
#define DISPLAY_MAP       "map"
#define DISPLAY_SCRIPTS   "scripts"
#define DISPLAY_SW_VERS   "version"

/// Command syntax: /exe/path -CONFIGURE_OPTION SETTING_CODE,SETTING_VALUE
#define CONFIGURE_MAP_SETTINGS "m"
#define CONFIGURE_SCRIPT_SETTINGS "s"
#define SETTING_DELIMITER  ","

// enum definitions 
enum pro_con_settings_type {
    CONTROLLER_MAP_SETTINGS,
    SCRIPT_SETTINGS
};

enum testing_modes {
    PROCESS_ALL_INPUTS,
    WRITE_A_TO_Z
};

// struct definitions

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
int  connectToController(char* controller_path, int* js_fd, int* isTestJS);
void displayAll(void);
void displayPossibleErrors(void);
void displaySettings(int settings_type);
void displaySoftwareVersion(void);
int  executeCommand(cli_args_t* args);
void freeArgsData(cli_args_t* args);
void getResourcePaths(cli_args_t* args ,char** error_file_path, char** key_map_path, char** script_map_path);
void initRoutine(cli_args_t* args);
void parseCLIArgs(int argc, char* argv[],cli_args_t* args);
void showHelp(void);
void startProConProcessing(int joystick_fd,char* joystick_file_name);



#endif //ifndef __CMD__
