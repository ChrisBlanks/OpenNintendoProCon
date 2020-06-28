/*

Author: ChrisB
Description: Processes joystick events from Nintendo Switch Pro Controller
Date: Summer 2020

*/

#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>

#include <linux/joystick.h>


//configuration values
#define CLI_ARGS_EXPECTED 2
#define JOYSTICK_DEV 1

#define NUM_OF_AXES 4

//return values
#define NUM_OF_ERRORS 6 //includes successful execution as an error
#define MAX_ERROR_MESSAGE_LEN

#define SUCCESSFUL_EXECUTION    0 
#define INPUT_ERROR            -1
#define BAD_FILE_PATH_ERROR    -2
#define EXECUTION_ERROR        -3
#define JOYSTICK_AXES_ERROR    -4
#define DEVICE_READ_ERROR      -5


//global variables
char* g_error_messages[MAX_ERROR_MESSAGE_LEN]= { 
                                                  "No error.",
                                                  "Bad CLI input.",
                                                  "Could not open file for reading.",
                                                  "Function could not execute properly.",
                                                  "Could not retrieve axes data",
                                                  "Could not read data from device" 
                                               };   //put into a text file or other data structure


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
char* getErrorMessage(int status);
int readControllerEvent(int joystick_fd, struct js_event* event_info);
int getJoystickInfo(struct js_event* event_info,joysticks_t* joystick_data);


int main(int argc, char* argv[]){

    if(argc != CLI_ARGS_EXPECTED){
        fprintf(stderr,"\nError(%d): %s Only provided %d argument(s).\n",
                INPUT_ERROR,
                getErrorMessage(INPUT_ERROR),
                argc
               );

        return INPUT_ERROR;
    }

    char* js_dev_name = argv[JOYSTICK_DEV];
    int js_fd = open(js_dev_name,O_RDONLY);

    if(js_fd < 0){
        fprintf(stderr,"\nError(%d): %s -> %s.\n",
                BAD_FILE_PATH_ERROR,
                getErrorMessage(BAD_FILE_PATH_ERROR),
                js_dev_name
               );

        return BAD_FILE_PATH_ERROR;    
    }

    int status = -1;
    char* joystick= NULL;

    struct js_event cur_event = {0};
    joysticks_t controler_js = {0};
    
    fprintf(stdout,"\n------Starting Nintendo Pro Controller Testing------\n");
    fprintf(stdout,"Joystick Device @ %s\n",js_dev_name);

    //process & print events
    while(status = readControllerEvent(js_fd,&cur_event) == SUCCESSFUL_EXECUTION){ 
        
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

    close(js_fd);

    return SUCCESSFUL_EXECUTION; 
}


char* getErrorMessage(int status){
    return g_error_messages[-1 * status]; //turn negative status into index for array
}


int readControllerEvent(int joystick_fd, struct js_event* event_info){
    int status = 0;
    ssize_t num_bytes;

    num_bytes = read(joystick_fd,event_info,sizeof(*event_info));
    status = (num_bytes == sizeof(*event_info)) ? SUCCESSFUL_EXECUTION : DEVICE_READ_ERROR;

    return status;
}

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

