#
# Author: ChrisB
# Purpose: Builds source code from src/ & places output in build/.
# Date: Summer 2020
#

gcc -o build/openprocon src/*.c -L/usr/X11R6/lib -lX11 -lXtst -lpthread
