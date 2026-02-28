#include <main.h>

Display *dpy;
Window w;
XImage *img;
int blackColor, whiteColor, s, width, height, channels, imgwidth, imgheight;
stbi_uc *imgdata, *buf;

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("%s: Provide image as console argument", 0[argv]);
		return 1;
	}
	dpy = XOpenDisplay(NULL);
	ASSERT(dpy);
	s = DefaultScreen(dpy);
	blackColor = BlackPixel(dpy, DefaultScreen(dpy));
	whiteColor = WhitePixel(dpy, DefaultScreen(dpy));

	w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0,
			200, 100, 0, blackColor, blackColor);
	XSelectInput(dpy, w, ExposureMask | StructureNotifyMask);
	XMapWindow(dpy, w);

	GC gc = XCreateGC(dpy, w, 0, NULL);
	XSetForeground(dpy, gc, whiteColor);

	imgdata = stbi_load(argv[1], &width, &height, &channels, 4);
	ASSERT(imgdata);
	imgwidth = width;
	imgheight = height;
	if (ImageByteOrder(dpy) == LSBFirst) {
		for (size_t i = 0; i < width * height * 4; i += 4) {
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
	for (;;) {
		XNextEvent(dpy, &e);
		switch(e.type) {
			case Expose:
				if (e.xexpose.count == 0) {
					XPutImage(dpy, w, gc, img, 0, 0, 0, 0, width, height);
				}
				break;
			case ConfigureNotify:
				XConfigureEvent xce = e.xconfigure;
				XDestroyImage(img);
				int neww = xce.width;
				int newh = imgheight * neww / imgwidth;
				if (newh >= xce.height) {
					newh = xce.height;
					neww = imgwidth * newh / imgheight;
				}

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
