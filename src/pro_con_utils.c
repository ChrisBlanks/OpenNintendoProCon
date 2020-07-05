/*

Author: ChrisB
Description: Utilities functions for interfacing with controller.
Date: Summer 2020

*/

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

//source code headers
#include "pro_con_utils.h"
#include "pro_con_errors.h"


int getButtonInfo(struct js_event* event_info,button_input_t* button_data){
    button_data->button_code = event_info->number;
    button_data->isPressed = event_info->value;

    switch(button_data->button_code){
        case B_BUTTON:
            button_data->button_name = "B-Button";
            break;
        case A_BUTTON:
            button_data->button_name = "A-Button";
            break;
        case Y_BUTTON:
            button_data->button_name = "Y-Button";
            break;
        case X_BUTTON:
            button_data->button_name = "X-Button";
            break;
        case L_BUTTON:
            button_data->button_name = "L-Button";
            break;     
        case R_BUTTON:
            button_data->button_name = "R-Button";
            break;   
        case ZL_BUTTON:
            button_data->button_name = "ZL Button";
            break;
        case ZR_BUTTON:
            button_data->button_name = "ZR Button";
            break;
        case MINUS_BUTTON:
            button_data->button_name = "Minus Button";
            break;
        case PLUS_BUTTON:
            button_data->button_name = "Plus Button";
            break;
        case LEFT_JS_PRESS:
            button_data->button_name = "Left Joystick";
            break;
        case RIGHT_JS_PRESS:
            button_data->button_name = "Right Joystick";
            break;
        case HOME_BUTTON:
            button_data->button_name = "Home Button";
            break;
        case SCREENSHOT_BUTTON:
            button_data->button_name = "Screenshot Button";
            break;
        default:
            break;
    }

    return SUCCESSFUL_EXECUTION;
}


int getJoystickInfo(struct js_event* event_info,direction_input_t* joystick_data){

    joystick_data->joysticks_pos[event_info->number] = event_info->value;
    joystick_data->changed_axis = event_info->number;

    joystick_data->direction_type = (joystick_data->changed_axis >= DIR_PAD_HORIZ)   ? "Arrow"  : "Stick";

    switch(joystick_data->changed_axis){
        case LEFT_JOY_HORIZ_AXIS:
            joystick_data->direction = "Left";
            joystick_data->other_axis = LEFT_JOY_VERT_AXIS;
            break;
        case LEFT_JOY_VERT_AXIS:
            joystick_data->direction = "Left";
            joystick_data->other_axis = LEFT_JOY_HORIZ_AXIS;
            break;
        case RIGHT_JOY_HORIZ_AXIS:
            joystick_data->direction = "Right";
            joystick_data->other_axis = RIGHT_JOY_VERT_AXIS;
            break;
        case RIGHT_JOY_VERT_AXIS:
            joystick_data->direction = "Right";
            joystick_data->other_axis = RIGHT_JOY_HORIZ_AXIS;
            break;
        case DIR_PAD_HORIZ:
            if(event_info->value == 0){
                joystick_data->direction = "Idle";
                joystick_data->direction_type = "Pad";
            } else{
                joystick_data->direction = event_info->value < 0 ? "Left" : "Right";
            }

            joystick_data->other_axis = DIR_PAD_VERT;
            break;
        case DIR_PAD_VERT:
            if(event_info->value == 0){
                joystick_data->direction = "Idle";
                joystick_data->direction_type = "Pad";
            } else{
                joystick_data->direction = event_info->value < 0 ? "Up" : "Down";
            }
            
            joystick_data->other_axis = DIR_PAD_HORIZ;
            break;
        default:
            break; //no other axes
    }
    
    return SUCCESSFUL_EXECUTION;
}


int readControllerEvent(int joystick_fd, struct js_event* event_info){
    int status = 0;
    ssize_t num_bytes;

    num_bytes = read(joystick_fd,event_info,sizeof(*event_info));
    status = (num_bytes == sizeof(*event_info)) ? SUCCESSFUL_EXECUTION : DEVICE_READ_ERROR;

    return status;
}


//Purpose: Loop over controller input & print to stdout
int testControllerInputs(int joystick_fd,char* joystick_file_name){

    int status = SUCCESSFUL_EXECUTION;

    char* direction      = NULL,
        * direction_type = NULL;

    struct js_event cur_event = {0};
    direction_input_t controler_js   = {0};
    button_input_t controller_button = {0};
    
    fprintf(stdout,"\n------Starting Nintendo Pro Controller Testing------\n");  
    fprintf(stdout,"Controller Device @ %s\n",joystick_file_name);

    //process & print events
    while( status == SUCCESSFUL_EXECUTION ){ 
        status = readControllerEvent(joystick_fd,&cur_event); 
        if(status == DEVICE_READ_ERROR){ continue; }

        switch(cur_event.type){
            case KEY_BUTTON_TYPE:
                getButtonInfo(&cur_event,&controller_button);
                fprintf(stdout,"\nJoystick Event: %s %s\n",
                        controller_button.button_name,
                        controller_button.isPressed ? "pressed": "released"
                       ); 
                break;
            case JOYSTICK_TYPE:
                getJoystickInfo(&cur_event,&controler_js);

                fprintf(stdout,"\nJoystick Event: %s %s @ position (%d, %d)\n",
                        controler_js.direction,
                        controler_js.direction_type,
                        controler_js.joysticks_pos[controler_js.changed_axis],
                        controler_js.joysticks_pos[controler_js.other_axis]
                       ); //add CLI for file logging enable
                break;

            default:
                //ignore JS_EVENT_INIT
                break;
        }

        fflush(stdout); //clear buffers for next read
    }

    return SUCCESSFUL_EXECUTION;
}