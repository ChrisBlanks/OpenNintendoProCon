/*

Author: ChrisB
Description: Header file defining functions that map controller input to X11 key events.
Date: Summer 2020

*/


#ifndef __KEY_MAP__
#define __KEY_MAP__

#include "pro_con_utils.h"

int convertControllerEventToKeyEvent(controller_input_t con_event);
int createXKeyEvent();
int initXInterface(void);
int loadKeyMap(char* key_map_path);
int processAllEvents(void);


#endif // ifndef __KEY_MAP__