/*

Author: ChrisB
Description: Provides functions for simulating keyboard input.
Date: Summer 2020

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//source code headers
#include "x11_map.h"
#include "pro_con_errors.h"


int convertControllerEventToKeyEvent(x11_display_objs_t* x11_interface,controller_input_t* con_event,XKeyEvent* key_event){
    int event_mods   = NO_MODS,
        button_state = 0,
        key_code     = 0;

    button_state = con_event->button_event->isPressed;
    
    //To-Do: Assign keycode based on mapping from key map file
    // keycode= some_val; 

    sendKeyEventToX(x11_interface,key_event,key_code,button_state,event_mods);

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

int modifyXKeyEvent(x11_display_objs_t* x11_interface,int isPressed,int keycode,int modifiers,XKeyEvent* event){
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
    event->keycode = XKeysymToKeycode((event->display),keycode); 
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
    
    return SUCCESSFUL_EXECUTION;
}

int processAllEvents(void){

    return SUCCESSFUL_EXECUTION;
}

int sendKeyEventToX(x11_display_objs_t* display_objs,XKeyEvent* key_event,int key_code, int isPressed,int modifiers){
    char* action = isPressed ? "Pressing" : "Releasing";

    fprintf(stdout,"\nPressing key (%s)\n",XKeysymToString(key_code));
    modifyXKeyEvent(display_objs, isPressed,key_code,modifiers,key_event);
    XSendEvent(key_event->display,key_event->window,PROPAGATE_EN,KeyPressMask,(XEvent*)key_event);

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