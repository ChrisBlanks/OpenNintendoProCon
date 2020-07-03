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


int getJoystickInfo(struct js_event* event_info,joysticks_t* joystick_data){

    joystick_data->joysticks_pos[event_info->number] = event_info->value;
    joystick_data->changed_axis = event_info->number;

    switch(joystick_data->changed_axis){
        case LEFT_JOY_HORIZ_AXIS:
            joystick_data->other_axis = LEFT_JOY_VERT_AXIS;
            break;
        case LEFT_JOY_VERT_AXIS:
            joystick_data->other_axis = LEFT_JOY_HORIZ_AXIS;
            break;
        case RIGHT_JOY_HORIZ_AXIS:
            joystick_data->other_axis = RIGHT_JOY_VERT_AXIS;
            break;
        case RIGHT_JOY_VERT_AXIS:
            joystick_data->other_axis = RIGHT_JOY_HORIZ_AXIS;
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

    int status = -1;
    char* joystick= NULL;

    struct js_event cur_event = {0};
    joysticks_t controler_js = {0};
    
    fprintf(stdout,"\n------Starting Nintendo Pro Controller Testing------\n");
    fprintf(stdout,"Joystick Device @ %s\n",joystick_file_name);

    //process & print events
    while( readControllerEvent(joystick_fd,&cur_event) == SUCCESSFUL_EXECUTION){ 
        
        switch(cur_event.type){
            case JS_EVENT_BUTTON:
                fprintf(stdout,"\nJoystick Event: %u %s\n",
                        cur_event.number,
                        cur_event.value ? "pressed": "released"
                       ); //add CLI for file logging enable
                break;
            case JS_EVENT_AXIS:
                status = getJoystickInfo(&cur_event,&controler_js);
                if(status != SUCCESSFUL_EXECUTION){
                    fprintf(stderr,"\nError(%d): %s\n",status,getErrorMessage(status));
                    return status;
                }
                
                joystick =  (controler_js.changed_axis < 2) ? "Left" : "Right" ;
                fprintf(stdout,"\nJoystick Event: %s Joystick @ position (%d, %d)\n",
                        joystick,
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