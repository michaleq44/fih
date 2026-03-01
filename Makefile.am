bin_PROGRAMS = fih

noinst_HEADERS = inc/main.h inc/stb_image.h inc/stb_image_resize.h

fih_SOURCES = src/main.c src/stb.c
fih_CFLAGS = $(FIH_FLAGS) $(X11_CFLAGS) -O3 -Wall -Wextra -pedantic \
			 -Wno-unused-value -Wno-unused-variable -Wno-unused-function -Wno-unused-parameter
fih_CPPFLAGS = -I$(top_srcdir)/inc
fih_LDADD = $(X11_LIBS) -lm
