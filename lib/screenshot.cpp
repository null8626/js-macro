#include "screenshot.hpp"
#include <cstdlib>
#include <cstring>
#include <cstdio>

static void pngWriteFunc(png_structp png_ptr, png_bytep data, png_size_t length) {
    ScreenshotBuffer * p = reinterpret_cast<ScreenshotBuffer *>(png_get_io_ptr(png_ptr));
    size_t nsize = p->size + length;
    
    if (p->buffer)
        p->buffer = reinterpret_cast<unsigned char *>(realloc(p->buffer, nsize));
    else
        p->buffer = reinterpret_cast<unsigned char *>(malloc(nsize));
    
    memcpy(p->buffer + p->size, data, length);
    p->size += length;
}

Screenshot::Screenshot(const int x, const int y, const int width, const int height): failed(false), _width(width), _height(height) {
    HDC hdc = CreateCompatibleDC(0);
    HBITMAP bitmap = CreateCompatibleBitmap(GetDC(0), width, height);
    
    SelectObject(hdc, bitmap);
    BitBlt(hdc, 0, 0, width, height, GetDC(0), x, y, SRCCOPY);

    BITMAP bmp;
    BITMAPINFO inf;
    
    HDC currentDC = CreateCompatibleDC(0);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(currentDC, bitmap);
    GetObject(bitmap, sizeof(BITMAP), &bmp);

    inf.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
    inf.bmiHeader.biWidth        = bmp.bmWidth  = width;
    inf.bmiHeader.biHeight       = bmp.bmHeight = -height;
    inf.bmiHeader.biPlanes       = 1;
    inf.bmiHeader.biBitCount     = 32;
    inf.bmiHeader.biCompression  = BI_RGB;
    inf.bmiHeader.biSizeImage    = _width * _height * 4;
    inf.bmiHeader.biClrUsed      = 0;
    inf.bmiHeader.biClrImportant = 0;
    
    pixels = new unsigned char[inf.bmiHeader.biSizeImage];
    GetDIBits(currentDC, bitmap, 0, height, &pixels[0], &inf, DIB_RGB_COLORS);
    
    png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    info = png_create_info_struct(png);

    DeleteDC(hdc);
    DeleteDC(currentDC);
    
    DeleteObject(bitmap);

    if (png == nullptr || info == nullptr || setjmp(png_jmpbuf(png))) {
        failed = true;
        return;
    }
}

Screenshot::~Screenshot(void) {
    delete[] pixels;
    png_destroy_write_struct(&png, &info);
}

void Screenshot::writePngPixels(void) const {
    const png_size_t r = png_get_rowbytes(png, info);
    png_bytepp ptr = new png_bytep[_height];

    int x, y, z;
    for (y = 0, z = 0; y < _height; y++) {
        ptr[y] = new png_byte[r];
        
        for (x = 0; x < _width * 4; x += 4) {
            ptr[y][x]     = pixels[z + 2];
            ptr[y][x + 1] = pixels[z + 1];
            ptr[y][x + 2] = pixels[z];
            ptr[y][x + 3] = 0xff;
            
            z += 4;
        }
    }

    png_write_image(png, ptr);

    for (y = 0; y < _height; y++) {
        delete[] ptr[y];
    }
    
    delete[] ptr;
}

void Screenshot::save(const char * filename) {
    FILE * fp = fopen(filename, "wb");
    
    png_init_io(png, fp);
    
    png_set_IHDR(
        png,
        info,
        _width, _height, 8,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );

    png_write_info(png, info);
    
    writePngPixels();
    
    png_write_end(png, nullptr);
    fclose(fp);
}

void Screenshot::buffer(ScreenshotBuffer * buff) {
    png_set_IHDR(
        png,
        info,
        _width, _height, 8,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );
    
    png_set_write_fn(png, buff, pngWriteFunc, nullptr);
    png_write_info(png, info);

    writePngPixels();

    png_write_end(png, nullptr);
}