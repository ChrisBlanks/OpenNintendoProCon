/*

Author: ChrisB
Description: Provides functions for simulating keyboard input.
Date: Summer 2020

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define __X11_MAP_MAIN__
//source code headers
#include "x11_map.h"
#include "pro_con_errors.h"


int convertControllerEventToKeyEvent(x11_display_objs_t* x11_interface,controller_input_t* con_event,XKeyEvent* key_event){
    int event_mods   = NO_MODS,
        button_state = 0,
        keysym_code  = 0;
    
    //if(keysym_code >= XK_A || keysym_code <= XK_Z){ event_mods = ShiftMask}
    button_state = con_event->button_event->isPressed;
    keysym_code =  KEY_MAP_TABLE.key_map_table[con_event->button_event->button_code].keysym;

    sendKeyEventToX(x11_interface,key_event,keysym_code,button_state,event_mods);
    XFlush(x11_interface->disp);

    return SUCCESSFUL_EXECUTION;
}


int freeXInterfaceObjs(x11_display_objs_t* x11_interface){
    XCloseDisplay(x11_interface->disp);
    x11_interface->disp = NULL;

    free(x11_interface->focus_win);   
    x11_interface->focus_win = NULL;

    free(x11_interface->root);
    x11_interface->root = NULL;

    return SUCCESSFUL_EXECUTION;
}


int modifyXKeyEvent(x11_display_objs_t* x11_interface,int isPressed,int keysym,int modifiers,XKeyEvent* event){
    event->display = (x11_interface->disp);
    event->window = *(x11_interface->focus_win);
    event->root = *(x11_interface->root);
    event->subwindow = None;

    event->time = CurrentTime; 
    
    event->x = 1;
    event->y = 1;
    event->x_root = 1;
    event->y_root = 1;

    event->same_screen = 1;
    event->keycode = XKeysymToKeycode((event->display),keysym); 
    event->state = modifiers;
    event->type = isPressed ? KeyPress : KeyRelease;

    return SUCCESSFUL_EXECUTION;
}

int initXInterface(x11_display_objs_t* x11_interface){
    int revert;
    
    Window* win_focus,
          * win_root;
           
    Display* display;

    //allocate memory
    display = (Display*) malloc(sizeof(Display*));
    win_focus = (Window*) malloc(sizeof(Window*));
    win_root = (Window*) malloc(sizeof(Window*));

    display= XOpenDisplay(0);
    if(display == NULL) { return COULD_NOT_OPEN_DISPLAY_ERROR; }

    *win_root = XDefaultRootWindow(display);

    XGetInputFocus(display,win_focus,&revert);

    x11_interface->disp = display;
    x11_interface->root = win_root;
    x11_interface->focus_win = win_focus;

    return SUCCESSFUL_EXECUTION;
}

int loadKeyMap(char* key_map_path){
    int key_map_num = 0,
        button_code = 0;
    
    unsigned long keysym = 0;

    FILE* key_map_file = NULL;

    char* path        = NULL,
        * keysym_str  = NULL,
        * foundSubStr = NULL,
        * line_parts  = NULL;
        
    char line_buf[MAX_LINE_SIZE]   = {0}, 
         button_str[MAX_LINE_SIZE] = {0};
    
    path = (key_map_path == NULL) ? KEY_MAP_PATH: key_map_path;    
    key_map_file = fopen(path,"r");

    if(key_map_file == NULL){ 
        fprintf(stdout,"\nError: Could not load key map file \"%s\".\n",path);
        exit(1); 
    }
    
    KEY_MAP_TABLE.key_map_table = (controller_key_map_t*) malloc(KEY_MAP_TABLE_DEFAULT_SIZE*sizeof(controller_key_map_t));
    
    fprintf(stdout,"\nLoading Key Map...\n");
    while(fgets(line_buf,MAX_LINE_SIZE,key_map_file) != NULL){

        if(foundSubStr = strstr(line_buf,"Key Map") ){ continue; } //ignore header

        line_parts = strtok(line_buf,KEY_MAP_LABEL_DELIM);
        strcpy(button_str,line_parts);

        line_parts = strtok(NULL,KEY_MAP_VALUE_DELIM);
        button_code = strtol(line_parts,NULL,10);

        line_parts = strtok(NULL,KEY_MAP_VALUE_DELIM);
        keysym = strtol(line_parts,NULL,10);

        KEY_MAP_TABLE.key_map_table[key_map_num].controller_code = button_code;
        KEY_MAP_TABLE.key_map_table[key_map_num].keysym = keysym;
        KEY_MAP_TABLE.key_map_table[key_map_num].button_name = button_str;

        fprintf(stdout,"Key Map -> %s: %d %d \n",button_str,button_code,keysym);
        key_map_num++;
    }

    KEY_MAP_TABLE.max_index = key_map_num;

    fclose(key_map_file); 

    return SUCCESSFUL_EXECUTION;
}

int processAllEvents(int joystick_fd,char* joystick_file_name){

    int status = SUCCESSFUL_EXECUTION;

    char* direction      = NULL,
        * direction_type = NULL;

    struct js_event cur_event = {0};
    
    //controller data structure initialization
    controller_input_t controller = {0};
    controller.button_event = (button_input_t*) malloc(sizeof(button_input_t));
    controller.js_keypad_event = (direction_input_t*) malloc(sizeof(direction_input_t));

    //X11 data structure initialization
    x11_display_objs_t x11_interface = {0};
    XKeyEvent key_event = {0};

    fprintf(stdout,"\n------Starting Nintendo Pro Controller X11 Mapping------\n");  
    fprintf(stdout,"Controller Device @ %s\n",joystick_file_name);

    //process & print events
    while( status == SUCCESSFUL_EXECUTION ){ 
        status = readControllerEvent(joystick_fd,&cur_event); 
        if(status == DEVICE_READ_ERROR){ continue; }

        switch(cur_event.type){
            case KEY_BUTTON_TYPE:
                controller.event_type = KEY_BUTTON_TYPE;
                initXInterface(&x11_interface); //updates window to what's currently focused on

                getButtonInfo(&cur_event,controller.button_event);
                convertControllerEventToKeyEvent(&x11_interface,&controller,&key_event);
                
                freeXInterfaceObjs(&x11_interface);
                break;
            
            case JOYSTICK_TYPE:
                controller.event_type = JOYSTICK_TYPE;
                getJoystickInfo(&cur_event,controller.js_keypad_event);
                
                fprintf(stdout,"\nJoystick Event: %s %s @ position (%d, %d)\n",
                        controller.js_keypad_event->direction,
                        controller.js_keypad_event->direction_type,
                        controller.js_keypad_event->joysticks_pos[controller.js_keypad_event->changed_axis],
                        controller.js_keypad_event->joysticks_pos[controller.js_keypad_event->other_axis]
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


int sendKeyEventToX(x11_display_objs_t* display_objs,XKeyEvent* key_event,int key_code, int isPressed,int modifiers){
    long event_mask = isPressed ? KeyPressMask: KeyRelease;

    modifyXKeyEvent(display_objs, isPressed,key_code,modifiers,key_event); 
    XSendEvent(key_event->display,key_event->window,PROPAGATE_EN,event_mask,(XEvent*)key_event);

    return SUCCESSFUL_EXECUTION; 
}


int testAtoZPresses(void){

    for(int key= XK_a; key <= XK_z; key++){
        testSendingKeyPresses(key);
    }

    return SUCCESSFUL_EXECUTION;
}


int testSendingKeyPresses(int key_code){
    int test_button_state = 1,
        mods              = ShiftMask; //mask makes letters uppercase

    x11_display_objs_t display_objs = {0};
    XKeyEvent test_event = {0};

    initXInterface(&display_objs);
    
    sleep(KEY_TEST_DELAY);

    sendKeyEventToX(&display_objs,&test_event,key_code,test_button_state,mods);
    sendKeyEventToX(&display_objs,&test_event,key_code,!test_button_state,mods);

    freeXInterfaceObjs(&display_objs);

    return SUCCESSFUL_EXECUTION;
}


int updateKeyMap(char* key_map_path){

}