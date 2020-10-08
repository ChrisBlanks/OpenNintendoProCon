/*

Author: ChrisB
Description: Configuration values.
Date: Summer 2020

*/

#ifndef __CONFIG__
#define __CONFIG__

#define CLI_ARGS_EXPECTED 2  //executing file name & default joystick path
#define JOYSTICK_ARG 1

#define APP_NAME "openprocon"
#define LOCAL_SHARE_DIR ".local/share"
#define DEFAULT_RESOURCE_DIR LOCAL_SHARE_DIR "/" APP_NAME

#define TEST_JOYSTICK_PATH "/dev/input/js0"

#define SOFTWARE_MAJOR_VERSION  0
#define SOFTWARE_MINOR_VERSION  1
#define SOFTWARE_PATCH_VERSION  0


#endif // ifndef __CONFIG__