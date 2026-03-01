#include <main.h>

Display *dpy;
Window w;
XImage *img;
int blackColor, whiteColor, s, width, height, channels, imgwidth, imgheight, wwidth, wheight;
stbi_uc *imgdata, *buf;
XConfigureEvent xce;

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("%s: Provide image as console argument\n", 0[argv]);
		return 1;
	}
	dpy = XOpenDisplay(NULL);
	ASSERT(dpy);
	s = DefaultScreen(dpy);
	blackColor = BlackPixel(dpy, DefaultScreen(dpy));
	whiteColor = WhitePixel(dpy, DefaultScreen(dpy));

	w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0,
			200, 100, 0, blackColor, blackColor);
	XSelectInput(dpy, w, ExposureMask | StructureNotifyMask | KeyPressMask);
	XMapWindow(dpy, w);

	Atom wmDeleteMessage = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(dpy, w, &wmDeleteMessage, 1);

	GC gc = XCreateGC(dpy, w, 0, NULL);
	XSetForeground(dpy, gc, whiteColor);

	imgdata = stbi_load(1[argv], &width, &height, &channels, 4);
	ASSERT(imgdata);
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

	XEvent e;
	int running = 1;
	while (running) {
		XNextEvent(dpy, &e);
		switch(e.type) {
			case KeyPress:
				if (e.xkey.keycode == 0x09) running = 0;
				break;
			case Expose:
				if (e.xexpose.count == 0) {
					XPutImage(dpy, w, gc, img, 0, 0, (wwidth - width) / 2, (wheight - height) / 2, width, height);
				}
				break;
			case ClientMessage:
				if (e.xclient.data.l[0] == wmDeleteMessage) running = 0;
				break;
			case ConfigureNotify:
				xce = e.xconfigure;
				wwidth = xce.width;
				wheight = xce.height;
				int neww = xce.width;
				int newh = imgheight * neww / imgwidth;
				if (newh >= xce.height) {
					newh = xce.height;
					neww = imgwidth * newh / imgheight;
				}
				if ((neww == width && newh == height) || neww > imgwidth || newh > imgheight) break;

				XDestroyImage(img);
				buf = malloc(neww * newh * 4);
				stbir_resize_uint8(imgdata, imgwidth, imgheight, 0, buf, neww, newh, 0, 4);

				width = neww;
				height = newh;
				img = XCreateImage(dpy, DefaultVisual(dpy, s),
						DefaultDepth(dpy, s), ZPixmap, 0,
						(char*)buf, width, height, 32, 0);
				break;
			default:
				break;
		}
	}

	XDestroyImage(img);
	XDestroyWindow(dpy, w);
	XCloseDisplay(dpy);

	return 0;
}
