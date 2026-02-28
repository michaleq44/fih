#pragma once
#undef STB_IMAGE_IMPLEMENTATION
#undef STB_IMAGE_RESIZE_IMPLEMENTATION 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stb_image.h>
#include <stb_image_resize.h>

#define ASSERT(x) if (!(x)) exit(1)
#define min(x,y) ((x) < (y) ? (x) : (y))
