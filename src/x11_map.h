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


//data structure definitions
typedef struct controller_key_map_t {
    int keycode;
    int controller_code;
} controller_key_map_t;

typedef struct key_map_table_t {
    controller_key_map_t* key_map_table;
} key_map_table_t;

typedef struct x11_display_objs_t{
    Display* disp;
    Window* root;
    Window* focus_win;   
} x11_display_objs_t;


int convertControllerEventToKeyEvent(x11_display_objs_t* x11_interface, controller_input_t* con_event,XKeyEvent* key_event);
int freeXInterfaceObjs(x11_display_objs_t* x11_interface);
int modifyXKeyEvent(x11_display_objs_t* x11_interface,int isPressed,int keycode,int modifiers,XKeyEvent* event);
int initXInterface(x11_display_objs_t* x11_interface);
int loadKeyMap(char* key_map_path);
int processAllEvents(void);
int sendKeyEventToX(x11_display_objs_t* display_objs,XKeyEvent* key_event,int key_code, int isPressed,int modifiers);
int testAtoZPresses(void);
int testSendingKeyPresses(int key_code);

#endif // ifndef __KEY_MAP__