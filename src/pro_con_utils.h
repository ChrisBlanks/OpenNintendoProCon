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

#define NUM_OF_AXES 4


//data structure definitions 
typedef struct joysticks_t {
    int joysticks_pos[NUM_OF_AXES];
    int changed_axis;
    int other_axis;
} joysticks_t;

enum event_type_map{
    LEFT_JOY_HORIZ_AXIS  = 0,
    LEFT_JOY_VERT_AXIS   = 1,  
    RIGHT_JOY_HORIZ_AXIS = 2,
    RIGHT_JOY_VERT_AXIS  = 3,   
};

//function prototpyes
int getJoystickInfo(struct js_event* event_info,joysticks_t* joystick_data);
int readControllerEvent(int joystick_fd, struct js_event* event_info);
int testControllerInputs(int joystick_fd,char* joystick_file_name);

#endif