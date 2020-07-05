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
#define NO_MODS 0
#define PROPAGATE_EN 1
#define KEY_TEST_DELAY 1 // seconds

#define KEY_MAP_PATH "key_map.def"
#define KEY_MAP_TABLE_DEFAULT_SIZE 20
#define MAX_LINE_SIZE 50
#define KEY_MAP_LABEL_DELIM ":"
#define KEY_MAP_VALUE_DELIM ","


//data structure definitions
typedef struct controller_key_map_t {
    unsigned long keysym;
    int controller_code;

    char* button_name;
} controller_key_map_t;

typedef struct key_map_table_t {
    int max_index;

    controller_key_map_t* key_map_table;

} key_map_table_t;

typedef struct x11_display_objs_t{
    Display* disp;
    Window* root;
    Window* focus_win;   
} x11_display_objs_t;


//global variables
#ifdef __X11_MAP_MAIN__
key_map_table_t KEY_MAP_TABLE = {0};

#else
extern key_map_table_t KEY_MAP_TABLE;
#endif //__X11_MAP_MAIN__


int convertControllerEventToKeyEvent(x11_display_objs_t* x11_interface, controller_input_t* con_event,XKeyEvent* key_event);
int displayLoadedKeyMap(void);
int freeXInterfaceObjs(x11_display_objs_t* x11_interface);
int modifyXKeyEvent(x11_display_objs_t* x11_interface,int isPressed,int keysym,int modifiers,XKeyEvent* event);
int initXInterface(x11_display_objs_t* x11_interface);
int loadKeyMap(char* key_map_path);
int processAllEvents(int joystick_fd,char* joystick_file_name);
int sendKeyEventToX(x11_display_objs_t* display_objs,XKeyEvent* key_event,int key_code, int isPressed,int modifiers);
int testAtoZPresses(void);
int testSendingKeyPresses(int key_code);
int updateKeyMap(char* key_map_path);

#endif // ifndef __KEY_MAP__