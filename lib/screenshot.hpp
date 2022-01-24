#ifndef SCREENSHOT_HPP
#define SCREENSHOT_HPP

#include <windows.h>
#include <png.h>

typedef struct {
    unsigned char * buffer;
    size_t size;
} ScreenshotBuffer;

class Screenshot {
	png_structp png;
	png_infop info;
	unsigned char * pixels;
	
	const int _width;
	const int _height;
	
	void writePngPixels(void) const;
	
	public:
		bool failed;
	
		Screenshot(const int, const int, const int, const int);
		~Screenshot(void);
		
		void save(const char * filename);
		void buffer(ScreenshotBuffer * buff);
};

#endif