/*

Author: ChrisB
Description: Header file defining functions that map controller input to X11 key events.
Date: Summer 2020

*/


#ifndef __KEY_MAP__
#define __KEY_MAP__

//Xlib headers
#include <X11/Xlib.h>
#include <X11/keysym.h>

//source code headers
#include "pro_con_utils.h"

//define macros
#define POINTER_MOVE_SCALAR   25
#define KEYPAD_ADJUST_SCALAR  5
#define CLICK_SLEEP_uS        20000   //20 milliseconds
#define JOYSTICK_UPDATE_DELAY 20000   //20 milliseconds

#define NO_MODS         0
#define PROPAGATE_EN    1
#define KEY_TEST_DELAY  1 // seconds

#define KEY_MAP_PATH                   "key_map.def"
#define KEY_MAP_TABLE_DEFAULT_SZ       20
#define KEY_MAP_MAX_LINE_SZ            50
#define KEY_MAP_LABEL_DELIM            ":"
#define KEY_MAP_VALUE_DELIM            ","
#define KEY_MAP_HEADER                 "--- Key Map Definitions ---\n"
#define KEY_MAP_HEADER_LINE_OFFSET     1
#define KEY_MAP_TABLE_INITIALIZED      1

#define SCRIPT_MAP_PATH                "script_map.def"
#define SCRIPT_MAP_TABLE_DEFAULT_SZ    20
#define SCRIPT_MAP_MAX_LINE_SZ         150
#define SCRIPT_MAP_LABEL_DELIM         KEY_MAP_LABEL_DELIM
#define SCRIPT_MAP_VALUE_DELIM         KEY_MAP_VALUE_DELIM
#define SCRIPT_MAP_HEADER              "--- Script Map Definitions ---\n"
#define SCRIPT_HEADER_LINE_OFFSET      KEY_MAP_HEADER_LINE_OFFSET
#define SCRIPT_MAP_TABLE_INITIALIZED   KEY_MAP_TABLE_INITIALIZED

#define RUN_SCRIPT_CONSTANT -1  //indicates whether to run script instead of sending key event
#define RUN_SCRIPT_CONSTANT_STR "-1"

//data structure definitions
typedef struct controller_key_map_t {
    unsigned long keysym;
    int controller_code;

    char* button_name;
} controller_key_map_t;

typedef struct key_map_table_t {
    int isInitialized;
    int max_index;

    controller_key_map_t* key_map_table;

} key_map_table_t;


typedef struct controller_script_map_t {
    int controller_code;

    char script_cmd[SCRIPT_MAP_MAX_LINE_SZ];
    char* button_name;
} controller_script_map_t;

typedef struct script_map_table_t {
    int isInitialized;
    int max_index;

    controller_script_map_t* script_map_table;

} script_map_table_t;


typedef struct x11_display_objs_t{
    Display* disp;
    Window* root;
    Window* focus_win;   
} x11_display_objs_t;


//global variables
#ifdef __X11_MAP_MAIN__
key_map_table_t KEY_MAP_TABLE = {0};
script_map_table_t SCRIPT_MAP_TABLE = {0};

#else
extern key_map_table_t KEY_MAP_TABLE;
extern script_map_table_t SCRIPT_MAP_TABLE; 

#endif //__X11_MAP_MAIN__


int clickMouseAtCurrentPos(x11_display_objs_t* x11_interface, int isPressed);
int convertControllerEventToKeyEvent(x11_display_objs_t* x11_interface, controller_input_t* con_event,XKeyEvent* key_event);
int convertControllerEventToMouseMove(x11_display_objs_t* x11_interface,direction_input_t* direction_input);
int displayLoadedKeyMap(void);
int displayLoadedScriptMap(void);
int executeScriptInThread(char* script_cmd);
int freeXInterfaceObjs(x11_display_objs_t* x11_interface);
int modifyXKeyEvent(x11_display_objs_t* x11_interface,int isPressed,int keysym,int modifiers,XKeyEvent* event);
int initXInterface(x11_display_objs_t* x11_interface);
int loadKeyMap(char* key_map_path);
int loadScriptMap(char* script_map_path);
int processAllEvents(int joystick_fd,char* joystick_file_name);
void* runScriptThread(void* arg);
int sendKeyEventToX(x11_display_objs_t* display_objs,XKeyEvent* key_event,int key_code, int isPressed,int modifiers);
int testAtoZPresses(void);
int testSendingKeyPresses(int key_code);
int updateKeyMap(char* key_map_path,int button_code,unsigned long keysym_code);
int updateScriptMap(char* script_map_path, int button_code, char* script_cmd);

#endif // ifndef __KEY_MAP__