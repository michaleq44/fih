#include "main.h"

const long UREDRAW_INTERVAL = 16000; // 16ms; 62.5 fps
const long NLOOP_INTERVAL = 2000000; // 2ms; 500 tps

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

int set_icon(void) {
	int icon_w, icon_h, icon_c;

	stbi_uc *icon_data = stbi_load_from_memory(
		fih_png, fih_png_len, &icon_w, &icon_h, &icon_c, 4
	);

	if (!icon_data) return 0;

	unsigned long *icon_prop = malloc((2 + icon_w * icon_h) * sizeof(unsigned long));
	if (!icon_prop) {
		free(icon_prop);
		stbi_image_free(icon_data);
		return 0;
	}

	icon_prop[0] = icon_w;
	icon_prop[1] = icon_h;

	for (int i = 0; i < icon_w * icon_h; i++) {
		unsigned char* pixel = &icon_data[i * 4];
		if (ImageByteOrder(dpy) == LSBFirst) {
			icon_prop[2 + i] = (pixel[3] << 24) |  // Alpha
							   (pixel[0] << 16) |  // Red
							   (pixel[1] << 8)  |  // Green
							   (pixel[2]);         // Blue
		} else {
			icon_prop[2 + i] = (pixel[2] << 24) |  // Blue
							   (pixel[1] << 16) |  // Green
							   (pixel[0] << 8)  |  // Red
							   (pixel[3]);         // Alpha
		}
	}

	Atom net_wm_icon = XInternAtom(dpy, "_NET_WM_ICON", False);
	Atom cardinal = XInternAtom(dpy, "CARDINAL", False);

	XChangeProperty(dpy, w, net_wm_icon, cardinal, 32,
		PropModeReplace,
		(unsigned char*)icon_prop,
		2 + icon_w * icon_h);

	XFlush(dpy);

	free(icon_prop);
	stbi_image_free(icon_data);

	return 1;
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
	if (!set_icon()) {
		fprintf(stderr, "failed to set icon\n");
	}
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

	struct timespec last, now;
	clock_gettime(CLOCK_MONOTONIC, &last);

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
				case Expose:
					if (e.xexpose.count == 0) {
						need_redraw = 1;
					}
				default:
					break;
			}
		}

		if (need_redraw) {
			clock_gettime(CLOCK_MONOTONIC, &now);
			long elapsed = (now.tv_sec - last.tv_sec) * 1000000L +
					   (now.tv_nsec - last.tv_nsec) / 1000;
			if (elapsed < UREDRAW_INTERVAL) continue;
			last = now;


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

		struct timespec ts;
		ts.tv_sec = 0;
		ts.tv_nsec = NLOOP_INTERVAL;
		nanosleep(&ts, NULL);
	}

	cleanup();

	return 0;
}