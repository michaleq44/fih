CC = gcc
CFLAGS = -O3 --std=c11 -Iinc $(shell pkg-config --cflags x11)
LDFLAGS = -lm $(shell pkg-config --libs x11)

all: ximgview

ximgview: ximgview.o stb.o
	${CC} ximgview.o stb.o ${LDFLAGS} -o ximgview

ximgview.o: src/main.c
	${CC} -c src/main.c ${CFLAGS} -o ximgview.o

stb.o: src/stb.c
	${CC} -c src/stb.c ${CFLAGS} -o stb.o

clean:
	rm -f ximgview main.o stb.o

.PHONY: clean all
