/*

Author: ChrisB
Description: Support file for error handling.
Date: Summer 2020

*/

#ifndef __PRO_CON_ERRORS__
#define __PRO_CON_ERRORS__

#define ERROR_DEF_PATH "errors.def"

#define NUM_OF_ERRORS 6 //includes successful execution as an error
#define MAX_ERROR_MESSAGE_LEN 100

#define SUCCESSFUL_EXECUTION    0 
#define INPUT_ERROR            -1
#define BAD_FILE_PATH_ERROR    -2
#define EXECUTION_ERROR        -3
#define JOYSTICK_AXES_ERROR    -4
#define DEVICE_READ_ERROR      -5
 

//function prototypes
char* getErrorMessage(int status);

#endif // ifndef __PRO_CON_ERRORS__