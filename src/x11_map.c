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
        keycode      = 0;

    button_state = con_event->button_event->isPressed;
    
    //To-Do: Assign keycode based on mapping from key map file
    // keycode= some_val; 

    modifyXKeyEvent(x11_interface,button_state,keycode,event_mods,key_event);
    sendKeypressToX(key_event);

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

int sendKeypressToX(XKeyEvent* key_press){
    XSendEvent(key_press->display,key_press->window,PROPAGATE_EN,KeyPressMask,(XEvent*)key_press);
}


int testAtoZPresses(void){
    for(int key= XK_A; key <= XK_Z; key++){
        testSendingKeyPresses(key);
    }

    return SUCCESSFUL_EXECUTION;
}


int testSendingKeyPresses(int key_code){
    int test_button_state = 1,
        mods              = NO_MODS;

    x11_display_objs_t display_objs = {0};
    XKeyEvent test_event = {0};

    initXInterface(&display_objs);
    
    //press test key
    fprintf(stdout,"\nPressing key (%s)\n",XKeysymToString(key_code));
    modifyXKeyEvent(&display_objs, test_button_state,key_code,mods,&test_event);
    sendKeypressToX(&test_event);

    sleep(KEY_TEST_DELAY);

    fprintf(stdout,"Success!\n");

    //release test key
    fprintf(stdout,"\nReleasing key (%s)\n",XKeysymToString(key_code));
    modifyXKeyEvent(&display_objs, !test_button_state, key_code, mods, &test_event);
    sendKeypressToX(&test_event); 

    fprintf(stdout,"Success!\n");   

    freeXInterfaceObjs(&display_objs);

    return SUCCESSFUL_EXECUTION;
}