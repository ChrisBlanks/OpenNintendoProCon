/*

Author: ChrisB
Description: Utilities definitions & functions for interfacing with controller.
Date: Summer 2020

*/

#ifndef __PRO_CON_UTILS__
#define __PRO_CON_UTILS__

#include <linux/joystick.h>


//configuration values
#define CLI_ARGS_EXPECTED 2
#define JOYSTICK_DEV 1

#define NUM_OF_AXES 6


//data structure definitions 

typedef struct button_input_t{
    int button_code;
    int isPressed;

    char* button_name;
} button_input_t;

typedef struct direction_input_t {
    int joysticks_pos[NUM_OF_AXES];
    int changed_axis;
    int other_axis;

    char* direction;
    char* direction_type;
} direction_input_t;


typedef struct controller_input_t {
    int event_type; //set to value listed in controller_event_type enum

    button_input_t*     button_event;
    direction_input_t*  js_keypad_event;
    
} controller_input_t;


// enum definitions 
enum controller_event_type {
    KEY_BUTTON_TYPE   = JS_EVENT_BUTTON,
    JOYSTICK_TYPE     = JS_EVENT_AXIS
};

enum js_event_type_map{
    LEFT_JOY_HORIZ_AXIS  = 0,
    LEFT_JOY_VERT_AXIS   = 1,  
    RIGHT_JOY_HORIZ_AXIS = 2,
    RIGHT_JOY_VERT_AXIS  = 3,
    DIR_PAD_HORIZ        = 4,
    DIR_PAD_VERT         = 5   
};

enum key_event_type_map{
    B_BUTTON          = 0,
    A_BUTTON          = 1,
    Y_BUTTON          = 2,
    X_BUTTON          = 3,
    L_BUTTON          = 4,
    R_BUTTON          = 5,
    ZL_BUTTON         = 6,
    ZR_BUTTON         = 7,
    MINUS_BUTTON      = 8,
    PLUS_BUTTON       = 9,
    LEFT_JS_PRESS     = 10,
    RIGHT_JS_PRESS    = 11,
    HOME_BUTTON       = 12,
    SCREENSHOT_BUTTON = 13  
};

//function prototpyes
int getButtonInfo(struct js_event* event_info,button_input_t* button_data);
int getJoystickInfo(struct js_event* event_info,direction_input_t* joystick_data);
int readControllerEvent(int joystick_fd, struct js_event* event_info);
int testControllerInputs(int joystick_fd,char* joystick_file_name);

#endif // ifndef __PRO_CON_UTILS__