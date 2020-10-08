#!/bin/bash

# Author: ChrisB
# Purpose: Installs application files to '.local' folders.
# Note: Probably won't work on systems that don't use .local for storing application data
#

APP_NAME="openprocon"

LOCAL_BIN=".local/bin"
LOCAL_SHARE=".local/share"

EXEC_NAME="openprocon"
EXEC_PARENT_DIR="build"
EXEC_ORIG_PATH=$EXEC_PARENT_DIR/$EXEC_NAME
EXEC_INSTALL_DIR=$HOME/$LOCAL_BIN

RESOURCE_FILE_EXTENSION=".def"
RESOURCE_PARENT_DIR="build"

RESOURCE_PATH_GLOB=$RESOURCE_PARENT_DIR/*$RESOURCE_FILE_EXTENSION
RESOURCE_INSTALL_FOLDER=$HOME/$LOCAL_SHARE/$APP_NAME

echo $EXEC_ORIG_PATH
echo $RESOURCE_PATH_GLOB
echo $EXEC_INSTALL_DIR
echo $RESOURCE_INSTALL_FOLDER

if test -e $RESOURCE_INSTALL_FOLDER; then
    echo "Resource installation directory already exists."
else
    mkdir $RESOURCE_INSTALL_FOLDER
fi

#install files in the proper directories
cp $EXEC_ORIG_PATH $EXEC_INSTALL_DIR
cp $RESOURCE_PATH_GLOB $RESOURCE_INSTALL_FOLDER