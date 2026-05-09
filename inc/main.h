#pragma once
#undef STB_IMAGE_IMPLEMENTATION
#undef STB_IMAGE_RESIZE_IMPLEMENTATION

#define _POSIX_C_SOURCE 199309L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "stb_image.h"
#include "stb_image_resize.h"
#include "icon.h"

#define min(x,y) ((x) < (y) ? (x) : (y))