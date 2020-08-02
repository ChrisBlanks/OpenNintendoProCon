/*

Author: ChrisB
Description: Server related defines, declarations, function prototypes, et cetera.
Date: Summer 2020

*/

#ifndef __SERVER__
#define __SERVER__


// enum definitions 
enum pro_con_settings_type {
    CONFIGURATION_SETTINGS,
    CONTROLLER_MAP_SETTINGS
};

enum testing_modes {
    PROCESS_ALL_INPUTS,
    WRITE_A_TO_Z
};

//function prototypes

void configureSettings(int settings_type, char* settings_name, char* settings_value);
void connectToController(char* controller_path);
void displayPossibleErrors(void);
void displaySettings(int settings_type);
void killServer(int delay);
void startServer(void);
void restartServer(int delay);
void startTestMode(int mode);



#endif //ifndef __SERVER__
