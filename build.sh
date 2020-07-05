#
# Author: ChrisB
# Purpose: Builds source code from src/ & places output in build/.
# Date: Summer 2020
#

gcc -o build/main src/*.c -L/usr/X11R6/lib -lX11
