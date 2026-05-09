bin_PROGRAMS = fih

noinst_HEADERS = inc/main.h inc/stb_image.h inc/stb_image_resize.h

BUILT_SOURCES = icon.h
icon.h: icon.stamp
	@test -f $@ || xxd -i fih.png | sed -e 's/^unsigned char .*/unsigned char fih_png[] = {/' \
                                    	               -e 's/^unsigned int .*_len =/unsigned int fih_png_len =/' > $@

icon.stamp: fih.png
	xxd -i $< | sed -e 's/^unsigned char .*/unsigned char fih_png[] = {/' \
                	               -e 's/^unsigned int .*_len =/unsigned int fih_png_len =/' > icon.h
	touch $@

CLEANFILES = icon.h icon.stamp
MAINTAINERCLEANFILES = icon.h icon.stamp

fih_SOURCES = src/main.c src/stb.c
fih_CFLAGS = $(FIH_FLAGS) $(X11_CFLAGS) -O3 -Wall -Wextra -pedantic \
			 -Wno-unused-value -Wno-unused-variable -Wno-unused-function -Wno-unused-parameter
fih_CPPFLAGS = -I$(top_srcdir)/inc
fih_LDADD = $(X11_LIBS) -lm

EXTRA_DIST = README.pl.md fih.png

licensedir = $(datadir)/licenses/$(PACKAGE_NAME)
dist_license_DATA = LICENSE