#include "main.h"

Display *dpy = NULL;
Window w;
XImage *img = NULL;
int blackColor, whiteColor, s, width, height, channels, imgwidth, imgheight, wwidth, wheight;
stbi_uc *imgdata = NULL, *buf = NULL;
XConfigureEvent xce;
Pixmap bg_pixmap = None;
Pixmap backbuffer = None;
char *title = NULL;
GC gc = None;

void update_window_bg(void) {
	if (bg_pixmap == None) return;

	XSetForeground(dpy, gc, blackColor);
	XFillRectangle(dpy, bg_pixmap, gc, 0, 0, wwidth, wheight);

	int x_offset = (wwidth - width) / 2;
	int y_offset = (wheight - height) / 2;
	XCopyArea(dpy, backbuffer, bg_pixmap, gc, 0, 0, width, height, x_offset, y_offset);

	XSetWindowBackgroundPixmap(dpy, w, bg_pixmap);
	XClearWindow(dpy, w);
	XFlush(dpy);
}

void set_title(const char *fname) {
	Atom net_wm_name = XInternAtom(dpy, "_NET_WM_NAME", False);
	Atom utf8_string = XInternAtom(dpy, "UTF8_STRING", False);

	const char *filename = strrchr(fname, '/');
	filename = filename != NULL ? filename + 1 : fname;

	title = malloc((strlen(filename) + 7) * sizeof(char));
	sprintf(title, "fih - %s", filename);

	XChangeProperty(dpy, w, net_wm_name, utf8_string, 8,
		PropModeReplace, (unsigned char*)title, strlen(title));

	XStoreName(dpy, w, title);
}

void cleanup(void) {
	if (gc) XFreeGC(dpy, gc);
	if (img) XDestroyImage(img);
	if (backbuffer) XFreePixmap(dpy, backbuffer);
	if (w) XDestroyWindow(dpy, w);
	if (dpy) XCloseDisplay(dpy);
	if (title) free(title);
	if (imgdata) stbi_image_free(imgdata);
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		fprintf(stderr, "%s: Provide image as console argument\n", argv[0]);
		return 1;
	}
	dpy = XOpenDisplay(NULL);
	if (!dpy) {
		cleanup();
		fprintf(stderr, "failed to open X display\n");
		return 1;
	}
	s = DefaultScreen(dpy);
	blackColor = BlackPixel(dpy, DefaultScreen(dpy));
	whiteColor = WhitePixel(dpy, DefaultScreen(dpy));

	imgdata = stbi_load(argv[1], &width, &height, &channels, 4);
	if (!imgdata) {
		cleanup();
		fprintf(stderr, "failed to load image\n");
		return 1;
	}
	imgwidth = width;
	imgheight = height;
	if (ImageByteOrder(dpy) == LSBFirst) {
		for (int i = 0; i < width * height * 4; i += 4) {
			stbi_uc tmp = imgdata[i];
			imgdata[i] = imgdata[i + 2];
			imgdata[i + 2] = tmp;
		}
	}
	buf = malloc(width*height*4);
	memcpy(buf, imgdata, width*height*4);
	img = XCreateImage(dpy, DefaultVisual(dpy, s),
			DefaultDepth(dpy, s), ZPixmap, 0,
			(char*)buf, width, height, 32, 0);

	wwidth = imgwidth;
	wheight = imgheight;

	w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0,
			wwidth, wheight, 0, blackColor, blackColor);
	XSelectInput(dpy, w, ExposureMask | StructureNotifyMask | KeyPressMask);
	set_title(argv[1]);
	XMapWindow(dpy, w);

	Atom wmDeleteMessage = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(dpy, w, &wmDeleteMessage, 1);

	gc = XCreateGC(dpy, w, 0, NULL);

	backbuffer = XCreatePixmap(dpy, w, wwidth, wheight, DefaultDepth(dpy, s));

	XPutImage(dpy, backbuffer, gc, img, 0, 0, 0, 0, width, height);
	update_window_bg();

	XEvent e;
	int running = 1;
	int need_redraw = 0;

	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 16000 * 1000;

	while (running) {
		while (XPending(dpy)) {
			XNextEvent(dpy, &e);
			switch(e.type) {
				case KeyPress:
					if (e.xkey.keycode == 0x09) running = 0;
					break;
				case ClientMessage:
					if ((Atom)e.xclient.data.l[0] == wmDeleteMessage) running = 0;
					break;
				case ConfigureNotify:
					xce = e.xconfigure;
					wwidth = xce.width;
					wheight = xce.height;

					need_redraw = 1;
					break;
				default:
					break;
			}
		}

		if (need_redraw) {
			int neww = xce.width;
			int newh = imgheight * neww / imgwidth;
			if (newh >= xce.height) {
				newh = xce.height;
				neww = imgwidth * newh / imgheight;
			}

			if (neww > imgwidth) neww = imgwidth;
			if (newh > imgheight) newh = imgheight;

			if (neww != width || newh != height) {
				if (img) {
					XDestroyImage(img);
				}
				XFreePixmap(dpy, backbuffer);

				width = neww;
				height = newh;

				buf = malloc(neww * newh * 4);
				stbir_resize_uint8(imgdata, imgwidth, imgheight, 0, buf, neww, newh, 0, 4);

				img = XCreateImage(dpy, DefaultVisual(dpy, s),
						DefaultDepth(dpy, s), ZPixmap, 0,
						(char*)buf, neww, newh, 32, 0);

				backbuffer = XCreatePixmap(dpy, w, width, height, DefaultDepth(dpy, s));
				XPutImage(dpy, backbuffer, gc, img, 0, 0, 0, 0, width, height);
			}

			static int last_ww, last_wh;
			if (wwidth != last_ww || wheight != last_wh) {
				if (bg_pixmap != None) XFreePixmap(dpy, bg_pixmap);
				bg_pixmap = XCreatePixmap(dpy, w, wwidth, wheight, DefaultDepth(dpy, s));
				last_ww = wwidth;
				last_wh = wheight;
			}

			update_window_bg();
			need_redraw = 0;
		}

		nanosleep(&ts, NULL);
	}

	cleanup();

	return 0;
}