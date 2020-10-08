/*

Author: ChrisB
Description: Support file for error handling.
Date: Summer 2020

*/

#ifndef __PRO_CON_ERRORS__
#define __PRO_CON_ERRORS__

#define ERROR_DEF_PATH "errors.def"

#define MAX_NUM_OF_ERRORS 50
#define MAX_ERROR_MESSAGE_LEN 100

#define SUCCESSFUL_EXECUTION                0 
#define INPUT_ERROR                        -1
#define BAD_FILE_PATH_ERROR                -2
#define EXECUTION_ERROR                    -3
#define JOYSTICK_AXES_ERROR                -4
#define DEVICE_READ_ERROR                  -5
#define COULD_NOT_OPEN_DISPLAY_ERROR       -6
#define COULD_NOT_OPEN_JS_FILE             -7

#define ERROR_TABLE_INITIALIZED 1
 

//data structure definitions
typedef struct error_table_t {
    int isInitialized;
    int max_index;

    char error_list[MAX_NUM_OF_ERRORS][MAX_ERROR_MESSAGE_LEN];
} error_table_t;


//global variables
#ifdef __PRO_CON_ERRORS_MAIN__
error_table_t GLOBAL_ERRORS = {0};

#else
extern error_table_t GLOBAL_ERRORS;

#endif //__PRO_CON_ERRORS_MAIN__

//function prototypes
void displayErrorList(void);
void loadErrorFile(char* error_file_path);
char* getErrorMessage(int status);

#endif // ifndef __PRO_CON_ERRORS__