# fih
* **Autor:** Michał Kulas<br>
<br>

[English](README.md) | [Polski](README.pl.md)
<br>
Bardzo prosta przeglądarka grafiki dla systemu Linux ([wersja Windows](https://github.com/michaleq44/fih-win32))<br>
Jedyne co robi to ładuje zdjęcia i je wyświetla.
#### Skalowanie
Program skaluje obraz aby zmieścił się w okno, ale nie rozciąga go powyżej oryginalnego rozmiaru.
#### Sposób użycia
Wykonaj: `fih [twoj/ścieżka/do_obarzu.png]`<br>
Wspierane formaty obrazków to te wspierane przez bibliotekę `stb_image.h`:
- BMP (.bmp)
- PNG (.png)
- JPEG (.jpg/.jpeg)
- GIF (.gif) __POKAZUJE TYLKO PIERWSZĄ KLATKĘ__
- TGA (.tga)
- PSD (.psd)
- RGBE (.hdr)
- PIC (.pic)

### Zależności czasu uruchomienia:
- **XLib**
- **libxcb**

## Compilation
Dependencies:
- **XLib** (biblioteka i nagłówki)
- **libxcb** (biblioteka i nagłówki)
- **GNU Autoconf**
- **GNU Make**
- **Kompilator C** (np. GCC lub Clang)

### Jak skompilować:
- wejdź do folderu projektu w terminalu<br><br>
- **pomiń jeśli używasz archiwum dystrybucyjnego a nie bezpośredniego archiwum z kodem:**
  - przygotuj autoconf: `./autogen.sh`<br><br>
- stwórz i wejdź do folderu `build`: `mkdir build && cd build`
- skonfiguruj projekt: `../configure {flagi}`
    - informacje o flagach możesz uzyskać z pomocą komendy: `../configure --help`
- skompiluj program: `make`
- w folderze `build` pojawi się plik wykonywalny `fih`

OPCJONALNIE:
- zainstaluj program: w tym samym folderze wykonaj: `make install`
    - domyślna lokalizacja instalacji to `/usr/local/bin`, ale możesz ją zmienić flagą `--prefix`
      (na przykład: `--prefix=/opt/fih` instaluje program w `/opt/fih/bin`)

## BIBLIOTEKI
- `stb_image.h` i `stb_image_resize.h` z <https://github.com/nothings/stb>
