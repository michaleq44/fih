# fih
* **Author:** Michał Kulas<br>
<br>
[English](README.md) | [Polski](README.pl.md)
<br>
Very simplistic image viewer for Linux ([Windows version](https://github.com/michaleq44/fih-win32))<br>
All it does is load images and display them.
#### Scaling
The program scales the image to fit into the window but doesn't stretch it to a bigger size than the original.
#### Usage
Execute: `fih [your/image_path.png]`<br>
The supported image formats are the ones supported by the `stb_image.h` library:
- BMP (.bmp)
- PNG (.png)
- JPEG (.jpg/.jpeg)
- GIF (.gif) __ONLY SHOWS FIRST FRAME__
- TGA (.tga)
- PSD (.psd)
- RGBE (.hdr)
- PIC (.pic)

### Runtime dependencies:
- **XLib**
- **libxcb**

## Compilation
Dependencies:
- **XLib** (library and headers)
- **libxcb** (library and headers)
- **GNU Autoconf**
- **GNU Make**
- a **C Compiler** (like GCC or Clang)

### How to compile:
- enter the project directory in a shell<br><br>
- **omit if you're building from the redistributable and not the source archive:** 
  - prepare autoconf: `./autogen.sh`<br><br>
- create and enter `build` directory: `mkdir build && cd build`
- configure the project: `../configure {flags}`
  - you can get info about the flags with `../configure --help`
- compile the program: `make`
- there will appear an executable `fih` in the `build` directory

OPTIONALLY:
- install the program: in the same directory execute `make install`
  - the default install location is `/usr/local/bin` but you can change it with the `--prefix` flag
    (for example: `--prefix=/opt/fih` installs in `/opt/fih/bin`)

## CREDITS
- uses `stb_image.h` and `stb_image_resize.h` from <https://github.com/nothings/stb>
