/*

Author: ChrisB
Description: Provides functions for simulating keyboard input.
Date: Summer 2020

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <X11/extensions/XTest.h>

#define __X11_MAP_MAIN__
//source code headers
#include "x11_map.h"
#include "pro_con_errors.h"


int clickMouseAtCurrentPos(x11_display_objs_t* x11_interface,int isPressed){
    XEvent click_event = {0};

    //set event values
    click_event.type = ButtonPress;
    click_event.xbutton.button = Button1;
    click_event.xbutton.same_screen = True;
    
    click_event.xbutton.subwindow = DefaultRootWindow(x11_interface->disp);;

    //iterate through windows until no more subwindows
    while(click_event.xbutton.subwindow){
        click_event.xbutton.window = click_event.xbutton.subwindow;
        XQueryPointer(x11_interface->disp,click_event.xbutton.window ,&(click_event.xbutton.root), &(click_event.xbutton.subwindow),
                      &(click_event.xbutton.x_root), &(click_event.xbutton.y_root),
		              &(click_event.xbutton.x), &(click_event.xbutton.y),&(click_event.xbutton.state)
                      );

    }

    if(isPressed){
        XSendEvent(x11_interface->disp,PointerWindow,True,ButtonPressMask,&click_event);
        XFlush(x11_interface->disp);
    }else{
        click_event.type = ButtonRelease;
        XSendEvent(x11_interface->disp,PointerWindow,True,ButtonReleaseMask,&click_event);
        XFlush(x11_interface->disp);        
    }

    usleep(CLICK_SLEEP_uS);

    return SUCCESSFUL_EXECUTION;
}


int convertControllerEventToKeyEvent(x11_display_objs_t* x11_interface,controller_input_t* con_event,XKeyEvent* key_event){
    int event_mods   = NO_MODS,
        button_state = 0,
        keysym_code  = 0;

    unsigned int no_delay = 0;

    if(KEY_MAP_TABLE.isInitialized != KEY_MAP_TABLE_INITIALIZED){
        fprintf(stdout,"\nKey map file has not be loaded.\n");
        return INPUT_ERROR;
    }  

    button_state = con_event->button_event->isPressed;
    keysym_code =  KEY_MAP_TABLE.key_map_table[con_event->button_event->button_code].keysym;

    if(keysym_code == XK_Pointer_Button1){ //left click
        XTestFakeButtonEvent(x11_interface->disp, 1, button_state, no_delay);
    } else if (keysym_code == XK_Pointer_Button3){ //right click
        XTestFakeButtonEvent(x11_interface->disp, 3, button_state, no_delay);
    } else{
        XTestFakeKeyEvent(x11_interface->disp, XKeysymToKeycode(x11_interface->disp,keysym_code),button_state,no_delay);
    }

    return SUCCESSFUL_EXECUTION;
}


int convertControllerEventToMouseMove(x11_display_objs_t* x11_interface,direction_input_t* direction_input){
    int x_destination = 0,
        y_destination = 0;

    int direction = direction_input->changed_axis,
        pos_val       = 0,
        isHoriz       = 0,
        move_scalar = 0;

    isHoriz = (direction % 2 == 0) ? 1 : 0;
    pos_val = direction_input->joysticks_pos[direction];
    move_scalar = (direction_input->changed_axis >= DIR_PAD_HORIZ ) ? KEYPAD_ADJUST_SCALAR : POINTER_MOVE_SCALAR;
    
    fprintf(stdout,"\nPos: %d, Scalar: %d",pos_val,move_scalar);   
    
    if(isHoriz){
        XWarpPointer(x11_interface->disp,None,None,0,0,0,0,POINTER_MOVE_SCALAR*pos_val/MAX_AXIS_VALUE,0);
        fprintf(stdout,"\nDelta X: %d\n",move_scalar*pos_val/MAX_AXIS_VALUE);
    } else{
        XWarpPointer(x11_interface->disp,None,None,0,0,0,0,0,POINTER_MOVE_SCALAR*pos_val/MAX_AXIS_VALUE);
        fprintf(stdout,"\nDelta Y: %d\n",move_scalar*pos_val/MAX_AXIS_VALUE);       
    }

    XFlush(x11_interface->disp);

    return SUCCESSFUL_EXECUTION;
}


int displayLoadedKeyMap(void){
    if(KEY_MAP_TABLE.isInitialized != KEY_MAP_TABLE_INITIALIZED){
        fprintf(stdout,"\nKey map file has not be loaded.\n");
        return INPUT_ERROR;
    }  

    fprintf(stdout,"\nKey Map List:\n\n");
    for(int indx = 0; indx < KEY_MAP_TABLE.max_index; indx++){
        fprintf(stdout,"Key Map #%d: %s -> %s (%d)\n",
        indx,
        KEY_MAP_TABLE.key_map_table[indx].button_name,
        XKeysymToString(KEY_MAP_TABLE.key_map_table[indx].keysym),
        KEY_MAP_TABLE.key_map_table[indx].keysym    
        );
    }  
    fprintf(stdout,"\nDone.\n");
    
    return SUCCESSFUL_EXECUTION;
}


int displayLoadedScriptMap(void){

    if(SCRIPT_MAP_TABLE.isInitialized != SCRIPT_MAP_TABLE_INITIALIZED){
        fprintf(stdout,"\nScript map file has not be loaded.\n");
        return INPUT_ERROR;
    }  

    fprintf(stdout,"\nScript Map List:\n\n");
    for(int indx = 0; indx < SCRIPT_MAP_TABLE.max_index; indx++){
        fprintf(stdout,"Script Map #%d: %s -> %s",
        indx,
        SCRIPT_MAP_TABLE.script_map_table[indx].button_name,
        SCRIPT_MAP_TABLE.script_map_table[indx].script_cmd  
        );
    }  
    fprintf(stdout,"\nDone.\n");
    
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
        
    char line_buf[KEY_MAP_MAX_LINE_SZ]   = {0}; 

    path = (key_map_path == NULL) ? KEY_MAP_PATH: key_map_path;    
    key_map_file = fopen(path,"r");

    if(key_map_file == NULL){ 
        fprintf(stdout,"\nError: Could not load key map file \"%s\".\n",path);
        exit(1); 
    }
    
    KEY_MAP_TABLE.key_map_table = (controller_key_map_t*) malloc(KEY_MAP_TABLE_DEFAULT_SZ*sizeof(controller_key_map_t));
    
    fprintf(stdout,"\nLoading Key Map...\n");
    while(fgets(line_buf,KEY_MAP_MAX_LINE_SZ,key_map_file) != NULL){
        char* button_str = (char*) malloc(KEY_MAP_MAX_LINE_SZ*sizeof(char));
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

        key_map_num++;
    }

    KEY_MAP_TABLE.max_index = key_map_num;
    KEY_MAP_TABLE.isInitialized = KEY_MAP_TABLE_INITIALIZED;

    fclose(key_map_file); 

    return SUCCESSFUL_EXECUTION;
}


int loadScriptMap(char* script_map_path){

    int script_map_num = 0,
        button_code = 0;

    FILE* script_map_file = NULL;

    char* path        = NULL,
        * keysym_str  = NULL,
        * foundSubStr = NULL,
        * line_parts  = NULL;
        
    char line_buf[SCRIPT_MAP_MAX_LINE_SZ]         = {0},
         local_script_cmd[SCRIPT_MAP_MAX_LINE_SZ] = {0}; 

    path = (script_map_path == NULL) ? SCRIPT_MAP_PATH: script_map_path;    
    script_map_file = fopen(path,"r");

    if(script_map_file == NULL){ 
        fprintf(stdout,"\nError: Could not load script map file \"%s\".\n",path);
        exit(1); 
    }
    
    SCRIPT_MAP_TABLE.script_map_table = (controller_script_map_t*) malloc(SCRIPT_MAP_TABLE_DEFAULT_SZ*sizeof(controller_script_map_t));
    
    fprintf(stdout,"\nLoading Script Map...\n");
    while(fgets(line_buf,SCRIPT_MAP_MAX_LINE_SZ,script_map_file) != NULL){
        char* button_str = (char*) malloc(KEY_MAP_MAX_LINE_SZ*sizeof(char));
        if(foundSubStr = strstr(line_buf,"Script Map") ){ continue; } //ignore header

        line_parts = strtok(line_buf,SCRIPT_MAP_LABEL_DELIM);
        strcpy(button_str,line_parts);

        line_parts = strtok(NULL,SCRIPT_MAP_VALUE_DELIM);
        button_code = strtol(line_parts,NULL,10);

        line_parts = strtok(NULL,SCRIPT_MAP_VALUE_DELIM);
        strcpy(local_script_cmd,line_parts);

        SCRIPT_MAP_TABLE.script_map_table[script_map_num].controller_code = button_code ;
        strcpy(SCRIPT_MAP_TABLE.script_map_table[script_map_num].script_cmd, local_script_cmd);
        SCRIPT_MAP_TABLE.script_map_table[script_map_num].button_name = button_str ;

        script_map_num++;
    }

    fclose(script_map_file);

    SCRIPT_MAP_TABLE.isInitialized = SCRIPT_MAP_TABLE_INITIALIZED;
    SCRIPT_MAP_TABLE.max_index = script_map_num;

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


int processAllEvents(int joystick_fd,char* joystick_file_name){

    int status = SUCCESSFUL_EXECUTION;
    int x_pos = 0,
        y_pos = 0;

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
                initXInterface(&x11_interface); //updates window to what's currently focused on
                getJoystickInfo(&cur_event,controller.js_keypad_event);

                convertControllerEventToMouseMove(&x11_interface,controller.js_keypad_event);
                freeXInterfaceObjs(&x11_interface);
                
                if(controller.js_keypad_event->changed_axis % 2 == 0){
                    x_pos = controller.js_keypad_event->changed_axis;
                    y_pos = controller.js_keypad_event->other_axis;
                } else{
                    x_pos = controller.js_keypad_event->other_axis;
                    y_pos = controller.js_keypad_event->changed_axis;                    
                }

                fprintf(stdout,"\nJoystick Event: %s %s @ position (%d, %d)\n",
                        controller.js_keypad_event->direction,
                        controller.js_keypad_event->direction_type,
                        controller.js_keypad_event->joysticks_pos[x_pos],
                        controller.js_keypad_event->joysticks_pos[y_pos]
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
    XFlush(display_objs->disp);
    
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


int updateKeyMap(char* key_map_path,int button_code,unsigned long keysym_code){
    int key_map_num = 0,
        current_button_code = 0;

    unsigned long current_keysym_code = 0;

    char* path = NULL;    
    char updated_line[KEY_MAP_MAX_LINE_SZ]   = {0};
    
    FILE* key_map_file = NULL;

    if(KEY_MAP_TABLE.isInitialized != KEY_MAP_TABLE_INITIALIZED){
        fprintf(stdout,"\nKey map file has not been loaded.\n");
        return INPUT_ERROR;
    }  

    if(KEY_MAP_TABLE.max_index < button_code){
        fprintf(stdout,"\nSpecified button code is not supported: %d\n", button_code);
        return INPUT_ERROR;
    }  

    path = (key_map_path == NULL) ? KEY_MAP_PATH: key_map_path;    
    key_map_file = fopen(path,"w");

    if(key_map_file == NULL){ 
        fprintf(stdout,"\nError: Could not load key map file \"%s\".\n",path);
        return INPUT_ERROR; 
    }

    //write header
    fputs(KEY_MAP_HEADER,key_map_file);

    for(int indx = 0; indx < KEY_MAP_TABLE.max_index; indx++){
        
        if(indx == button_code){
            current_button_code = button_code;
            current_keysym_code = keysym_code;

        } else{
            current_button_code = KEY_MAP_TABLE.key_map_table[indx].controller_code ;
            current_keysym_code = KEY_MAP_TABLE.key_map_table[indx].keysym;
        }

        sprintf(updated_line,"%s: %d, %d\n",
                KEY_MAP_TABLE.key_map_table[indx].button_name,
                current_button_code,
                current_keysym_code
        );
        fputs(updated_line,key_map_file);

    }      

    fclose(key_map_file);

    //clear out data structure before reloading
    free(KEY_MAP_TABLE.key_map_table);
    KEY_MAP_TABLE.isInitialized = !KEY_MAP_TABLE_INITIALIZED;
    KEY_MAP_TABLE.max_index = 0;

    //reload key map file
    loadKeyMap(path);

    return SUCCESSFUL_EXECUTION;
}


int updateScriptMap(char* script_map_path, int button_code, char* script_cmd){
    int current_button_code = 0;

    FILE* script_map_file = NULL;

    char* path = (script_map_path == NULL) ? SCRIPT_MAP_PATH: script_map_path;
    char updated_line[SCRIPT_MAP_MAX_LINE_SZ]   = {0},
         local_buf[SCRIPT_MAP_MAX_LINE_SZ]      = {0};

    char* current_script_cmd = NULL;

    if(script_cmd == NULL){
        fprintf(stdout,"\nScript command not provided for button #%d.\n",button_code);
        return INPUT_ERROR;        
    }

    if(strchr(script_cmd,'\n') == NULL ){
        snprintf(local_buf,SCRIPT_MAP_MAX_LINE_SZ,"%s\n",script_cmd);
    } else{
        strcpy(local_buf,script_cmd);
    }

    if(SCRIPT_MAP_TABLE.isInitialized != SCRIPT_MAP_TABLE_INITIALIZED){
        fprintf(stdout,"\nScript map file has not been loaded.\n");
        return INPUT_ERROR;
    }  

    if(SCRIPT_MAP_TABLE.max_index < button_code){
        fprintf(stdout,"\nSpecified button code is not supported: %d\n", button_code);
        return INPUT_ERROR;
    }  
   
    script_map_file = fopen(path,"w");
    if(script_map_file == NULL){ 
        fprintf(stdout,"\nError: Could not load script map file \"%s\".\n",path);
        return INPUT_ERROR; 
    }

    //write header
    fputs(SCRIPT_MAP_HEADER,script_map_file);

    for(int indx = 0; indx < SCRIPT_MAP_TABLE.max_index; indx++){
        
        if(indx == button_code){
            current_button_code = button_code;
            current_script_cmd = local_buf;

        } else{
            current_button_code = SCRIPT_MAP_TABLE.script_map_table[indx].controller_code ;
            current_script_cmd = SCRIPT_MAP_TABLE.script_map_table[indx].script_cmd;
        }

        sprintf(updated_line,"%s: %d,%s",
                SCRIPT_MAP_TABLE.script_map_table[indx].button_name,
                current_button_code,
                current_script_cmd
        );
        fputs(updated_line,script_map_file);

    }      

    fclose(script_map_file);

    //clear out data structure before reloading
    free(SCRIPT_MAP_TABLE.script_map_table);
    SCRIPT_MAP_TABLE.isInitialized = !SCRIPT_MAP_TABLE_INITIALIZED;
    SCRIPT_MAP_TABLE.max_index = 0;

    //reload key map file
    loadScriptMap(path);

    return SUCCESSFUL_EXECUTION;
}